#include <QVBoxLayout>

#include <customvariantpropertymanager.h>
#include <magic_enum/magic_enum.hpp>
#include <qttreepropertybrowser.h>
#include <qtvariantproperty.h>

#include "reportitem.h"
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
    QtVariantProperty* pRectProperty = mpManager->addProperty(kRect, QMetaType::QRect, tr("Position"));
    pRectProperty->setValue(mpItem->rect);
    mpEditor->addProperty(pRectProperty);

    QtVariantProperty* pAngleProperty = mpManager->addProperty(kAngle, QMetaType::Double, tr("Rotation, %1").arg(QChar(0x00b0)));
    pAngleProperty->setValue(mpItem->angle);
    mpEditor->addProperty(pAngleProperty);

    QtVariantProperty* pFontProperty = mpManager->addProperty(kFont, QMetaType::QFont, tr("Font"));
    pFontProperty->setValue(mpItem->font);
    QtBrowserItem* pFontItem = mpEditor->addProperty(pFontProperty);
    mpEditor->setExpanded(pFontItem, false);
}

//! Create properties specific for text items
void ReportPropertyEditor::addTextProperties()
{
    TextReportItem* pItem = (TextReportItem*) mpItem;

    QtVariantProperty* pTextProperty = mpManager->addProperty(kText, QMetaType::QString, tr("Text"));
    pTextProperty->setValue(pItem->text);
    mpEditor->addProperty(pTextProperty);
}

//! Create properties specific for graph items
void ReportPropertyEditor::addGraphProperties()
{
    QStringList const kAlignmentNames = {tr("Top right"), tr("Bottom right"), tr("Bottom left"), tr("Top left")};

    GraphReportItem* pItem = (GraphReportItem*) mpItem;

    QtVariantProperty* pXLabelProperty = mpManager->addProperty(kXLabel, QMetaType::QString, tr("X label"));
    pXLabelProperty->setValue(pItem->xLabel);
    mpEditor->addProperty(pXLabelProperty);

    QtVariantProperty* pYLabelProperty = mpManager->addProperty(kYLabel, QMetaType::QString, tr("Y label"));
    pYLabelProperty->setValue(pItem->yLabel);
    mpEditor->addProperty(pYLabelProperty);

    QtVariantProperty* pScaleRangeProperty = mpManager->addProperty(kScaleRange, QMetaType::Double, tr("Scale range"));
    pScaleRangeProperty->setValue(pItem->scaleRange);
    mpEditor->addProperty(pScaleRangeProperty);

    QtVariantProperty* pNumTicksProperty = mpManager->addProperty(kNumTicks, QMetaType::Int, tr("Ticks number"));
    pNumTicksProperty->setValue(pItem->numTicks);
    mpEditor->addProperty(pNumTicksProperty);

    QtVariantProperty* pGridWidthProperty = mpManager->addProperty(kGridWidth, QMetaType::Double, tr("Grid width"));
    pGridWidthProperty->setValue(pItem->gridWidth);
    mpEditor->addProperty(pGridWidthProperty);

    QtVariantProperty* pSwapAxesProperty = mpManager->addProperty(kSwapAxes, QMetaType::Bool, tr("Swap axes"));
    pSwapAxesProperty->setValue(pItem->swapAxes);
    mpEditor->addProperty(pSwapAxesProperty);

    QtVariantProperty* pLegendAlignmentProperty = mpManager->addProperty(kLegendAlignment, QtVariantPropertyManager::enumTypeId(),
                                                                         tr("Legend alignment"));
    pLegendAlignmentProperty->setAttribute("enumNames", kAlignmentNames);
    auto alignments = magic_enum::enum_values<Alignment>();
    int numAlignments = alignments.size();
    for (int i = 0; i != numAlignments; ++i)
    {
        if (getAlignmentValue(alignments[i]) == pItem->legendAlignment)
        {
            pLegendAlignmentProperty->setValue(i);
            break;
        }
    }
    mpEditor->addProperty(pLegendAlignmentProperty);

    QtVariantProperty* pShowLegendProperty = mpManager->addProperty(kShowLegend, QMetaType::Bool, tr("Legend"));
    pShowLegendProperty->setValue(pItem->showLegend);
    mpEditor->addProperty(pShowLegendProperty);

    QtVariantProperty* pShowBundleFreqProperty = mpManager->addProperty(kShowBundleFreq, QMetaType::Bool, tr("Bundle freq."));
    pShowBundleFreqProperty->setValue(pItem->showBundleFreq);
    mpEditor->addProperty(pShowBundleFreqProperty);
}

//! Change the item property value
void ReportPropertyEditor::setValue(QtProperty* pProperty, QVariant value)
{
    if (!mpManager->contains(pProperty))
        return;
    int id = mpManager->id(pProperty);
    switch (id)
    {
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
    case kScaleRange:
        static_cast<GraphReportItem*>(mpItem)->scaleRange = value.toDouble();
        break;
    case kNumTicks:
        static_cast<GraphReportItem*>(mpItem)->numTicks = value.toInt();
        break;
    case kGridWidth:
        static_cast<GraphReportItem*>(mpItem)->gridWidth = value.toDouble();
        break;
    case kSwapAxes:
        static_cast<GraphReportItem*>(mpItem)->swapAxes = value.toBool();
        break;
    case kLegendAlignment:
        static_cast<GraphReportItem*>(mpItem)->legendAlignment = getAlignmentValue((Alignment) value.toInt());
        break;
    case kShowLegend:
        static_cast<GraphReportItem*>(mpItem)->showLegend = value.toBool();
        break;
    case kShowBundleFreq:
        static_cast<GraphReportItem*>(mpItem)->showBundleFreq = value.toBool();
        break;
    default:
        return;
    }
    emit edited();
}

//! Get corner alignment value by enum key
Qt::Alignment ReportPropertyEditor::getAlignmentValue(Alignment key)
{
    switch (key)
    {
    case kTopRight:
        return Qt::AlignTop | Qt::AlignRight;
    case kBottomRight:
        return Qt::AlignBottom | Qt::AlignRight;
    case kBottomLeft:
        return Qt::AlignBottom | Qt::AlignLeft;
    case kTopLeft:
        return Qt::AlignTop | Qt::AlignLeft;
    default:
        break;
    };
    return Qt::Alignment();
}
