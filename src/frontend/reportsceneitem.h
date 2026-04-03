#ifndef REPORTSCENEITEM_H
#define REPORTSCENEITEM_H

#include <QGraphicsItem>

namespace Backend::Core
{
class ReportItem;
class TextReportItem;
class GraphReportItem;
}

namespace Frontend
{

class CustomPlot;

//! Class to provide base interactions for report scene items
class ReportSceneItem : public QGraphicsItem
{
public:
    ReportSceneItem(QGraphicsItem* pParent = nullptr);
    virtual ~ReportSceneItem() = default;

    void paint(QPainter* pPainter, QStyleOptionGraphicsItem const* pOption, QWidget* pWidget) override;

private:
    void drawBorder(QPainter* pPainter);
};

//! Class to render and manipulate text objects
class TextReportSceneItem : public ReportSceneItem
{
public:
    TextReportSceneItem(Backend::Core::TextReportItem* pItem, QGraphicsItem* pParent = nullptr);
    virtual ~TextReportSceneItem() = default;

protected:
    QRectF boundingRect() const override;
    void paint(QPainter* pPainter, QStyleOptionGraphicsItem const* pOption, QWidget* pWidget) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

private:
    void drawText(QPainter* pPainter);

private:
    Backend::Core::TextReportItem* mpItem;
};

//! Class to render graphs
class GraphReportSceneItem : public ReportSceneItem
{
public:
    GraphReportSceneItem(Backend::Core::GraphReportItem* pItem, QGraphicsItem* pParent = nullptr);
    virtual ~GraphReportSceneItem();

    void refresh();

protected:
    QRectF boundingRect() const override;
    void paint(QPainter* pPainter, QStyleOptionGraphicsItem const* pOption, QWidget* pWidget) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

private:
    void drawPlot(QPainter* pPainter);

private:
    Backend::Core::GraphReportItem* mpItem;
    CustomPlot* mpPlot;
};

}

#endif // REPORTSCENEITEM_H
