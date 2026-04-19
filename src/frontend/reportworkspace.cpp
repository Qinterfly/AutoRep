#include <QFileDialog>
#include <QPrinter>
#include <QSettings>
#include <QToolBar>
#include <QVBoxLayout>

#include "customtabwidget.h"
#include "reportdesigner.h"
#include "reportworkspace.h"
#include "uiconstants.h"
#include "uiutility.h"

using namespace Frontend;
using namespace Backend::Core;
using namespace Constants;

// Helper function
ReportPage createImRePage();
ReportPage createMultiImRePage();
ReportPage createFreqAmpPage();
ReportPage createModeshapePage();

ReportWorkspace::ReportWorkspace(QSettings& settings, GeometryView* pGeometryView, ResponseEditor* pResponseEditor, QWidget* pParent)
    : QWidget(pParent)
    , mSettings(settings)
    , mpGeometryView(pGeometryView)
    , mpResponseEditor(pResponseEditor)
{
    setFont(Utility::getFont());
    createContent();
    createConnections();
    initialize();
    rebuild();
}

QSize ReportWorkspace::sizeHint() const
{
    return QSize(500, 1000);
}

//! Retrieve the current designer
ReportDesigner* ReportWorkspace::currentDesigner()
{
    return designer(mpDesignerTabs->currentIndex());
}

//! Retrieve the designer associated with the page index
ReportDesigner* ReportWorkspace::designer(int iPage)
{
    if (iPage >= 0 && iPage < mpDesignerTabs->count())
        return (ReportDesigner*) mpDesignerTabs->widget(iPage);
    return nullptr;
}

//! Retrieve the designer associated with the name
ReportDesigner* ReportWorkspace::designer(QString const& name)
{
    int numPages = mpDesignerTabs->count();
    for (int iPage = 0; iPage != numPages; ++iPage)
    {
        if (name == mpDesignerTabs->tabText(iPage))
            return (ReportDesigner*) mpDesignerTabs->widget(iPage);
    }
    return nullptr;
}

//! Print all the pages to a pdf file
bool ReportWorkspace::print(QString const& pathFile)
{
    // Create the printer
    QPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(pathFile);

    // Start the painter
    QPainter painter;
    if (!painter.begin(&printer))
        return false;

    int numPages = mDocument.pages.size();
    for (int iPage = 0; iPage != numPages; ++iPage)
    {
        ReportDesigner* pDesigner = designer(iPage);
        if (!pDesigner)
            continue;
        if (!pDesigner->print(printer, painter))
            return false;

        // Create the next page
        if (iPage != numPages - 1)
            printer.newPage();
    }

    // Close the painter
    painter.end();

    qInfo() << tr("Document is printed to the file %1").arg(pathFile);
    return true;
}

//! Print all the pages to a pdf file using a file dialog
bool ReportWorkspace::printDialog()
{
    // Constants
    QString const kExpectedSuffix = "pdf";

    // Get the file path
    QString pathFile = Utility::getLastPathFile(mSettings);
    Utility::modifyFileSuffix(pathFile, kExpectedSuffix);
    pathFile = QFileDialog::getSaveFileName(this, tr("Print Document"), pathFile, tr("Document file format (*%1)").arg(kExpectedSuffix));
    if (pathFile.isEmpty())
        return false;

    // Modify the suffix, if necessary
    Utility::modifyFileSuffix(pathFile, kExpectedSuffix);

    // Store the path
    Utility::setLastPathFile(mSettings, pathFile);

    return print(pathFile);
}

