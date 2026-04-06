#include "reportdocument.h"

using namespace Backend::Core;

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

ReportItem::ReportItem()
    : name(QString())
    , rect(0, 0, 0, 0)
    , angle(0)
    , font("Times New Roman", 12)
{
}

ReportItem::ReportItem(ReportItem const* pAnother)
{
    name = pAnother->name;
    rect = pAnother->rect;
    angle = pAnother->angle;
    font = pAnother->font;
}

TextReportItem::TextReportItem()
{
    alignment = Qt::AlignHCenter | Qt::AlignVCenter;
}

TextReportItem::TextReportItem(ReportItem const* pAnother)
    : ReportItem(pAnother)
{
}

ReportItem::Type TextReportItem::type() const
{
    return ReportItem::kText;
}

ReportItem* TextReportItem::clone() const
{
    TextReportItem* pResult = new TextReportItem(this);
    pResult->alignment = alignment;
    pResult->text = text;
    return pResult;
}

GraphReportPoint::GraphReportPoint()
{
}

GraphReportPoint::GraphReportPoint(QString const& uComponent, QString const& uNode, QString const& uName)
    : component(uComponent)
    , node(uNode)
    , name(uName)
{
}

GraphReportCurve::GraphReportCurve()
{
}

GraphReportItem::GraphReportItem()
    : subType(kNone)
{
}

GraphReportItem::GraphReportItem(ReportItem const* pAnother)
    : ReportItem(pAnother)
{
}

ReportItem::Type GraphReportItem::type() const
{
    return ReportItem::kGraph;
}

ReportItem* GraphReportItem::clone() const
{
    GraphReportItem* pResult = new GraphReportItem(this);
    pResult->subType = subType;
    pResult->coordDir = coordDir;
    pResult->responseDir = responseDir;
    pResult->curves = curves;
    pResult->xLabel = xLabel;
    pResult->yLabel = yLabel;
    return pResult;
}
