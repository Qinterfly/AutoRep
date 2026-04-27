#ifndef REPORTINTERFACE_H
#define REPORTINTERFACE_H

#include <QPair>

#include <functional>

QT_FORWARD_DECLARE_CLASS(QJsonObject)

using PairInt = QPair<int, int>;
using PairDouble = QPair<double, double>;

namespace Backend::Core
{

class ReportItem;
class GraphReportCurve;

using ReportItemGetter = std::function<ReportItem*()>;
using ReportCurveGetter = std::function<GraphReportCurve*()>;

class ISerializable
{
public:
    virtual ~ISerializable() = default;

    virtual QJsonObject toJson() const = 0;
    virtual void fromJson(QJsonObject const& obj) = 0;
};

}

#endif // REPORTINTERFACE_H
