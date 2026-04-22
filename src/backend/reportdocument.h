#ifndef REPORTDOCUMENT_H
#define REPORTDOCUMENT_H

#include <QList>
#include <QPageLayout>

#include "reporttextengine.h"

namespace Backend::Core
{

class ReportItem;
class GraphReportCurve;

//! Class to define report page layout
class ReportPage : public ISerializable
{
public:
    ReportPage();
    ReportPage(QString const& uName);
    ReportPage(ReportPage const& another);
    ~ReportPage();

    ReportPage& operator=(ReportPage const& another);

    int count() const;
    ReportItem* get(int index);
    ReportItem const* get(int index) const;
    ReportItem* get(QString const& name);
    ReportItem* get(QUuid const& id);
    void add(ReportItem* pItem);
    bool remove(ReportItem* pItem);
    void swap(int iFirst, int iSecond);
    int find(ReportItem* pItem) const;
    ReportItem* take(int index);
    void clear();

    QJsonObject toJson() const override;
    void fromJson(QJsonObject const& obj) override;

public:
    QPageLayout layout;
    QString name;

private:
    QList<ReportItem*> mItems;
};

//! Collection of report pages
class ReportDocument : public ISerializable
{
public:
    ReportDocument();
    ~ReportDocument() = default;

    static QString fileVersion();
    static QString fileSuffix();
    QJsonObject toJson() const override;
    void fromJson(QJsonObject const& obj) override;
    bool read(QString const& pathFile);
    bool write(QString const& pathFile) const;

public:
    QString name;
    QList<ReportPage> pages;
    ReportTextEngine textEngine;
};
}

#endif // REPORTDOCUMENT_H
