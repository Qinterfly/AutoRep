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

    int count() const;
    ResponseBundle& get(int index);
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
