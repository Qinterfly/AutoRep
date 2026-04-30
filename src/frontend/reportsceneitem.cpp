#include <QCursor>
#include <QGraphicsSceneEvent>
#include <QPainter>
#include <QSvgRenderer>

#include "reportdocument.h"
#include "reportitem.h"
#include "reportsceneitem.h"
#include "reporttextengine.h"

using namespace Backend::Core;
using namespace Frontend;

// Constants
static double const skEps = std::numeric_limits<double>::epsilon();
static double const skInf = std::numeric_limits<double>::infinity();

// Helper function
QFont sceneFont(ReportItem* pItem);
QRectF rotatedRect(QRectF const& rect, qreal angle);

ReportSceneItem::ReportSceneItem(ReportItem* pItem, QGraphicsItem* pParent)
    : QGraphicsItem(pParent)
    , mpItem(pItem)
    , mMode(Mode::kNone)
    , mHandle(Handle::kNone)
{
    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsFocusable);
}

int ReportSceneItem::type() const
{
    return QGraphicsItem::UserType + 1 + (int) mpItem->type();
}

ReportItem* ReportSceneItem::item()
{
    return mpItem;
}

bool ReportSceneItem::isMovable() const
{
    return flags().testFlag(QGraphicsItem::ItemIsMovable);
}

QRectF ReportSceneItem::boundingRect() const
{
    qreal const m = 2;
    QRectF rect = rotatedRect(mpItem->rect, mpItem->angle);
    return rect.adjusted(-m, -m, m, m);
}

void ReportSceneItem::paint(QPainter* pPainter, QStyleOptionGraphicsItem const* pOption, QWidget* pWidget)
{
    if (isSelected())
    {
        drawBorder(pPainter);
        if (isMovable())
            drawHandles(pPainter);
    }
}

void ReportSceneItem::hoverMoveEvent(QGraphicsSceneHoverEvent* pEvent)
{
    if (isMovable())
    {
        mHandle = detectHandle(pEvent->pos());
        switch (mHandle)
        {
        case Handle::kTopLeft:
        case Handle::kBottomRight:
            setCursor(Qt::SizeFDiagCursor);
            break;
        case Handle::kTopRight:
        case Handle::kBottomLeft:
            setCursor(Qt::SizeBDiagCursor);
            break;
        case Handle::kTop:
        case Handle::kBottom:
            setCursor(Qt::SizeVerCursor);
            break;
        case Handle::kLeft:
        case Handle::kRight:
            setCursor(Qt::SizeHorCursor);
            break;
        default:
            setCursor(Qt::OpenHandCursor);
            break;
        }
    }
    QGraphicsItem::hoverMoveEvent(pEvent);
}

void ReportSceneItem::mousePressEvent(QGraphicsSceneMouseEvent* pEvent)
{
    mLastPos = pEvent->pos();
    if (isMovable())
    {
        mHandle = detectHandle(pEvent->pos());
        if (mHandle != Handle::kNone)
        {
            mMode = Mode::kResize;
        }
        else
        {
            mMode = Mode::kMove;
            setCursor(Qt::ClosedHandCursor);
        }
    }
    QGraphicsItem::mousePressEvent(pEvent);
}

void ReportSceneItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* pEvent)
{
    emit requestEdit();
    QGraphicsItem::mouseDoubleClickEvent(pEvent);
}

void ReportSceneItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* pEvent)
{
    if (mMode == Mode::kMove || mMode == Mode::kResize)
    {
        emit changed();
        mMode = Mode::kNone;
        setCursor(Qt::ArrowCursor);
    }
    QGraphicsItem::mouseReleaseEvent(pEvent);
}

void ReportSceneItem::mouseMoveEvent(QGraphicsSceneMouseEvent* pEvent)
{
    if (mMode == Mode::kResize)
    {
        QPointF delta = pEvent->pos() - mLastPos;
        prepareGeometryChange();
        QRectF rect = rotatedRect(mpItem->rect, mpItem->angle);
        switch (mHandle)
        {
        case Handle::kBottomRight:
            rect.setBottomRight(rect.bottomRight() + delta);
            break;
        case Handle::kBottomLeft:
            rect.setBottomLeft(rect.bottomLeft() + delta);
            break;
        case Handle::kTopRight:
            rect.setTopRight(rect.topRight() + delta);
            break;
        case Handle::kTopLeft:
            rect.setTopLeft(rect.topLeft() + delta);
            break;
        case Handle::kTop:
            rect.setTop(rect.top() + delta.y());
            break;
        case Handle::kBottom:
            rect.setBottom(rect.bottom() + delta.y());
            break;
        case Handle::kLeft:
            rect.setLeft(rect.left() + delta.x());
            break;
        case Handle::kRight:
            rect.setRight(rect.right() + delta.x());
            break;
        default:
            break;
        }
        mpItem->rect = rotatedRect(rect, -mpItem->angle).toRect();
    }
    else if (mMode == Mode::kMove)
    {
        setCursor(Qt::ClosedHandCursor);
        QPointF delta = pEvent->pos() - mLastPos;
        prepareGeometryChange();
        mpItem->rect.translate(delta.toPoint());
    }
    if (mMode != Mode::kNone)
    {
        mLastPos = pEvent->pos();
        update();
    }
}

