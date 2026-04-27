#include <QVBoxLayout>

#include <customvariantpropertymanager.h>
#include <magic_enum/magic_enum.hpp>
#include <qttreepropertybrowser.h>
#include <qtvariantproperty.h>

#include "reportdocument.h"
#include "reportitem.h"
#include "reportpropertyeditor.h"
#include "uiutility.h"

using namespace Backend::Core;
using namespace Frontend;

// Helper function
PairDouble convert(QString text);
QString convert(PairDouble const& data);

ReportPropertyEditor::ReportPropertyEditor(QWidget* pParent)
    : QWidget(pParent)
{
    setFont(Utility::getFont());
    createContent();
    createConnections();
}

//! Set the item to edit
void ReportPropertyEditor::setItemGetter(ReportItemGetter getter)
{
    mItemGetter = std::move(getter);
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

    // Get the item
    if (!mItemGetter)
        return;
    ReportItem* pItem = mItemGetter();
    if (!pItem)
        return;

    // Add properties
    addBaseProperties(pItem);
    switch (pItem->type())
    {
    case ReportItem::kGraph:
        addGraphProperties((GraphReportItem*) pItem);
        break;
    case ReportItem::kTable:
        addTableProperties((TableReportItem*) pItem);
        break;
    default:
        break;
    }
}

//! Create the properties which are common for all item types
void ReportPropertyEditor::addBaseProperties(ReportItem* pItem)
{
    QtVariantProperty* pRectProperty = mpManager->addProperty(kRect, QMetaType::QRect, tr("Position"));
    pRectProperty->setValue(pItem->rect);
    mpEditor->addProperty(pRectProperty);

    QtVariantProperty* pAngleProperty = mpManager->addProperty(kAngle, QMetaType::Double, tr("Rotation, %1").arg(QChar(0x00b0)));
    pAngleProperty->setValue(pItem->angle);
    mpEditor->addProperty(pAngleProperty);

    QtVariantProperty* pFontProperty = mpManager->addProperty(kFont, QMetaType::QFont, tr("Font"));
    pFontProperty->setValue(pItem->font);
    QtBrowserItem* pFontItem = mpEditor->addProperty(pFontProperty);
    mpEditor->setExpanded(pFontItem, false);
}

//! Create properties specific for graph items
void ReportPropertyEditor::addGraphProperties(GraphReportItem* pItem)
{
    QStringList const kAlignmentNames = {tr("Top right"), tr("Bottom right"), tr("Bottom left"), tr("Top left")};

    QtVariantProperty* pXRangeProperty = mpManager->addProperty(kXRange, QMetaType::QString, tr("X range"));
    pXRangeProperty->setValue(convert(pItem->xRange));
    mpEditor->addProperty(pXRangeProperty);

    QtVariantProperty* pYRangeProperty = mpManager->addProperty(kYRange, QMetaType::QString, tr("Y range"));
    pYRangeProperty->setValue(convert(pItem->yRange));
    mpEditor->addProperty(pYRangeProperty);

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

    QtVariantProperty* pGridZeroWidthProperty = mpManager->addProperty(kGridZeroWidth, QMetaType::Double, tr("Grid zero width"));
    pGridZeroWidthProperty->setValue(pItem->gridZeroWidth);
    mpEditor->addProperty(pGridZeroWidthProperty);

    QtVariantProperty* pSwapAxesProperty = mpManager->addProperty(kSwapAxes, QMetaType::Bool, tr("Swap axes"));
    pSwapAxesProperty->setValue(pItem->swapAxes);
    mpEditor->addProperty(pSwapAxesProperty);

    QtVariantProperty* pReverseXProperty = mpManager->addProperty(kReverseX, QMetaType::Bool, tr("Reverse X axis"));
    pReverseXProperty->setValue(pItem->reverseX);
    mpEditor->addProperty(pReverseXProperty);

    QtVariantProperty* pReverseYProperty = mpManager->addProperty(kReverseY, QMetaType::Bool, tr("Reverse Y axis"));
    pReverseYProperty->setValue(pItem->reverseY);
    mpEditor->addProperty(pReverseYProperty);

    QtVariantProperty* pLegendAlignProperty = mpManager->addProperty(kLegendAlign, QtVariantPropertyManager::enumTypeId(),
                                                                     tr("Legend alignment"));
    pLegendAlignProperty->setAttribute("enumNames", kAlignmentNames);
    auto aligns = magic_enum::enum_values<Align>();
    int numAligns = aligns.size();
    for (int i = 0; i != numAligns; ++i)
    {
        if (getAlignValue(aligns[i]) == pItem->legendAlign)
        {
            pLegendAlignProperty->setValue(i);
            break;
        }
    }
    mpEditor->addProperty(pLegendAlignProperty);

    QtVariantProperty* pShowLegendProperty = mpManager->addProperty(kShowLegend, QMetaType::Bool, tr("Legend"));
    pShowLegendProperty->setValue(pItem->showLegend);
    mpEditor->addProperty(pShowLegendProperty);

    QtVariantProperty* pShowBundleFreqProperty = mpManager->addProperty(kShowBundleFreq, QMetaType::Bool, tr("Bundle freq."));
    pShowBundleFreqProperty->setValue(pItem->showBundleFreq);
    mpEditor->addProperty(pShowBundleFreqProperty);
}

