#include <QApplication>
#include <QFileDialog>
#include <QHeaderView>
#include <QInputDialog>
#include <QMessageBox>
#include <QPrinter>
#include <QSettings>
#include <QTimer>
#include <QToolBar>
#include <QVBoxLayout>

#include "customtabwidget.h"
#include "fileutility.h"
#include "reportdefaults.h"
#include "reportdesigner.h"
#include "reportworkspace.h"
#include "uiconstants.h"
#include "uiutility.h"

using namespace Frontend;
using namespace Backend::Core;
using namespace Constants;

ReportWorkspaceOptions::ReportWorkspaceOptions()
{
    autoSaveDuration = 120000;
}

ReportWorkspace::ReportWorkspace(QSettings& settings, GeometryView* pGeometryView, ResponseEditor* pResponseEditor,
                                 ReportWorkspaceOptions const& options, QWidget* pParent)
    : QWidget(pParent)
    , mSettings(settings)
    , mpGeometryView(pGeometryView)
    , mpResponseEditor(pResponseEditor)
    , mOptions(options)
{
    setFont(Utility::getFont());
    createContent();
    createConnections();
    setDefaultDocument();
    setAutoSave();
}

QSize ReportWorkspace::sizeHint() const
{
    return QSize(500, 1000);
}

ReportDocument const& ReportWorkspace::document() const
{
    return mDocument;
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

//! Set a new document
void ReportWorkspace::setNewDocument()
{
    setDocument(ReportDocument());
    mOptions.lastPathFile = QString();
}

//! Set a default document
void ReportWorkspace::setDefaultDocument()
{
    setDocument(ReportDefaults::document());
    mOptions.lastPathFile = QString();
}

//! Replace the current document with the new one
void ReportWorkspace::setDocument(ReportDocument const& document)
{
    mDocument = document;
    recreateDesigners();
}

//! Open the document from a file
void ReportWorkspace::openDocumentDialog()
{
    // Get the file path
    QString pathFile = QFileDialog::getOpenFileName(this, tr("Open Document"), Utility::getLastDirectory(mSettings).path(),
                                                    tr("Document format (*.%1)").arg(ReportDocument::fileSuffix()));
    if (pathFile.isEmpty())
        return;

    // Read the document
    ReportDocument document;
    if (document.read(pathFile))
        setDocument(document);

    // Store the path
    Utility::setLastPathFile(mSettings, pathFile);
    mOptions.lastPathFile = pathFile;
}

//! Save the document
void ReportWorkspace::saveDocument()
{
    if (mOptions.lastPathFile.isEmpty())
    {
        saveAsDocumentDialog();
        return;
    }
    if (mDocument.write(mOptions.lastPathFile))
        emit saved();
}

//! Save the document to a file by means of a dialog
void ReportWorkspace::saveAsDocumentDialog()
{
    // Get the file path
    QString pathFile = QFileDialog::getSaveFileName(this, tr("Save Document"), Utility::getLastDirectory(mSettings).path(),
                                                    tr("Document format (*.%1)").arg(ReportDocument::fileSuffix()));
    if (pathFile.isEmpty())
        return;

    // Modify the suffix, if necessary
    Utility::modifyFileSuffix(pathFile, ReportDocument::fileSuffix());

    // Store the path
    Utility::setLastPathFile(mSettings, pathFile);

    // Write the document
    if (mDocument.write(pathFile))
    {
        mOptions.lastPathFile = pathFile;
        emit saved();
    }
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

    int numPages = mDocument.count();
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

//! Create a designer
void ReportWorkspace::addPage()
{
    QSignalBlocker blockerDesignerTabs(mpDesignerTabs);
    mDocument.add();
    addDesigner(mDocument.count() - 1);
    mpDesignerTabs->setCurrentIndex(mpDesignerTabs->count() - 1);
    emit edited();
}

//! Rename the currently active designer
void ReportWorkspace::renameCurrentPage()
{
    // Get the current page index
    int iPage = mpDesignerTabs->currentIndex();
    if (iPage < 0)
        return;

    // Construct the dialog
    bool isOk;
    QString name = QInputDialog::getText(this, tr("Change page name"), tr("New page name: "), QLineEdit::Normal, mpDesignerTabs->tabText(iPage),
                                         &isOk);
    if (!isOk)
        return;

    // Set the page name
    QSignalBlocker blockerDesignerTabs(mpDesignerTabs);
    mDocument.get(iPage).name = name;
    mpDesignerTabs->setTabText(iPage, name);
    emit edited();
}

//! Duplicate the currently active designer
void ReportWorkspace::duplicateCurrentPage()
{
    // Get the current page index
    int iPage = mpDesignerTabs->currentIndex();
    if (iPage < 0)
        return;

    // Duplicate the page
    QSignalBlocker blockerDesignerTabs(mpDesignerTabs);
    mDocument.add(mDocument.get(iPage));
    addDesigner(mDocument.count() - 1);
    emit edited();
}

//! Remove the currently active designer
void ReportWorkspace::removeCurrentPage()
{
    // Get the current page index
    int iPage = mpDesignerTabs->currentIndex();
    if (iPage < 0)
        return;

    // Construct the dialog
    auto reply = QMessageBox::question(this, tr("Remove page"), tr("Are you sure that you want to remove the current page?"),
                                       QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes)
        return;

    // Remove the page
    QSignalBlocker blockerDesignerTabs(mpDesignerTabs);
    mpDesignerTabs->removePage(iPage);
    mDocument.remove(iPage);
    emit edited();
}

//! Move the currently selected designer
void ReportWorkspace::moveCurrentPage(int iShift)
{
    if (iShift == 0)
        return;

    // Get the current page index
    int iPage = mpDesignerTabs->currentIndex();
    if (iPage < 0)
        return;

    // Get the new page index
    int iSwapPage = iPage + iShift;
    if (iSwapPage < 0 || iSwapPage >= mpDesignerTabs->count())
        return;

    // Move the designers
    QSignalBlocker blockerDesignerTabs(mpDesignerTabs);
    mpDesignerTabs->moveTab(iPage, iSwapPage);
    mpDesignerTabs->setCurrentIndex(iSwapPage);

    // Swap the pages
    mDocument.swap(iPage, iSwapPage);
    emit edited();
}

//! Create all the widgets
void ReportWorkspace::createContent()
{
    // Create the toolbar
    QToolBar* pToolBar = new QToolBar;
    pToolBar->addAction(QIcon(":/icons/document-new.svg"), tr("New document"), QKeySequence::New, this, &ReportWorkspace::setNewDocumentDialog);
    pToolBar->addAction(QIcon(":/icons/document-default.svg"), tr("Default document"), this, &ReportWorkspace::setDefaultDocumentDialog);
    pToolBar->addAction(QIcon(":/icons/document-variable.svg"), tr("Variable edtior"), this, &ReportWorkspace::editTextEngine);
    pToolBar->addSeparator();
    pToolBar->addAction(QIcon(":/icons/document-open.svg"), tr("Open document"), QKeySequence::Open, this, &ReportWorkspace::openDocumentDialog);
    pToolBar->addAction(QIcon(":/icons/document-save.svg"), tr("Save document"), QKeySequence::Save, this, &ReportWorkspace::saveDocument);
    pToolBar->addAction(QIcon(":/icons/document-save-as.svg"), tr("Save as document..."), QKeySequence::SaveAs, this,
                        &ReportWorkspace::saveAsDocumentDialog);
    pToolBar->addAction(QIcon(":/icons/document-print.svg"), tr("Print document"), QKeySequence::Print, this, &ReportWorkspace::printDialog);
    pToolBar->addSeparator();
    pToolBar->addAction(QIcon(":/icons/list-add.svg"), tr("Add page"), this, &ReportWorkspace::addPage);
    pToolBar->addAction(QIcon(":/icons/list-rename.svg"), tr("Rename page"), this, &ReportWorkspace::renameCurrentPage);
    pToolBar->addAction(QIcon(":/icons/edit-copy.svg"), tr("Duplicate page"), this, &ReportWorkspace::duplicateCurrentPage);
    pToolBar->addAction(QIcon(":/icons/arrow-left.svg"), tr("Move page left"), this, [this]() { moveCurrentPage(-1); });
    pToolBar->addAction(QIcon(":/icons/arrow-right.svg"), tr("Move page right"), this, [this]() { moveCurrentPage(+1); });
    pToolBar->addAction(QIcon(":/icons/list-remove.svg"), tr("Remove page"), this, &ReportWorkspace::removeCurrentPage);
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

//! Replot the designer tabs
void ReportWorkspace::refresh()
{
    mpDesignerTabs->count();
    int numPages = mDocument.count();
    for (int i = 0; i != numPages; ++i)
        designer(i)->refresh();
}

//! Rebuild the designers
void ReportWorkspace::recreateDesigners()
{
    QSignalBlocker blockerDesignerTabs(mpDesignerTabs);
    mpDesignerTabs->removeAllPages();
    int numPages = mDocument.count();
    for (int i = 0; i != numPages; ++i)
        addDesigner(i);
    mpDesignerTabs->setCurrentIndex(mpDesignerTabs->count() - 1);
}

void ReportWorkspace::setNewDocumentDialog()
{
    auto reply = QMessageBox::question(this, tr("Set new document"), tr("Are you sure that you want to set a new document?"),
                                       QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes)
        setNewDocument();
}

void ReportWorkspace::setDefaultDocumentDialog()
{
    auto reply = QMessageBox::question(this, tr("Set default document"), tr("Are you sure that you want to set a default document?"),
                                       QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes)
        setDefaultDocument();
}

//! Add a designer to the workspace
void ReportWorkspace::addDesigner(int iPage)
{
    ReportPage& page = mDocument.get(iPage);
    ReportDesigner* pDesigner = new ReportDesigner(mSettings, mpGeometryView, mpResponseEditor, page, mDocument.textEngine);
    QString name = page.name;
    if (name.isEmpty())
        name = tr("Page %1").arg(1 + iPage);
    connect(pDesigner, &ReportDesigner::edited, this, &ReportWorkspace::edited);
    mpDesignerTabs->addTab(pDesigner, name);
    pDesigner->fit();
}

//! Set the autosave timer
void ReportWorkspace::setAutoSave()
{
    if (mOptions.autoSaveDuration <= 0)
        return;

    // Set the autosave path
    QString dir = Backend::Utility::combineFilePath(qApp->applicationDirPath(), "autosave");
    QString fileName = QString("%1.%2").arg(QDateTime::currentDateTime().toString("dd.mm.yyyy - hh.mm"), ReportDocument::fileSuffix());
    mOptions.autoSavePathFile = Backend::Utility::combineFilePath(dir, fileName);
    if (!QFileInfo::exists(dir))
        QDir().mkpath(dir);

    // Run the timer
    QTimer* pTimer = new QTimer(this);
    connect(pTimer, &QTimer::timeout, this,
            [this]()
            {
                ReportDocument document(mDocument);
                document.write(mOptions.autoSavePathFile);
                qInfo() << tr("Document is automatically saved to %1").arg(mOptions.autoSavePathFile);
            });
    pTimer->start(mOptions.autoSaveDuration);
}

//! Fit the designer on selection
void ReportWorkspace::processDesignerSelected()
{
    ReportDesigner* pDesigner = currentDesigner();
    if (pDesigner)
        pDesigner->fit();
}

//! Show an editor to create and remove text variables
void ReportWorkspace::editTextEngine()
{
    ReportTextEngineEditor* pEditor = new ReportTextEngineEditor(mSettings, mDocument.textEngine);
    connect(pEditor, &ReportTextEngineEditor::edited, this, &ReportWorkspace::refresh);
    Utility::showAsDialog(pEditor, tr("Text Engine Editor"), this, false);
}

ReportTextEngineEditor::ReportTextEngineEditor(QSettings& settings, ReportTextEngine& textEngine, QWidget* pParent)
    : QWidget(pParent)
    , mSettings(settings)
    , mTextEngine(textEngine)
{
    createContent();
    refresh();
}

QSize ReportTextEngineEditor::sizeHint() const
{
    return QSize(600, 600);
}

//! Update the widgets content
void ReportTextEngineEditor::refresh()
{
    // Set the table properties
    QSignalBlocker blockerTable(mpTable);
    int numVars = mTextEngine.numVariables();
    mpTable->clear();
    mpTable->setRowCount(numVars);
    mpTable->setColumnCount(2);
    mpTable->setHorizontalHeaderLabels({tr("Key"), tr("Value")});
    mpTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    mpTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

    // Set the table content
    QStringList keys = mTextEngine.keys();
    std::sort(keys.begin(), keys.end(), [](QString const& a, QString const& b) { return a.size() < b.size(); });
    for (int i = 0; i != numVars; ++i)
    {
        QString const& key = keys[i];
        QString value = mTextEngine.getValue(key);
        mpTable->setItem(i, 0, Utility::createTableItem(key));
        mpTable->setItem(i, 1, Utility::createTableItem(value));
    }
}

//! Insert a new variable
void ReportTextEngineEditor::addVariable()
{
    if (mTextEngine.addVariable("NEW"))
    {
        refresh();
        qInfo() << tr("New variable is added");
    }
}

//! Remove the selected variables
void ReportTextEngineEditor::removeVariables()
{
    // Retrieve the selected variables
    QList<int> selected = selectedRows();
    if (selected.isEmpty())
        return;

    // Remove the selected variables
    int numSelected = selected.size();
    for (int i = 0; i != numSelected; ++i)
    {
        QString key = mpTable->item(selected[i], 0)->text();
        mTextEngine.removeVariable(key);
    }
    qInfo() << tr("Selected variables are removed");

    // Update the content
    refresh();
}

//! Create all the widgets
void ReportTextEngineEditor::createContent()
{
    // Create the toolbar
    QToolBar* pToolBar = new QToolBar;
    pToolBar->addAction(QIcon(":/icons/list-add.svg"), tr("Add variable"), this, &ReportTextEngineEditor::addVariable);
    pToolBar->addAction(QIcon(":/icons/list-remove.svg"), tr("Remove variable"), this, &ReportTextEngineEditor::removeVariables);
    pToolBar->setIconSize(Size::skToolBarIcon);
    Utility::setShortcutHints(pToolBar);

    // Create the table widget
    mpTable = new CustomTable;
    mpTable->setSelectionBehavior(QAbstractItemView::SelectItems);
    mpTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
    connect(mpTable, &CustomTable::itemChanged, this, &ReportTextEngineEditor::processItemChanged);

    // Combine the widgets
    QVBoxLayout* pLayout = new QVBoxLayout;
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->addWidget(pToolBar);
    pLayout->addWidget(mpTable);
    setLayout(pLayout);
}

//! Retrieve selected tabular rows
QList<int> ReportTextEngineEditor::selectedRows()
{
    QSet<int> selectedRows;
    QList<QTableWidgetItem*> items = mpTable->selectedItems();
    int numItems = items.size();
    for (int i = 0; i != numItems; ++i)
        selectedRows.insert(items[i]->row());
    return selectedRows.values();
}

//! Process changing items
void ReportTextEngineEditor::processItemChanged()
{
    int numRows = mpTable->rowCount();
    mTextEngine.clearVariables();
    for (int i = 0; i != numRows; ++i)
    {
        QString key = mpTable->item(i, 0)->text();
        QString value = mpTable->item(i, 1)->text();
        mTextEngine.setVariable(key, value);
    }
    emit edited();
}
