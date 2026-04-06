#ifndef REPORTDESIGNER_H
#define REPORTDESIGNER_H

#include <QGraphicsView>
#include <QWidget>

#include "reportdocument.h"

QT_FORWARD_DECLARE_CLASS(QListWidget)
QT_FORWARD_DECLARE_CLASS(QPrinter)
QT_FORWARD_DECLARE_CLASS(QListWidgetItem)
QT_FORWARD_DECLARE_CLASS(QComboBox)

namespace Frontend
{

class ReportSceneView;
class ReportPropertyEditor;
class ReportDataManager;

//! Class to design page content
class ReportDesigner : public QWidget
{
    Q_OBJECT

public:
    ReportDesigner(Backend::Core::ReportPage& page, QWidget* pParent = nullptr);
    virtual ~ReportDesigner() = default;

    void fit();
    void refresh();
    bool print(QPrinter& printer);

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

private:
    Backend::Core::ReportPage& mPage;
    QGraphicsScene* mpScene;
    ReportSceneView* mpView;
    QComboBox* mpScaleSelector;
    QListWidget* mpItemList;
    ReportPropertyEditor* mpPropertyEditor;
    ReportDataManager* mpDataManager;
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
