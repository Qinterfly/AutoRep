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
    // Create the project layout
    QHBoxLayout* pProjectLayout = new QHBoxLayout;
    mpProjectPath = new Edit1s;
    QPushButton* pOpenProjectButton = new QPushButton(QIcon(":/icons/document-open.svg"), QString());
    connect(pOpenProjectButton, &QPushButton::clicked, this, &SessionEditor::openProjectDialog);
    mpProjectPath->setReadOnly(true);
    pProjectLayout->addWidget(new QLabel(tr("Testlab project: ")));
    pProjectLayout->addWidget(mpProjectPath);
    pProjectLayout->addWidget(pOpenProjectButton);

    // Create the tab widget
    mpGeometryView = new GeometryView;
    mpResponseEditor = new ResponseEditor(mSession);
    CustomTabWidget* pTabWidget = new CustomTabWidget;
    pTabWidget->setTabsRenamable(false);
    pTabWidget->setTabsClosable(false);
    pTabWidget->addTab(mpGeometryView, tr("Geometry"));
    pTabWidget->addTab(mpResponseEditor, tr("Responses"));
    pTabWidget->setCurrentIndex(1);

    // Create the main layout
    QVBoxLayout* pMainLayout = new QVBoxLayout;
    pMainLayout->addLayout(pProjectLayout);
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

ResponseEditor::ResponseEditor(Session& session, QWidget* pParent)
    : QWidget(pParent)
    , mSession(session)
{
    setFont(Utility::getFont());
    createContent();
}

ResponseCollection const& ResponseEditor::collection() const
{
    return mCollection;
}

//! Add the response bundle to the current collection
bool ResponseEditor::addBundle(QStringList const& paths)
{
    auto responses = mSession.getResponses(paths);
    if (responses.empty())
        return false;
    mCollection.add(responses);
    refresh();
    emit edited();
    return true;
}

//! Add the selected response bundle to the current collection
bool ResponseEditor::addSelectedBundle()
{
    auto responses = mSession.getSelectedResponses();
    if (responses.empty())
        return false;
    mCollection.add(responses);
    refresh();
    emit edited();
    return true;
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
    }
}

//! Remove all the bundles
void ResponseEditor::removeAllBundles()
{
    mCollection.clear();
    refresh();
    emit edited();
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
    mpResponseList->setSelectionMode(QAbstractItemView::NoSelection);

    // Combine the widgets
    QVBoxLayout* pLayout = new QVBoxLayout;
    pLayout->setContentsMargins(5, 20, 5, 5);
    pLayout->addWidget(mpResponseCountLabel);
    pLayout->addWidget(mpResponseList);
    return pLayout;
}
