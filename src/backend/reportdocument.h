#ifndef REPORTDOCUMENT_H
#define REPORTDOCUMENT_H

#include <QFont>
#include <QList>
#include <QMap>
#include <QPageSize>
#include <QRect>

namespace Backend::Core
{

class ReportItem;

//! Class to define report page layout
class ReportPage
{
public:
    ReportPage(QPageSize const& uSize = QPageSize::A4, QString const& uName = QString());
    ReportPage(ReportPage const& another);
    ~ReportPage();

    ReportPage& operator=(ReportPage const& another);

    int count() const;
    ReportItem* get(int index);
    ReportItem const* get(int index) const;
    void add(ReportItem* pItem);
    bool remove(ReportItem* pItem);
    void swap(int iFirst, int iSecond);
    int find(ReportItem* pItem) const;
    ReportItem* take(int index);
    void clear();

public:
    QPageSize size;
    QString name;

private:
    QList<ReportItem*> mItems;
};

//! Collection of report pages
class ReportDocument
{
public:
    ReportDocument();
    ~ReportDocument() = default;

public:
    QString name;
    QList<ReportPage> pages;
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

enum Direction
{
    kNone,
    kX,
    kY,
    kZ
};

//! Class to a define a layout of a graph point
class GraphReportPoint
{
public:
    GraphReportPoint();
    GraphReportPoint(QString const& uComponent, QString const& uNode, QString const& uName = QString());
    ~GraphReportPoint() = default;

public:
    QString name;
    QString component;
    QString node;
};

//! Class to define a layout of a graph curve
class GraphReportCurve
{
public:
    GraphReportCurve();
    ~GraphReportCurve() = default;

public:
    QString name;
    QList<GraphReportPoint> points;
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

public:
    SubType subType;
    Direction coordDir;
    Direction responseDir;
    QList<GraphReportCurve> curves;
    QString xLabel;
    QString yLabel;
};
}

#endif // REPORTDOCUMENT_H
