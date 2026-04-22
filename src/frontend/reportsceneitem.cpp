#include <complex.h>
#include <testlab/common.h>
#include <QSvgGenerator>
#include <QSvgRenderer>

#include "customplot.h"
#include "reportdocument.h"
#include "reportitem.h"
#include "reportsceneitem.h"
#include "reportdefaults.h"
#include "reporttextengine.h"
#include "session.h"
#include "uiconstants.h"
#include "uiutility.h"

using namespace Backend::Core;
using namespace Frontend;
using namespace Constants;

static double const skEps = std::numeric_limits<double>::epsilon();
static double const skInf = std::numeric_limits<double>::infinity();

// Helper function
QFont sceneFont(ReportItem* pItem);
QRectF rotatedRect(QRectF const& rect, qreal angle);
QVector<double> convert(std::vector<double> const& data);
int findResponse(ResponseBundle const& bundle, GraphReportPoint const& point, ReportDirection dir, Testlab::ResponseType type);
Testlab::Response getAcceleration(ResponseBundle const& bundle, GraphReportPoint const& point, GraphReportItem* pItem);
int findClosestKey(Testlab::Response const& response, double searchKey);
std::vector<double> getCoords(Testlab::Geometry const& geometry, GraphReportPoint const& point);
QString getDirLabel(ReportDirection dir);

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

Qt::Alignment TextReportSceneItem::textAlignment() const
{
    TextReportItem* pItem = (TextReportItem*) mpItem;
    return pItem->alignment;
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
    pPainter->drawText(mpItem->rect, textAlignment(), processedText());
    pPainter->restore();
}

GraphReportSceneItem::GraphReportSceneItem(GraphReportItem* pItem, ReportTextEngine& textEngine, ResponseCollection const& collection,
                                           int iSelectedBundle, Testlab::Geometry const& geometry, QGraphicsItem* pParent)
    : ReportSceneItem(pItem, pParent)
    , mTextEngine(textEngine)
    , mCollection(collection)
    , mISelectedBundle(iSelectedBundle)
    , mGeometry(geometry)
    , mpPlot(new CustomPlot)
{
    setState();
}

GraphReportSceneItem::~GraphReportSceneItem()
{
    delete mpPlot;
}

//! Get Y-axis range
QPair<double, double> GraphReportSceneItem::yRange()
{
    auto [pXAxis, pYAxis] = axes();
    QCPRange range = pYAxis->range();
    return {range.lower, range.upper};
}

