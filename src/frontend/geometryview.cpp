#include <vtkCameraOrientationWidget.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkNamedColors.h>
#include <vtkRenderer.h>
#include <QVTKOpenGLNativeWidget.h>

#include <QVBoxLayout>

#include "geometryview.h"

using namespace Frontend;

vtkNew<vtkNamedColors> const vtkColors;

GeometryViewOptions::GeometryViewOptions()
{
    // Color scheme
    sceneColor = vtkColors->GetColor3d("aliceblue");
    sceneColor2 = vtkColors->GetColor3d("white");
}

GeometryView::GeometryView(GeometryViewOptions const& options)
    : mOptions(options)
{
    createContent();
    initialize();
}

GeometryView::~GeometryView()
{
    clear();
}

void GeometryView::clear()
{
    // TODO
}

void GeometryView::plot()
{
    clear();
    // TODO
}

void GeometryView::refresh()
{
    // TODO
}

void GeometryView::setGeometry(Testlab::Geometry geometry)
{
    mGeometry = geometry;
    plot();
}

//! Set the initial state of widgets
void GeometryView::initialize()
{
    int const kNumOrientationFrames = 10;

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
void GeometryView::createContent()
{
    QVBoxLayout* pLayout = new QVBoxLayout;

    // Create the VTK widget
    mRenderWidget = new QVTKOpenGLNativeWidget(this);

    // Combine the widgets
    pLayout->addWidget(mRenderWidget);
    setLayout(pLayout);
}
