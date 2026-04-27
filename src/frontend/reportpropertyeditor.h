#ifndef REPORTPROPERTYEDITOR_H
#define REPORTPROPERTYEDITOR_H

#include <QUuid>
#include <QWidget>

#include "reportinterface.h"

class CustomVariantPropertyManager;
class QtVariantEditorFactory;
class QtTreePropertyBrowser;
class QtProperty;

namespace Backend::Core
{
class ReportPage;
class TextReportItem;
class GraphReportItem;
class TableReportItem;
}

namespace Frontend
{

class ReportPropertyEditor : public QWidget
{
    Q_OBJECT

public:
    enum Align
    {
        kTopRight,
        kBottomRight,
        kBottomLeft,
        kTopLeft,
    };
    enum Type
    {
        // Base
        kRect,
        kAngle,
        kFont,

        // Text
        kText,

        // Graph
        kXRange,
        kYRange,
        kXLabel,
        kYLabel,
        kScaleRange,
        kNumTicks,
        kGridWidth,
        kGridZeroWidth,
        kSwapAxes,
        kReverseX,
        kReverseY,
        kLegendAlign,
        kShowLegend,
        kShowBundleFreq,

        // Table
        kNumRows,
        kNumCols,
        kShowLabels
    };

    ReportPropertyEditor(QWidget* pParent = nullptr);
    virtual ~ReportPropertyEditor() = default;

    void refresh();
    void setItemGetter(Backend::Core::ReportItemGetter getter);

signals:
    void edited();

private:
    void createContent();
    void createConnections();
    void addBaseProperties(Backend::Core::ReportItem* pItem);
    void addGraphProperties(Backend::Core::GraphReportItem* pItem);
    void addTableProperties(Backend::Core::TableReportItem* pItem);
    void setValue(QtProperty* pProperty, QVariant value);
    Qt::Alignment getAlignValue(Align key);

private:
    Backend::Core::ReportItemGetter mItemGetter;
    CustomVariantPropertyManager* mpManager;
    QtVariantEditorFactory* mpFactory;
    QtTreePropertyBrowser* mpEditor;
};

}

#endif // REPORTPROPERTYEDITOR_H