//! Set Y-axis range
void GraphReportSceneItem::setYRange(double lower, double upper)
{
    auto [pXAxis, pYAxis] = axes();
    pYAxis->setRangeLower(lower);
    pYAxis->setRangeUpper(upper);
    mpPlot->replot();
    update();
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
        case GraphReportItem::kFreqAmp:
            processFreqAmp();
            break;
        case GraphReportItem::kModeshape:
            processModeshape(refBundle);
            break;
        default:
            break;
        }
    }

    // Update the parser
    mTextEngine.setVariable("unit", pItem->unit);
    mTextEngine.setVariable("cdir", getDirLabel(pItem->coordDir));
    mTextEngine.setVariable("rdir", getDirLabel(pItem->responseDir));

    // Set the legend
    bool isPlottables = mpPlot->plottableCount() > 0;
    mpPlot->legend->setVisible(pItem->showLegend && isPlottables);
    mpPlot->legend->setFont(pItem->font);
    mpPlot->setLegendAlignment(pItem->legendAlign);

    // Get the axes
    auto [pXAxis, pYAxis] = axes();

    // Set the axes labels
    pXAxis->setTickLabelFont(pItem->font);
    pYAxis->setTickLabelFont(pItem->font);
    pXAxis->setLabelFont(pItem->font);
    pYAxis->setLabelFont(pItem->font);
    pXAxis->setLabel(mTextEngine.process(pItem->xLabel));
    pYAxis->setLabel(mTextEngine.process(pItem->yLabel));

    // Set the grid options
    QPen gridPen = QPen(kGridColor, pItem->gridWidth, Qt::DotLine);
    pXAxis->grid()->setPen(gridPen);
    pYAxis->grid()->setPen(gridPen);
    pXAxis->grid()->setZeroLinePen(gridPen);
    pYAxis->grid()->setZeroLinePen(gridPen);

    // Set the axes range
    mpPlot->rescaleAxes();
    if (std::abs(pItem->xRange.second - pItem->xRange.first) > skEps)
        pXAxis->setRange(pItem->xRange.first, pItem->xRange.second);
    if (std::abs(pItem->yRange.second - pItem->yRange.first) > skEps)
        pYAxis->setRange(pItem->yRange.first, pItem->yRange.second);
    if (isPlottables)
    {
        pXAxis->scaleRange(pItem->scaleRange);
        pYAxis->scaleRange(pItem->scaleRange);
    }

    // Set the axes ticks
    if (pItem->numTicks > 0)
    {
        pXAxis->ticker()->setTickCount(pItem->numTicks);
        pYAxis->ticker()->setTickCount(pItem->numTicks);
        pXAxis->ticker()->setTickStepStrategy(QCPAxisTicker::tssMeetTickCount);
        pYAxis->ticker()->setTickStepStrategy(QCPAxisTicker::tssMeetTickCount);
    }
    else
    {
        pXAxis->ticker()->setTickStepStrategy(QCPAxisTicker::tssReadability);
        pYAxis->ticker()->setTickStepStrategy(QCPAxisTicker::tssReadability);
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
            // Get the response which has the requested unit and direction
            GraphReportPoint const& point = curve.points[iPoint];
            Testlab::Response response = getAcceleration(bundle, point, pItem);

            // Set the data
            QList<double> xData = convert(response.keys);
            QList<double> yData = pItem->subType == GraphReportItem::kReal ? convert(response.realValues) : convert(response.imagValues);
            if (xData.isEmpty() || xData.size() != yData.size())
                continue;
            if (pItem->swapAxes)
                std::swap(xData, yData);

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
    QList<GraphReportCurve> const kDefaultCurves = ReportDefaults::curves();

    GraphReportItem* pItem = (GraphReportItem*) mpItem;

    // Process only the first curve
    if (pItem->curves.isEmpty())
        return;
    GraphReportCurve const& baseCurve = pItem->curves.first();

    // Process only the first curve point
    if (baseCurve.points.isEmpty())
        return;

    // Set the variable
    GraphReportPoint const& point = baseCurve.points.first();
    mTextEngine.setVariable("point", point.name());

    // Loop through all the bundles
    int numBundles = mCollection.count();
    for (int iBundle = 0; iBundle != numBundles; ++iBundle)
    {
        ResponseBundle const& bundle = mCollection.get(iBundle);

        // Get the response which has the requested unit and direction
        Testlab::Response response = getAcceleration(bundle, point, pItem);

        // Set the data
        QList<double> xData = convert(response.keys);
        QList<double> yData = pItem->subType == GraphReportItem::kMultiReal ? convert(response.realValues) : convert(response.imagValues);
        if (xData.isEmpty() || xData.size() != yData.size())
            continue;
        if (pItem->swapAxes)
            std::swap(xData, yData);

        // Set the curve for plotting
        int iDefaultCurve = Utility::getRepeatedIndex(iBundle, kDefaultCurves.size());
        GraphReportCurve currentCurve = kDefaultCurves[iDefaultCurve];
        currentCurve.points = {point};
        currentCurve.lineStyle = baseCurve.lineStyle;
        currentCurve.lineWidth = baseCurve.lineWidth;
        currentCurve.markerSize = baseCurve.markerSize;

        // Add the plottable
        QString name = tr("F = %1 N").arg(QString::number(bundle.force));
        addPlottable(xData, yData, currentCurve, name);
    }
}

//! Process the item of the freq amplitude subtype
void GraphReportSceneItem::processFreqAmp()
{
    GraphReportItem* pItem = (GraphReportItem*) mpItem;

    // Loop through all the curves
    int numCurves = pItem->curves.size();
    int numBundles = mCollection.count();
    for (int iCurve = 0; iCurve != numCurves; ++iCurve)
    {
        GraphReportCurve const& curve = pItem->curves[iCurve];

        // Process only the first point
        if (curve.isEmpty())
            continue;
        GraphReportPoint const& point = curve.points.first();

        // Loop through all the bundles
        QList<double> xData(numBundles, 0.0);
        QList<double> yData(numBundles, 0.0);
        for (int iBundle = 0; iBundle != numBundles; ++iBundle)
        {
            ResponseBundle const& bundle = mCollection.get(iBundle);
            if (bundle.freq < skEps)
                continue;

            // Get the response which has the requested unit and direction
            Testlab::Response response = getAcceleration(bundle, point, pItem);
            if (response.keys.size() == 0)
                continue;

            // Find the closest frequency to the resonance one
            int iFound = findClosestKey(response, bundle.freq);
            if (iFound < 0)
                continue;

            // Store the resonance frequency and response value
            double re = response.realValues[iFound];
            double im = response.imagValues[iFound];
            xData[iBundle] = response.keys[iFound];
            yData[iBundle] = std::sqrt(std::pow(re, 2.0) + std::pow(im, 2.0));
        }

        // Add the curve
        if (pItem->swapAxes)
            std::swap(xData, yData);
        addPlottable(xData, yData, curve, point.node);
    }
}