//! Create all the widgets
void ReportWorkspace::createContent()
{
    // Create the toolbar
    QToolBar* pToolBar = new QToolBar;
    pToolBar->addAction(QIcon(":/icons/document-new.svg"), tr("Default template"), this, &ReportWorkspace::setDefaultDocument);
    pToolBar->addAction(QIcon(":/icons/document-import.svg"), tr("Import template"));
    pToolBar->addAction(QIcon(":/icons/document-export.svg"), tr("Export template"));
    pToolBar->addAction(QIcon(":/icons/document-print.svg"), tr("Print document"), this, &ReportWorkspace::printDialog);
    pToolBar->setIconSize(Constants::Size::skToolBarIcon);
    Utility::setShortcutHints(pToolBar);

    // Create the tab widget
    mpDesignerTabs = new CustomTabWidget;
    mpDesignerTabs->setTabsRenamable(false);
    mpDesignerTabs->setTabsClosable(false);

    // Create the layout
    QVBoxLayout* pLayout = new QVBoxLayout;
    pLayout->addWidget(pToolBar);
    pLayout->addWidget(mpDesignerTabs);
    setLayout(pLayout);
}

void ReportWorkspace::createConnections()
{
    connect(mpDesignerTabs, &CustomTabWidget::currentChanged, this, &ReportWorkspace::processDesignerSelected);
}

//! Initialize the editor
void ReportWorkspace::initialize()
{
    mDocument.pages.push_back(createImRePage());
    mDocument.pages.push_back(createMultiImRePage());
    mDocument.pages.push_back(createFreqAmpPage());
    mDocument.pages.push_back(createModeshapePage());
}

//! Replot the designer tabs
void ReportWorkspace::refresh()
{
    mpDesignerTabs->count();
    int numPages = mDocument.pages.size();
    for (int i = 0; i != numPages; ++i)
        designer(i)->refresh();
}

//! Rebuild the designer tabs
void ReportWorkspace::rebuild()
{
    QSignalBlocker blockerDesignerTabs(mpDesignerTabs);
    mpDesignerTabs->removeAllPages();
    int numPages = mDocument.pages.size();
    for (int i = 0; i != numPages; ++i)
    {
        ReportPage& page = mDocument.pages[i];
        ReportDesigner* pDesigner = new ReportDesigner(mSettings, mpGeometryView, mpResponseEditor, page);
        QString name = page.name;
        if (name.isEmpty())
            name = tr("Page %1").arg(1 + i);
        mpDesignerTabs->addTab(pDesigner, name);
        pDesigner->fit();
    }
    mpDesignerTabs->setCurrentIndex(mpDesignerTabs->count() - 1);
}

//! Set the default document
void ReportWorkspace::setDefaultDocument()
{
    mDocument = ReportDocument();
    initialize();
    rebuild();
}

//! Fit the designer on selection
void ReportWorkspace::processDesignerSelected()
{
    ReportDesigner* pDesigner = currentDesigner();
    if (pDesigner)
        pDesigner->fit();
}

//! Helper function to create a default page with imaginary and real parts of a spectrum
ReportPage createImRePage()
{
    ReportPage page(QObject::tr("Im-Re"));

    // Create an imaginary graph
    GraphReportItem* pImag = new GraphReportItem;
    pImag->name = QObject::tr("Imaginary");
    pImag->rect = QRect(25, 35, 150, 110);
    pImag->subType = GraphReportItem::kImag;
    pImag->responseDir = ReportDirection::kY;
    pImag->unit = Units::skM_S2;
    pImag->xLabel = QObject::tr("f, Hz");
    pImag->yLabel = QObject::tr("a, ${UNIT}");
    pImag->showBundleFreq = true;

    // Create a real graph
    GraphReportItem* pReal = new GraphReportItem;
    pReal->name = QObject::tr("Real");
    pReal->rect = QRect(25, 150, 150, 110);
    pReal->subType = GraphReportItem::kReal;
    pReal->responseDir = ReportDirection::kY;
    pReal->unit = Units::skM_S2;
    pReal->link = pImag->id;
    pReal->xLabel = QObject::tr("f, Hz");
    pReal->yLabel = QObject::tr("a, ${UNIT}");
    pReal->showBundleFreq = true;

    // Create title
    TextReportItem* pTitle = new TextReportItem;
    pTitle->name = QObject::tr("Title");
    pTitle->rect = QRect(25, 10, 150, 20);
    pTitle->text = QObject::tr("Mode name\nf = ${FREQ} Hz\nExcitation F = ${FORCE} N");

    // Create the caption
    TextReportItem* pCaption = new TextReportItem;
    pCaption->name = QObject::tr("Caption");
    pCaption->rect = QRect(65, 265, 80, 10);
    pCaption->text = QObject::tr("Figure X.YY");

    // Create the page number
    TextReportItem* pNumber = new TextReportItem;
    pNumber->name = QObject::tr("Page number");
    pNumber->rect = QRect(100, 280, 10, 10);
    pNumber->text = QObject::tr("Z");

    // Combine
    page.add(pImag);
    page.add(pReal);
    page.add(pTitle);
    page.add(pCaption);
    page.add(pNumber);

    return page;
}

