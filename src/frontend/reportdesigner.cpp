#include <QGroupBox>
#include <QListWidget>
#include <QPrinter>
#include <QSplitter>
#include <QVBoxLayout>
#include <QWheelEvent>

#include "reportdesigner.h"
#include "reportdocument.h"
#include "reportsceneitem.h"
#include "uiutility.h"

using namespace Backend::Core;
using namespace Frontend;

// Helper functions
QListWidgetItem* createListItem(ReportItem const* pItem, int index);

ReportDesigner::ReportDesigner(ReportPage& page, QWidget* pParent)
    : QWidget(pParent)
    , mPage(page)
{
    setFont(Utility::getFont());
    createContent();
    refresh();
}

//! Fit the content
void ReportDesigner::fit()
{
    mpView->resetTransform();
    mpView->fitToPage();
}

//! Render the page content
void ReportDesigner::refresh()
{
    drawScene();
    drawBorder();
    refreshItems();
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

    // Print to the file
    QPainter painter;
    if (!painter.begin(&printer))
        return false;
    mpScene->render(&painter);
    painter.end();

    return true;
}

//! Draw the scene
void ReportDesigner::drawScene()
{
    // Set the scene
    mpScene->clear();
    mpScene->setSceneRect(mPage.size.rect(QPageSize::Millimeter));

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

//! Update the list of page items
void ReportDesigner::refreshItems()
{
    QSignalBlocker blocker(mpItemList);
    int iSelected = mpItemList->currentRow();
    mpItemList->clear();
    int numItems = mPage.count();
    for (int i = 0; i != numItems; ++i)
    {
        ReportItem* pReportItem = mPage.get(i);
        QListWidgetItem* pListItem = createListItem(pReportItem, i);
        mpItemList->addItem(pListItem);
    }
    if (iSelected < 0 || iSelected >= numItems)
        iSelected = numItems - 1;
    mpItemList->setCurrentRow(iSelected);
}

//! Create all the widgets
void ReportDesigner::createContent()
{
    // Create the control layout
    QVBoxLayout* pControlLayout = new QVBoxLayout;
    pControlLayout->addWidget(createItemGroupBox());
    pControlLayout->addWidget(createPropertyGroupBox());
    pControlLayout->setStretch(1, 1);
    QWidget* pControlWidget = new QWidget;
    pControlWidget->setLayout(pControlLayout);

    // Combine the widgets
    QSplitter* pSplitter = new QSplitter(Qt::Horizontal);
    pSplitter->addWidget(createSceneGroupBox());
    pSplitter->addWidget(pControlWidget);
    pSplitter->setHandleWidth(3);
    pSplitter->setStretchFactor(0, 2);
    pSplitter->setStretchFactor(1, 1);

    // Create the main layout
    QVBoxLayout* pMainLayout = new QVBoxLayout;
    pMainLayout->addWidget(pSplitter);
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
    mpItemList->setFont(font());
    mpItemList->setSelectionMode(QAbstractItemView::SingleSelection);
    mpItemList->setResizeMode(QListWidget::Adjust);
    mpItemList->setSizeAdjustPolicy(QListWidget::AdjustToContents);

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

//! Helper function to create a list item
QListWidgetItem* createListItem(ReportItem const* pItem, int index)
{
    QString prefix;
    QIcon icon;
    switch (pItem->type())
    {
    case ReportItem::kText:
        icon = QIcon(":/icons/item-text.svg");
        prefix = QObject::tr("Text");
        break;
    case ReportItem::kGraph:
        icon = QIcon(":/icons/item-graph.svg");
        prefix = QObject::tr("Graph");
        break;
    default:
        break;
    }
    QString name = pItem->name;
    if (name.isEmpty())
        name = QObject::tr("%1 %2").arg(prefix).arg(1 + index);
    QListWidgetItem* pResult = new QListWidgetItem(icon, name);
    return pResult;
}
