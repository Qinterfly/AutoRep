#include <QPdfWriter>
#include <QPrinter>
#include <QSettings>
#include <QVBoxLayout>

#include "customtabwidget.h"
#include "reporteditor.h"
#include "reportpageeditor.h"
#include "uiutility.h"

using namespace Frontend;
using namespace Backend::Core;

// Helper function
ReportPage createImagRealPage();

ReportEditor::ReportEditor(QSettings& settings, QWidget* pParent)
    : QWidget(pParent)
    , mSettings(settings)
{
    setFont(Utility::getFont());
    createContent();
    initialize();
    refresh();
}

QSize ReportEditor::sizeHint() const
{
    return QSize(500, 1000);
}

bool ReportEditor::writeReport(QString const& pathFile)
{
    QPrinter printer;
    printer.setOutputFileName(pathFile);
    int numPages = mDocument.pages.size();
    for (int i = 0; i != numPages; ++i)
    {
        ReportPageEditor* pEditor = (ReportPageEditor*) mpTabWidget->widget(i);
        if (!pEditor->print(printer))
            return false;
    }
    return true;
}

//! Create all the widgets
void ReportEditor::createContent()
{
    // Create the tab widget
    mpTabWidget = new CustomTabWidget;
    mpTabWidget->setTabsRenamable(false);
    mpTabWidget->setTabsClosable(false);

    // Create the layout
    QVBoxLayout* pLayout = new QVBoxLayout;
    pLayout->addWidget(mpTabWidget);
    setLayout(pLayout);
}

//! Initialize the editor
void ReportEditor::initialize()
{
    mDocument.pages.push_back(createImagRealPage());
}

//! Update the widgets content
void ReportEditor::refresh()
{
    QSignalBlocker blockerTabWidget(mpTabWidget);
    mpTabWidget->removeAllPages();
    int numPages = mDocument.pages.size();
    for (int i = 0; i != numPages; ++i)
    {
        ReportPage& page = mDocument.pages[i];
        ReportPageEditor* pEditor = new ReportPageEditor(page);
        QString name = page.name;
        if (name.isEmpty())
            name = tr("Page %1").arg(1 + i);
        mpTabWidget->addTab(pEditor, name);
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
