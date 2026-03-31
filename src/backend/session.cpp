#include <QList>
#include <QObject>

#include <testlab/api.h>

#include "session.h"

using namespace Backend::Core;

ResponseCollection::ResponseCollection()
{
}

ResponseCollection::~ResponseCollection()
{
}

int ResponseCollection::count() const
{
    return mBundles.size();
}

ResponseBundle const& ResponseCollection::get(int index) const
{
    return mBundles[index];
}

void ResponseCollection::add(Responses const& responses, QString name)
{
    if (name.isEmpty())
        name = QObject::tr("Bundle %1").arg(mBundles.size() + 1);
    ResponseBundle bundle;
    bundle.name = name;
    bundle.responses = responses;
    mBundles.push_back(bundle);
}

void ResponseCollection::merge(int index, Responses const& responses)
{
    if (index < 0 || index >= mBundles.size())
        return;
    ResponseBundle& bundle = mBundles[index];
    for (Testlab::Response const& v : responses)
        bundle.responses.push_back(v);
}

bool ResponseCollection::remove(int index)
{
    if (index >= 0 && index < mBundles.size())
    {
        mBundles.remove(index);
        return true;
    }
    return false;
}

void ResponseCollection::clear()
{
    mBundles.clear();
}

Session::Session()
    : mpProject(nullptr)
{
}

Session::~Session()
{
    closeProject();
}

//! Check if the project is valid
bool Session::isProjectValid()
{
    return mpProject && mpProject->isValid();
}

//! Open a Testlab project
bool Session::openProject(QString const& pathFile)
{
    closeProject();
    mpProject = Testlab::openProject(pathFile.toStdWString());
    return mpProject != nullptr;
}

//! Close the project, if opened
void Session::closeProject()
{
    if (!mpProject)
        return;
    delete mpProject;
    mpProject = nullptr;
}

//! Retrieve the current geometry
Testlab::Geometry Session::getGeometry()
{
    if (isProjectValid())
        return mpProject->getGeometry();
    return {};
}

//! Retrieve the responses using their paths inside the project
Responses Session::getResponses(QStringList const& paths)
{
    if (!isProjectValid())
        return {};
    int numPaths = paths.size();
    std::vector<std::wstring> cPaths(numPaths);
    for (int i = 0; i != numPaths; ++i)
        cPaths[i] = paths[i].toStdWString();
    return mpProject->getResponses(cPaths);
}

//! Retrieve the currently selected responses
Responses Session::getSelectedResponses()
{
    if (isProjectValid())
        return mpProject->getSelectedResponses();
    return {};
}
