#ifndef MATHUTILITY_H
#define MATHUTILITY_H

#include <QList>

#include <testlab/api.h>

#include "reportitem.h"

namespace Backend::Core
{
class ResponseBundle;
}

namespace Backend::Utility
{

// Common
QList<double> convert(std::vector<double> const& data);
int findClosestKey(Testlab::Response const& response, double searchKey);
QString getDirLabel(Backend::Core::ReportDirection dir);

// Response
int findResponse(Backend::Core::ResponseBundle const& bundle, Backend::Core::GraphReportPoint const& point, Backend::Core::ReportDirection dir,
                 Testlab::ResponseType type, QString const& unit = QString());
Testlab::Response getAcceleration(Backend::Core::ResponseBundle const& bundle, Backend::Core::GraphReportPoint const& point,
                                  Backend::Core::GraphReportItem* pItem);
std::vector<double> getCoords(Testlab::Geometry const& geometry, Backend::Core::GraphReportPoint const& point);

// Roots
struct Root
{
    double key;
    double value;
    int index;
};
std::vector<Root> findRoots(QList<double> const& keys, QList<double> const& values);
}

#endif // MATHUTILITY_H
