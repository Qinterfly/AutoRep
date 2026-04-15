#ifndef REPORTWORKSPACE_H
#define REPORTWORKSPACE_H

#include <QWidget>

#include "reportdocument.h"

QT_FORWARD_DECLARE_CLASS(QSettings)

namespace Frontend
{

class CustomTabWidget;
class ReportDesigner;
class GeometryView;
class ResponseEditor;

class ReportWorkspace : public QWidget
{
    Q_OBJECT

public:
    ReportWorkspace(QSettings& settings, GeometryView* pGeometryView, ResponseEditor* pResponseEditor, QWidget* pParent = nullptr);
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

    // Slots
    void setDefaultDocument();

private:
    QSettings& mSettings;
    GeometryView* mpGeometryView;
    ResponseEditor* mpResponseEditor;
    Backend::Core::ReportDocument mDocument;
    CustomTabWidget* mpDesignerTabs;
};

}

#endif // REPORTWORKSPACE_H
