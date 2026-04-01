#include "reportdocument.h"
#include "reportitem.h"

using namespace Frontend;

ReportPage::ReportPage(QPageSize const& uSize)
    : size(uSize)
{
}

ReportPage::ReportPage(ReportPage const& another)
{
    *this = another;
}

ReportPage::~ReportPage()
{
    clear();
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

int ReportPage::find(ReportItem* pItem)
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
