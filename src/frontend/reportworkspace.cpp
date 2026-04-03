#include <QPrinter>
#include <QSettings>
#include <QVBoxLayout>

#include "customtabwidget.h"
#include "reportdesigner.h"
#include "reportworkspace.h"
#include "uiutility.h"

using namespace Frontend;
using namespace Backend::Core;

// Helper function
ReportPage createImagRealPage();

ReportWorkspace::ReportWorkspace(QSettings& settings, QWidget* pParent)
    : QWidget(pParent)
    , mSettings(settings)
{
    setFont(Utility::getFont());
    createContent();
    initialize();
    refresh();
}

QSize ReportWorkspace::sizeHint() const
{
    return QSize(500, 1000);
}

//! Retrieve the designer associated with the page index
ReportDesigner* ReportWorkspace::designer(int iPage)
{
    if (iPage >= 0 && iPage < mpDesignerTabs->count())
        return (ReportDesigner*) mpDesignerTabs->widget(iPage);
    return nullptr;
}

//! Write all the specified to the pdf file
bool ReportWorkspace::writePage(QString const& pathFile, int iPage)
{
    QPrinter printer;
    printer.setOutputFileName(pathFile);
    ReportDesigner* pDesigner = designer(iPage);
    if (!pDesigner || pDesigner->print(printer))
        return false;
    return true;
}

//! Write all the pages to the pdf file
bool ReportWorkspace::writeAll(QString const& pathFile)
{
    QPrinter printer;
    printer.setOutputFileName(pathFile);
    int numPages = mDocument.pages.size();
    for (int iPage = 0; iPage != numPages; ++iPage)
    {
        ReportDesigner* pDesigner = designer(iPage);
        if (!pDesigner || !pDesigner->print(printer))
            return false;
    }
    return true;
}

//! Create all the widgets
void ReportWorkspace::createContent()
{
    // Create the tab widget
    mpDesignerTabs = new CustomTabWidget;
    mpDesignerTabs->setTabsRenamable(false);
    mpDesignerTabs->setTabsClosable(false);

    // Create the layout
    QVBoxLayout* pLayout = new QVBoxLayout;
    pLayout->addWidget(mpDesignerTabs);
    setLayout(pLayout);
}

//! Initialize the editor
void ReportWorkspace::initialize()
{
    mDocument.pages.push_back(createImagRealPage());
}

//! Update the widgets content
void ReportWorkspace::refresh()
{
    QSignalBlocker blockerTabWidget(mpDesignerTabs);
    mpDesignerTabs->removeAllPages();
    int numPages = mDocument.pages.size();
    for (int i = 0; i != numPages; ++i)
    {
        ReportPage& page = mDocument.pages[i];
        ReportDesigner* pDesigner = new ReportDesigner(page);
        QString name = page.name;
        if (name.isEmpty())
            name = tr("Page %1").arg(1 + i);
        mpDesignerTabs->addTab(pDesigner, name);
    }
}

//! Helper function to create a default page with imaginary and real parts of a spectrum
ReportPage createImagRealPage()
{
    ReportPage page(QPageSize::A4, QObject::tr("Im/Re"));

    // Create title
    TextReportItem* pTitle = new TextReportItem;
    pTitle->rect = QRect(40, 20, 125, 20);
    pTitle->text = "Test";

    // Create an imaginary graph
    GraphReportItem* pImag = new GraphReportItem;
    pImag->rect = QRect(30, 40, 150, 95);
    pImag->xLabel = QObject::tr("Frequency, Hz");
    pImag->yLabel = QObject::tr("a, m/s%1").arg(QChar(0x00B2));

    // Create a real graph
    GraphReportItem* pReal = new GraphReportItem;
    pReal->rect = QRect(30, 145, 150, 95);
    pReal->xLabel = QObject::tr("Frequency, Hz");
    pReal->yLabel = QObject::tr("a, m/s%1").arg(QChar(0x00B2));

    // Create the caption
    TextReportItem* pCaption = new TextReportItem;
    pCaption->rect = QRect(65, 245, 80, 10);
    pCaption->text = QObject::tr("Figure X.YY");

    // Create the page number
    TextReportItem* pNumber = new TextReportItem;
    pNumber->rect = QRect(100, 265, 10, 10);
    pNumber->text = QObject::tr("Z");

    // Combine
    page.add(pTitle);
    page.add(pImag);
    page.add(pReal);
    page.add(pCaption);
    page.add(pNumber);

    return page;
}
