#include <QPdfWriter>
#include <QSettings>
#include <QVBoxLayout>

#include "customtabwidget.h"
#include "reporteditor.h"
#include "reportpageview.h"
#include "uiutility.h"

using namespace Frontend;

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

bool ReportEditor::exportReport(QString const& pathFile)
{
    QPdfWriter writer(pathFile);
    // TODO
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
    ReportPage page;
    TextReportItem* pTextItem = new TextReportItem;
    pTextItem->rect = QRectF(50, 50, 50, 50);
    pTextItem->text = "Test";
    page.add(pTextItem);
    mDocument.pages.push_back(page);
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
        ReportPageView* pView = new ReportPageView(page);
        QString name = page.name;
        if (name.isEmpty())
            name = tr("Page %1").arg(1 + i);
        mpTabWidget->addTab(pView, name);
    }
}
