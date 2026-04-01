#ifndef REPORTDOCUMENT_H
#define REPORTDOCUMENT_H

#include <QList>
#include <QMap>
#include <QPageSize>

#include "reportitem.h"

namespace Frontend
{

class ReportPage
{
public:
    ReportPage(QPageSize const& uSize = QPageSize::A4);
    ReportPage(ReportPage const& another);
    ~ReportPage();

    ReportPage& operator=(ReportPage const& another);

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
    QList<ReportPage> pages;
};

}

#endif // REPORTDOCUMENT_H
