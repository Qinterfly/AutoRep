#include <testlab/common.h>
#include <QSvgGenerator>
#include <QSvgRenderer>

#include "customplot.h"
#include "reportdocument.h"
#include "reportitem.h"
#include "reportsceneitem.h"
#include "session.h"

using namespace Backend::Core;
using namespace Frontend;

// Helper function
QRectF rotatedRect(QRectF const& rect, qreal angle);
QVector<double> convert(std::vector<double> const& data);
int findResponse(ResponseBundle const& bundle, GraphReportPoint const& point, ReportDirection dir, Testlab::ResponseType type);

ReportSceneItem::ReportSceneItem(ReportItem* pItem, QGraphicsItem* pParent)
    : QGraphicsItem(pParent)
    , mpItem(pItem)
    , mMode(Mode::kNone)
    , mHandle(Handle::kNone)
{
    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsMovable);
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

void ReportSceneItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* pEvent)
{
    if (mMode != Mode::kNone)
        emit changed();
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
    pPainter->translate(pItem->rect.center());
    pPainter->rotate(pItem->angle);
    pPainter->translate(-pItem->rect.center());
    pPainter->drawText(pItem->rect, pItem->alignment, pItem->text);
    pPainter->restore();
}

GraphReportSceneItem::GraphReportSceneItem(GraphReportItem* pItem, ResponseCollection const& collection, int iSelectedBundle,
                                           QGraphicsItem* pParent)
    : ReportSceneItem(pItem, pParent)
    , mCollection(collection)
    , mISelectedBundle(iSelectedBundle)
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
    // Constants
    QColor const kGridColor(200, 200, 200);

    // Retrieve the item
    GraphReportItem* pItem = (GraphReportItem*) mpItem;

    // Clear the plot
    mpPlot->clear();

    // Add the plottables
    if (!mCollection.isEmpty())
    {
        ResponseBundle const& refBundle = mISelectedBundle >= 0 ? mCollection.get(mISelectedBundle) : mCollection.get(0);
        switch (pItem->subType)
        {
        case GraphReportItem::kReal:
        case GraphReportItem::kImag:
            processReIm(refBundle);
            break;
        case GraphReportItem::kMultiReal:
        case GraphReportItem::kMultiImag:
            processMultiReIm();
            break;
        case GraphReportItem::kModeshape:
            processModeshape(refBundle);
            break;
        default:
            break;
        }
    }

    // Set the legend visibility
    bool isPlottables = mpPlot->plottableCount() > 0;
    mpPlot->legend->setVisible(isPlottables);

    // Set the axes labels
    mpPlot->xAxis->setTickLabelFont(pItem->font);
    mpPlot->yAxis->setTickLabelFont(pItem->font);
    mpPlot->xAxis->setLabelFont(pItem->font);
    mpPlot->yAxis->setLabelFont(pItem->font);
    mpPlot->xAxis->setLabel(pItem->xLabel);
    mpPlot->yAxis->setLabel(pItem->yLabel);

    // Set the grid options
    QPen gridPen = QPen(kGridColor, pItem->gridWidth, Qt::DotLine);
    mpPlot->xAxis->grid()->setPen(gridPen);
    mpPlot->yAxis->grid()->setPen(gridPen);
    mpPlot->xAxis->grid()->setZeroLinePen(gridPen);
    mpPlot->yAxis->grid()->setZeroLinePen(gridPen);

    // Set the axes range
    mpPlot->rescaleAxes();
    if (isPlottables)
    {
        mpPlot->xAxis->scaleRange(pItem->scaleRange);
        mpPlot->yAxis->scaleRange(pItem->scaleRange);
    }

    // Set the axes ticks
    if (pItem->numTicks > 0)
    {
        mpPlot->xAxis->ticker()->setTickCount(pItem->numTicks);
        mpPlot->yAxis->ticker()->setTickCount(pItem->numTicks);
        mpPlot->xAxis->ticker()->setTickStepStrategy(QCPAxisTicker::tssMeetTickCount);
        mpPlot->yAxis->ticker()->setTickStepStrategy(QCPAxisTicker::tssMeetTickCount);
    }
    else
    {
        mpPlot->xAxis->ticker()->setTickStepStrategy(QCPAxisTicker::tssReadability);
        mpPlot->yAxis->ticker()->setTickStepStrategy(QCPAxisTicker::tssReadability);
    }

    // Render the plot
    mpPlot->replot();
}

