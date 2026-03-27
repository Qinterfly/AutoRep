#ifndef GEOMETRYVIEW_H
#define GEOMETRYVIEW_H

#include <QWidget>

#include <vtkColor.h>
#include <vtkPolyDataMapper.h>

#include <testlab/common.h>

class QVTKOpenGLNativeWidget;
class vtkCameraOrientationWidget;

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

private:
    void initialize();

    // Content
    void createContent();

private:
    Testlab::Geometry mGeometry;
    GeometryViewOptions mOptions;
    QVTKOpenGLNativeWidget* mRenderWidget;
    vtkSmartPointer<vtkRenderWindow> mRenderWindow;
    vtkSmartPointer<vtkRenderer> mRenderer;
    vtkSmartPointer<vtkCameraOrientationWidget> mOrientationWidget;
};

}

#endif // GEOMETRYVIEW_H
