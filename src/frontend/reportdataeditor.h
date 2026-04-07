#ifndef REPORTDATAEDITOR_H
#define REPORTDATAEDITOR_H

#include <QWidget>

#include "reportitem.h"

QT_FORWARD_DECLARE_CLASS(QComboBox)
QT_FORWARD_DECLARE_CLASS(QListWidget)

namespace Frontend
{

class ReportDataEditor : public QWidget
{
public:
    ReportDataEditor(QWidget* pParent = nullptr);
    virtual ~ReportDataEditor() = default;

    virtual Backend::Core::ReportItem::Type type() const = 0;
    virtual void refresh() = 0;
    void setItem(Backend::Core::ReportItem* pItem);

protected:
    Backend::Core::ReportItem* mpItem;
};

class GraphReportDataEditor : public ReportDataEditor
{
    Q_OBJECT

public:
    GraphReportDataEditor(QWidget* pParent = nullptr);
    virtual ~GraphReportDataEditor() = default;

    Backend::Core::ReportItem::Type type() const override;
    void refresh() override;

private:
    void createContent();
    void createConnections();
    QLayout* createHeaderLayout();
    QLayout* createCurveLayout();

private:
    QComboBox* mpSubTypeSelector;
    QComboBox* mpCoordDirSelector;
    QComboBox* mpResponseDirSelector;
    QComboBox* mpUnitSelector;
    QListWidget* mpCurveList;
    QListWidget* mpPointList;
};
}

#endif // REPORTDATAEDITOR_H
