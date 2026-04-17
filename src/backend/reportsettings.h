#ifndef REPORTSETTINGS_H
#define REPORTSETTINGS_H

#include <QList>

namespace Backend::Core
{

class GraphReportCurve;

class ReportSettings
{
public:
    ReportSettings() = delete;

    static QList<GraphReportCurve> curves;
};

}

#endif // REPORTSETTINGS_H
