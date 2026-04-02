#ifndef REPORTPAGEEDITOR_H
#define REPORTPAGEEDITOR_H

#include <QGraphicsItem>
#include <QGraphicsView>
#include <QSvgRenderer>
#include <QWidget>

QT_FORWARD_DECLARE_CLASS(QListWidget)
QT_FORWARD_DECLARE_CLASS(QGroupBox)
QT_FORWARD_DECLARE_CLASS(QPrinter)
QT_FORWARD_DECLARE_CLASS(QBuffer)

namespace Backend::Core
{
class ReportPage;
class TextReportItem;
class GraphReportItem;
}

namespace Frontend
{

class CustomPlot;
class ReportGraphicsView;

//! Class to render page content
class ReportPageEditor : public QWidget
{
    Q_OBJECT

public:
    ReportPageEditor(Backend::Core::ReportPage& page, QWidget* pParent = nullptr);
    virtual ~ReportPageEditor() = default;

    void paint();
    bool print(QPrinter& printer);

private:
    void createContent();
    QGroupBox* createSceneGroupBox();
    QGroupBox* createItemGroupBox();
    QGroupBox* createPropertyGroupBox();

    // Draw
    void drawBorder();

private:
    Backend::Core::ReportPage& mPage;
    QGraphicsScene* mpScene;
    ReportGraphicsView* mpView;
    QListWidget* mpItemList;
};

//! Class to view page
class ReportGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    ReportGraphicsView(QWidget* pParent = nullptr);
    ReportGraphicsView(QGraphicsScene* pScene, QWidget* pParent = nullptr);
    virtual ~ReportGraphicsView() = default;

    void fitToPage();

protected:
    void wheelEvent(QWheelEvent* pEvent) override;
};

class TextReportSceneItem : public QGraphicsItem
{
public:
    TextReportSceneItem(Backend::Core::TextReportItem* pItem);
    virtual ~TextReportSceneItem() = default;

    QRectF boundingRect() const override;
    void paint(QPainter* pPainter, QStyleOptionGraphicsItem const* pOption, QWidget* pWidget) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

private:
    Backend::Core::TextReportItem* mpItem;
};

class GraphReportSceneItem : public QGraphicsItem
{
public:
    GraphReportSceneItem(Backend::Core::GraphReportItem* pItem);
    virtual ~GraphReportSceneItem();

    void refresh();

    QRectF boundingRect() const override;
    void paint(QPainter* pPainter, QStyleOptionGraphicsItem const* pOption, QWidget* pWidget) override;
    void print(QPainter* pPainter);
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

private:
    void renderToSvg(QString const& pathFile, QSize const& size);
    void renderToBuffer(QBuffer& buffer, QSize const& size);

private:
    Backend::Core::GraphReportItem* mpItem;
    CustomPlot* mpPlot;
};
}

#endif // REPORTPAGEEDITOR_H
