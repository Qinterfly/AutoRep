#ifndef SESSION_H
#define SESSION_H

#include <QString>

namespace Testlab
{
class IProject;
struct Geometry;
}

namespace Backend::Core
{

class Session
{
public:
    Session();
    ~Session();

    bool isProjectValid();

    bool openProject(QString const& pathFile);
    void closeProject();
    Testlab::Geometry getGeometry();

private:
    Testlab::IProject* mpProject;
};

}

#endif // SESSION_H
