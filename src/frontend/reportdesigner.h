#ifndef REPORTDESIGNER_H
#define REPORTDESIGNER_H

#include <QGraphicsView>
#include <QWidget>

QT_FORWARD_DECLARE_CLASS(QListWidget)
QT_FORWARD_DECLARE_CLASS(QGroupBox)
QT_FORWARD_DECLARE_CLASS(QPrinter)

namespace Backend::Core
{
class ReportPage;
class ReportItem;
}

namespace Frontend
{

class ReportSceneView;
class ReportPropertyEditor;

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
    QGroupBox* createSceneGroupBox();
    QGroupBox* createListGroupBox();
    QGroupBox* createEditorGroupBox();

    // Draw
    void drawAll();
    void drawItems();
    void drawBorder();

    // Refresh
    void refreshList();
    void refreshEditor();

    // Slots
    void selectByList();
    void selectByScene();

private:
    Backend::Core::ReportPage& mPage;
    QGraphicsScene* mpScene;
    ReportSceneView* mpView;
    QListWidget* mpItemList;
    ReportPropertyEditor* mpItemEditor;
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
