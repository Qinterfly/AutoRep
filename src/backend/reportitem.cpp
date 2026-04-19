#include "reportitem.h"

using namespace Backend::Core;

ReportItem::ReportItem()
    : id(QUuid::createUuid())
    , name(QString())
    , rect(0, 0, 0, 0)
    , angle(0)
    , font("Times New Roman", 12)
    , link(QUuid())
{
}

ReportItem::ReportItem(ReportItem const* pAnother)
{
    id = pAnother->id;
    name = pAnother->name;
    rect = pAnother->rect;
    angle = pAnother->angle;
    font = pAnother->font;
    link = pAnother->link;
}

TextReportItem::TextReportItem()
{
    alignment = Qt::AlignHCenter | Qt::AlignVCenter;
}

TextReportItem::TextReportItem(ReportItem const* pAnother)
    : ReportItem(pAnother)
{
}

ReportItem::Type TextReportItem::type() const
{
    return ReportItem::kText;
}

ReportItem* TextReportItem::clone() const
{
    TextReportItem* pResult = new TextReportItem(this);
    pResult->alignment = alignment;
    pResult->text = text;
    return pResult;
}

GraphReportPoint::GraphReportPoint()
{
}

GraphReportPoint::GraphReportPoint(QString const& uName)
{
    QStringList tokens = uName.split(':');
    if (tokens.size() == 2)
    {
        component = tokens[0];
        node = tokens[1];
    }
    else
    {
        node = uName;
    }
}

GraphReportPoint::GraphReportPoint(QString const& uComponent, QString const& uNode)
    : component(uComponent)
    , node(uNode)
{
}

QString GraphReportPoint::name() const
{
    return QString("%1:%2").arg(component, node);
}

GraphReportCurve::GraphReportCurve()
{
    lineStyle = Qt::SolidLine;
    lineWidth = 1.25;
    lineColor = Qt::red;
    markerShape = ReportMarkerShape::kDisc;
    markerSize = 6;
    markerFill = false;
}

GraphReportCurve::GraphReportCurve(QList<GraphReportPoint> const& uPoints, QString const& uName)
    : GraphReportCurve()
{
    name = uName;
    points = uPoints;
}

GraphReportCurve::GraphReportCurve(QList<QString> const& uPoints, QString const& uName)
    : GraphReportCurve()
{
    name = uName;
    int numPoints = uPoints.size();
    points.resize(numPoints);
    for (int i = 0; i != numPoints; ++i)
        points[i] = GraphReportPoint(uPoints[i]);
}

GraphReportCurve::GraphReportCurve(QColor const& uLineColor, ReportMarkerShape const& uMarkerShape, bool uMarkerFill)
    : GraphReportCurve()
{
    lineColor = uLineColor;
    markerShape = uMarkerShape;
    markerFill = uMarkerFill;
}

bool GraphReportCurve::isEmpty() const
{
    return points.isEmpty();
}

GraphReportItem::GraphReportItem()
{
    // Header
    subType = kNone;

    // Axes
    scaleRange = 1.1;
    numTicks = 5;
    gridWidth = 1.0;
    swapAxes = false;
    legendAlignment = Qt::AlignRight | Qt::AlignTop;

    // Flags
    showLegend = true;
    showBundleFreq = false;
}

GraphReportItem::GraphReportItem(ReportItem const* pAnother)
    : ReportItem(pAnother)
{
}

ReportItem::Type GraphReportItem::type() const
{
    return ReportItem::kGraph;
}

ReportItem* GraphReportItem::clone() const
{
    GraphReportItem* pResult = new GraphReportItem(this);
    pResult->curves = curves;

    // Header
    pResult->subType = subType;
    pResult->coordDir = coordDir;
    pResult->responseDir = responseDir;
    pResult->unit = unit;

    // Axes
    pResult->xLabel = xLabel;
    pResult->yLabel = yLabel;
    pResult->scaleRange = scaleRange;
    pResult->numTicks = numTicks;
    pResult->gridWidth = gridWidth;
    pResult->swapAxes = swapAxes;
    pResult->legendAlignment = legendAlignment;

    // Flags
    pResult->showLegend = showLegend;
    pResult->showBundleFreq = showBundleFreq;

    return pResult;
}

bool GraphReportItem::isMultiPointCurve() const
{
    return subType == GraphReportItem::kModeshape;
}

GraphReportCurve& GraphReportItem::addCurve(QStringList const& points, QString const& name)
{
    return curves.emplace_back(GraphReportCurve(points, name));
}

GraphReportCurve& GraphReportItem::addPoint(QString const& point, QString const& name)
{
    return addCurve({point}, name);
}