//! Process the item of the modeshape subtype
void GraphReportSceneItem::processModeshape(ResponseBundle const& bundle)
{
    GraphReportItem* pItem = (GraphReportItem*) mpItem;

    // Check if the resonance frequency specified for the bundle
    if (bundle.freq < skEps)
    {
        qWarning() << tr("Resonance frequency for bundle %1 is zero");
        return;
    }

    // Check if the coordinates direction is specified
    if (pItem->coordDir == ReportDirection::kNone)
    {
        qWarning() << tr("Coordinate direction is not specified for a modeshape");
        return;
    }

    // Loop through all the curves
    int numCurves = pItem->curves.size();
    for (int iCurve = 0; iCurve != numCurves; ++iCurve)
    {
        GraphReportCurve const& curve = pItem->curves[iCurve];
        if (curve.isEmpty())
            continue;
        int numPoints = curve.points.size();

        // Loop through all the points
        QList<double> xData(numPoints, 0.0);
        QList<double> yData(numPoints, 0.0);
        for (int iPoint = 0; iPoint != numPoints; ++iPoint)
        {
            GraphReportPoint const& point = curve.points[iPoint];

            // Get the response which has the requested unit and direction
            Testlab::Response response = getAcceleration(bundle, point, pItem);
            if (response.keys.size() == 0)
                continue;

            // Find the closest frequency to the resonance one
            int iFound = findClosestKey(response, bundle.freq);
            if (iFound < 0)
                continue;

            // Get the point coordinates
            std::vector<double> coords = getCoords(mGeometry, point);
            if (coords.empty())
                continue;

            // Set the data
            xData[iPoint] = coords[(int) pItem->coordDir - 1];
            yData[iPoint] = response.imagValues[iFound] * response.header.point.sign;

            // Add the variable
            QString varName = QString("%1:%2").arg(point.name(), getDirLabel(pItem->responseDir));
            QString varValue = QString::number(yData[iPoint], 'f', 3);
            mTextEngine.setVariable(varName, varValue);
        }

        // Add the curve
        if (pItem->swapAxes)
            std::swap(xData, yData);
        addPlottable(xData, yData, curve, curve.name);
    }
}

//! Add the plottable to the plot
void GraphReportSceneItem::addPlottable(QList<double> const& xData, QList<double> const& yData, GraphReportCurve const& curve,
                                        QString const& name)
{
    // Define the style
    QPen pen(curve.lineColor, curve.lineWidth, curve.lineStyle);
    QCPScatterStyle scatterStyle((QCPScatterStyle::ScatterShape) curve.markerShape, curve.markerSize);
    if (curve.markerFill)
        scatterStyle.setBrush(curve.lineColor);
    QString label = name.isEmpty() ? curve.name : name;

    // Modify the style, so that the markers are visible when the curve is not
    auto lineStyle = QCPCurve::lsLine;
    if (pen.style() == Qt::NoPen)
    {
        lineStyle = QCPCurve::lsNone;
        pen.setStyle(Qt::SolidLine);
    }

    // Create the plottable
    QCPCurve* pPlottable = new QCPCurve(mpPlot->xAxis, mpPlot->yAxis);
    pPlottable->setData(xData, yData);
    pPlottable->setLineStyle(lineStyle);
    pPlottable->setPen(pen);
    pPlottable->setName(label);
    pPlottable->setScatterStyle(scatterStyle);
    pPlottable->setScatterSkip(curve.markerSkip);
}

//! Retrieve the axes, taking into account the swap flag
QPair<QCPAxis*, QCPAxis*> GraphReportSceneItem::axes()
{
    GraphReportItem* pItem = (GraphReportItem*) mpItem;
    QCPAxis* pXAxis = mpPlot->xAxis;
    QCPAxis* pYAxis = mpPlot->yAxis;
    if (pItem->swapAxes)
        std::swap(pXAxis, pYAxis);
    return {pXAxis, pYAxis};
}

