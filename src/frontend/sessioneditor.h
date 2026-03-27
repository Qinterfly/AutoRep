#ifndef SESSIONEDITOR_H
#define SESSIONEDITOR_H

#include <QWidget>

#include "session.h"
#include "uialiasdata.h"

QT_FORWARD_DECLARE_CLASS(QSettings)

namespace Frontend
{

class GeometryView;

class SessionEditor : public QWidget
{
    Q_OBJECT

public:
    SessionEditor(QSettings& settings, QWidget* pParent = nullptr);
    virtual ~SessionEditor() = default;

    QSize sizeHint() const;

    bool openProject(QString const& pathFile);

private:
    void createContent();

    // Slots
    void openProjectDialog();

private:
    Backend::Core::Session mSession;
    QSettings& mSettings;

    // Project
    Edit1s* mpProjectPath;

    // Geometry
    GeometryView* mpGeometryView;
};

}

#endif // SESSIONEDITOR_H
