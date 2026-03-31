#include "reportdocument.h"
#include "reportelement.h"

using namespace Frontend;

ReportDocument::ReportDocument()
{
}

ReportDocument::ReportDocument(QString const& name)
    : mName(name)
{
}

ReportDocument::~ReportDocument()
{
    clear();
}

QString const& ReportDocument::name() const
{
    return mName;
}

void ReportDocument::setName(QString const& name)
{
    mName = name;
}

void ReportDocument::addElement(IReportElement* pElement, ReportPosition const& position)
{
    mContent[position] = pElement;
}

bool ReportDocument::removeElement(IReportElement* pElement)
{
    ReportPosition position = findElement(pElement);
    if (!position.isValid())
        return false;
    delete mContent[position];
    mContent.remove(position);
    return true;
}

ReportPosition ReportDocument::findElement(IReportElement* pElement)
{
    for (auto [key, value] : mContent.asKeyValueRange())
    {
        if (value == pElement)
            return key;
    }
    return ReportPosition();
}

IReportElement* ReportDocument::takeElement(ReportPosition const& position)
{
    if (!mContent.contains(position))
        return nullptr;
    IReportElement* result = mContent[position];
    mContent.remove(position);
    return result;
}

void ReportDocument::clear()
{
    QList<IReportElement*> elements = mContent.values();
    int numElements = elements.size();
    for (int i = 0; i != numElements; ++i)
        delete elements[i];
    mContent.clear();
}

ReportPosition::ReportPosition()
    : iRow(-1)
    , iCol(-1)
    , rowSpan(1)
    , colSpan(1)
{
}

ReportPosition::ReportPosition(int uiRow, int uiCol, int uRowSpan, int uColSpan)
    : iRow(uiRow)
    , iCol(uiCol)
    , rowSpan(uRowSpan)
    , colSpan(uColSpan)
{
}

bool ReportPosition::isValid() const
{
    return iRow >= 0 && iCol >= 0;
}

bool ReportPosition::operator==(ReportPosition const& another) const
{
    return std::tie(iRow, iCol) == std::tie(another.iRow, another.iCol);
}

bool ReportPosition::operator!=(ReportPosition const& another) const
{
    return !(*this == another);
}

bool ReportPosition::operator<(ReportPosition const& another) const
{
    return std::tie(iRow, iCol) < std::tie(another.iRow, another.iCol);
}

bool ReportPosition::operator>(ReportPosition const& another) const
{
    return !(*this < another);
}

bool ReportPosition::operator<=(ReportPosition const& another) const
{
    return *this < another || *this == another;
}

bool ReportPosition::operator>=(ReportPosition const& another) const
{
    return *this > another || *this == another;
}
