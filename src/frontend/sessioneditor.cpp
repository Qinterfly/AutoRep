#include <QFileDialog>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QToolBar>
#include <QVBoxLayout>

#include "customlineedit.h"
#include "customtabwidget.h"
#include "geometryview.h"
#include "sessioneditor.h"
#include "uiconstants.h"
#include "uiutility.h"

using namespace Frontend;
using namespace Backend::Core;

SessionEditor::SessionEditor(QSettings& settings, QWidget* pParent)
    : QWidget(pParent)
    , mSettings(settings)
{
    setFont(Utility::getFont());
    createContent();
}

QSize SessionEditor::sizeHint() const
{
    return QSize(500, 1000);
}

GeometryView* SessionEditor::geometryView()
{
    return mpGeometryView;
}

ResponseEditor* SessionEditor::responseEditor()
{
    return mpResponseEditor;
}

//! Open the project located at the specfied path
bool SessionEditor::openProject(QString const& pathFile)
{
    if (!mSession.openProject(pathFile))
        return false;
    bool isValid = mSession.isProjectValid();
    if (isValid)
    {
        mpGeometryView->setGeometry(mSession.getGeometry());
        mpResponseEditor->removeAllBundles();
        mpProjectPath->setText(pathFile);
        Utility::setLastPathFile(mSettings, pathFile);
        qInfo() << tr("Testlab project is successfully opened");
    }
    else
    {
        mpGeometryView->setGeometry(Testlab::Geometry());
        mSession.closeProject();
        qWarning() << tr("Could not connect to a Testlab project. Make sure that the license server is running");
    }
    return isValid;
}

//! Create all the widgets
void SessionEditor::createContent()
{
    // Create the toolbar
    mpProjectPath = new Edit1s;
    mpProjectPath->setReadOnly(true);
    QToolBar* pToolBar = new QToolBar;
    pToolBar->addWidget(new QLabel(tr("Testlab project: ")));
    pToolBar->addWidget(mpProjectPath);
    pToolBar->addAction(QIcon(":/icons/document-open.svg"), tr("Open project"), this, &SessionEditor::openProjectDialog);
    pToolBar->setIconSize(Constants::Size::skToolBarIcon);
    Utility::setShortcutHints(pToolBar);

    // Create the tab widget
    mpGeometryView = new GeometryView;
    mpResponseEditor = new ResponseEditor(mSettings, mSession);
    CustomTabWidget* pTabWidget = new CustomTabWidget;
    pTabWidget->setTabsRenamable(false);
    pTabWidget->setTabsClosable(false);
    pTabWidget->addTab(mpGeometryView, tr("Geometry"));
    pTabWidget->addTab(mpResponseEditor, tr("Responses"));

    // Create the main layout
    QVBoxLayout* pMainLayout = new QVBoxLayout;
    pMainLayout->addWidget(pToolBar);
    pMainLayout->addWidget(pTabWidget);
    setLayout(pMainLayout);
}

//! Create a file dialog to open project
void SessionEditor::openProjectDialog()
{
    QString pathFile = QFileDialog::getOpenFileName(this, tr("Open Testlab Project"), Utility::getLastDirectory(mSettings).path(),
                                                    tr("Testlab file format (*.lms)"));
    if (pathFile.isEmpty())
        return;
    openProject(pathFile);
}

ResponseEditor::ResponseEditor(QSettings& settings, Session& session, QWidget* pParent)
    : QWidget(pParent)
    , mSettings(settings)
    , mSession(session)
{
    setFont(Utility::getFont());
    createContent();
}

ResponseCollection const& ResponseEditor::collection() const
{
    return mCollection;
}

//! Add the response bundle
bool ResponseEditor::addBundle(Backend::Core::Responses const& responses)
{
    if (responses.empty())
        return false;

    // Construct the default name
    QString path = QString::fromStdWString(responses.front().header.path);
    QString name;
    if (!path.isEmpty())
    {
        QStringList tokens = path.split('/', Qt::SkipEmptyParts);
        int numTokens = tokens.size();
        if (numTokens > 2)
            name = tokens[numTokens - 3];
    }

    // Add to the collection
    mCollection.add(responses, name);
    refresh();
    emit edited();
    qInfo() << tr("New response bundle is successfuly created. The number of responses is %1").arg(responses.size());
    return true;
}

//! Add the response bundle
bool ResponseEditor::addBundle(QStringList const& paths)
{
    auto responses = mSession.getResponses(paths);
    return addBundle(responses);
}