//! Process the item of the real (imag) subtype
void GraphReportSceneItem::processReIm(ResponseBundle const& bundle)
{
    GraphReportItem* pItem = (GraphReportItem*) mpItem;

    // Loop through all the curves
    int numCurves = pItem->curves.size();
    for (int iCurve = 0; iCurve != numCurves; ++iCurve)
    {
        GraphReportCurve const& curve = pItem->curves[iCurve];

        // Loop through all the points belonged to the curve
        int numPoints = curve.points.size();
        for (int iPoint = 0; iPoint != numPoints; ++iPoint)
        {
            // Find the response associated with the point which has the same direction
            GraphReportPoint const& point = curve.points[iPoint];
            int iResponse = findResponse(bundle, point, pItem->responseDir, Testlab::ResponseType::kAccel);
            if (iResponse < 0)
                continue;
            Testlab::Response const& response = bundle.responses[iResponse];

            // Set the data
            QList<double> xData = convert(response.keys);
            QList<double> yData = pItem->subType == GraphReportItem::kReal ? convert(response.realValues) : convert(response.imagValues);
            if (xData.isEmpty() || xData.size() != yData.size())
                continue;

            // Add the plottable
            addPlottable(xData, yData, curve, point.node);
        }
    }

    // Display the bundle frequency
    if (pItem->showBundleFreq)
    {
        QCPItemStraightLine* pLine = new QCPItemStraightLine(mpPlot);
        pLine->setPen(QPen(Qt::black, 1.0, Qt::DashLine));
        pLine->point1->setCoords(bundle.freq, 0);
        pLine->point2->setCoords(bundle.freq, 1);
    }
}

//! Process the item of the multi real (imag) subtype
void GraphReportSceneItem::processMultiReIm()
{
    // TODO
}

//! Process the item of the modeshape subtype
void GraphReportSceneItem::processModeshape(ResponseBundle const& bundle)
{
    // TODO
}

//! Add the plottable to the plot
void GraphReportSceneItem::addPlottable(QList<double> const& xData, QList<double> const& yData, GraphReportCurve const& curve,
                                        QString const& name)
{
    // Define the style
    QPen pen(curve.lineColor, curve.lineWidth, curve.penStyle);
    QCPScatterStyle style((QCPScatterStyle::ScatterShape) curve.markerShape, curve.markerSize);
    QString label = name.isEmpty() ? curve.name : name;

    // Create the plottable
    QCPCurve* pPlottable = new QCPCurve(mpPlot->xAxis, mpPlot->yAxis);
    pPlottable->setData(xData, yData);
    pPlottable->setPen(pen);
    pPlottable->setName(label);
    pPlottable->setScatterStyle(style);
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
    // Constants
    qreal const kInchToMM = 25.4;

    // Get the dimensions
    double dpi = pPainter->device()->logicalDpiX();
    auto mmToPx = [dpi, kInchToMM](double mm) { return mm * dpi / kInchToMM; };
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
    pos -= QPoint(mmToPx(mpItem->rect.center().x()), mmToPx(mpItem->rect.center().y()));

    // Render the picture
    pPainter->save();
    pPainter->translate(mpItem->rect.center());
    pPainter->rotate(mpItem->angle);
    pPainter->scale(scaleFactor, scaleFactor);
    pPainter->drawPicture(pos, pic);
    pPainter->restore();
}

//! Draw the plot as the svg image
void GraphReportSceneItem::drawAsImage(QPainter* pPainter, QSize const& size)
{
    // Create the buffer
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);

    // Render to the buffer
    renderToBuffer(buffer, size);
    QSvgRenderer renderer(byteArray);

    // Copy the result to the painter
    pPainter->save();
    renderer.render(pPainter, mpItem->rect);
    pPainter->restore();
}

//! Render the plot to a svg file
void GraphReportSceneItem::renderToSvg(QString const& pathFile, QSize const& size)
{
    // Set up the generator
    QSvgGenerator generator;
    generator.setFileName(pathFile);
    generator.setSize(QSize(size.width(), size.height()));
    generator.setViewBox(QRect(0, 0, size.width(), size.height()));

    // Draw the content
    QCPPainter painter;
    painter.begin(&generator);
    mpPlot->toPainter(&painter, size.width(), size.height());
    painter.end();
}

//! Render the  plot to a buffer
void GraphReportSceneItem::renderToBuffer(QBuffer& buffer, QSize const& size)
{
    // Set up the generator
    QSvgGenerator generator;
    generator.setOutputDevice(&buffer);
    generator.setSize(QSize(size.width(), size.height()));
    generator.setViewBox(QRect(0, 0, size.width(), size.height()));

    // Draw the content
    QCPPainter painter;
    painter.begin(&generator);
    mpPlot->toPainter(&painter, size.width(), size.height());
    painter.end();
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

//! Helper function to convert double data
QVector<double> convert(std::vector<double> const& data)
{
    return QVector<double>(data.begin(), data.end());
}

//! Helper function to find the response measured at the specified point along the requested direction
int findResponse(ResponseBundle const& bundle, GraphReportPoint const& point, ReportDirection dir, Testlab::ResponseType type)
{
    int iFound = -1;
    int numResponses = bundle.responses.size();
    for (int i = 0; i != numResponses; ++i)
    {
        Testlab::Response const& response = bundle.responses[i];

        // Slice the response data
        Testlab::ResponsePoint const& responsePoint = response.header.point;
        QString componentName = QString::fromStdWString(responsePoint.component);
        QString nodeName = QString::fromStdWString(responsePoint.node);

        // Check the flags
        bool isPoint = componentName == point.component && nodeName == point.node;
        bool isDir = (int) dir == (int) responsePoint.direction;
        bool isType = response.header.type == type;
        if (isPoint && isDir && isType)
            return i;
    }
    return iFound;
}
