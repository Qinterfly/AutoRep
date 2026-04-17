#include "reportsettings.h"
#include "reportitem.h"

using namespace Backend::Core;

//! Helper function to create default curves
QList<GraphReportCurve> generateDefaultCurves()
{
    QList<GraphReportCurve> result;
    result.emplaceBack("red", ReportMarkerShape::kCircle, true);
    result.emplaceBack("green", ReportMarkerShape::kCircle, false);
    result.emplaceBack("blue", ReportMarkerShape::kSquare, true);
    result.emplaceBack("orange", ReportMarkerShape::kSquare, false);
    result.emplaceBack("cyan", ReportMarkerShape::kTriangle, true);
    result.emplaceBack("magenta", ReportMarkerShape::kTriangle, false);
    result.emplaceBack("gray", ReportMarkerShape::kTriangleInverted, true);
    result.emplaceBack("purple", ReportMarkerShape::kTriangleInverted, false);
    result.emplaceBack("brown", ReportMarkerShape::kCross);
    result.emplaceBack("chocolate", ReportMarkerShape::kPlus);
    result.emplaceBack("olive", ReportMarkerShape::kStar);
    result.emplaceBack("steelblue", ReportMarkerShape::kCrossSquare);
    result.emplaceBack("firebrick", ReportMarkerShape::kPlusSquare);
    return result;
}

QList<GraphReportCurve> ReportSettings::curves = generateDefaultCurves();
