#include <QVBoxLayout>
#include <QVTKOpenGLNativeWidget.h>

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCameraOrientationWidget.h>
#include <vtkCellArray.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkGlyph3DMapper.h>
#include <vtkNamedColors.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolygon.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

#include "geometryview.h"
#include "uiutility.h"

using namespace Frontend;
using namespace Eigen;

vtkNew<vtkNamedColors> const vtkColors;

double getMaximumDimension(Testlab::Geometry const& geometry);
Vector3d convert3d(std::vector<double> const& data);
vtkSmartPointer<vtkActor> createSphereActor(Eigen::Vector3d const& position, double radius);

GeometryViewOptions::GeometryViewOptions()
{
    // Color scheme
    sceneColor = vtkColors->GetColor3d("aliceblue");
    sceneColor2 = vtkColors->GetColor3d("white");
    edgeColor = vtkColors->GetColor3d("gainsboro");
    componentColors = {vtkColors->GetColor3d("red"),  vtkColors->GetColor3d("green"),   vtkColors->GetColor3d("blue"),
                       vtkColors->GetColor3d("cyan"), vtkColors->GetColor3d("magenta"), vtkColors->GetColor3d("orange")};

    // Opacity
    edgeOpacity = 0.5;

    // Flags
    showEdges = false;
    showWireframe = false;
    showVertices = true;
    showLines = true;
    showTrias = true;
    showQuads = true;

    // Scales
    sceneScale = {1.0, 1.0, 1.0};

    // Size
    lineWidth = 2;
    pointScale = 0.005;
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
    // Remove the actors
    auto actors = mRenderer->GetActors();
    while (actors->GetLastActor())
        mRenderer->RemoveActor(actors->GetLastActor());

    // Remove the view properties
    auto props = mRenderer->GetViewProps();
    while (props->GetLastProp())
        mRenderer->RemoveViewProp(props->GetLastProp());
}

void GeometryView::plot()
{
    clear();
    drawGeometry();
    mRenderWindow->Render();
}

void GeometryView::refresh()
{
    mRenderWindow->Render();
}

void GeometryView::setGeometry(Testlab::Geometry geometry)
{
    mGeometry = geometry;
    plot();
    setIsometricView();
}

