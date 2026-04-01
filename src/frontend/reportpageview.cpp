#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGroupBox>
#include <QListWidget>
#include <QVBoxLayout>

#include "reportpageview.h"

using namespace Frontend;

ReportPageView::ReportPageView(ReportPage& page, QWidget* pParent)
    : QWidget(pParent)
    , mPage(page)
{
    createContent();
    paint();
}

void ReportPageView::paint()
{
    int numItems = mPage.count();
    mpScene->clear();
    for (int i = 0; i != numItems; ++i)
    {
        ReportItem* pItem = mPage.get(i);
        ReportGraphicsItem* pGraphicsItem = new ReportGraphicsItem(pItem);
        mpScene->addItem(pGraphicsItem);
    }
    mpView->show();
}

//! Create all the widgets
void ReportPageView::createContent()
{
    // Create the control layout
    QVBoxLayout* pControlLayout = new QVBoxLayout;
    pControlLayout->addWidget(createItemGroupBox());
    pControlLayout->addWidget(createPropertyGroupBox());

    // Combine the widgets
    QHBoxLayout* pMainLayout = new QHBoxLayout;
    pMainLayout->addWidget(createSceneGroupBox());
    pMainLayout->addLayout(pControlLayout);
    pMainLayout->setStretch(0, 3);
    pMainLayout->setStretch(1, 1);
    setLayout(pMainLayout);
}

//! Create the group of scene widgets
QGroupBox* ReportPageView::createSceneGroupBox()
{
    // Create the widgets
    mpScene = new QGraphicsScene;
    mpScene->setSceneRect(mPage.size.rect(QPageSize::Millimeter));
    mpView = new QGraphicsView(mpScene);
    mpView->fitInView(mpScene->sceneRect(), Qt::KeepAspectRatio);

    // Construct the group box
    QGroupBox* pGroupBox = new QGroupBox(tr("Layout"));
    QVBoxLayout* pLayout = new QVBoxLayout;
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->addWidget(mpView);
    pGroupBox->setLayout(pLayout);
    return pGroupBox;
}

//! Create the group of item widgets
QGroupBox* ReportPageView::createItemGroupBox()
{
    // Create the widgets
    mpItemList = new QListWidget;

    // Construct the group box
    QGroupBox* pGroupBox = new QGroupBox(tr("Items"));
    QVBoxLayout* pLayout = new QVBoxLayout;
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->addWidget(mpItemList);
    pGroupBox->setLayout(pLayout);
    return pGroupBox;
}

//! Create the group of properties
QGroupBox* ReportPageView::createPropertyGroupBox()
{
    // TODO
    // Construct the group box
    QGroupBox* pGroupBox = new QGroupBox(tr("Properties"));
    QVBoxLayout* pLayout = new QVBoxLayout;
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->addWidget(new QWidget);
    pGroupBox->setLayout(pLayout);
    return pGroupBox;
}

ReportGraphicsItem::ReportGraphicsItem(ReportItem* pItem)
    : mpItem(pItem)
{
}

QRectF ReportGraphicsItem::boundingRect() const
{
    return mpItem->rect;
}

void ReportGraphicsItem::paint(QPainter* pPainter, QStyleOptionGraphicsItem const* pOption, QWidget* pWidget)
{
    mpItem->paint(pPainter);
}
