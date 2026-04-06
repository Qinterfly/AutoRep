#ifndef REPORTDATAEDITOR_H
#define REPORTDATAEDITOR_H

#include <QWidget>

namespace Backend::Core
{
class ReportItem;
}

namespace Frontend
{

class ReportDataEditor : public QWidget
{
public:
    enum Type
    {
        kGraph
    };
    ReportDataEditor(Backend::Core::ReportItem* pItem, QWidget* pParent = nullptr);
    virtual ~ReportDataEditor() = default;

    virtual Type type() const = 0;
    virtual void refresh() = 0;

protected:
    Backend::Core::ReportItem* mpItem;
};

class GraphReportDataEditor : public ReportDataEditor
{
    Q_OBJECT

public:
    GraphReportDataEditor(Backend::Core::ReportItem* pItem, QWidget* pParent = nullptr);
    virtual ~GraphReportDataEditor() = default;

    Type type() const override;
    void refresh() override;
};
}

#endif // REPORTDATAEDITOR_H
