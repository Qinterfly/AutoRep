#include <QGroupBox>
#include <QListWidget>
#include <QPrinter>
#include <QSvgGenerator>
#include <QSvgRenderer>
#include <QVBoxLayout>
#include <QWheelEvent>

#include "customplot.h"
#include "reportdocument.h"
#include "reportpageeditor.h"

using namespace Backend::Core;
using namespace Frontend;

ReportPageEditor::ReportPageEditor(ReportPage& page, QWidget* pParent)
    : QWidget(pParent)
    , mPage(page)
{
    createContent();
    paint();
}

//! Render the page content
void ReportPageEditor::paint()
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
bool ReportPageEditor::print(QPrinter& printer)
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
void ReportPageEditor::drawBorder()
{
    QGraphicsRectItem* pBorder = mpScene->addRect(mpScene->sceneRect(), QPen(Qt::black, 0), QBrush(Qt::white));
    pBorder->setZValue(-1000);
    pBorder->setFlag(QGraphicsItem::ItemIsSelectable, false);
    pBorder->setFlag(QGraphicsItem::ItemIsMovable, false);
}

//! Create all the widgets
void ReportPageEditor::createContent()
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
QGroupBox* ReportPageEditor::createSceneGroupBox()
{
    // Create the widgets
    mpScene = new QGraphicsScene;
    mpView = new ReportGraphicsView;

    // Initialize the view
    mpView->setScene(mpScene);
    mpView->setRenderHint(QPainter::Antialiasing);
    mpView->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    mpView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    mpView->setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    mpView->setBackgroundBrush(QColor(210, 210, 210));

    // Construct the group box
    QGroupBox* pGroupBox = new QGroupBox(tr("Layout"));
    QVBoxLayout* pLayout = new QVBoxLayout;
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->addWidget(mpView);
    pGroupBox->setLayout(pLayout);
    return pGroupBox;
}

//! Create the group of item widgets
QGroupBox* ReportPageEditor::createItemGroupBox()
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
QGroupBox* ReportPageEditor::createPropertyGroupBox()
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

ReportGraphicsView::ReportGraphicsView(QWidget* pParent)
    : QGraphicsView(pParent)
{
}

void ReportGraphicsView::fitToPage()
{
    if (!scene())
        return;
    resetTransform();
    fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
}

//! Enable zooming
void ReportGraphicsView::wheelEvent(QWheelEvent* pEvent)
{
    double const kScaleFactor = 1.15;
    if (pEvent->angleDelta().y() > 0)
        scale(kScaleFactor, kScaleFactor);
    else
        scale(1.0 / kScaleFactor, 1.0 / kScaleFactor);
}

TextReportSceneItem::TextReportSceneItem(Backend::Core::TextReportItem* pItem)
    : mpItem(pItem)
{
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
}

QRectF TextReportSceneItem::boundingRect() const
{
    return mpItem->rect;
}

void TextReportSceneItem::paint(QPainter* pPainter, QStyleOptionGraphicsItem const* pOption, QWidget* pWidget)
{
    qreal kPointFactor = 4.0;
    pPainter->save();
    QFont f = mpItem->font;
    f.setPointSizeF(f.pointSize() / kPointFactor);
    pPainter->setFont(f);
    pPainter->drawText(mpItem->rect, mpItem->alignment, mpItem->text);
    pPainter->restore();
}

QVariant TextReportSceneItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if (change == ItemPositionChange)
    {
        QPoint newPos = value.toPoint();
        mpItem->rect.moveTo(newPos);
    }
    return QGraphicsItem::itemChange(change, value);
}

GraphReportSceneItem::GraphReportSceneItem(Backend::Core::GraphReportItem* pItem)
    : mpItem(pItem)
    , mpPlot(new CustomPlot)
{
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
}

GraphReportSceneItem::~GraphReportSceneItem()
{
    delete mpPlot;
}

//! Set the plot
void GraphReportSceneItem::refresh()
{
    mpPlot->xAxis->setTickLabelFont(mpItem->font);
    mpPlot->yAxis->setTickLabelFont(mpItem->font);
    mpPlot->xAxis->setLabelFont(mpItem->font);
    mpPlot->yAxis->setLabelFont(mpItem->font);
    mpPlot->xAxis->setLabel(mpItem->xLabel);
    mpPlot->yAxis->setLabel(mpItem->yLabel);
    mpPlot->replot();
}

QRectF GraphReportSceneItem::boundingRect() const
{
    return mpItem->rect;
}

//! Render plot content
void GraphReportSceneItem::paint(QPainter* pPainter, QStyleOptionGraphicsItem const* pOption, QWidget* pWidget)
{
    // Update the plot
    refresh();

    // Get the dimensions
    double dpi = pPainter->device()->logicalDpiX();
    auto mmToPx = [dpi](double mm) { return mm * dpi / 25.4; };
    QSize pxSize(mmToPx(mpItem->rect.width()), mmToPx(mpItem->rect.height()));

    // Create the picture
    QPicture pic;
    QCPPainter qcpPainter;
    qcpPainter.begin(&pic);
    mpPlot->toPainter(&qcpPainter, pxSize.width(), pxSize.height());
    qcpPainter.end();

    // Compute the transformation parameters
    QSize finalSize = pic.boundingRect().size();
    finalSize.scale(QSize(mpItem->rect.width(), mpItem->rect.height()), Qt::KeepAspectRatio);
    double scaleFactor = finalSize.width() / (double) pic.boundingRect().size().width();
    QPoint pos(mmToPx(mpItem->rect.x()), mmToPx(mpItem->rect.y()));

    // Render the picture
    pPainter->save();
    pPainter->scale(scaleFactor, scaleFactor);
    pPainter->drawPicture(pos, pic);
    pPainter->restore();
}

//! Process item change
QVariant GraphReportSceneItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if (change == ItemPositionChange)
    {
        QPoint newPos = value.toPoint();
        mpItem->rect.moveTo(newPos);
    }
    return QGraphicsItem::itemChange(change, value);
}

//! Render the plot to a svg file
void GraphReportSceneItem::renderToSvg(QString const& pathFile, QSize const& size)
{
    // Set up the generator
    QSvgGenerator generator;
    generator.setFileName(pathFile);
    generator.setSize(QSize(size.width(), size.height()));
    generator.setViewBox(QRect(0, 0, size.width(), size.height()));

    // Draw the content
    QCPPainter painter;
    painter.begin(&generator);
    mpPlot->toPainter(&painter, size.width(), size.height());
    painter.end();
}

//! Render the  plot to a buffer
void GraphReportSceneItem::renderToBuffer(QBuffer& buffer, QSize const& size)
{
    // Set up the generator
    QSvgGenerator generator;
    generator.setOutputDevice(&buffer);
    generator.setSize(QSize(size.width(), size.height()));
    generator.setViewBox(QRect(0, 0, size.width(), size.height()));

    // Draw the content
    QCPPainter painter;
    painter.begin(&generator);
    mpPlot->toPainter(&painter, size.width(), size.height());
    painter.end();
}