//! Add the selected response bundle
bool ResponseEditor::addSelectedBundle()
{
    auto responses = mSession.getSelectedResponses();
    return addBundle(responses);
}

//! Merge the currently selected bundle with the selected responses
bool ResponseEditor::mergeSelectedBundle()
{
    int iBundle = mpBundleList->currentRow();
    if (iBundle < 0)
        return false;
    auto responses = mSession.getSelectedResponses();
    if (responses.empty())
        return false;
    mCollection.merge(iBundle, responses);
    refresh();
    emit edited();
    qInfo() << tr("The selected responses are added to the selected bundle");
    return true;
}

//! Remove the currently selected bundle
void ResponseEditor::removeBundle()
{
    int iBundle = mpBundleList->currentRow();
    if (mCollection.remove(iBundle))
    {
        refresh();
        emit edited();
        qInfo() << tr("Response bundle is deleted");
    }
}

//! Remove all the bundles
void ResponseEditor::removeAllBundles()
{
    mCollection.clear();
    refresh();
    emit edited();
    qInfo() << tr("All the response bundles are removed");
}

//! Update the widgets content
void ResponseEditor::refresh()
{
    // Add the bundles
    QSignalBlocker blockerBundleList(mpBundleList);
    int iBundle = mpBundleList->currentRow();
    mpBundleList->clear();
    int numBundles = mCollection.count();
    for (int i = 0; i != numBundles; ++i)
    {
        QListWidgetItem* pItem = new QListWidgetItem(mCollection.get(i).name);
        mpBundleList->addItem(pItem);
    }
    if (iBundle < 0 || iBundle >= numBundles)
        iBundle = numBundles - 1;
    mpBundleList->setCurrentRow(iBundle);

    // Add the responses
    QSignalBlocker blockerResponseList(mpResponseList);
    mpResponseList->clear();
    iBundle = mpBundleList->currentRow();
    int numResponses = 0;
    if (iBundle >= 0)
    {
        ResponseBundle const& bundle = mCollection.get(iBundle);
        numResponses = bundle.responses.size();
        for (int i = 0; i != numResponses; ++i)
        {
            Testlab::Response const& response = bundle.responses[i];
            QListWidgetItem* pItem = new QListWidgetItem(QString::fromStdWString(response.header.name));
            mpResponseList->addItem(pItem);
        }
    }

    // Set the response label
    mpResponseCountLabel->setText(tr("Number of responses: %1").arg(numResponses));
}

//! Create all the widgets
void ResponseEditor::createContent()
{
    QHBoxLayout* pLayout = new QHBoxLayout;
    pLayout->addLayout(createBundleLayout());
    pLayout->addLayout(createResponseLayout());
    setLayout(pLayout);
}

//! Create bundle related widgets
QLayout* ResponseEditor::createBundleLayout()
{
    // Create the toolbar
    QToolBar* pToolBar = new QToolBar;
    pToolBar->addAction(QIcon(":/icons/list-add.svg"), tr("Add bundle"), this, &ResponseEditor::addSelectedBundle);
    pToolBar->addAction(QIcon(":/icons/list-merge.svg"), tr("Merge bundle"), this, &ResponseEditor::mergeSelectedBundle);
    pToolBar->addAction(QIcon(":/icons/list-remove.svg"), tr("Remove bundle"), this, &ResponseEditor::removeBundle);

    // Create the list
    mpBundleList = new QListWidget;
    mpBundleList->setFont(font());
    mpBundleList->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(mpBundleList, &QListWidget::currentRowChanged, this, &ResponseEditor::refresh);

    // Combine the widgets
    QVBoxLayout* pLayout = new QVBoxLayout;
    pLayout->setContentsMargins(0, 0, 0, 5);
    pLayout->addWidget(pToolBar);
    pLayout->addWidget(mpBundleList);
    return pLayout;
}

//! Create response related widgets
QLayout* ResponseEditor::createResponseLayout()
{
    // Create the label
    mpResponseCountLabel = new QLabel;

    // Create the list
    mpResponseList = new QListWidget;
    mpResponseList->setFont(font());
    mpResponseList->setSelectionMode(QAbstractItemView::ExtendedSelection);

    // Combine the widgets
    QVBoxLayout* pLayout = new QVBoxLayout;
    pLayout->setContentsMargins(5, 20, 5, 5);
    pLayout->addWidget(mpResponseCountLabel);
    pLayout->addWidget(mpResponseList);
    return pLayout;
}
