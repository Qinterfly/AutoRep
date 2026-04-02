#include "reportdocument.h"

using namespace Backend::Core;

ReportPage::ReportPage(QPageSize const& uSize, QString const& uName)
    : size(uSize)
    , name(uName)
{
}

ReportPage::ReportPage(ReportPage&& another)
{
    *this = std::move(another);
}

ReportPage& ReportPage::operator=(ReportPage&& another)
{
    if (this == &another)
        return *this;
    clear();
    size = std::move(another.size);
    name = std::move(another.name);
    mItems = std::move(another.mItems);
    another.mItems.clear();
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

ReportItem::ReportItem()
    : rect(0, 0, 0, 0)
    , font("Times New Roman", 12)
{
}

TextReportItem::TextReportItem()
{
    alignment = Qt::AlignHCenter | Qt::AlignVCenter;
}

ReportItem::Type TextReportItem::type() const
{
    return ReportItem::kText;
}

GraphReportItem::GraphReportItem()
{
}

ReportItem::Type GraphReportItem::type() const
{
    return ReportItem::kGraph;
}
