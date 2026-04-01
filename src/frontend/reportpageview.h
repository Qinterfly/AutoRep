#ifndef REPORTPAGEVIEW_H
#define REPORTPAGEVIEW_H

#include <QGraphicsItem>
#include <QWidget>

#include "reportdocument.h"

QT_FORWARD_DECLARE_CLASS(QGraphicsScene)
QT_FORWARD_DECLARE_CLASS(QGraphicsView)
QT_FORWARD_DECLARE_CLASS(QListWidget)
QT_FORWARD_DECLARE_CLASS(QGroupBox)

namespace Frontend
{

class ReportPageView : public QWidget
{
    Q_OBJECT

public:
    ReportPageView(ReportPage& page, QWidget* pParent = nullptr);
    virtual ~ReportPageView() = default;

    void paint();
    void write(QString const& pathFile);

private:
    void createContent();
    QGroupBox* createSceneGroupBox();
    QGroupBox* createItemGroupBox();
    QGroupBox* createPropertyGroupBox();

private:
    ReportPage& mPage;
    QGraphicsScene* mpScene;
    QGraphicsView* mpView;
    QListWidget* mpItemList;
};

class ReportGraphicsItem : public QGraphicsItem
{
public:
    ReportGraphicsItem(ReportItem* pItem);
    virtual ~ReportGraphicsItem() = default;

    QRectF boundingRect() const override;
    void paint(QPainter* pPainter, QStyleOptionGraphicsItem const* pOption, QWidget* pWidget) override;

private:
    ReportItem* mpItem;
};
}

#endif // REPORTPAGEVIEW_H
