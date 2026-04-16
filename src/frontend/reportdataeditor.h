#ifndef REPORTDATAEDITOR_H
#define REPORTDATAEDITOR_H

#include <QWidget>

#include "reportitem.h"

QT_FORWARD_DECLARE_CLASS(QComboBox)
QT_FORWARD_DECLARE_CLASS(QListWidget)

class QtTreePropertyBrowser;
class CustomVariantPropertyManager;
class QtVariantEditorFactory;
class QtProperty;

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
    void replaceSelectedCurve();
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

class ReportCurvePropertyEditor : public QWidget
{
    Q_OBJECT

public:
    enum Type
    {
        kLineStyle,
        kLineWidth,
        kLineColor,
        kMarkerShape,
        kMarkerSize,
    };

    ReportCurvePropertyEditor(Backend::Core::GraphReportCurve& curve, QWidget* pParent = nullptr);
    virtual ~ReportCurvePropertyEditor() = default;

signals:
    void edited();

protected:
    QSize sizeHint() const override;

private:
    void createContent();
    void createProperties();
    void createConnections();

    void setValue(QtProperty* pProperty, QVariant value);

private:
    Backend::Core::GraphReportCurve& mCurve;
    CustomVariantPropertyManager* mpManager;
    QtVariantEditorFactory* mpFactory;
    QtTreePropertyBrowser* mpEditor;
};
}

#endif // REPORTDATAEDITOR_H
