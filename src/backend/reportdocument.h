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

class ReportPage
{
public:
    ReportPage(QPageSize const& uSize = QPageSize::A4, QString const& uName = QString());
    ReportPage(ReportPage const& another) = delete;
    ReportPage(ReportPage&& another);
    ~ReportPage();

    ReportPage& operator=(ReportPage const& another) = delete;
    ReportPage& operator=(ReportPage&& another);

    int count() const;
    ReportItem* get(int index);
    void add(ReportItem* pItem);
    bool remove(ReportItem* pItem);
    int find(ReportItem* pItem);
    ReportItem* take(int index);
    void clear();

public:
    QPageSize size;
    QString name;

private:
    QList<ReportItem*> mItems;
};

class ReportDocument
{
public:
    ReportDocument();
    ~ReportDocument() = default;

public:
    QString name;
    std::vector<ReportPage> pages;
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
    virtual ~ReportItem() = default;

    virtual Type type() const = 0;

public:
    QRect rect;
    QFont font;
};

class TextReportItem : public ReportItem
{
public:
    TextReportItem();
    virtual ~TextReportItem() = default;

    Type type() const override;

public:
    QFlags<Qt::AlignmentFlag> alignment;
    QString text;
};

class GraphReportItem : public ReportItem
{
public:
    GraphReportItem();
    virtual ~GraphReportItem() = default;

    Type type() const override;

public:
    QString xLabel;
    QString yLabel;
};

}

#endif // REPORTDOCUMENT_H
