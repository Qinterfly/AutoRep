#ifndef MODEREPORTSCENEITEM_H
#define MODEREPORTSCENEITEM_H

#include <Eigen/Core>

#include <testlab/common.h>

#include <vtkColor.h>
#include <vtkPolyDataMapper.h>

#include "reportinterface.h"
#include "reportsceneitem.h"

class vtkPoints;
class vtkCellArray;
class vtkLookupTable;
class vtkDoubleArray;

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
    vtkColor3d edgeColor;
    vtkColor3d undeformedColor;

    // Opacity
    double edgeOpacity;

    // Flags
    bool showWireframe;
    bool showUndeformed;
    bool showLines;
    bool showTrias;
    bool showQuads;

    // Dimensions
    double lineWidth;
    double fontSize;
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
    void clear();
    void refresh();

protected:
    void paint(QPainter* pPainter, QStyleOptionGraphicsItem const* pOption, QWidget* pWidget) override;

private:
    void initialize();

    // Drawing
    void setView();
    void drawGeometry();
    void drawUndeformedState();
    void drawDeformedState();
    void drawElements(vtkSmartPointer<vtkPoints> points, std::vector<std::vector<int>> const& indices, int iShift, vtkColor3d color,
                      double opacity = 1.0, bool isEdgeVisible = true, bool isWireframe = false);
    void drawElements(vtkSmartPointer<vtkPoints> points, std::vector<std::vector<int>> const& indices, int iShift,
                      vtkSmartPointer<vtkDoubleArray> scalars, vtkSmartPointer<vtkLookupTable> lut);
    void drawAsImage(QString const& pathFile);
    void render(QPainter* pPainter);
    vtkSmartPointer<vtkPoints> createPoints(double scale = 0.0);
    vtkSmartPointer<vtkCellArray> createPolygons(std::vector<std::vector<int>> const& indices, int iShift);
    vtkSmartPointer<vtkDoubleArray> getMagnitudes(vtkSmartPointer<vtkPoints> points);
    Eigen::Vector3d getNodeValues(QString const& componentName, QString const& nodeName);

private:
    Backend::Core::ReportTextEngine& mTextEngine;
    Backend::Core::ResponseCollection const& mCollection;
    int const mISelectedBundle;
    Testlab::Geometry const& mGeometry;
    ModeOptions mOptions;

    // Data
    QHash<PairString, Eigen::Vector3d> mState;
    double mMaximumDimension;

    // VTK
    vtkSmartPointer<vtkRenderWindow> mRenderWindow;
    vtkSmartPointer<vtkRenderer> mRenderer;
};

}

#endif // MODEREPORTSCENEITEM_H
