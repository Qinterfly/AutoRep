#include <QVBoxLayout>
#include <QVTKOpenGLNativeWidget.h>

#include <testlab/common.h>

#include <vtkCameraOrientationWidget.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkNamedColors.h>
#include <vtkRenderer.h>

#include "modereportsceneitem.h"
#include "reportitem.h"
#include "reporttextengine.h"
#include "session.h"

using namespace Backend::Core;
using namespace Frontend;

// Constants
vtkNew<vtkNamedColors> const vtkColors;

ModeOptions::ModeOptions()
{
    // Color scheme
    sceneColor = vtkColors->GetColor3d("aliceblue");
    sceneColor2 = vtkColors->GetColor3d("white");
}

ModeReportSceneItem::ModeReportSceneItem(ModeReportItem* pItem, ReportTextEngine& textEngine, ResponseCollection const& collection,
                                         int iSelectedBundle, Testlab::Geometry const& geometry, ModeOptions const& options,
                                         QGraphicsItem* pParent)
    : ReportSceneItem(pItem, pParent)
    , mTextEngine(textEngine)
    , mCollection(collection)
    , mISelectedBundle(iSelectedBundle)
    , mGeometry(geometry)
    , mOptions(options)
{
    createContent();
    initialize();
    setState();
}

ModeReportSceneItem::~ModeReportSceneItem()
{
    mRenderWidget->deleteLater();
}

//! Set the item state
void ModeReportSceneItem::setState()
{
    if (!mCollection.isEmpty())
    {
        ResponseBundle const& bundle = mISelectedBundle >= 0 ? mCollection.get(mISelectedBundle) : mCollection.get(0);
    }
    // TODO
}

//! Process paint event
void ModeReportSceneItem::paint(QPainter* pPainter, QStyleOptionGraphicsItem const* pOption, QWidget* pWidget)
{
    // TODO
    ReportSceneItem::paint(pPainter, pOption, pWidget);
}

//! Set the initial state of widgets
void ModeReportSceneItem::initialize()
{
    int const kNumOrientationFrames = 10;

    // Specify the format for the VTK library
    QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());
    vtkObject::GlobalWarningDisplayOff();

    // Set up the scence
    mRenderer = vtkRenderer::New();
    mRenderer->SetBackground(mOptions.sceneColor.GetData());
    mRenderer->SetBackground2(mOptions.sceneColor2.GetData());
    mRenderer->GradientBackgroundOn();
    mRenderer->ResetCamera();

    // Create the window
    mRenderWindow = vtkGenericOpenGLRenderWindow::New();
    mRenderWindow->AddRenderer(mRenderer);
    mRenderWidget->setRenderWindow(mRenderWindow);

    // Create the orientation widget
    mOrientationWidget = vtkCameraOrientationWidget::New();
    mOrientationWidget->SetParentRenderer(mRenderer);
    mOrientationWidget->On();
    mOrientationWidget->SetAnimatorTotalFrames(kNumOrientationFrames);
}

//! Create all the widgets and corresponding actions
void ModeReportSceneItem::createContent()
{
    mRenderWidget = new QVTKOpenGLNativeWidget;
}
