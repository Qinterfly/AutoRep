#include "reportdocument.h"
#include "reportitem.h"

using namespace Backend::Core;

// Helper functions
QList<GraphReportCurve> generateDefaultCurves();

ReportPage::ReportPage(QPageSize const& uSize, QString const& uName)
    : size(uSize)
    , name(uName)
{
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
    size = another.size;
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

ReportDocument::ReportDocument()
{
}
