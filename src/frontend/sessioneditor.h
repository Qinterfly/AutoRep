#ifndef SESSIONEDITOR_H
#define SESSIONEDITOR_H

#include <QLabel>
#include <QWidget>

#include "session.h"
#include "uialiasdata.h"

QT_FORWARD_DECLARE_CLASS(QSettings)
QT_FORWARD_DECLARE_CLASS(QListWidget)

namespace Frontend
{

class GeometryView;
class ResponseEditor;

class SessionEditor : public QWidget
{
    Q_OBJECT

public:
    SessionEditor(QSettings& settings, QWidget* pParent = nullptr);
    virtual ~SessionEditor() = default;

    QSize sizeHint() const;

    GeometryView* geometryView();
    ResponseEditor* responseEditor();
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

    // Response
    ResponseEditor* mpResponseEditor;
};

class ResponseEditor : public QWidget
{
    Q_OBJECT

public:
    ResponseEditor(Backend::Core::Session& session, QWidget* pParent = nullptr);
    virtual ~ResponseEditor() = default;

    Backend::Core::ResponseCollection const& collection() const;
    bool addBundle(Backend::Core::Responses const& responses);
    bool addBundle(QStringList const& paths);
    bool addSelectedBundle();
    bool mergeSelectedBundle();
    void removeBundle();
    void removeAllBundles();

signals:
    void edited();

private:
    void refresh();
    void createContent();
    QLayout* createBundleLayout();
    QLayout* createResponseLayout();

private:
    Backend::Core::Session& mSession;
    Backend::Core::ResponseCollection mCollection;
    QListWidget* mpBundleList;
    QListWidget* mpResponseList;
    QLabel* mpResponseCountLabel;
};
}

#endif // SESSIONEDITOR_H
