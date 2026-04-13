#ifndef REPORTWORKSPACE_H
#define REPORTWORKSPACE_H

#include <QWidget>

#include "reportdocument.h"

QT_FORWARD_DECLARE_CLASS(QSettings)

namespace Backend::Core
{
class ResponseCollection;
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
    ReportWorkspace(QSettings& settings, GeometryView* pGeometryView, Backend::Core::ResponseCollection const& collection,
                    QWidget* pParent = nullptr);
    virtual ~ReportWorkspace() = default;

    QSize sizeHint() const;
    ReportDesigner* designer(int iPage);
    ReportDesigner* designer(QString const& name);
    void refresh();

    bool print(QString const& pathFile, int iPage);
    bool print(QString const& pathFile);
    bool printDialog();

private:
    void createContent();
    void initialize();
    void rebuild();

private:
    QSettings& mSettings;
    GeometryView* mpGeometryView;
    Backend::Core::ResponseCollection const& mCollection;
    Backend::Core::ReportDocument mDocument;
    CustomTabWidget* mpDesignerTabs;
};

}

#endif // REPORTWORKSPACE_H
