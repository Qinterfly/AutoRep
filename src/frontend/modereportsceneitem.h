#ifndef MODEREPORTSCENEITEM_H
#define MODEREPORTSCENEITEM_H

#include <vtkColor.h>
#include <vtkPolyDataMapper.h>

#include "reportsceneitem.h"

class QVTKOpenGLNativeWidget;
class vtkCameraOrientationWidget;

namespace Testlab
{
class Geometry;
}

namespace Backend::Core
{
class ResponseCollection;
class ModeReportItem;
}

namespace Frontend
{

//! Rendering options
struct ModeOptions
{
    ModeOptions();
    ~ModeOptions() = default;

    // Color scheme
    vtkColor3d sceneColor;
    vtkColor3d sceneColor2;
};

//! Class to render report mode items
class ModeReportSceneItem : public ReportSceneItem
{
    Q_OBJECT

public:
    ModeReportSceneItem(Backend::Core::ModeReportItem* pItem, Backend::Core::ReportTextEngine& textEngine,
                        Backend::Core::ResponseCollection const& collection, int iSelectedBundle, Testlab::Geometry const& geometry,
                        ModeOptions const& options = ModeOptions(), QGraphicsItem* pParent = nullptr);
    virtual ~ModeReportSceneItem();

    void setState();

protected:
    void paint(QPainter* pPainter, QStyleOptionGraphicsItem const* pOption, QWidget* pWidget) override;

private:
    void initialize();

    // Content
    void createContent();

private:
    // Data
    Backend::Core::ReportTextEngine& mTextEngine;
    Backend::Core::ResponseCollection const& mCollection;
    int const mISelectedBundle;
    Testlab::Geometry const& mGeometry;
    ModeOptions mOptions;
    // VTK
    QVTKOpenGLNativeWidget* mRenderWidget;
    vtkSmartPointer<vtkRenderWindow> mRenderWindow;
    vtkSmartPointer<vtkRenderer> mRenderer;
    vtkSmartPointer<vtkCameraOrientationWidget> mOrientationWidget;
};

}

#endif // MODEREPORTSCENEITEM_H
