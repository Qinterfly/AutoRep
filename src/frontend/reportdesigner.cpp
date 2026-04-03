#include <QGroupBox>
#include <QListWidget>
#include <QPrinter>
#include <QVBoxLayout>
#include <QWheelEvent>

#include "reportdesigner.h"
#include "reportdocument.h"
#include "reportsceneitem.h"

using namespace Backend::Core;
using namespace Frontend;

ReportDesigner::ReportDesigner(ReportPage& page, QWidget* pParent)
    : QWidget(pParent)
    , mPage(page)
{
    createContent();
    paint();
}

//! Render the page content
void ReportDesigner::paint()
{
    // Set the scene
    mpScene->clear();
    mpScene->setSceneRect(mPage.size.rect(QPageSize::Millimeter));

    // Set the view
    mpView->resetTransform();
    mpView->fitToPage();

    // Draw all the items
    int numItems = mPage.count();
    for (int i = 0; i != numItems; ++i)
    {
        ReportItem* pItem = mPage.get(i);
        QGraphicsItem* pSceneItem = nullptr;
        switch (pItem->type())
        {
        case ReportItem::kText:
            pSceneItem = new TextReportSceneItem((TextReportItem*) pItem);
            break;
        case ReportItem::kGraph:
            pSceneItem = new GraphReportSceneItem((GraphReportItem*) pItem);
            break;
        default:
            break;
        }
        if (pSceneItem)
            mpScene->addItem(pSceneItem);
    }

    // Draw the border
    drawBorder();
}

//! Print the page content to a pdf file
bool ReportDesigner::print(QPrinter& printer)
{
    // Set up the printer
    printer.setPageSize(mPage.size);
    printer.setFullPage(true);
    printer.newPage();

    // Set the view
    mpView->fitToPage();

    // Print
    QPainter painter;
    if (!painter.begin(&printer))
        return false;
    mpScene->render(&painter);
    painter.end();

    return true;
}

//! Draw the border around the page
void ReportDesigner::drawBorder()
{
    QGraphicsRectItem* pBorder = mpScene->addRect(mpScene->sceneRect(), QPen(Qt::black, 0), QBrush(Qt::white));
    pBorder->setZValue(-1000);
    pBorder->setFlag(QGraphicsItem::ItemIsSelectable, false);
    pBorder->setFlag(QGraphicsItem::ItemIsMovable, false);
    mpView->setBackgroundBrush(QColor(210, 210, 210));
}

//! Create all the widgets
void ReportDesigner::createContent()
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
QGroupBox* ReportDesigner::createSceneGroupBox()
{
    // Create the widgets
    mpScene = new QGraphicsScene;
    mpView = new ReportSceneView;

    // Initialize the view
    mpView->setScene(mpScene);
    mpView->setRenderHint(QPainter::Antialiasing);
    mpView->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    mpView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    mpView->setResizeAnchor(QGraphicsView::AnchorUnderMouse);

    // Construct the group box
    QGroupBox* pGroupBox = new QGroupBox(tr("Layout"));
    QVBoxLayout* pLayout = new QVBoxLayout;
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->addWidget(mpView);
    pGroupBox->setLayout(pLayout);
    return pGroupBox;
}

//! Create the group of item widgets
QGroupBox* ReportDesigner::createItemGroupBox()
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
QGroupBox* ReportDesigner::createPropertyGroupBox()
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

ReportSceneView::ReportSceneView(QWidget* pParent)
    : QGraphicsView(pParent)
{
}

//! Fit page to width/height
void ReportSceneView::fitToPage()
{
    if (!scene())
        return;
    resetTransform();
    fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
}

//! Enable scrolling and zooming
void ReportSceneView::wheelEvent(QWheelEvent* pEvent)
{
    double const kScaleFactor = 1.15;
    if (pEvent->angleDelta().y() > 0)
        scale(kScaleFactor, kScaleFactor);
    else
        scale(1.0 / kScaleFactor, 1.0 / kScaleFactor);
}
