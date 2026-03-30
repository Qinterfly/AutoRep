#ifndef GEOMETRYVIEW_H
#define GEOMETRYVIEW_H

#include <QWidget>

#include <Eigen/Core>

#include <vtkColor.h>
#include <vtkPolyDataMapper.h>

#include <testlab/common.h>

class QVTKOpenGLNativeWidget;
class vtkCameraOrientationWidget;
class vtkPoints;
class vtkCellArray;

namespace Frontend
{

//! Rendering options
struct GeometryViewOptions
{
    GeometryViewOptions();
    ~GeometryViewOptions() = default;

    // Color scheme
    vtkColor3d sceneColor;
    vtkColor3d sceneColor2;
    vtkColor3d edgeColor;
    QList<vtkColor3d> componentColors;

    // Opacity
    double edgeOpacity;

    // Flags
    bool showEdges;
    bool showWireframe;
    bool showVertices;
    bool showLines;
    bool showTrias;
    bool showQuads;

    // Scales
    Eigen::Vector3d sceneScale;

    // Dimensions
    double lineWidth;
    double pointScale;
};

class GeometryView : public QWidget
{
    Q_OBJECT

public:
    GeometryView(GeometryViewOptions const& options = GeometryViewOptions());
    virtual ~GeometryView();

    void clear();
    void plot();
    void refresh();

    void setGeometry(Testlab::Geometry geometry);
    void setIsometricView();

private:
    void initialize();

    // Content
    void createContent();

    // Drawing
    void drawGeometry();
    void drawComponent(Testlab::Component const& component, vtkColor3d color);
    void drawVertices(vtkSmartPointer<vtkPoints> points, vtkColor3d color, double opacity = 1.0);
    void drawElements(vtkSmartPointer<vtkPoints> points, std::vector<std::vector<int>> const& indices, vtkColor3d color, double opacity = 1.0);
    vtkSmartPointer<vtkPoints> createPoints(std::vector<Testlab::Node> const& nodes);
    vtkSmartPointer<vtkCellArray> createPolygons(std::vector<std::vector<int>> const& indices);

private:
    Testlab::Geometry mGeometry;
    GeometryViewOptions mOptions;
    QVTKOpenGLNativeWidget* mRenderWidget;
    vtkSmartPointer<vtkRenderWindow> mRenderWindow;
    vtkSmartPointer<vtkRenderer> mRenderer;
    vtkSmartPointer<vtkCameraOrientationWidget> mOrientationWidget;
    double mMaximumDimension;
};

}

#endif // GEOMETRYVIEW_H
