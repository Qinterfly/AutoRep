#include <QPainter>
#include <QTemporaryFile>

#include <vtkCamera.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkPNGWriter.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolygon.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkScalarBarActor.h>
#include <vtkTextProperty.h>
#include <vtkWindowToImageFilter.h>

#include "mathutility.h"
#include "modereportsceneitem.h"
#include "reportitem.h"
#include "reporttextengine.h"
#include "session.h"
#include "uiutility.h"

using namespace Backend::Core;
using namespace Frontend;
using namespace Eigen;

// Constants
vtkNew<vtkNamedColors> const vtkColors;

// Constants
static double const skEps = std::numeric_limits<double>::epsilon();

ModeOptions::ModeOptions()
{
    // Color scheme
    sceneColor = vtkColors->GetColor3d("white");
    edgeColor = vtkColors->GetColor3d("gainsboro");
    undeformedColor = vtkColors->GetColor3d("grey");

    // Opacity
    edgeOpacity = 0.5;

    // Flags
    showUndeformed = true;
    showLines = true;
    showTrias = true;
    showQuads = true;

    // Dimensions
    lineWidth = 2.0;
    fontSize = 12;
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
    initialize();
    setState();
}

ModeReportSceneItem::~ModeReportSceneItem()
{
}

//! Set the item state
void ModeReportSceneItem::setState()
{
    // Reset the state
    mState.clear();

    // Get the report item
    if (!mpItem)
        return;
    ModeReportItem* pItem = (ModeReportItem*) mpItem;

    // Get the currently active bundle of responses
    if (mCollection.isEmpty())
        return;
    ResponseBundle const& bundle = mISelectedBundle >= 0 ? mCollection.get(mISelectedBundle) : mCollection.get(0);
    if (bundle.freq < skEps)
    {
        qWarning() << tr("The bundle %1 has zero frequency. Skipping rendering the modeshape").arg(bundle.name);
        return;
    }

    // Set the vertex field
    int numResponses = bundle.responses.size();
    int iFound = -1;
    for (int i = 0; i != numResponses; ++i)
    {
        // Retrieve the acceleration which has the requested units
        Testlab::Response const& response = bundle.responses[i];
        if (response.header.type != Testlab::ResponseType::kAccel)
            continue;
        Testlab::Response accel = Backend::Utility::convertAcceleration(bundle, response, pItem->unit);
        int numKeys = accel.keys.size();
        if (numKeys == 0)
            continue;

        // Find the closest frequency to the resonance one
        if (iFound < 0 || iFound > numKeys)
            iFound = Backend::Utility::findClosestKey(accel, bundle.freq);
        if (iFound < 0)
            continue;

        // Set the field value
        QString componentName = QString::fromStdWString(accel.header.point.component);
        QString nodeName = QString::fromStdWString(accel.header.point.node);
        Vector3d value = Backend::Utility::projectResponse(accel, mGeometry, iFound).imag();
        PairString key(componentName, nodeName);
        if (!mState.contains(key))
            mState[key] = Vector3d::Zero();
        mState[key] += value;
    }
}

//! Clean up the scene
void ModeReportSceneItem::clear()
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

//! Update the scene
void ModeReportSceneItem::refresh()
{
    mRenderWindow->Render();
}

//! Render the item to the painter
void ModeReportSceneItem::render(QPainter* pPainter)
{
    // Constants
    qreal const kInchToMM = 25.4;

    // Check if the item is valid
    if (!mpItem)
        return;

    // Set the window size
    double dpi = pPainter->device()->logicalDpiX();
    auto mmToPx = [dpi, kInchToMM](double mm) { return mm * dpi / kInchToMM; };
    QSize pxSize(mmToPx(mpItem->rect.width()), mmToPx(mpItem->rect.height()));
    mRenderWindow->SetSize(pxSize.width(), pxSize.height());

    // Draw the scene
    clear();
    drawGeometry();
    setView();

    // Write it to the file
    QTemporaryFile file;
    QString pathFile;
    if (file.open())
    {
        pathFile = file.fileName();
        drawAsImage(pathFile);
    }

    // Set the painter
    pPainter->save();
    pPainter->translate(mpItem->rect.center());
    pPainter->rotate(mpItem->angle);
    pPainter->translate(-mpItem->rect.center());

    // Draw the picture
    QPixmap pixmap(pathFile);
    if (!pixmap.isNull())
        pPainter->drawPixmap(mpItem->rect, pixmap);

    // Restore the painter
    pPainter->restore();
}

