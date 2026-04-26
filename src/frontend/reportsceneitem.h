#ifndef REPORTSCENEITEM_H
#define REPORTSCENEITEM_H

#include <QGraphicsItem>

#include "reporttextengine.h"

QT_FORWARD_DECLARE_CLASS(QBuffer)

namespace Testlab
{
class Geometry;
}

namespace Backend::Core
{
class ReportItem;
class TextReportItem;
class GraphReportItem;
class ResponseCollection;
class ResponseBundle;
class GraphReportCurve;
class ReportDefaults;
class PictureReportItem;
class TableReportItem;
}

class QCPAxis;

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

    int type() const override;
    Backend::Core::ReportItem* item();
    bool isMovable() const;

protected:
    QRectF boundingRect() const override;
    void paint(QPainter* pPainter, QStyleOptionGraphicsItem const* pOption, QWidget* pWidget) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent* pEvent) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* pEvent) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* pEvent) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* pEvent) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* pEvent) override;

signals:
    void changed();
    void requestEdit();

protected:
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
    Mode mMode;
    Handle mHandle;
    QPointF mLastPos;
};

//! Class to render and manipulate text objects
class TextReportSceneItem : public ReportSceneItem
{
    Q_OBJECT

public:
    TextReportSceneItem(Backend::Core::TextReportItem* pItem, Backend::Core::ReportTextEngine& textEngine, QGraphicsItem* pParent = nullptr);
    virtual ~TextReportSceneItem() = default;

    QString rawText() const;
    QString processedText() const;
    Qt::Alignment textAlign() const;

protected:
    void paint(QPainter* pPainter, QStyleOptionGraphicsItem const* pOption, QWidget* pWidget) override;

private:
    void drawText(QPainter* pPainter);

private:
    Backend::Core::ReportTextEngine& mTextEngine;
};

//! Class to render graphs
class GraphReportSceneItem : public ReportSceneItem
{
    Q_OBJECT

public:
    friend class ReportGraphEditor;

    GraphReportSceneItem(Backend::Core::GraphReportItem* pItem, Backend::Core::ReportTextEngine& textEngine,
                         Backend::Core::ResponseCollection const& collection, int iSelectedBundle, Testlab::Geometry const& geometry,
                         QGraphicsItem* pParent = nullptr);
    virtual ~GraphReportSceneItem();

    QPair<double, double> yRange();
    void setYRange(double lower, double upper);

protected:
    void paint(QPainter* pPainter, QStyleOptionGraphicsItem const* pOption, QWidget* pWidget) override;

private:
    void setState();
    void processReIm(Backend::Core::ResponseBundle const& bundle);
    void processMultiReIm();
    void processFreqAmp();
    void processModeshape(Backend::Core::ResponseBundle const& bundle);

    void addPlottable(QList<double> const& xData, QList<double> const& yData, Backend::Core::GraphReportCurve const& curve,
                      QString const& name = QString());
    QPair<QCPAxis*, QCPAxis*> axes();

    // Rendering
    void drawPlot(QPainter* pPainter);
    void drawAsImage(QPainter* pPainter, QSize const& size);
    void renderToSvg(QString const& pathFile, QSize const& size);
    void renderToBuffer(QBuffer& buffer, QSize const& size);

private:
    Backend::Core::ReportTextEngine& mTextEngine;
    Backend::Core::ResponseCollection const& mCollection;
    int const mISelectedBundle;
    Testlab::Geometry const& mGeometry;
    CustomPlot* mpPlot;
};

//! Class to render pictures
class PictureReportSceneItem : public ReportSceneItem
{
    Q_OBJECT

public:
    PictureReportSceneItem(Backend::Core::PictureReportItem* pItem, QGraphicsItem* pParent = nullptr);
    virtual ~PictureReportSceneItem() = default;

protected:
    void paint(QPainter* pPainter, QStyleOptionGraphicsItem const* pOption, QWidget* pWidget) override;
};

//! Class to render tables
class TableReportSceneItem : public ReportSceneItem
{
    Q_OBJECT

public:
    TableReportSceneItem(Backend::Core::TableReportItem* pItem, Backend::Core::ReportTextEngine& textEngine, QGraphicsItem* pParent = nullptr);
    virtual ~TableReportSceneItem() = default;

protected:
    void paint(QPainter* pPainter, QStyleOptionGraphicsItem const* pOption, QWidget* pWidget) override;

private:
    Backend::Core::ReportTextEngine& mTextEngine;
};
}

#endif // REPORTSCENEITEM_H
