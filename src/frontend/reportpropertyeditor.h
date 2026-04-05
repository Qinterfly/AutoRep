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
    enum Type
    {
        kRect,
        kAngle,
        kFont,
        kText,
        kXLabel,
        kYLabel
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

private:
    Backend::Core::ReportItem* mpItem;
    CustomVariantPropertyManager* mpManager;
    QtVariantEditorFactory* mpFactory;
    QtTreePropertyBrowser* mpEditor;
};

}

#endif // REPORTPROPERTYEDITOR_H
