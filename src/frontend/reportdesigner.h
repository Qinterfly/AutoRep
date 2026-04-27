#ifndef REPORTDESIGNER_H
#define REPORTDESIGNER_H

#include <QGraphicsView>
#include <QTextEdit>
#include <QWidget>

#include "customplot.h"
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
class GraphReportSceneItem;
class ReportTextEditor;
class ReportGraphEditor;
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
                   Backend::Core::ReportTextEngine& textEngine, ReportDesignerOptions const& options = ReportDesignerOptions(),
                   QWidget* pParent = nullptr);
    virtual ~ReportDesigner() = default;

    Backend::Core::ReportPage& page();
    void fit();
    void refresh();
    bool print(QPrinter& printer, QPainter& painter);
    bool printDialog();
    void selectItem(int index);

signals:
    void edited();

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
    void processSceneItemChanged();
    void processItemEdited();
    void processEditItemRequest(ReportSceneItem* pSceneItem);

    // Helper
    Backend::Core::ReportItemGetter createItemGetter(QUuid const& id);
    void updateTextEngine();
    void resolveItemLinks();
    void setUniteModeshapeRange();

private:
    QSettings& mSettings;
    GeometryView* mpGeometryView;
    ResponseEditor* mpResponseEditor;
    Backend::Core::ReportPage& mPage;
    Backend::Core::ReportTextEngine& mTextEngine;
    ReportDesignerOptions mOptions;

    // Scene
    Backend::Core::ReportPage mScenePage;
    QGraphicsScene* mpScene;
    ReportSceneView* mpSceneView;
    QUndoStack* mpSceneUndoStack;
    ReportTextEditor* mpTextEditor;
    ReportGraphEditor* mpGraphEditor;
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
    QList<QUuid> mSelectedItemIDs;
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

//! Class to edit report graph items
class ReportGraphEditor : public CustomPlot
{
    Q_OBJECT

public:
    ReportGraphEditor(QWidget* pParent = nullptr);
    virtual ~ReportGraphEditor() = default;

    void startEditing(QRect const& rect, GraphReportSceneItem* pItem);

signals:
    void editingFinished();

protected:
    void focusOutEvent(QFocusEvent* pEvent) override;
    void keyPressEvent(QKeyEvent* pEvent) override;

private:
    GraphReportSceneItem* mpItem;
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

//! Class to edit page
class EditPage : public QUndoCommand
{
public:
    EditPage(Backend::Core::ReportPage& page, Backend::Core::ReportPage const& value);
    virtual ~EditPage() = default;

    void undo() override;
    void redo() override;

private:
    Backend::Core::ReportPage& mPage;
    Backend::Core::ReportPage mOldValue;
    Backend::Core::ReportPage mNewValue;
};
}

#endif // REPORTDESIGNER_H
