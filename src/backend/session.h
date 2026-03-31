#ifndef SESSION_H
#define SESSION_H

#include <QList>
#include <QString>

namespace Testlab
{
class IProject;
struct Geometry;
struct Response;
}

namespace Backend::Core
{

using Responses = std::vector<Testlab::Response>;

struct ResponseBundle
{
    QString name;
    Responses responses;
};

class ResponseCollection
{
public:
    ResponseCollection();
    ~ResponseCollection();

    int count() const;
    ResponseBundle const& get(int index) const;
    void add(Responses const& responses, QString name = QString());
    void merge(int index, Responses const& responses);
    bool remove(int index);
    void clear();

private:
    QList<ResponseBundle> mBundles;
};

class Session
{
public:
    Session();
    ~Session();

    bool isProjectValid();

    bool openProject(QString const& pathFile);
    void closeProject();
    Testlab::Geometry getGeometry();
    Responses getResponses(QStringList const& paths);
    Responses getSelectedResponses();

private:
    Testlab::IProject* mpProject;
};

}

#endif // SESSION_H
