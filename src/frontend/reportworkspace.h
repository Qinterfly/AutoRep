#ifndef REPORTWORKSPACE_H
#define REPORTWORKSPACE_H

#include <QWidget>

#include "reportdocument.h"

QT_FORWARD_DECLARE_CLASS(QSettings)

namespace Backend::Core
{
class GraphReportCurve;
}

namespace Frontend
{

class CustomTabWidget;
class ReportDesigner;
class GeometryView;

class ReportWorkspace : public QWidget
{
    Q_OBJECT

public:
    ReportWorkspace(QSettings& settings, GeometryView* pGeometryView, QWidget* pParent = nullptr);
    virtual ~ReportWorkspace() = default;

    QSize sizeHint() const;
    ReportDesigner* designer(int iPage);
    ReportDesigner* designer(QString const& name);

    bool print(QString const& pathFile, int iPage);
    bool print(QString const& pathFile);

private:
    void createContent();
    void initialize();
    void refresh();

    // Slots
    void processSelectCurve(Backend::Core::GraphReportCurve curve);
    void processAddCurve(int iPage);
    void processEditCurve(int iPage);

private:
    QSettings& mSettings;
    GeometryView* mpGeometryView;
    Backend::Core::ReportDocument mDocument;
    CustomTabWidget* mpDesignerTabs;
};

}

#endif // REPORTWORKSPACE_H