//! Process paint event
void ModeReportSceneItem::paint(QPainter* pPainter, QStyleOptionGraphicsItem const* pOption, QWidget* pWidget)
{
    render(pPainter);
    ReportSceneItem::paint(pPainter, pOption, pWidget);
}

//! Set the initial state of widgets
void ModeReportSceneItem::initialize()
{
    // Specify the format for the VTK library
    vtkObject::GlobalWarningDisplayOff();

    // Set up the scene
    mRenderer = vtkRenderer::New();
    mRenderer->SetBackground(mOptions.sceneColor.GetData());
    mRenderer->GradientBackgroundOff();
    mRenderer->ResetCamera();

    // Create the window
    mRenderWindow = vtkRenderWindow::New();
    mRenderWindow->SetOffScreenRendering(true);
    mRenderWindow->AddRenderer(mRenderer);
}

//! Set the camera position as well as zoom
void ModeReportSceneItem::setView()
{
    ModeReportItem* pItem = (ModeReportItem*) mpItem;

    // Set the camera position
    switch (pItem->view)
    {
    case ReportView::kFront:
        Utility::setPlaneView(mRenderer, 0, 1);
        break;
    case ReportView::kRear:
        Utility::setPlaneView(mRenderer, 0, -1);
        break;
    case ReportView::kTop:
        Utility::setPlaneView(mRenderer, 1, 1);
        break;
    case ReportView::kBottom:
        Utility::setPlaneView(mRenderer, 1, -1);
        break;
    case ReportView::kLeft:
        Utility::setPlaneView(mRenderer, 2, 1);
        break;
    case ReportView::kRight:
        Utility::setPlaneView(mRenderer, 2, -1);
        break;
    case ReportView::kIsometric:
        Utility::setIsometricView(mRenderer);
        break;
    case ReportView::kCustom:
        // TODO
        break;
    default:
        break;
    }

    // Set the zoom
    mRenderer->GetActiveCamera()->Zoom(pItem->zoom);

    // Update the scene
    refresh();
}

//! Represent geometry
void ModeReportSceneItem::drawGeometry()
{
    // Estimate the maximum dimension
    mMaximumDimension = Backend::Utility::getMaximumDimension(mGeometry);
    if (mMaximumDimension < skEps)
        mMaximumDimension = 1.0;

    // Render the undeformed state
    if (mOptions.showUndeformed)
        drawUndeformedState();

    // Render the deformed state
    drawDeformedState();
}

//! Represent the initial configuration
void ModeReportSceneItem::drawUndeformedState()
{
    // Construct the vertices
    vtkSmartPointer<vtkPoints> points = createPoints();

    // Loop through all the components
    int numComponents = mGeometry.components.size();
    int iShift = 0;
    for (int i = 0; i != numComponents; ++i)
    {
        Testlab::Component const& component = mGeometry.components[i];

        mOptions.showWireframe = true;
        // Draw the elements
        if (mOptions.showLines)
            drawElements(points, component.lines, iShift, mOptions.undeformedColor, 1.0, true, true);
        if (mOptions.showTrias)
            drawElements(points, component.trias, iShift, mOptions.undeformedColor, 1.0, true, true);
        if (mOptions.showQuads)
            drawElements(points, component.quads, iShift, mOptions.undeformedColor, 1.0, true, true);

        mOptions.showWireframe = false;

        // Increase the counter
        iShift += component.nodes.size();
    }
}

