#ifndef REPORTWORKSPACE_H
#define REPORTWORKSPACE_H

#include <QWidget>

#include "reportdocument.h"

QT_FORWARD_DECLARE_CLASS(QSettings)

namespace Frontend
{

class CustomTabWidget;
class ReportDesigner;

class ReportWorkspace : public QWidget
{
    Q_OBJECT

public:
    ReportWorkspace(QSettings& settings, QWidget* pParent = nullptr);
    virtual ~ReportWorkspace() = default;

    QSize sizeHint() const;
    ReportDesigner* designer(int iPage);

    bool writePage(QString const& pathFile, int iPage);
    bool writeAll(QString const& pathFile);

private:
    void createContent();
    void initialize();
    void refresh();

private:
    QSettings& mSettings;
    Backend::Core::ReportDocument mDocument;
    CustomTabWidget* mpDesignerTabs;
};

}

#endif // REPORTWORKSPACE_H
