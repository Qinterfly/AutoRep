#ifndef REPORTDESIGNER_H
#define REPORTDESIGNER_H

#include <QGraphicsView>
#include <QWidget>

#include "reportdocument.h"
#include "reportitem.h"

QT_FORWARD_DECLARE_CLASS(QListWidget)
QT_FORWARD_DECLARE_CLASS(QPrinter)
QT_FORWARD_DECLARE_CLASS(QListWidgetItem)
QT_FORWARD_DECLARE_CLASS(QComboBox)
QT_FORWARD_DECLARE_CLASS(QSettings)

namespace Frontend
{

class ReportSceneView;
class ReportPropertyEditor;
class ReportDataEditor;
class GeometryView;
class ResponseEditor;

//! Designer options
struct ReportDesignerOptions
{
    ReportDesignerOptions();
    ~ReportDesignerOptions() = default;

    // Flags
    bool lockItems;
};

//! Class to design page content
class ReportDesigner : public QWidget
{
    Q_OBJECT

public:
    ReportDesigner(QSettings& settings, GeometryView* pGeometryView, ResponseEditor* pResponseEditor, Backend::Core::ReportPage& page,
                   ReportDesignerOptions const& options = ReportDesignerOptions(), QWidget* pParent = nullptr);
    virtual ~ReportDesigner() = default;

    Backend::Core::ReportPage& page();
    void fit();
    void refresh();
    bool print(QPrinter& printer);
    bool printDialog();
    void selectItem(int index);

private:
    void createContent();
    void createConnections();
    QWidget* createSceneWidget();
    QWidget* createListWidget();
    QWidget* createEditorWidget();

    // Draw
    void drawAll();
    void drawItems();
    void drawBorder();

    // Refresh
    void refreshList();
    void refreshEditor();

    // Slots
    void addItem(Backend::Core::ReportItem::Type type);
    void removeSelectedItems();
    void moveSelectedItems(int iShift);
    void selectByList();
    void selectByScene();
    void changeItemByList(QListWidgetItem* pListItem);
    void setScaleBySelector();
    void setDataEditor(Backend::Core::ReportItem* pItem);

private:
    QSettings& mSettings;
    GeometryView* mpGeometryView;
    ResponseEditor* mpResponseEditor;
    Backend::Core::ReportPage& mPage;
    ReportDesignerOptions mOptions;

    // Scene
    QGraphicsScene* mpScene;
    ReportSceneView* mpSceneView;
    QComboBox* mpScaleSelector;
    bool mIsPrinting;

    // Item
    QListWidget* mpItemList;

    // Editor
    ReportPropertyEditor* mpPropertyEditor;
    ReportDataEditor* mpDataEditor;
    QWidget* mpDataEditorContainer;

    // Selection
    QList<Backend::Core::ReportItem*> mSelectedItems;
};

//! Class to view scene
class ReportSceneView : public QGraphicsView
{
    Q_OBJECT

public:
    ReportSceneView(QWidget* pParent = nullptr);
    ReportSceneView(QGraphicsScene* pScene, QWidget* pParent = nullptr);
    virtual ~ReportSceneView() = default;

    void fitToPage();

protected:
    void wheelEvent(QWheelEvent* pEvent) override;
};
}

#endif // REPORTDESIGNER_H
