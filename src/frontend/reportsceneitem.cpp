#include "reportsceneitem.h"
#include "customplot.h"
#include "reportdocument.h"

using namespace Backend::Core;
using namespace Frontend;

ReportSceneItem::ReportSceneItem(ReportItem* pItem, QGraphicsItem* pParent)
    : QGraphicsItem(pParent)
    , mpItem(pItem)
    , mMode(Mode::kNone)
    , mHandle(Handle::kNone)
{
    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
}

QRectF ReportSceneItem::boundingRect() const
{
    qreal const m = 2;
    return mpItem->rect.adjusted(-m, -m, m, m);
}

void ReportSceneItem::paint(QPainter* pPainter, QStyleOptionGraphicsItem const* pOption, QWidget* pWidget)
{
    if (isSelected())
    {
        drawBorder(pPainter);
        drawHandles(pPainter);
    }
}

void ReportSceneItem::hoverMoveEvent(QGraphicsSceneHoverEvent* pEvent)
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
    QGraphicsItem::hoverMoveEvent(pEvent);
}

void ReportSceneItem::mousePressEvent(QGraphicsSceneMouseEvent* pEvent)
{
    mHandle = detectHandle(pEvent->pos());
    if (mHandle != Handle::kNone)
    {
        mMode = Mode::kResize;
        mLastPos = pEvent->pos();
    }
    else
    {
        mMode = Mode::kMove;
        setCursor(Qt::ClosedHandCursor);
    }
    QGraphicsItem::mousePressEvent(pEvent);
}

void ReportSceneItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* pEvent)
{
    mMode = Mode::kNone;
    setCursor(Qt::ArrowCursor);
    QGraphicsItem::mouseReleaseEvent(pEvent);
}

void ReportSceneItem::mouseMoveEvent(QGraphicsSceneMouseEvent* pEvent)
{
    if (mMode == Mode::kResize)
    {
        QPointF delta = pEvent->pos() - mLastPos;
        prepareGeometryChange();
        QRectF rect = mpItem->rect;
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
        mLastPos = pEvent->pos();
        mpItem->rect = rect.toRect();
    }
    else if (mMode == Mode::kMove)
    {
        setCursor(Qt::ClosedHandCursor);
        QGraphicsItem::mouseMoveEvent(pEvent);
    }
    update();
}

//! Save changes after item change
QVariant ReportSceneItem::itemChange(GraphicsItemChange change, QVariant const& value)
{
    if (change == ItemPositionChange)
    {
        QPoint newPos = value.toPoint();
        mpItem->rect.moveTo(newPos);
    }
    return QGraphicsItem::itemChange(change, value);
}

//! Draw the border around the content
void ReportSceneItem::drawBorder(QPainter* pPainter)
{
    QPen pen(Qt::SolidLine);
    pen.setWidthF(0.5);
    pen.setColor(QColor(38, 128, 235));
    pPainter->save();
    pPainter->setPen(pen);
    pPainter->setBrush(Qt::NoBrush);
    pPainter->drawRect(mpItem->rect);
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
    QRectF rect = mpItem->rect;

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

TextReportSceneItem::TextReportSceneItem(TextReportItem* pItem, QGraphicsItem* pParent)
    : ReportSceneItem(pItem, pParent)
{
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
    qreal kPointFactor = 4.0;
    TextReportItem* pItem = (TextReportItem*) mpItem;
    pPainter->save();
    QFont f = pItem->font;
    f.setPointSizeF(f.pointSize() / kPointFactor);
    pPainter->setFont(f);
    pPainter->drawText(pItem->rect, pItem->alignment, pItem->text);
    pPainter->restore();
}

GraphReportSceneItem::GraphReportSceneItem(GraphReportItem* pItem, QGraphicsItem* pParent)
    : ReportSceneItem(pItem, pParent)
    , mpPlot(new CustomPlot)
{
}

GraphReportSceneItem::~GraphReportSceneItem()
{
    delete mpPlot;
}

//! Set the item state
void GraphReportSceneItem::setState()
{
    GraphReportItem* pItem = (GraphReportItem*) mpItem;
    mpPlot->xAxis->setTickLabelFont(pItem->font);
    mpPlot->yAxis->setTickLabelFont(pItem->font);
    mpPlot->xAxis->setLabelFont(pItem->font);
    mpPlot->yAxis->setLabelFont(pItem->font);
    mpPlot->xAxis->setLabel(pItem->xLabel);
    mpPlot->yAxis->setLabel(pItem->yLabel);
    mpPlot->replot();
}

//! Process paint event
void GraphReportSceneItem::paint(QPainter* pPainter, QStyleOptionGraphicsItem const* pOption, QWidget* pWidget)
{
    setState();
    drawPlot(pPainter);
    ReportSceneItem::paint(pPainter, pOption, pWidget);
}

//! Draw the plot to the vector picture
void GraphReportSceneItem::drawPlot(QPainter* pPainter)
{
    // Get the dimensions
    double dpi = pPainter->device()->logicalDpiX();
    auto mmToPx = [dpi](double mm) { return mm * dpi / 25.4; };
    QSize pxSize(mmToPx(mpItem->rect.width()), mmToPx(mpItem->rect.height()));

    // Create the picture
    QPicture pic;
    QCPPainter qcpPainter;
    qcpPainter.begin(&pic);
    mpPlot->toPainter(&qcpPainter, pxSize.width(), pxSize.height());
    qcpPainter.end();

    // Compute the transformation parameters
    QSize finalSize = pic.boundingRect().size();
    finalSize.scale(QSize(mpItem->rect.width(), mpItem->rect.height()), Qt::KeepAspectRatio);
    double scaleFactor = finalSize.width() / (double) pic.boundingRect().size().width();
    QPoint pos(mmToPx(mpItem->rect.x()), mmToPx(mpItem->rect.y()));

    // Render the picture
    pPainter->save();
    pPainter->scale(scaleFactor, scaleFactor);
    pPainter->drawPicture(pos, pic);
    pPainter->restore();
}
