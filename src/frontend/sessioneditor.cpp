#include <QFileDialog>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "customlineedit.h"
#include "customtabwidget.h"
#include "geometryview.h"
#include "sessioneditor.h"
#include "uiutility.h"

using namespace Frontend;

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
    CustomTabWidget* pTabWidget = new CustomTabWidget;
    pTabWidget->setTabsRenamable(false);
    pTabWidget->setTabsClosable(false);
    pTabWidget->addTab(mpGeometryView, tr("Geometry"));
    pTabWidget->addTab(new QWidget, tr("Responses"));

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
