#ifndef REPORTSCENEITEM_H
#define REPORTSCENEITEM_H

#include <QGraphicsItem>

#include "reporttextparser.h"

QT_FORWARD_DECLARE_CLASS(QBuffer)

namespace Backend::Core
{
class ReportItem;
class TextReportItem;
class GraphReportItem;
class ResponseCollection;
class ResponseBundle;
class GraphReportCurve;
}

namespace Frontend
{

class CustomPlot;

//! Class to provide base interactions for report scene items
class ReportSceneItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    ReportSceneItem(Backend::Core::ReportItem* pItem, QGraphicsItem* pParent = nullptr);
    virtual ~ReportSceneItem() = default;

    Backend::Core::ReportItem* item();
    bool isMovable() const;

    void setTextParser(Backend::Core::ReportTextParser const& textParser);

    QRectF boundingRect() const override;
    void paint(QPainter* pPainter, QStyleOptionGraphicsItem const* pOption, QWidget* pWidget) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent* pEvent) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* pEvent) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* pEvent) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* pEvent) override;

signals:
    void changed();

private:
    enum class Mode
    {
        kNone,
        kMove,
        kResize
    };
    enum class Handle
    {
        kNone,

        // Corners
        kTopLeft,
        kTopRight,
        kBottomLeft,
        kBottomRight,

        // Edges
        kTop,
        kBottom,
        kLeft,
        kRight
    };
    void drawBorder(QPainter* pPainter);
    void drawHandles(QPainter* pPainter);
    QMap<Handle, QRectF> handles() const;
    Handle detectHandle(QPointF const& pos) const;

protected:
    Backend::Core::ReportItem* mpItem;
    Backend::Core::ReportTextParser mTextParser;

private:
    Mode mMode;
    Handle mHandle;
    QPointF mLastPos;
};

//! Class to render and manipulate text objects
class TextReportSceneItem : public ReportSceneItem
{
public:
    TextReportSceneItem(Backend::Core::TextReportItem* pItem, QGraphicsItem* pParent = nullptr);
    virtual ~TextReportSceneItem() = default;

protected:
    void paint(QPainter* pPainter, QStyleOptionGraphicsItem const* pOption, QWidget* pWidget) override;

private:
    void drawText(QPainter* pPainter);
};

//! Class to render graphs
class GraphReportSceneItem : public ReportSceneItem
{
public:
    GraphReportSceneItem(Backend::Core::GraphReportItem* pItem, Backend::Core::ResponseCollection const& collection, int iSelectedBundle,
                         QGraphicsItem* pParent = nullptr);
    virtual ~GraphReportSceneItem();

protected:
    void paint(QPainter* pPainter, QStyleOptionGraphicsItem const* pOption, QWidget* pWidget) override;

private:
    void setState();
    void processReIm(Backend::Core::ResponseBundle const& bundle);
    void processMultiReIm();
    void processModeshape(Backend::Core::ResponseBundle const& bundle);

    void addPlottable(QList<double> const& xData, QList<double> const& yData, Backend::Core::GraphReportCurve const& curve,
                      QString const& name = QString());

    // Rendering
    void drawPlot(QPainter* pPainter);
    void drawAsImage(QPainter* pPainter, QSize const& size);
    void renderToSvg(QString const& pathFile, QSize const& size);
    void renderToBuffer(QBuffer& buffer, QSize const& size);

private:
    Backend::Core::ResponseCollection const& mCollection;
    int const mISelectedBundle;
    CustomPlot* mpPlot;
};

}

#endif // REPORTSCENEITEM_H
