#ifndef REPORTITEM_H
#define REPORTITEM_H

#include <QColor>
#include <QFont>
#include <QList>
#include <QRect>

namespace Backend::Core
{

// Common types
enum class ReportDirection
{
    kNone,
    kX,
    kY,
    kZ
};

enum class ReportMarkerShape
{
    kNone,
    kDot,
    kCross,
    kPlus,
    kCircle,
    kDisc,
    kSquare,
    kDiamond,
    kStar,
    kTriangle,
    kTriangleInverted,
    kCrossSquare,
    kPlusSquare,
    kCrossCircle,
    kPlusCircle,
    kPeace
};

//! Base class for items
class ReportItem
{
public:
    enum Type
    {
        kText,
        kGraph
    };
    ReportItem();
    ReportItem(ReportItem const* pAnother);
    virtual ~ReportItem() = default;

    virtual Type type() const = 0;
    virtual ReportItem* clone() const = 0;

public:
    QString name;
    QRect rect;
    double angle;
    QFont font;
};

//! Class to define a layout of a text element
class TextReportItem : public ReportItem
{
public:
    TextReportItem();
    TextReportItem(ReportItem const* pAnother);
    virtual ~TextReportItem() = default;

    Type type() const override;
    ReportItem* clone() const override;

public:
    QFlags<Qt::AlignmentFlag> alignment;
    QString text;
};

//! Class to a define a layout of a graph point
class GraphReportPoint
{
public:
    GraphReportPoint();
    GraphReportPoint(QString const& uName);
    GraphReportPoint(QString const& uComponent, QString const& uNode);
    ~GraphReportPoint() = default;

    QString name() const;

public:
    QString component;
    QString node;
};

//! Class to define a layout of a graph curve
class GraphReportCurve
{
public:
    GraphReportCurve();
    GraphReportCurve(QList<GraphReportPoint> const& uPoints, QString const& uName = QString());
    GraphReportCurve(QList<QString> const& uPoints, QString const& uName = QString());
    ~GraphReportCurve() = default;

    bool isEmpty() const;

public:
    QString name;
    QList<GraphReportPoint> points;
    // Line
    Qt::PenStyle lineStyle;
    double lineWidth;
    QColor lineColor;
    // Marker
    ReportMarkerShape markerShape;
    int markerSize;
};

//! Class to define a layout of a graph element
class GraphReportItem : public ReportItem
{
public:
    enum SubType
    {
        kNone,
        kReal,
        kImag,
        kMultiReal,
        kMultiImag,
        kFreqReal,
        kFreqImag,
        kModeshape
    };
    GraphReportItem();
    GraphReportItem(ReportItem const* pAnother);
    virtual ~GraphReportItem() = default;

    Type type() const override;
    ReportItem* clone() const override;

    bool isMultiPointCurve() const;
    GraphReportCurve& addCurve(QStringList const& points, QString const& name = QString());
    GraphReportCurve& addPoint(QString const& point, QString const& name = QString());

public:
    QList<GraphReportCurve> curves;
    // Header
    SubType subType;
    ReportDirection coordDir;
    ReportDirection responseDir;
    QString unit;
    // Axes
    QString xLabel;
    QString yLabel;
    double scaleRange;
    int numTicks;
    double gridWidth;
    // Flags
    bool showBundleFreq;
};
}

#endif // REPORTITEM_H
