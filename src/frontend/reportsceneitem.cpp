#include "reportsceneitem.h"
#include "customplot.h"
#include "reportdocument.h"

using namespace Frontend;

ReportSceneItem::ReportSceneItem(QGraphicsItem* pParent)
    : QGraphicsItem(pParent)
{
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
}

void ReportSceneItem::paint(QPainter* pPainter, QStyleOptionGraphicsItem const* pOption, QWidget* pWidget)
{
    if (isSelected())
        drawBorder(pPainter);
}

//! Draw the border around the content
void ReportSceneItem::drawBorder(QPainter* pPainter)
{
    QRectF rect = boundingRect();
    QPen pen(Qt::DashLine);
    pen.setWidth(0);
    pen.setColor(Qt::black);
    pPainter->save();
    pPainter->setPen(pen);
    pPainter->setBrush(Qt::NoBrush);
    pPainter->drawRect(rect);
    pPainter->restore();
}

TextReportSceneItem::TextReportSceneItem(Backend::Core::TextReportItem* pItem, QGraphicsItem* pParent)
    : ReportSceneItem(pParent)
    , mpItem(pItem)
{
}

//! Retrieve the bounding rectangle from the item
QRectF TextReportSceneItem::boundingRect() const
{
    return mpItem->rect;
}

//! Process paint event
void TextReportSceneItem::paint(QPainter* pPainter, QStyleOptionGraphicsItem const* pOption, QWidget* pWidget)
{
    drawText(pPainter);
    ReportSceneItem::paint(pPainter, pOption, pWidget);
}

//! Save changes after item change
QVariant TextReportSceneItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if (change == ItemPositionChange)
    {
        QPoint newPos = value.toPoint();
        mpItem->rect.moveTo(newPos);
    }
    return QGraphicsItem::itemChange(change, value);
}

//! Render the text
void TextReportSceneItem::drawText(QPainter* pPainter)
{
    qreal kPointFactor = 4.0;
    pPainter->save();
    QFont f = mpItem->font;
    f.setPointSizeF(f.pointSize() / kPointFactor);
    pPainter->setFont(f);
    pPainter->drawText(mpItem->rect, mpItem->alignment, mpItem->text);
    pPainter->restore();
}

GraphReportSceneItem::GraphReportSceneItem(Backend::Core::GraphReportItem* pItem, QGraphicsItem* pParent)
    : ReportSceneItem(pParent)
    , mpItem(pItem)
    , mpPlot(new CustomPlot)
{
}

GraphReportSceneItem::~GraphReportSceneItem()
{
    delete mpPlot;
}

//! Set the item state
void GraphReportSceneItem::refresh()
{
    mpPlot->xAxis->setTickLabelFont(mpItem->font);
    mpPlot->yAxis->setTickLabelFont(mpItem->font);
    mpPlot->xAxis->setLabelFont(mpItem->font);
    mpPlot->yAxis->setLabelFont(mpItem->font);
    mpPlot->xAxis->setLabel(mpItem->xLabel);
    mpPlot->yAxis->setLabel(mpItem->yLabel);
    mpPlot->replot();
}

//! Retrieve the bounding rectangle from the item
QRectF GraphReportSceneItem::boundingRect() const
{
    return mpItem->rect;
}

//! Process paint event
void GraphReportSceneItem::paint(QPainter* pPainter, QStyleOptionGraphicsItem const* pOption, QWidget* pWidget)
{
    refresh();
    drawPlot(pPainter);
    ReportSceneItem::paint(pPainter, pOption, pWidget);
}

//! Save changes after item change
QVariant GraphReportSceneItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if (change == ItemPositionChange)
    {
        QPoint newPos = value.toPoint();
        mpItem->rect.moveTo(newPos);
    }
    return QGraphicsItem::itemChange(change, value);
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
