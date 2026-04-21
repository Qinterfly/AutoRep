#ifndef REPORTDEFAULTS_H
#define REPORTDEFAULTS_H

#include <QList>

namespace Backend::Core
{

class GraphReportCurve;
class ReportPage;

class ReportDefaults
{
public:
    ReportDefaults() = delete;

    static QList<GraphReportCurve> curves();
    static ReportPage imRePage();
    static ReportPage multiImRePage();
    static ReportPage freqAmpPage();
    static ReportPage modeshapePage();
};

}

#endif // REPORTDEFAULTS_H
