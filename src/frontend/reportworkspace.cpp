#include <QFileDialog>
#include <QPrinter>
#include <QSettings>
#include <QToolBar>
#include <QVBoxLayout>

#include "customtabwidget.h"
#include "reportdefaults.h"
#include "reportdesigner.h"
#include "reportworkspace.h"
#include "uiconstants.h"
#include "uiutility.h"

using namespace Frontend;
using namespace Backend::Core;
using namespace Constants;

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
    pToolBar->setIconSize(Size::skToolBarIcon);
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
    mDocument.pages.push_back(ReportDefaults::imRePage());
    mDocument.pages.push_back(ReportDefaults::multiImRePage());
    mDocument.pages.push_back(ReportDefaults::freqAmpPage());
    mDocument.pages.push_back(ReportDefaults::modeshapePage());
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