//! Represent the vertex field
void ModeReportSceneItem::drawDeformedState()
{
    ModeReportItem* pItem = (ModeReportItem*) mpItem;
    if (!pItem)
        return;

    // Create the colormap
    vtkSmartPointer<vtkLookupTable> lut = Utility::createBlueToRedColorMap();

    // Set the relative mode scale
    double scale = pItem->scale * mMaximumDimension;

    // Construct the vertices
    vtkSmartPointer<vtkPoints> points = createPoints(scale);

    // Compute the magnitudes
    vtkSmartPointer<vtkDoubleArray> magnitudes = getMagnitudes(points);

    // Loop through all the components
    int numComponents = mGeometry.components.size();
    int iShift = 0;
    for (int i = 0; i != numComponents; ++i)
    {
        Testlab::Component const& component = mGeometry.components[i];

        // Draw the elements
        if (mOptions.showLines)
            drawElements(points, component.lines, iShift, magnitudes, lut);
        if (mOptions.showTrias)
            drawElements(points, component.trias, iShift, magnitudes, lut);
        if (mOptions.showQuads)
            drawElements(points, component.quads, iShift, magnitudes, lut);

        // Increase the counter
        iShift += component.nodes.size();
    }

    // Show the scalar bar
    int maxWidth = ceil((double) mRenderWindow->GetSize()[0] / 5);
    vtkNew<vtkScalarBarActor> scalarBar;
    scalarBar->SetLabelFormat("%5.2f");
    scalarBar->GetLabelTextProperty()->SetShadow(false);
    scalarBar->GetLabelTextProperty()->SetBold(false);
    scalarBar->GetLabelTextProperty()->SetColor(vtkColors->GetColor3d("black").GetData());
    scalarBar->SetLookupTable(lut);
    scalarBar->SetNumberOfLabels(4);
    scalarBar->SetMaximumWidthInPixels(maxWidth);
    scalarBar->SetPosition(0.9, 0.05);
    scalarBar->SetPosition2(0.95, 0.6);
    mRenderer->AddViewProp(scalarBar);
}

//! Render elements using one color
void ModeReportSceneItem::drawElements(vtkSmartPointer<vtkPoints> points, std::vector<std::vector<int>> const& indices, int iShift,
                                       vtkColor3d color, double opacity, bool isEdgeVisible, bool isWireframe)
{
    // Check if there are any elements to render
    if (indices.empty())
        return;

    // Create polygons
    vtkSmartPointer<vtkCellArray> polygons = createPolygons(indices, iShift);

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

    // Set the offset
    mapper->SetResolveCoincidentTopologyToPolygonOffset();
    if (isPolys)
        mapper->SetResolveCoincidentTopologyPolygonOffsetParameters(1.0, 1.0);
    else
        mapper->SetResolveCoincidentTopologyLineOffsetParameters(1.0, -1.0);

    // Create the actor
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(color.GetData());
    actor->GetProperty()->SetOpacity(opacity);
    actor->GetProperty()->SetLineWidth(mOptions.lineWidth);
    if (isEdgeVisible)
    {
        actor->GetProperty()->SetEdgeColor(mOptions.edgeColor.GetData());
        actor->GetProperty()->SetEdgeOpacity(mOptions.edgeOpacity);
        actor->GetProperty()->EdgeVisibilityOn();
    }
    if (isWireframe)
        actor->GetProperty()->SetRepresentationToWireframe();

    // Add the actor to the scene
    mRenderer->AddActor(actor);
}

//! Render color interpolated elements
void ModeReportSceneItem::drawElements(vtkSmartPointer<vtkPoints> points, std::vector<std::vector<int>> const& indices, int iShift,
                                       vtkSmartPointer<vtkDoubleArray> scalars, vtkSmartPointer<vtkLookupTable> lut)
{
    // Check if there are any elements to render
    if (indices.empty())
        return;

    // Create polygons
    vtkSmartPointer<vtkCellArray> polygons = createPolygons(indices, iShift);

    // Group polygons
    bool isPolys = indices.front().size() != 2;
    vtkNew<vtkPolyData> polyData;
    polyData->SetPoints(points);
    if (isPolys)
        polyData->SetPolys(polygons);
    else
        polyData->SetLines(polygons);
    polyData->GetPointData()->SetScalars(scalars);

    // Build the mapper
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(polyData);
    mapper->SetScalarRange(scalars->GetRange()[0], scalars->GetRange()[1]);
    mapper->SetLookupTable(lut);

    // Create the actor and add to the scene
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetLineWidth(mOptions.lineWidth);
    actor->GetProperty()->SetEdgeColor(mOptions.edgeColor.GetData());
    actor->GetProperty()->SetEdgeOpacity(mOptions.edgeOpacity);
    actor->GetProperty()->EdgeVisibilityOn();
    if (mOptions.showWireframe)
        actor->GetProperty()->SetRepresentationToWireframe();

    // Add the actor to the scene
    mRenderer->AddActor(actor);
}

