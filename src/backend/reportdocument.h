#ifndef REPORTDOCUMENT_H
#define REPORTDOCUMENT_H

#include <QList>
#include <QPageSize>

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
    ReportItem* get(QString const& name);
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
}

#endif // REPORTDOCUMENT_H
