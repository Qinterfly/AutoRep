#ifndef REPORTDESIGNER_H
#define REPORTDESIGNER_H

#include <QGraphicsView>
#include <QTextEdit>
#include <QWidget>

#include "customtable.h"
#include "reportdocument.h"
#include "reportitem.h"
#include "reporttextengine.h"

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
class ReportSceneItem;
class ReportTextEditor;
class ReportTableEditor;

//! Designer options
struct ReportDesignerOptions
{
    ReportDesignerOptions();
    ~ReportDesignerOptions() = default;

    // Flags
    bool lockItems;
    bool uniteModeshapeRange;
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
    bool print(QPrinter& printer, QPainter& painter);
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
    void duplicateSelectedItems();
    void removeSelectedItems();
    void moveSelectedItems(int iShift);
    void selectByList();
    void selectByScene();
    void changeItemByList(QListWidgetItem* pListItem);
    void setScaleBySelector();
    void changePageOrientation();
    void setDataEditor(Backend::Core::ReportItem* pItem);
    void processEditItemRequest(ReportSceneItem* pSceneItem);

    // Helper
    void updateTextEngine();
    void resolveItemLinks();
    void setUniteModeshapeRange();

private:
    QSettings& mSettings;
    GeometryView* mpGeometryView;
    ResponseEditor* mpResponseEditor;
    Backend::Core::ReportPage& mPage;
    ReportDesignerOptions mOptions;
    Backend::Core::ReportTextEngine mTextEngine;

    // Scene
    QGraphicsScene* mpScene;
    ReportSceneView* mpSceneView;
    ReportTextEditor* mpTextEditor;
    ReportTableEditor* mpTableEditor;
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
    void zoomIn();
    void zoomOut();

protected:
    void wheelEvent(QWheelEvent* pEvent) override;

private:
    double mZoomFactor;
};

//! Class to edit report text items
class ReportTextEditor : public QTextEdit
{
    Q_OBJECT

public:
    ReportTextEditor(QWidget* pParent = nullptr);
    virtual ~ReportTextEditor() = default;

    void startEditing(QRect const& rect, Backend::Core::TextReportItem* pItem);

signals:
    void editingFinished();

protected:
    void focusOutEvent(QFocusEvent* pEvent) override;
    void keyPressEvent(QKeyEvent* pEvent) override;

private:
    Backend::Core::TextReportItem* mpItem;
};

//! Class to edit report table items
class ReportTableEditor : public CustomTable
{
    Q_OBJECT

public:
    ReportTableEditor(QWidget* pParent = nullptr);
    virtual ~ReportTableEditor() = default;

    void startEditing(QRect const& rect, Backend::Core::TableReportItem* pItem);

signals:
    void editingFinished();

protected:
    void focusOutEvent(QFocusEvent* pEvent) override;
    void keyPressEvent(QKeyEvent* pEvent) override;

private:
    Backend::Core::TableReportItem* mpItem;
};
}

#endif // REPORTDESIGNER_H