//! Create points which are associated with the geometry
vtkSmartPointer<vtkPoints> ModeReportSceneItem::createPoints(double scale)
{
    vtkNew<vtkPoints> points;
    int numPoints = 0;
    int numComponents = mGeometry.components.size();
    for (int iComponent = 0; iComponent != numComponents; ++iComponent)
    {
        Testlab::Component const& component = mGeometry.components[iComponent];
        QString componentName = QString::fromStdWString(component.name);
        int numNodes = component.nodes.size();
        for (int iNode = 0; iNode != numNodes; ++iNode)
        {
            Testlab::Node const& node = component.nodes[iNode];
            QString nodeName = QString::fromStdWString(node.name);

            // Get the nodal position
            Vector3d position = Utility::convert3d(node.coordinates);

            // Apply the values
            Vector3d values = getNodeValues(componentName, nodeName);
            position += values * scale;

            // Add the point
            points->InsertPoint(numPoints, position[0], position[1], position[2]);
            ++numPoints;
        }
    }
    return points;
}

//! Create polygons using given indices
vtkSmartPointer<vtkCellArray> ModeReportSceneItem::createPolygons(std::vector<std::vector<int>> const& indices, int iShift)
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
            int iVertex = iShift + elementIndices[j];
            polygon->GetPointIds()->InsertNextId(iVertex);
        }
        polygons->InsertNextCell(polygon);
    }
    return polygons;
}

//! Get magnitudes at each node
vtkSmartPointer<vtkDoubleArray> ModeReportSceneItem::getMagnitudes(vtkSmartPointer<vtkPoints> points)
{
    // Allocate the result
    vtkNew<vtkDoubleArray> magnitudes;
    magnitudes->SetNumberOfTuples(points->GetNumberOfPoints());

    // Loop through all the nodes
    int numComponents = mGeometry.components.size();
    int numPoints = 0;
    for (int i = 0; i != numComponents; ++i)
    {
        Testlab::Component const& component = mGeometry.components[i];
        QString componentName = QString::fromStdWString(component.name);
        int numNodes = component.nodes.size();
        for (int iNode = 0; iNode != numNodes; ++iNode)
        {
            Testlab::Node const& node = component.nodes[iNode];
            QString nodeName = QString::fromStdWString(node.name);

            // Find the maximum absolute node value
            Vector3d values = getNodeValues(componentName, nodeName);
            double magnitude = 0.0;
            int numValues = values.size();
            for (int iValue = 0; iValue != numValues; ++iValue)
                magnitude = std::max(magnitude, std::abs(values[iValue]));

            // Set the magnitude
            magnitudes->SetValue(numPoints, magnitude);
            ++numPoints;
        }
    }
    return magnitudes;
}

//! Get the vertex field value related to the node
Eigen::Vector3d ModeReportSceneItem::getNodeValues(QString const& componentName, QString const& nodeName)
{
    Vector3d result = Vector3d::Zero();
    PairString key(componentName, nodeName);
    if (mState.contains(key))
        result = mState[key];
    return result;
}

//! Render the current scene to an image
void ModeReportSceneItem::drawAsImage(QString const& pathFile)
{
    ModeReportItem* pItem = (ModeReportItem*) mpItem;

    // Construct an image
    vtkNew<vtkWindowToImageFilter> filter;
    filter->SetInput(mRenderWindow);
    filter->SetScale(pItem->quality);
    filter->SetInputBufferTypeToRGBA();
    filter->ReadFrontBufferOff();
    filter->Update();

    // Write a file
    vtkNew<vtkPNGWriter> writer;
    writer->SetFileName(pathFile.toStdString().c_str());
    writer->SetInputConnection(filter->GetOutputPort());
    writer->Write();
}
