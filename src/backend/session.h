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
    ResponseBundle();
    ResponseBundle(QString const& uName, Responses const& uResponses);
    ~ResponseBundle() = default;

    QString name;
    Responses responses;
    double freq;
    double force;
};

class ResponseCollection
{
public:
    ResponseCollection();
    ~ResponseCollection() = default;

    bool isEmpty() const;
    int count() const;
    ResponseBundle& get(int index);
    ResponseBundle const& get(int index) const;
    ResponseBundle& add(Responses const& responses, QString const& name);
    void add(ResponseBundle const& bundle);
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
