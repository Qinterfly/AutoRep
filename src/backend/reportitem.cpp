#include "reportitem.h"

using namespace Backend::Core;

ReportItem::ReportItem()
    : name(QString())
    , rect(0, 0, 0, 0)
    , angle(0)
    , font("Times New Roman", 12)
{
}

ReportItem::ReportItem(ReportItem const* pAnother)
{
    name = pAnother->name;
    rect = pAnother->rect;
    angle = pAnother->angle;
    font = pAnother->font;
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
}

GraphReportCurve::GraphReportCurve(QList<GraphReportPoint> const& uPoints, QString const& uName)
    : name(uName)
    , points(uPoints)
{
}

GraphReportCurve::GraphReportCurve(QList<QString> const& uPoints, QString const& uName)
    : name(uName)
{
    int numPoints = uPoints.size();
    points.resize(numPoints);
    for (int i = 0; i != numPoints; ++i)
        points[i] = GraphReportPoint(uPoints[i]);
}

GraphReportItem::GraphReportItem()
    : subType(kNone)
{
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
    pResult->subType = subType;
    pResult->coordDir = coordDir;
    pResult->responseDir = responseDir;
    pResult->unit = unit;
    pResult->curves = curves;
    pResult->xLabel = xLabel;
    pResult->yLabel = yLabel;
    return pResult;
}

void GraphReportItem::addCurve(QStringList const& points, QString const& name)
{
    curves.push_back(GraphReportCurve(points, name));
}

void GraphReportItem::addPoint(QString const& point, QString const& name)
{
    addCurve({point}, name);
}