//! Create properties specific for table items
void ReportPropertyEditor::addTableProperties(TableReportItem* pItem)
{
    QtVariantProperty* pNumRowsProperty = mpManager->addProperty(kNumRows, QMetaType::Int, tr("Number of rows"));
    pNumRowsProperty->setValue(pItem->numRows());
    mpEditor->addProperty(pNumRowsProperty);

    QtVariantProperty* pNumColsProperty = mpManager->addProperty(kNumCols, QMetaType::Int, tr("Number of columns"));
    pNumColsProperty->setValue(pItem->numCols());
    mpEditor->addProperty(pNumColsProperty);

    QtVariantProperty* pShowLabelsProperty = mpManager->addProperty(kShowLabels, QMetaType::Bool, tr("Header"));
    pShowLabelsProperty->setValue(pItem->showLabels);
    mpEditor->addProperty(pShowLabelsProperty);
}

//! Change the item property value
void ReportPropertyEditor::setValue(QtProperty* pProperty, QVariant value)
{
    // Get the item
    if (!mItemGetter)
        return;
    ReportItem* pItem = mItemGetter();
    if (!pItem)
        return;

    // Get the property id
    if (!mpManager->contains(pProperty))
        return;
    int propertyID = mpManager->id(pProperty);
    switch (propertyID)
    {
    // Base
    case kRect:
        pItem->rect = value.toRect();
        break;
    case kAngle:
        pItem->angle = value.toDouble();
        break;
    case kFont:
        pItem->font = value.value<QFont>();
        break;

    // Text
    case kText:
        static_cast<TextReportItem*>(pItem)->text = value.toString();
        break;

    // Graph
    case kXRange:
        static_cast<GraphReportItem*>(pItem)->xRange = convert(value.toString());
        break;
    case kYRange:
        static_cast<GraphReportItem*>(pItem)->yRange = convert(value.toString());
        break;
    case kXLabel:
        static_cast<GraphReportItem*>(pItem)->xLabel = value.toString();
        break;
    case kYLabel:
        static_cast<GraphReportItem*>(pItem)->yLabel = value.toString();
        break;
    case kScaleRange:
        static_cast<GraphReportItem*>(pItem)->scaleRange = value.toDouble();
        break;
    case kNumTicks:
        static_cast<GraphReportItem*>(pItem)->numTicks = value.toInt();
        break;
    case kGridWidth:
        static_cast<GraphReportItem*>(pItem)->gridWidth = value.toDouble();
        break;
    case kGridZeroWidth:
        static_cast<GraphReportItem*>(pItem)->gridZeroWidth = value.toDouble();
        break;
    case kSwapAxes:
        static_cast<GraphReportItem*>(pItem)->swapAxes = value.toBool();
        break;
    case kReverseX:
        static_cast<GraphReportItem*>(pItem)->reverseX = value.toBool();
        break;
    case kReverseY:
        static_cast<GraphReportItem*>(pItem)->reverseY = value.toBool();
        break;
    case kLegendAlign:
        static_cast<GraphReportItem*>(pItem)->legendAlign = getAlignValue((Align) value.toInt());
        break;
    case kShowLegend:
        static_cast<GraphReportItem*>(pItem)->showLegend = value.toBool();
        break;
    case kShowBundleFreq:
        static_cast<GraphReportItem*>(pItem)->showBundleFreq = value.toBool();
        break;

    // Table
    case kNumRows:
        static_cast<TableReportItem*>(pItem)->setNumRows(value.toInt());
        break;
    case kNumCols:
        static_cast<TableReportItem*>(pItem)->setNumCols(value.toInt());
        break;
    case kShowLabels:
        static_cast<TableReportItem*>(pItem)->showLabels = value.toBool();
        break;

    default:
        return;
    }
    emit edited();
}

//! Get corner alignment value by enum key
Qt::Alignment ReportPropertyEditor::getAlignValue(Align key)
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

//! Helper function to convert text to pair of double values
PairDouble convert(QString text)
{
    PairDouble result(0.0, 0.0);
    text.replace(',', '.');
    QStringList tokens = text.split(' ');
    if (tokens.size() == 2)
    {
        result.first = tokens.first().toDouble();
        result.second = tokens.last().toDouble();
    }
    return result;
}

//! Helper function to convert pair of double values to text
QString convert(PairDouble const& data)
{
    return QString("%1 %2").arg(QString::number(data.first), QString::number(data.second));
}