//! Set the isometric view
void GeometryView::setIsometricView()
{
    vtkCamera* camera = mRenderer->GetActiveCamera();
    camera->SetPosition(1, 1, -1);
    camera->SetFocalPoint(0, 0, 0);
    camera->SetViewUp(0, 1, 0);
    mRenderer->ResetCamera();
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

//! Represent geometry
void GeometryView::drawGeometry()
{
    // Estimate the maximum dimension
    mMaximumDimension = getMaximumDimension(mGeometry);

    // Draw all the components
    int numComponents = mGeometry.components.size();
    int numColors = mOptions.componentColors.size();
    for (int iComponent = 0; iComponent != numComponents; ++iComponent)
    {
        int iColor = Utility::getRepeatedIndex(iComponent, numColors);
        drawComponent(mGeometry.components[iComponent], mOptions.componentColors[iColor]);
    }
}

//! Represent the geometrical component
void GeometryView::drawComponent(Testlab::Component const& component, vtkColor3d color)
{
    // Construct the vertices out of nodes
    vtkSmartPointer<vtkPoints> points = createPoints(component.nodes);

    // Draw
    if (mOptions.showVertices)
        drawVertices(points, color);
    if (mOptions.showLines)
        drawElements(points, component.lines, color);
    if (mOptions.showTrias)
        drawElements(points, component.trias, color);
    if (mOptions.showQuads)
        drawElements(points, component.quads, color);
}

//! Render vertices using one color
void GeometryView::drawVertices(vtkSmartPointer<vtkPoints> points, vtkColor3d color, double opacity)
{
    int numPoints = points->GetNumberOfPoints();
    double data[3];
    double radius = mOptions.pointScale * mMaximumDimension;
    for (int i = 0; i != numPoints; ++i)
    {
        // Create the point actor
        points->GetPoint(i, data);
        Vector3d position = {data[0], data[1], data[2]};
        vtkSmartPointer<vtkActor> actor = createSphereActor(position, radius);

        // Set the actor properties
        actor->GetProperty()->SetColor(color.GetData());

        // Add the actor to the scene
        mRenderer->AddActor(actor);
    }
}

//! Render elements using one color
void GeometryView::drawElements(vtkSmartPointer<vtkPoints> points, std::vector<std::vector<int>> const& indices, vtkColor3d color, double opacity)
{
    // Check if there are any elements to render
    if (indices.empty())
        return;

    // Create polygons
    vtkSmartPointer<vtkCellArray> polygons = createPolygons(indices);

    // Group polygons
    bool isPolys = indices.front().size() != 2;
    vtkNew<vtkPolyData> polyData;
    polyData->SetPoints(points);
    if (isPolys)
        polyData->SetPolys(polygons);
    else
        polyData->SetLines(polygons);

    // Build the mapper
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(polyData);

    // Create the actor
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(color.GetData());
    actor->GetProperty()->SetOpacity(opacity);
    actor->GetProperty()->SetLineWidth(mOptions.lineWidth);
    if (mOptions.showEdges)
    {
        actor->GetProperty()->SetEdgeColor(mOptions.edgeColor.GetData());
        actor->GetProperty()->SetEdgeOpacity(mOptions.edgeOpacity);
        actor->GetProperty()->EdgeVisibilityOn();
    }
    if (mOptions.showWireframe)
        actor->GetProperty()->SetRepresentationToWireframe();

    // Add the actor to the scene
    mRenderer->AddActor(actor);
}

//! Create points which are associated with the geometry
vtkSmartPointer<vtkPoints> GeometryView::createPoints(std::vector<Testlab::Node> const& nodes)
{
    vtkNew<vtkPoints> points;
    int numNodes = nodes.size();
    for (int i = 0; i != numNodes; ++i)
    {
        Vector3d position = convert3d(nodes[i].coordinates);
        position = position.cwiseProduct(mOptions.sceneScale);
        points->InsertPoint(i, position[0], position[1], position[2]);
    }
    return points;
}

//! Create polygons using given indices
vtkSmartPointer<vtkCellArray> GeometryView::createPolygons(std::vector<std::vector<int>> const& indices)
{
    vtkNew<vtkCellArray> polygons;
    int numElements = indices.size();
    for (int i = 0; i != numElements; ++i)
    {
        vtkNew<vtkPolygon> polygon;
        std::vector<int> const& elementIndices = indices[i];
        int numElementIndices = elementIndices.size();
        for (int j = 0; j != numElementIndices; ++j)
        {
            int iVertex = elementIndices[j];
            polygon->GetPointIds()->InsertNextId(iVertex);
        }
        polygons->InsertNextCell(polygon);
    }
    return polygons;
}

//! Estimate the maximum dimension of the model
double getMaximumDimension(Testlab::Geometry const& geometry)
{
    // Constants
    double const kInf = std::numeric_limits<double>::infinity();
    int numComponents = geometry.components.size();

    // Find the minimum and maximum coordinates
    Vector3d minCoords;
    Vector3d maxCoords;
    minCoords.fill(kInf);
    maxCoords.fill(-kInf);
    int numCoords = minCoords.size();
    for (int iComponent = 0; iComponent != numComponents; ++iComponent)
    {
        Testlab::Component const& component = geometry.components[iComponent];
        int numNodes = component.nodes.size();
        for (int iNode = 0; iNode != numNodes; ++iNode)
        {
            Testlab::Node const& node = component.nodes[iNode];
            for (int iCoord = 0; iCoord != numCoords; ++iCoord)
            {
                double coord = node.coordinates[iCoord];
                minCoords[iCoord] = std::min(minCoords[iCoord], coord);
                maxCoords[iCoord] = std::max(maxCoords[iCoord], coord);
            }
        }
    }

    // Estimate the dimensions
    double result = 0.0;
    result = std::max(result, std::abs(maxCoords[0] - minCoords[0]));
    result = std::max(result, std::abs(maxCoords[1] - minCoords[1]));
    result = std::max(result, std::abs(maxCoords[2] - minCoords[2]));

    return result;
}

//! Helper function to convert data to 3d vector
Vector3d convert3d(std::vector<double> const& data)
{
    Vector3d result;
    if (data.size() == result.size())
        std::copy(data.begin(), data.end(), result.begin());
    return result;
}

//! Helper function to construct a sphere actor
vtkSmartPointer<vtkActor> createSphereActor(Eigen::Vector3d const& position, double radius)
{
    // Construct the source to be rendered at each location
    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->SetRadius(radius);

    // Add the points
    vtkNew<vtkPoints> points;
    points->InsertNextPoint(position[0], position[1], position[2]);

    // Construct the polygons
    vtkNew<vtkPolyData> data;
    data->SetPoints(points);

    // Build up the mapper
    vtkNew<vtkGlyph3DMapper> mapper;
    mapper->SetInputData(data);
    mapper->SetSourceConnection(sphereSource->GetOutputPort());
    mapper->ScalarVisibilityOff();
    mapper->ScalingOff();

    // Create the actor
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    return actor;
}