//! Draw the border around the content
void ReportSceneItem::drawBorder(QPainter* pPainter)
{
    QRectF rect = rotatedRect(mpItem->rect, mpItem->angle);
    QPen pen(Qt::SolidLine);
    pen.setWidthF(0.5);
    pen.setColor(QColor(38, 128, 235));
    pPainter->save();
    pPainter->setPen(pen);
    pPainter->setBrush(Qt::NoBrush);
    pPainter->drawRect(rect);
    pPainter->restore();
}

//! Draw the handles to manipulate item size
void ReportSceneItem::drawHandles(QPainter* pPainter)
{
    QPen pen(Qt::SolidLine);
    pen.setWidthF(0.5);
    pen.setColor(QColor(38, 128, 235));
    pPainter->save();
    pPainter->setPen(pen);
    pPainter->setBrush(Qt::white);
    for (QRectF const& h : handles())
        pPainter->drawEllipse(h);
    pPainter->restore();
}

//! Get the handles geometry
QMap<ReportSceneItem::Handle, QRectF> ReportSceneItem::handles() const
{
    const qreal s = 3.0;
    const qreal r = s / 2.0;

    QMap<Handle, QRectF> result;
    QRectF rect = rotatedRect(mpItem->rect, mpItem->angle);

    // Corners
    result[Handle::kTopLeft] = QRectF(rect.topLeft() - QPointF(r, r), QSizeF(s, s));
    result[Handle::kTopRight] = QRectF(rect.topRight() - QPointF(r, r), QSizeF(s, s));
    result[Handle::kBottomLeft] = QRectF(rect.bottomLeft() - QPointF(r, r), QSizeF(s, s));
    result[Handle::kBottomRight] = QRectF(rect.bottomRight() - QPointF(r, r), QSizeF(s, s));

    // Edgets
    result[Handle::kTop] = QRectF(QPointF(rect.center().x() - r, rect.top() - r), QSizeF(s, s));
    result[Handle::kBottom] = QRectF(QPointF(rect.center().x() - r, rect.bottom() - r), QSizeF(s, s));
    result[Handle::kLeft] = QRectF(QPointF(rect.left() - r, rect.center().y() - r), QSizeF(s, s));
    result[Handle::kRight] = QRectF(QPointF(rect.right() - r, rect.center().y() - r), QSizeF(s, s));

    return result;
}

//! Check if the position belongs to one of the handles
ReportSceneItem::Handle ReportSceneItem::detectHandle(QPointF const& pos) const
{
    auto map = handles();
    for (auto [key, value] : map.asKeyValueRange())
    {
        if (value.contains(pos))
            return key;
    }
    return Handle::kNone;
}

TextReportSceneItem::TextReportSceneItem(TextReportItem* pItem, ReportTextEngine& textEngine, QGraphicsItem* pParent)
    : ReportSceneItem(pItem, pParent)
    , mTextEngine(textEngine)
{
}

//! Retrieve the raw text
QString TextReportSceneItem::rawText() const
{
    TextReportItem* pItem = (TextReportItem*) mpItem;
    return pItem->text;
}

//! Retrieve the parsed text
QString TextReportSceneItem::processedText() const
{
    return mTextEngine.process(rawText());
}

Qt::Alignment TextReportSceneItem::textAlign() const
{
    TextReportItem* pItem = (TextReportItem*) mpItem;
    return pItem->align;
}

//! Process paint event
void TextReportSceneItem::paint(QPainter* pPainter, QStyleOptionGraphicsItem const* pOption, QWidget* pWidget)
{
    drawText(pPainter);
    ReportSceneItem::paint(pPainter, pOption, pWidget);
}

//! Render the text
void TextReportSceneItem::drawText(QPainter* pPainter)
{
    pPainter->save();
    pPainter->setFont(sceneFont(mpItem));
    pPainter->translate(mpItem->rect.center());
    pPainter->rotate(mpItem->angle);
    pPainter->translate(-mpItem->rect.center());
    pPainter->drawText(mpItem->rect, textAlign(), processedText());
    pPainter->restore();
}

PictureReportSceneItem::PictureReportSceneItem(PictureReportItem* pItem, QGraphicsItem* pParent)
    : ReportSceneItem(pItem, pParent)
{
}