//! Process paint event
void GraphReportSceneItem::paint(QPainter* pPainter, QStyleOptionGraphicsItem const* pOption, QWidget* pWidget)
{
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

//! Helper function to retrieve acceleration response
Testlab::Response getAcceleration(ResponseBundle const& bundle, GraphReportPoint const& point, GraphReportItem* pItem)
{
    // Find the acceleration response
    Testlab::Response null;
    int iResponse = findResponse(bundle, point, pItem->responseDir, Testlab::ResponseType::kAccel);
    if (iResponse < 0)
        return null;
    Testlab::Response const accel = bundle.responses[iResponse];

    // Check if the response has the requested unit or the units are not set
    QString targetUnit = pItem->unit;
    QString unit = QString::fromStdWString(accel.header.unit.name);
    if (unit.isEmpty() || targetUnit.isEmpty() || unit == targetUnit)
        return accel;

    // Build up all the possible units as to choose from them later on
    QMap<QString, Testlab::Response> responseSet;
    responseSet[unit] = accel;

    // Process the force, if presented
    int numKeys = accel.keys.size();
    int iForce = findResponse(bundle, point, pItem->responseDir, Testlab::ResponseType::kForce);
    if (iForce >= 0)
    {
        Testlab::Response const force = bundle.responses[iForce];
        Testlab::Response response = accel;
        bool isFRF = unit == Units::skM_S2_N;
        for (int i = 0; i != numKeys; ++i)
        {
            std::complex<double> a = {accel.realValues[i], accel.imagValues[i]};
            std::complex<double> F = {force.realValues[i], force.imagValues[i]};
            std::complex<double> r = {0.0, 0.0};
            if (isFRF)
                r = a * F;
            else if (std::abs(F) > skEps)
                r = a / F;
            response.realValues[i] = r.real();
            response.imagValues[i] = r.imag();
        }
        if (isFRF)
            responseSet[Units::skM_S2] = response;
        else
            responseSet[Units::skM_S2_N] = response;
    }

    // Double integrate to compute displacements
    if (responseSet.contains(Units::skM_S2))
    {
        Testlab::Response response = responseSet[Units::skM_S2];
        for (int i = 0; i != numKeys; ++i)
        {
            std::complex<double> a = {response.realValues[i], response.imagValues[i]};
            std::complex<double> r = {0.0, 0.0};
            if (std::abs(response.keys[i]) > skEps)
                r = -a / std::pow(2.0 * M_PI * response.keys[i], 2.0);
            response.realValues[i] = r.real();
            response.imagValues[i] = r.imag();
        }
        responseSet[Units::skM] = response;
    }

    // Return the result
    if (responseSet.contains(targetUnit))
        return responseSet[targetUnit];
    return null;
}

//! Helper function find closest key to the requested one
int findClosestKey(Testlab::Response const& response, double searchKey)
{
    int iFound = -1;
    double minDist = skInf;
    int numKeys = response.keys.size();
    for (int iKey = 0; iKey != numKeys; ++iKey)
    {
        double dist = std::abs(response.keys[iKey] - searchKey);
        if (dist < minDist)
        {
            minDist = dist;
            iFound = iKey;
        }
    }
    return iFound;
}

//! Helper function to get point location
std::vector<double> getCoords(Testlab::Geometry const& geometry, GraphReportPoint const& point)
{
    int numComponents = geometry.components.size();
    for (int iComponent = 0; iComponent != numComponents; ++iComponent)
    {
        Testlab::Component const& component = geometry.components[iComponent];
        QString componentName = QString::fromStdWString(component.name);
        if (componentName != point.component)
            continue;
        int numNodes = component.nodes.size();
        for (int iNode = 0; iNode != numNodes; ++iNode)
        {
            Testlab::Node const& node = component.nodes[iNode];
            QString nodeName = QString::fromStdWString(node.name);
            if (nodeName == point.node)
                return node.coordinates;
        }
    }
    return {};
}

//! Get direction label
QString getDirLabel(ReportDirection dir)
{
    switch (dir)
    {
    case ReportDirection::kX:
        return "X";
    case ReportDirection::kY:
        return "Y";
    case ReportDirection::kZ:
        return "Z";
    default:
        break;
    }
    return QString();
}