//! Helper function to create a default page with multiple imaginary and real parts of a spectrum
ReportPage createMultiImRePage()
{
    ReportPage page(QObject::tr("Multi Im-Re"));

    // Create an imaginary graph
    GraphReportItem* pImag = new GraphReportItem;
    pImag->name = QObject::tr("Imaginary");
    pImag->rect = QRect(25, 35, 150, 110);
    pImag->subType = GraphReportItem::kMultiImag;
    pImag->responseDir = ReportDirection::kY;
    pImag->unit = Units::skM_S2;
    pImag->xLabel = QObject::tr("f, Hz");
    pImag->yLabel = QObject::tr("a, ${UNIT}");

    // Create a real graph
    GraphReportItem* pReal = new GraphReportItem;
    pReal->name = QObject::tr("Real");
    pReal->rect = QRect(25, 150, 150, 110);
    pReal->subType = GraphReportItem::kMultiReal;
    pReal->responseDir = ReportDirection::kY;
    pReal->unit = Units::skM_S2;
    pReal->link = pImag->id;
    pReal->xLabel = QObject::tr("f, Hz");
    pReal->yLabel = QObject::tr("a, ${UNIT}");

    // Create title
    TextReportItem* pTitle = new TextReportItem;
    pTitle->name = QObject::tr("Title");
    pTitle->rect = QRect(25, 10, 150, 20);
    pTitle->text = QObject::tr("Mode name\nExcitation\nPoint ${POINT}");

    // Create the caption
    TextReportItem* pCaption = new TextReportItem;
    pCaption->name = QObject::tr("Caption");
    pCaption->rect = QRect(65, 265, 80, 10);
    pCaption->text = QObject::tr("Figure X.YY");

    // Create the page number
    TextReportItem* pNumber = new TextReportItem;
    pNumber->name = QObject::tr("Page number");
    pNumber->rect = QRect(100, 280, 10, 10);
    pNumber->text = QObject::tr("Z");

    // Combine
    page.add(pImag);
    page.add(pReal);
    page.add(pTitle);
    page.add(pCaption);
    page.add(pNumber);

    return page;
}

//! Helper function to create a default page with freq imaginary and real parts of a spectrum
ReportPage createFreqAmpPage()
{
    ReportPage page(QObject::tr("Freq Amp"));

    // Create an imaginary graph
    GraphReportItem* pAmp = new GraphReportItem;
    pAmp->name = QObject::tr("Amplitude");
    pAmp->rect = QRect(25, 35, 150, 110);
    pAmp->subType = GraphReportItem::kFreqAmp;
    pAmp->responseDir = ReportDirection::kY;
    pAmp->unit = Units::skM;
    pAmp->xLabel = QObject::tr("f, Hz");
    pAmp->yLabel = QObject::tr("a, ${UNIT}");
    pAmp->swapAxes = true;

    // Create title
    TextReportItem* pTitle = new TextReportItem;
    pTitle->name = QObject::tr("Title");
    pTitle->rect = QRect(25, 10, 150, 20);
    pTitle->text = QObject::tr("Mode name\nExcitation");

    // Create the caption
    TextReportItem* pCaption = new TextReportItem;
    pCaption->name = QObject::tr("Caption");
    pCaption->rect = QRect(65, 265, 80, 10);
    pCaption->text = QObject::tr("Figure X.YY");

    // Create the page number
    TextReportItem* pNumber = new TextReportItem;
    pNumber->name = QObject::tr("Page number");
    pNumber->rect = QRect(100, 280, 10, 10);
    pNumber->text = QObject::tr("Z");

    // Combine
    page.add(pAmp);
    page.add(pTitle);
    page.add(pCaption);
    page.add(pNumber);

    return page;
}

