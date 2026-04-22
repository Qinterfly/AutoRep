#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "fileutility.h"
#include "reportdocument.h"
#include "reportitem.h"

using namespace Backend::Core;

ReportPage::ReportPage()
{
    layout.setPageSize(QPageSize::A4);
    layout.setMargins(QMargins(0, 0, 0, 0));
    layout.setUnits(QPageLayout::Millimeter);
    layout.setOrientation(QPageLayout::Portrait);
}

ReportPage::ReportPage(QString const& uName)
    : ReportPage()
{
    name = uName;
}

ReportPage::ReportPage(ReportPage const& another)
{
    *this = another;
}

ReportPage& ReportPage::operator=(ReportPage const& another)
{
    if (this == &another)
        return *this;
    clear();
    layout = another.layout;
    name = another.name;
    int numItems = another.mItems.size();
    mItems.resize(numItems);
    for (int i = 0; i != numItems; ++i)
        mItems[i] = another.mItems[i]->clone();
    return *this;
}

ReportPage::~ReportPage()
{
    clear();
}

int ReportPage::count() const
{
    return mItems.size();
}

ReportItem* ReportPage::get(int index)
{
    if (index >= 0 && index < mItems.size())
        return mItems[index];
    return nullptr;
}

ReportItem const* ReportPage::get(int index) const
{
    if (index >= 0 && index < mItems.size())
        return mItems[index];
    return nullptr;
}

ReportItem* ReportPage::get(QString const& name)
{
    int numItems = mItems.size();
    for (int i = 0; i != numItems; ++i)
    {
        ReportItem* pItem = mItems[i];
        if (pItem->name == name)
            return pItem;
    }
    return nullptr;
}

ReportItem* ReportPage::get(QUuid const& id)
{
    int numItems = mItems.size();
    for (int i = 0; i != numItems; ++i)
    {
        ReportItem* pItem = mItems[i];
        if (pItem->id == id)
            return pItem;
    }
    return nullptr;
}

void ReportPage::add(ReportItem* pItem)
{
    mItems.push_back(pItem);
}

bool ReportPage::remove(ReportItem* pItem)
{
    int index = find(pItem);
    if (index < 0)
        return false;
    delete mItems[index];
    mItems.remove(index);
    return true;
}

void ReportPage::swap(int iFirst, int iSecond)
{
    int numItems = mItems.size();
    bool isFirst = iFirst >= 0 && iFirst < numItems;
    bool isSecond = iSecond >= 0 && iSecond < numItems;
    if (isFirst && isSecond)
        mItems.swapItemsAt(iFirst, iSecond);
}

int ReportPage::find(ReportItem* pItem) const
{
    int numItems = mItems.size();
    for (int i = 0; i != numItems; ++i)
    {
        if (mItems[i] == pItem)
            return i;
    }
    return -1;
}

ReportItem* ReportPage::take(int index)
{
    if (index >= 0 && index < mItems.size())
        return mItems[index];
    return nullptr;
}

void ReportPage::clear()
{
    int numItems = mItems.size();
    for (int i = 0; i != numItems; ++i)
        delete mItems[i];
    mItems.clear();
}

QJsonObject ReportPage::toJson() const
{
    QJsonObject obj;
    obj["layout"] = Utility::toJson(layout);
    obj["name"] = name;
    QJsonArray jsonItems;
    for (auto const& it : mItems)
        jsonItems.push_back(it->toJson());
    obj["items"] = jsonItems;
    return obj;
}

void ReportPage::fromJson(QJsonObject const& obj)
{
    // TODO
}

ReportDocument::ReportDocument()
{
}

QString ReportDocument::fileVersion()
{
    return "0.0.1";
}

QString ReportDocument::fileSuffix()
{
    return "json";
}

QJsonObject ReportDocument::toJson() const
{
    QJsonObject obj;
    obj["version"] = fileVersion();
    obj["name"] = name;
    QJsonArray jsonPages;
    for (auto const& p : pages)
        jsonPages.push_back(p.toJson());
    obj["pages"] = jsonPages;
    obj["textEngine"] = textEngine.toJson();
    return obj;
}

void ReportDocument::fromJson(QJsonObject const& obj)
{
    // TODO
}

//! Read a document layout from a file
bool ReportDocument::read(QString const& pathFile)
{
    // TODO
    return true;
}

//! Write a document layout to a file
bool ReportDocument::write(QString const& pathFile) const
{
    // Open file for writing
    auto pFile = Utility::openFile(pathFile, fileSuffix(), QIODevice::WriteOnly);
    if (!pFile)
        return false;

    // Write the document
    QJsonDocument jsonDoc(toJson());
    pFile->write(jsonDoc.toJson());

    return true;
}