//! Render the vector picture content
void PictureReportSceneItem::paint(QPainter* pPainter, QStyleOptionGraphicsItem const* pOption, QWidget* pWidget)
{
    PictureReportItem* pItem = (PictureReportItem*) mpItem;
    bool isValid = false;

    // Set the painter
    pPainter->save();
    pPainter->setFont(sceneFont(pItem));
    pPainter->translate(pItem->rect.center());
    pPainter->rotate(pItem->angle);
    pPainter->translate(-pItem->rect.center());

    // Draw the picture
    if (pItem->format == "svg")
    {
        QSvgRenderer renderer(pItem->data);
        isValid = renderer.isValid();
        if (isValid)
            renderer.render(pPainter, pItem->rect);
    }
    else
    {
        QPixmap pixmap;
        pixmap.loadFromData(pItem->data, pItem->format.toStdString().data());
        isValid = !pixmap.isNull();
        if (isValid)
            pPainter->drawPixmap(pItem->rect, pixmap);
    }

    // Restore the painter
    pPainter->restore();

    // Draw the default image, if the picture data could not be resolved
    if (!isValid)
    {
        pPainter->save();
        pPainter->setPen(Qt::black);
        pPainter->fillRect(mpItem->rect, Qt::lightGray);
        pPainter->restore();
    }
    ReportSceneItem::paint(pPainter, pOption, pWidget);
}

TableReportSceneItem::TableReportSceneItem(TableReportItem* pItem, ReportTextEngine& textEngine, QGraphicsItem* pParent)
    : ReportSceneItem(pItem, pParent)
    , mTextEngine(textEngine)
{
}

//! Render the table content
void TableReportSceneItem::paint(QPainter* pPainter, QStyleOptionGraphicsItem const* pOption, QWidget* pWidget)
{
    TableReportItem* pItem = (TableReportItem*) mpItem;

    // Check if there is any tabular data to display
    if (pItem->isEmpty())
        return;

    // Set the transformation
    pPainter->save();
    pPainter->setFont(sceneFont(pItem));
    pPainter->translate(pItem->rect.center());
    pPainter->rotate(pItem->angle);
    pPainter->translate(-pItem->rect.center());
    pPainter->translate(pItem->rect.topLeft());

    // Get dimensions
    int numRows = pItem->numRows();
    int numCols = pItem->numCols();
    int iHeader = pItem->showLabels;
    qreal cellWidth = (qreal) pItem->rect.width() / (numCols + iHeader);
    qreal cellHeight = (qreal) pItem->rect.height() / (numRows + iHeader);

    // Set the data style
    QPen pen(Qt::black);
    pen.setWidthF(pItem->gridWidth);
    pPainter->setPen(pen);
    QTextOption textOption;
    textOption.setAlignment(Qt::AlignCenter);

    // Set the table data
    QRectF cell;
    for (int iRow = 0; iRow != numRows; ++iRow)
    {
        for (int iCol = 0; iCol != numCols; ++iCol)
        {
            cell = QRectF((iHeader + iCol) * cellWidth, (iHeader + iRow) * cellHeight, cellWidth, cellHeight);
            pPainter->drawRect(cell);
            pPainter->drawText(cell, mTextEngine.process(pItem->data[iRow][iCol]), textOption);
        }
    }

    // Set the header
    if (iHeader > 0)
    {
        // Middle
        cell = QRectF(0, 0, cellWidth, cellHeight);
        pPainter->drawRect(cell);
        pPainter->drawText(cell, mTextEngine.process(pItem->midLabel), textOption);

        // Horizontal
        for (int iCol = 1; iCol <= numCols; ++iCol)
        {
            cell = QRectF(iCol * cellWidth, 0, cellWidth, cellHeight);
            int iLabel = iCol - 1;
            pPainter->drawRect(cell);
            if (iLabel < pItem->horLabels.size())
                pPainter->drawText(cell, mTextEngine.process(pItem->horLabels[iLabel]), textOption);
        }

        // Vertical
        for (int iRow = 1; iRow <= numRows; ++iRow)
        {
            cell = QRectF(0, iRow * cellHeight, cellWidth, cellHeight);
            int iLabel = iRow - 1;
            pPainter->drawRect(cell);
            if (iLabel < pItem->verLabels.size())
                pPainter->drawText(cell, mTextEngine.process(pItem->verLabels[iLabel]), textOption);
        }
    }

    // Restore the painter
    pPainter->restore();

    ReportSceneItem::paint(pPainter, pOption, pWidget);
}

//! Helper function to get scene related font
QFont sceneFont(ReportItem* pItem)
{
    qreal kPointFactor = 4.0;
    QFont f = pItem->font;
    f.setPointSizeF(f.pointSize() / kPointFactor);
    return f;
}

//! Helper function to compute rotated rectangle
QRectF rotatedRect(QRectF const& rect, qreal angle)
{
    QTransform t;
    QPointF c = rect.center();
    t.translate(c.x(), c.y());
    t.rotate(angle);
    t.translate(-c.x(), -c.y());
    return t.mapRect(rect);
}

