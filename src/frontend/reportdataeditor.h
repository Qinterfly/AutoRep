#ifndef REPORTDATAEDITOR_H
#define REPORTDATAEDITOR_H

#include <QWidget>

#include "reportitem.h"

QT_FORWARD_DECLARE_CLASS(QComboBox)
QT_FORWARD_DECLARE_CLASS(QListWidget)

namespace Frontend
{

class GeometryView;

class ReportDataEditor : public QWidget
{
    Q_OBJECT

public:
    ReportDataEditor(QWidget* pParent = nullptr);
    virtual ~ReportDataEditor() = default;

    virtual Backend::Core::ReportItem::Type type() const = 0;
    virtual void refresh() = 0;
    void setItem(Backend::Core::ReportItem* pItem);

signals:
    void edited();

protected:
    Backend::Core::ReportItem* mpItem;
};

class GraphReportDataEditor : public ReportDataEditor
{
    Q_OBJECT

public:
    GraphReportDataEditor(GeometryView* pGeometryView, QWidget* pParent = nullptr);
    virtual ~GraphReportDataEditor() = default;

    Backend::Core::ReportItem::Type type() const override;
    void refresh() override;

    void addCurve();
    void editSelectedCurve();
    void renameSelectedCurve();
    void removeSelectedCurve();
    QList<Backend::Core::GraphReportPoint> getSelectedPoints();

private:
    void createContent();
    void createConnections();
    QLayout* createHeaderLayout();
    QWidget* createToolBar();
    QLayout* createCurveLayout();

    // Slots
    void processCurveSelected();
    void processHeaderChanged();

private:
    GeometryView* mpGeometryView;
    QComboBox* mpSubTypeSelector;
    QComboBox* mpCoordDirSelector;
    QComboBox* mpResponseDirSelector;
    QComboBox* mpUnitSelector;
    QListWidget* mpCurveList;
    QListWidget* mpPointList;
};
}

#endif // REPORTDATAEDITOR_H
