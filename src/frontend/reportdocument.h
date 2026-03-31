#ifndef REPORTDOCUMENT_H
#define REPORTDOCUMENT_H

#include <QList>
#include <QMap>

namespace Frontend
{

class IReportElement;
struct ReportPosition;

class ReportDocument
{
public:
    ReportDocument();
    ReportDocument(QString const& name);
    ~ReportDocument();

    QString const& name() const;
    void setName(QString const& name);

    // Elements
    void addElement(IReportElement* pElement, ReportPosition const& position);
    bool removeElement(IReportElement* pElement);
    ReportPosition findElement(IReportElement* pElement);
    IReportElement* takeElement(ReportPosition const& position);
    void clear();

private:
    QString mName;
    QMap<ReportPosition, IReportElement*> mContent;
};

struct ReportPosition
{
    ReportPosition();
    ReportPosition(int uiRow, int uiCol, int uRowSpan = 1, int uColSpan = 1);
    ~ReportPosition() = default;

    bool isValid() const;

    bool operator==(ReportPosition const& another) const;
    bool operator!=(ReportPosition const& another) const;
    bool operator<(ReportPosition const& another) const;
    bool operator>(ReportPosition const& another) const;
    bool operator<=(ReportPosition const& another) const;
    bool operator>=(ReportPosition const& another) const;

    int iRow;
    int iCol;
    int rowSpan;
    int colSpan;
};
}

#endif // REPORTDOCUMENT_H
