#ifndef REPORTPROPERTYEDITOR_H
#define REPORTPROPERTYEDITOR_H

#include <QWidget>

class CustomVariantPropertyManager;
class QtVariantEditorFactory;
class QtTreePropertyBrowser;
class QtProperty;

namespace Backend::Core
{
class ReportItem;
}

namespace Frontend
{

class ReportPropertyEditor : public QWidget
{
    Q_OBJECT

public:
    enum Alignment
    {
        kTopRight,
        kBottomRight,
        kBottomLeft,
        kTopLeft,
    };
    enum Type
    {
        kRect,
        kAngle,
        kFont,
        kText,
        kXLabel,
        kYLabel,
        kScaleRange,
        kNumTicks,
        kGridWidth,
        kLegendAlignment,
        kShowLegend,
        kShowBundleFreq
    };

    ReportPropertyEditor(QWidget* pParent = nullptr);
    virtual ~ReportPropertyEditor() = default;

    void refresh();
    void setItem(Backend::Core::ReportItem* pItem);

signals:
    void edited();

private:
    void createContent();
    void createConnections();
    void addBaseProperties();
    void addTextProperties();
    void addGraphProperties();
    void setValue(QtProperty* pProperty, QVariant value);
    Qt::Alignment getAlignmentValue(Alignment key);

private:
    Backend::Core::ReportItem* mpItem;
    CustomVariantPropertyManager* mpManager;
    QtVariantEditorFactory* mpFactory;
    QtTreePropertyBrowser* mpEditor;
};

}

#endif // REPORTPROPERTYEDITOR_H
