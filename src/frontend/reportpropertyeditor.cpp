#include <QVBoxLayout>

#include <customvariantpropertymanager.h>
#include <qttreepropertybrowser.h>
#include <qtvariantproperty.h>

#include "reportdocument.h"
#include "reportpropertyeditor.h"
#include "uiutility.h"

using namespace Backend::Core;
using namespace Frontend;

ReportPropertyEditor::ReportPropertyEditor(QWidget* pParent)
    : QWidget(pParent)
    , mpItem(nullptr)
{
    setFont(Utility::getFont());
    createContent();
    createConnections();
}

//! Set the item to edit
void ReportPropertyEditor::setItem(ReportItem* pItem)
{
    mpItem = pItem;
    refresh();
}

//! Create all the widgets
void ReportPropertyEditor::createContent()
{
    // Create the widgets
    mpManager = new CustomVariantPropertyManager;
    mpFactory = new QtVariantEditorFactory;
    mpEditor = new QtTreePropertyBrowser;

    // Initialize the widgets
    mpEditor->setFactoryForManager((QtVariantPropertyManager*) mpManager, mpFactory);
    mpEditor->setFont(font());
    mpEditor->setTreeWidgetFont(font());

    // Combine the widgets
    QVBoxLayout* pLayout = new QVBoxLayout;
    pLayout->addWidget(mpEditor);
    setLayout(pLayout);
}

//! Specify connections between widgets
void ReportPropertyEditor::createConnections()
{
    connect(mpManager, &CustomVariantPropertyManager::valueChanged, this, &ReportPropertyEditor::setValue);
}

//! Update the widgets content
void ReportPropertyEditor::refresh()
{
    // Clear all the properties
    QSignalBlocker blockerEditor(mpEditor);
    QSignalBlocker blockerManager(mpManager);
    mpEditor->clear();
    mpManager->clear();

    // Check if the item is valid
    if (!mpItem)
        return;

    // Add properties
    addBaseProperties();
    switch (mpItem->type())
    {
    case ReportItem::kText:
        addTextProperties();
        break;
    case ReportItem::kGraph:
        addGraphProperties();
        break;
    default:
        break;
    }
}

//! Create the properties which are common for all item types
void ReportPropertyEditor::addBaseProperties()
{
    QtVariantProperty* pNameProperty = mpManager->addProperty(kName, QVariant::String, tr("Name"));
    pNameProperty->setValue(mpItem->name);
    mpEditor->addProperty(pNameProperty);

    QtVariantProperty* pRectProperty = mpManager->addProperty(kRect, QVariant::Rect, tr("Position"));
    pRectProperty->setValue(mpItem->rect);
    mpEditor->addProperty(pRectProperty);

    QtVariantProperty* pAngleProperty = mpManager->addProperty(kAngle, QVariant::Double, tr("Rotation"));
    pAngleProperty->setValue(mpItem->angle);
    mpEditor->addProperty(pAngleProperty);

    QtVariantProperty* pFontProperty = mpManager->addProperty(kFont, QVariant::Font, tr("Font"));
    pFontProperty->setValue(mpItem->font);
    QtBrowserItem* pFontItem = mpEditor->addProperty(pFontProperty);
    mpEditor->setExpanded(pFontItem, false);
}

//! Create properties specific for text items
void ReportPropertyEditor::addTextProperties()
{
    TextReportItem* pItem = (TextReportItem*) mpItem;

    QtVariantProperty* pTextProperty = mpManager->addProperty(kText, QVariant::String, tr("Text"));
    pTextProperty->setValue(pItem->text);
    mpEditor->addProperty(pTextProperty);
}

//! Create properties specific for graph items
void ReportPropertyEditor::addGraphProperties()
{
    GraphReportItem* pItem = (GraphReportItem*) mpItem;

    QtVariantProperty* pXLabelProperty = mpManager->addProperty(kXLabel, QVariant::String, tr("X label"));
    pXLabelProperty->setValue(pItem->xLabel);
    mpEditor->addProperty(pXLabelProperty);

    QtVariantProperty* pYLabelProperty = mpManager->addProperty(kYLabel, QVariant::String, tr("Y label"));
    pYLabelProperty->setValue(pItem->yLabel);
    mpEditor->addProperty(pYLabelProperty);
}

//! Change the item property value
void ReportPropertyEditor::setValue(QtProperty* pProperty, QVariant value)
{
    if (!mpManager->contains(pProperty))
        return;
    int id = mpManager->id(pProperty);
    switch (id)
    {
    case kName:
        mpItem->name = value.toString();
        break;
    case kRect:
        mpItem->rect = value.toRect();
        break;
    case kAngle:
        mpItem->angle = value.toDouble();
        break;
    case kFont:
        mpItem->font = value.value<QFont>();
        break;
    case kText:
        static_cast<TextReportItem*>(mpItem)->text = value.toString();
        break;
    case kXLabel:
        static_cast<GraphReportItem*>(mpItem)->xLabel = value.toString();
        break;
    case kYLabel:
        static_cast<GraphReportItem*>(mpItem)->yLabel = value.toString();
        break;
    }
    emit edited();
}
