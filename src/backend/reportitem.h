#ifndef REPORTITEM_H
#define REPORTITEM_H

#include <QColor>
#include <QFont>
#include <QList>
#include <QRect>

namespace Backend::Core
{

// Common types
enum ReportDirection
{
    kNone,
    kX,
    kY,
    kZ
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
    QString lineStyle;
    double lineWidth;
    QColor lineColor;
    QString markerShape;
    int markerSize;
    QColor markerColor;
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
    void addCurve(QStringList const& points, QString const& name = QString());
    void addPoint(QString const& point, QString const& name = QString());

public:
    SubType subType;
    ReportDirection coordDir;
    ReportDirection responseDir;
    QString unit;
    QList<GraphReportCurve> curves;
    QString xLabel;
    QString yLabel;
};
}

#endif // REPORTITEM_H