//! Helper function to create a default page with modeshape
ReportPage createModeshapePage()
{
    ReportPage page(QObject::tr("Modeshape"));
    page.layout.setOrientation(QPageLayout::Landscape);

    // Create an fuselage graph
    GraphReportItem* pFuselage = new GraphReportItem;
    pFuselage->name = QObject::tr("Fuselage");
    pFuselage->rect = QRect(20, 30, 170, 50);
    pFuselage->subType = GraphReportItem::kModeshape;
    pFuselage->coordDir = ReportDirection::kX;
    pFuselage->responseDir = ReportDirection::kZ;
    pFuselage->unit = Units::skM_S2;
    pFuselage->xLabel = QObject::tr("X");
    pFuselage->yLabel = QObject::tr("Z; a, ${UNIT}");
    pFuselage->showLegend = false;

    // Create a wing graph
    GraphReportItem* pWing = new GraphReportItem;
    pWing->name = QObject::tr("Wing");
    pWing->rect = QRect(20, 80, 170, 50);
    pWing->subType = GraphReportItem::kModeshape;
    pWing->coordDir = ReportDirection::kZ;
    pWing->responseDir = ReportDirection::kX;
    pWing->unit = Units::skM_S2;
    pWing->xLabel = QObject::tr("Z");
    pWing->yLabel = QObject::tr("X; a, ${UNIT}");
    pWing->showLegend = false;

    // Create a horizontal stabilizer graph
    GraphReportItem* pHStab = new GraphReportItem;
    pHStab->name = QObject::tr("Hor. stab.");
    pHStab->rect = QRect(45, 130, 120, 50);
    pHStab->subType = GraphReportItem::kModeshape;
    pHStab->coordDir = ReportDirection::kZ;
    pHStab->responseDir = ReportDirection::kX;
    pHStab->unit = Units::skM_S2;
    pHStab->xLabel = QObject::tr("Z");
    pHStab->yLabel = QObject::tr("X; a, ${UNIT}");
    pHStab->showLegend = false;

    // Create a vertical stabilizer graph
    GraphReportItem* pVStab = new GraphReportItem;
    pVStab->name = QObject::tr("Vert. stab.");
    pVStab->rect = QRect(200, 75, 70, 110);
    pVStab->subType = GraphReportItem::kModeshape;
    pVStab->coordDir = ReportDirection::kY;
    pVStab->responseDir = ReportDirection::kX;
    pVStab->unit = Units::skM_S2;
    pVStab->xLabel = QObject::tr("Z");
    pVStab->yLabel = QObject::tr("X; a, ${UNIT}");
    pVStab->swapAxes = true;
    pVStab->showLegend = false;

    // Create title
    TextReportItem* pTitle = new TextReportItem;
    pTitle->name = QObject::tr("Title");
    pTitle->rect = QRect(65, 15, 150, 20);
    pTitle->text = QObject::tr("Mode name\nExcitation");

    // Create the caption
    TextReportItem* pCaption = new TextReportItem;
    pCaption->name = QObject::tr("Caption");
    pCaption->rect = QRect(100, 190, 80, 10);
    pCaption->text = QObject::tr("Figure X.YY");

    // Create the page number
    TextReportItem* pNumber = new TextReportItem;
    pNumber->name = QObject::tr("Page number");
    pNumber->rect = QRect(5, 100, 10, 10);
    pNumber->text = QObject::tr("Z");
    pNumber->angle = 90;

    // Combine
    page.add(pFuselage);
    page.add(pWing);
    page.add(pHStab);
    page.add(pVStab);
    page.add(pTitle);
    page.add(pCaption);
    page.add(pNumber);

    return page;
}
