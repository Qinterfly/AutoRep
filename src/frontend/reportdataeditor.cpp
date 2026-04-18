#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QInputDialog>
#include <QLabel>
#include <QListWidget>
#include <QToolBar>

#include <customvariantpropertymanager.h>
#include <qttreepropertybrowser.h>
#include <qtvariantproperty.h>

#include "customplot.h"
#include "geometryview.h"
#include "reportdataeditor.h"
#include "reportdocument.h"
#include "reportitem.h"
#include "reportsettings.h"
#include "uiconstants.h"
#include "uiutility.h"

using namespace Backend::Core;
using namespace Frontend;
using namespace Constants;

// Helper function
QComboBox* createDirComboBox();

ReportDataEditor::ReportDataEditor(QWidget* pParent)
    : QWidget(pParent)
    , mpItem(nullptr)
{
}

void ReportDataEditor::setItem(ReportItem* pItem)
{
    if (pItem && pItem->type() == type())
        mpItem = pItem;
    refresh();
}

GraphReportDataEditor::GraphReportDataEditor(GeometryView* pGeometryView, QWidget* pParent)
    : ReportDataEditor(pParent)
    , mpGeometryView(pGeometryView)
{
    setFont(Utility::getFont());
    createContent();
    createConnections();
}

//! Update the widgets state
void GraphReportDataEditor::refresh()
{
    // Cast to the graph item
    if (!mpItem)
        return;
    GraphReportItem* pItem = (GraphReportItem*) mpItem;

    // Set the subtype
    QSignalBlocker blockerSubType(mpSubTypeSelector);
    Utility::setIndexByKey(mpSubTypeSelector, pItem->subType);

    // Set the unit
    QSignalBlocker blockerUnit(mpUnitSelector);
    int numUnits = mpUnitSelector->count();
    int iUnit = -1;
    for (int i = 0; i != numUnits; ++i)
    {
        if (mpUnitSelector->itemData(i).toString() == pItem->unit)
        {
            iUnit = i;
            break;
        }
    }
    mpUnitSelector->setCurrentIndex(iUnit);

    // Set the coordinate direction
    QSignalBlocker blockerCoordDir(mpCoordDirSelector);
    Utility::setIndexByKey(mpCoordDirSelector, (int) pItem->coordDir);

    // Set the response direction
    QSignalBlocker blockerResponseDir(mpResponseDirSelector);
    Utility::setIndexByKey(mpResponseDirSelector, (int) pItem->responseDir);

    // Set the curve list
    QSignalBlocker blockerCurveList(mpCurveList);
    int iCurve = mpCurveList->currentRow();
    mpCurveList->clear();
    int numCurves = pItem->curves.size();
    for (int i = 0; i != numCurves; ++i)
    {
        GraphReportCurve const& curve = pItem->curves[i];
        QListWidgetItem* pListItem = new QListWidgetItem;

        // Construct the name
        QString name = curve.name;
        if (name.isEmpty())
            name = curve.points.size() == 1 ? curve.points.first().name() : tr("Curve %1").arg(1 + i);
        pListItem->setText(name);

        // Construct the icon
        if (pItem->subType != GraphReportItem::kMultiReal && pItem->subType != GraphReportItem::kMultiImag)
        {
            QPen pen(curve.lineColor, curve.lineWidth, curve.lineStyle);
            QCPScatterStyle style((QCPScatterStyle::ScatterShape) curve.markerShape, curve.markerSize);
            if (curve.markerFill)
                style.setBrush(curve.lineColor);
            style.setPen(pen);
            bool isLine = curve.lineStyle != Qt::NoPen;
            bool isMarker = curve.markerShape != ReportMarkerShape::kNone;
            QIcon icon = Utility::getIcon(style, mpCurveList->iconSize(), isLine, isMarker);
            pListItem->setIcon(icon);
        }

        // Add the item to the list
        mpCurveList->addItem(pListItem);
    }
    if (iCurve >= 0 && iCurve < numCurves)
        mpCurveList->setCurrentRow(iCurve);

    // Set the point list
    QSignalBlocker blockerPointList(mpPointList);
    mpPointList->clear();
    mpPointList->parentWidget()->setVisible(pItem->subType == GraphReportItem::kModeshape);
    iCurve = mpCurveList->currentRow();
    if (iCurve >= 0 && iCurve < numCurves)
    {
        GraphReportCurve const& curve = pItem->curves[iCurve];
        int numPoints = curve.points.size();
        for (int i = 0; i != numPoints; ++i)
        {
            QListWidgetItem* pPoint = new QListWidgetItem(curve.points[i].name());
            mpPointList->addItem(pPoint);
        }
    }
}

ReportItem::Type GraphReportDataEditor::type() const
{
    return ReportItem::kGraph;
}

//! Create all the widgets
void GraphReportDataEditor::createContent()
{
    QVBoxLayout* pLayout = new QVBoxLayout;
    pLayout->addLayout(createHeaderLayout());
    pLayout->addWidget(createToolBar());
    pLayout->addLayout(createCurveLayout());
    setLayout(pLayout);
}

//! Specify the connections between widgets
void GraphReportDataEditor::createConnections()
{
    // Header
    connect(mpSubTypeSelector, &QComboBox::currentIndexChanged, this, &GraphReportDataEditor::processHeaderChanged);
    connect(mpCoordDirSelector, &QComboBox::currentIndexChanged, this, &GraphReportDataEditor::processHeaderChanged);
    connect(mpResponseDirSelector, &QComboBox::currentIndexChanged, this, &GraphReportDataEditor::processHeaderChanged);
    connect(mpUnitSelector, &QComboBox::currentIndexChanged, this, &GraphReportDataEditor::processHeaderChanged);

    // List
    connect(mpCurveList, &QListWidget::itemSelectionChanged, this, &GraphReportDataEditor::processCurveSelected);
    connect(mpCurveList, &QListWidget::itemDoubleClicked, this, &GraphReportDataEditor::editSelectedCurve);
}

//! Create the layout of header widgets
QLayout* GraphReportDataEditor::createHeaderLayout()
{
    // Create the widgets
    mpSubTypeSelector = new QComboBox;
    mpCoordDirSelector = createDirComboBox();
    mpResponseDirSelector = createDirComboBox();
    mpUnitSelector = new QComboBox;

    // Initialize the widgets
    mpSubTypeSelector->addItem(QString(), GraphReportItem::kNone);
    mpSubTypeSelector->addItem(tr("Re"), GraphReportItem::kReal);
    mpSubTypeSelector->addItem(tr("Im"), GraphReportItem::kImag);
    mpSubTypeSelector->addItem(tr("Multi Re"), GraphReportItem::kMultiReal);
    mpSubTypeSelector->addItem(tr("Multi Im"), GraphReportItem::kMultiImag);
    mpSubTypeSelector->addItem(tr("Freq Re"), GraphReportItem::kFreqReal);
    mpSubTypeSelector->addItem(tr("Freq Im"), GraphReportItem::kFreqImag);
    mpSubTypeSelector->addItem(tr("Modeshape"), GraphReportItem::kModeshape);
    mpUnitSelector->addItem(QString());
    mpUnitSelector->addItem(tr("m/s%1").arg(QChar(0x00B2)), Units::skM_S2);
    mpUnitSelector->addItem(tr("(m/s%1)/N").arg(QChar(0x00B2)), Units::skM_S2_N);
    mpUnitSelector->addItem(tr("m"), Units::skM);

    // Combine the widgets
    QGridLayout* pLayout = new QGridLayout;
    pLayout->addWidget(new QLabel(tr("Type: ")), 0, 0);
    pLayout->addWidget(mpSubTypeSelector, 0, 1);
    pLayout->addWidget(new QLabel(tr("Unit: ")), 1, 0);
    pLayout->addWidget(mpUnitSelector, 1, 1);
    pLayout->addWidget(new QLabel(tr("Coord dir: ")), 2, 0);
    pLayout->addWidget(mpCoordDirSelector, 2, 1);
    pLayout->addWidget(new QLabel(tr("Response dir: ")), 3, 0);
    pLayout->addWidget(mpResponseDirSelector, 3, 1);
    pLayout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Preferred), 0, 2);

    return pLayout;
}

//! Create a set of actions to manipulate curves
QWidget* GraphReportDataEditor::createToolBar()
{
    QToolBar* pToolBar = new QToolBar;
    pToolBar->setIconSize(Constants::Size::skToolBarIcon);
    pToolBar->addAction(QIcon(":/icons/data-add.svg"), tr("Add curve"), Qt::SHIFT | Qt::Key_A, this, &GraphReportDataEditor::addCurve);
    pToolBar->addAction(QIcon(":/icons/data-edit.svg"), tr("Edit curve"), Qt::SHIFT | Qt::Key_E, this, &GraphReportDataEditor::editSelectedCurve);
    pToolBar->addAction(QIcon(":/icons/data-replace.svg"), tr("Replace curve"), Qt::SHIFT | Qt::Key_Q, this,
                        &GraphReportDataEditor::replaceSelectedCurve);
    pToolBar->addAction(QIcon(":/icons/data-rename.svg"), tr("Rename curve"), Qt::SHIFT | Qt::Key_R, this,
                        &GraphReportDataEditor::renameSelectedCurve);
    pToolBar->addAction(QIcon(":/icons/data-remove.svg"), tr("Remove curve"), Qt::SHIFT | Qt::Key_D, this,
                        &GraphReportDataEditor::removeSelectedCurve);
    Utility::setShortcutHints(pToolBar);
    return pToolBar;
}

//! Create the layout of curve widgets
QLayout* GraphReportDataEditor::createCurveLayout()
{
    // Constants
    QMargins const kMargins(3, 3, 3, 3);
    QSize const kIconSize(32, 32);

    // Create the lists
    mpCurveList = new QListWidget;
    mpPointList = new QListWidget;

    // Initialize the lists
    mpCurveList->setFont(font());
    mpPointList->setFont(font());
    mpCurveList->setSelectionMode(QListWidget::SingleSelection);
    mpPointList->setSelectionMode(QListWidget::NoSelection);
    mpCurveList->setIconSize(kIconSize);
    mpPointList->setIconSize(kIconSize);

    // Create the curve group box
    QGroupBox* pCurveGroupBox = new QGroupBox(tr("Curves"));
    QVBoxLayout* pCurveLayout = new QVBoxLayout;
    pCurveLayout->setContentsMargins(kMargins);
    pCurveLayout->addWidget(mpCurveList);
    pCurveGroupBox->setLayout(pCurveLayout);

    // Create the point group box
    QGroupBox* pPointGroupBox = new QGroupBox(tr("Points"));
    QVBoxLayout* pPointLayout = new QVBoxLayout;
    pPointLayout->setContentsMargins(kMargins);
    pPointLayout->addWidget(mpPointList);
    pPointGroupBox->setLayout(pPointLayout);

    // Combine the widgets
    QHBoxLayout* pMainLayout = new QHBoxLayout;
    pMainLayout->addWidget(pCurveGroupBox);
    pMainLayout->addWidget(pPointGroupBox);

    return pMainLayout;
}

//! Add a new curve
void GraphReportDataEditor::addCurve()
{
    // Retrieve the graph item
    if (!mpItem)
        return;
    GraphReportItem* pItem = (GraphReportItem*) mpItem;

    // Helper function
    auto createCurve = [pItem](QList<GraphReportPoint> const& points)
    {
        int iDefaultCurve = Utility::getRepeatedIndex(pItem->curves.count(), ReportSettings::curves.size());
        GraphReportCurve curve = ReportSettings::curves[iDefaultCurve];
        curve.points = points;
        pItem->curves.push_back(curve);
    };

    // Add the curve
    QList<GraphReportPoint> selectedPoints = getSelectedPoints();
    if (!selectedPoints.isEmpty())
    {
        if (pItem->isMultiPointCurve())
        {
            createCurve(selectedPoints);
        }
        else
        {
            for (GraphReportPoint const& p : std::as_const(selectedPoints))
                createCurve({p});
        }
        qInfo() << tr("Curve consisted of %1 points is added").arg(selectedPoints.size());
    }
    else
    {
        qWarning() << tr("Cannot add a curve, since there are no selected points");
    }

    // Update the widgets content
    refresh();

    // Select the last curve
    if (!selectedPoints.isEmpty())
        mpCurveList->setCurrentRow(mpCurveList->count() - 1);

    // Finish up the editing
    emit edited();
}

//! Edit the selected curve
void GraphReportDataEditor::editSelectedCurve()
{
    // Retrieve the graph item
    if (!mpItem)
        return;
    GraphReportItem* pItem = (GraphReportItem*) mpItem;

    // Retrieve the selected curve
    int iCurve = mpCurveList->currentRow();
    if (iCurve < 0)
        return;
    GraphReportCurve& curve = pItem->curves[iCurve];

    // Create the editor and show it as a dialog window
    ReportCurvePropertyEditor* pEditor = new ReportCurvePropertyEditor(curve);
    connect(pEditor, &ReportCurvePropertyEditor::edited, this, &GraphReportDataEditor::edited);
    Utility::showAsDialog(pEditor, tr("Edit curve properties"), nullptr, false);
}

//! Replace the selected curve with the new one
void GraphReportDataEditor::replaceSelectedCurve()
{
    // Retrieve the graph item
    if (!mpItem)
        return;
    GraphReportItem* pItem = (GraphReportItem*) mpItem;

    // Construct the dialog
    auto reply = QMessageBox::question(this, tr("Replace Curve"),
                                       tr("Are you sure that you want to replace all the curve points with the selectd ones?"),
                                       QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes)
        return;

    // Replace the current curve
    QList<GraphReportPoint> selectedPoints = getSelectedPoints();
    int iCurve = mpCurveList->currentRow();
    if (iCurve >= 0 && iCurve < pItem->curves.size())
    {
        if (!selectedPoints.isEmpty())
        {
            GraphReportCurve curve;
            if (pItem->isMultiPointCurve())
                curve = GraphReportCurve(selectedPoints);
            else
                curve = GraphReportCurve({selectedPoints.first()});
            pItem->curves[iCurve].points = curve.points;
            qInfo() << tr("Curve is replaced");
        }
        else
        {
            qWarning() << tr("Cannot replace the curve, since there are no selected points");
        }
    }

    // Update the widgets content
    refresh();

    // Finish up the editing
    emit edited();
}

//! Rename the selected curve
void GraphReportDataEditor::renameSelectedCurve()
{
    // Retrieve the graph item
    if (!mpItem)
        return;
    GraphReportItem* pItem = (GraphReportItem*) mpItem;

    // Get the curve
    int iCurve = mpCurveList->currentRow();
    if (iCurve < 0 || iCurve >= pItem->curves.size())
    {
        qWarning() << tr("There is no selected curve to rename");
        return;
    }
    GraphReportCurve& curve = pItem->curves[iCurve];

    // Create the input dialog
    bool ok = false;
    QString text = QInputDialog::getText(this, tr("Rename curve"), tr("Curve name: "), QLineEdit::Normal, mpCurveList->currentItem()->text(),
                                         &ok);
    if (ok && !text.isEmpty())
        curve.name = text;

    // Update the widgets content
    refresh();

    // Finish up the editing
    emit edited();
}

//! Process removing the current curve
void GraphReportDataEditor::removeSelectedCurve()
{
    // Retrieve the graph item
    if (!mpItem)
        return;
    GraphReportItem* pItem = (GraphReportItem*) mpItem;

    // Remove the selected curve
    int iCurve = mpCurveList->currentRow();
    if (iCurve >= 0 && iCurve < pItem->curves.size())
    {
        pItem->curves.remove(iCurve);
        qInfo() << tr("Curve is successfully removed");
    }

    // Update the widgets content
    refresh();

    // Select the last curve
    mpCurveList->setCurrentRow(mpCurveList->count() - 1);

    // Finish up the editing
    emit edited();
}

//! Retrieve the selected points from the geometry view
QList<GraphReportPoint> GraphReportDataEditor::getSelectedPoints()
{
    auto selections = mpGeometryView->selectionPairs();
    int numSelections = selections.size();
    QList<GraphReportPoint> result(numSelections);
    for (int i = 0; i != numSelections; ++i)
    {
        auto selection = selections[i];
        result[i] = GraphReportPoint(selection.first, selection.second);
    }
    return result;
}

//! Process changing curve selection
void GraphReportDataEditor::processCurveSelected()
{
    // Retrieve the graph item
    if (!mpItem)
        return;
    GraphReportItem* pItem = (GraphReportItem*) mpItem;

    // Update the widgets content
    refresh();

    // Highlight the selected curve
    int iCurve = mpCurveList->currentRow();
    if (iCurve >= 0 && iCurve < pItem->curves.size())
    {
        GraphReportCurve const& curve = pItem->curves[iCurve];
        if (curve.isEmpty())
            return;
        int numPoints = curve.points.size();
        mpGeometryView->clearSelection();
        for (int i = 0; i != numPoints; ++i)
        {
            GraphReportPoint const& point = curve.points[i];
            mpGeometryView->addSelection(point.component, point.node);
        }
        mpGeometryView->refresh();
    }
}

//! Process changing header of the item
void GraphReportDataEditor::processHeaderChanged()
{
    // Retrieve the graph item
    if (!mpItem)
        return;
    GraphReportItem* pItem = (GraphReportItem*) mpItem;

    // Set the item data
    pItem->subType = (GraphReportItem::SubType) mpSubTypeSelector->currentData().toInt();
    pItem->coordDir = (ReportDirection) mpCoordDirSelector->currentData().toInt();
    pItem->responseDir = (ReportDirection) mpResponseDirSelector->currentData().toInt();
    pItem->unit = mpUnitSelector->currentData().toString();

    // Update the content
    refresh();

    // Finish up the editing
    emit edited();
}

ReportCurvePropertyEditor::ReportCurvePropertyEditor(GraphReportCurve& curve, QWidget* pParent)
    : mCurve(curve)
{
    setFont(Utility::getFont());
    createContent();
    createProperties();
    createConnections();
}

QSize ReportCurvePropertyEditor::sizeHint() const
{
    return QSize(400, 300);
}

//! Create all the widgets
void ReportCurvePropertyEditor::createContent()
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

//! Create the plottable properties
void ReportCurvePropertyEditor::createProperties()
{
    QStringList const kLineStyleNames = {QString(), tr("Solid"), tr("Dash"), tr("Dotted"), tr("Dash-dotted")};
    QStringList const kMarkerShapeNames = {QString(),
                                           tr("Dot"),
                                           tr("Cross"),
                                           tr("Plus"),
                                           tr("Circle"),
                                           tr("Disc"),
                                           tr("Square"),
                                           tr("Diamond"),
                                           tr("Star"),
                                           tr("Triangle"),
                                           tr("Inverted Triangle"),
                                           tr("Cross Square"),
                                           tr("Plus Square"),
                                           tr("Cross Circle"),
                                           tr("Plus Circle"),
                                           tr("Peace")};

    // Remove the previous properties
    QSignalBlocker blockerEditor(mpEditor);
    QSignalBlocker blockerManager(mpManager);
    mpEditor->clear();
    mpManager->clear();

    // Create the properties
    QtVariantProperty* pLineStyleProperty = mpManager->addProperty(kLineStyle, QtVariantPropertyManager::enumTypeId(), tr("Line style"));
    pLineStyleProperty->setAttribute("enumNames", kLineStyleNames);
    pLineStyleProperty->setValue((int) mCurve.lineStyle);
    mpEditor->addProperty(pLineStyleProperty);

    QtVariantProperty* pLineWidthProperty = mpManager->addProperty(kLineWidth, QMetaType::Double, tr("Line width"));
    pLineWidthProperty->setValue(mCurve.lineWidth);
    mpEditor->addProperty(pLineWidthProperty);

    QtVariantProperty* pLineColorProperty = mpManager->addProperty(kLineColor, QMetaType::QColor, tr("Line color"));
    pLineColorProperty->setValue(mCurve.lineColor);
    QtBrowserItem* pLineColorItem = mpEditor->addProperty(pLineColorProperty);
    mpEditor->setExpanded(pLineColorItem, false);

    QtVariantProperty* pMarkerShapeProperty = mpManager->addProperty(kMarkerShape, QtVariantPropertyManager::enumTypeId(), tr("Marker shape"));
    pMarkerShapeProperty->setAttribute("enumNames", kMarkerShapeNames);
    pMarkerShapeProperty->setValue((QCPScatterStyle::ScatterShape) mCurve.markerShape);
    mpEditor->addProperty(pMarkerShapeProperty);

    QtVariantProperty* pMarkerSizeProperty = mpManager->addProperty(kMarkerSize, QMetaType::Int, tr("Marker size"));
    pMarkerSizeProperty->setValue(mCurve.markerSize);
    mpEditor->addProperty(pMarkerSizeProperty);
}

//! Specify connections
void ReportCurvePropertyEditor::createConnections()
{
    connect(mpManager, &CustomVariantPropertyManager::valueChanged, this, &ReportCurvePropertyEditor::setValue);
}

//! Change the plottable property value
void ReportCurvePropertyEditor::setValue(QtProperty* pProperty, QVariant value)
{
    if (!mpManager->contains(pProperty))
        return;
    int id = mpManager->id(pProperty);
    switch (id)
    {
    case kLineStyle:
        mCurve.lineStyle = (Qt::PenStyle) value.toInt();
        break;
    case kLineWidth:
        mCurve.lineWidth = value.toDouble();
        break;
    case kLineColor:
        mCurve.lineColor = value.value<QColor>();
        break;
    case kMarkerShape:
        mCurve.markerShape = (ReportMarkerShape) value.toInt();
        break;
    case kMarkerSize:
        mCurve.markerSize = value.toInt();
        break;
    }
    emit edited();
}

//! Helper function to create a combobox with predefined directions
QComboBox* createDirComboBox()
{
    QComboBox* pResult = new QComboBox;
    pResult->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    pResult->addItem(QString(), (int) ReportDirection::kNone);
    pResult->addItem("X", (int) ReportDirection::kX);
    pResult->addItem("Y", (int) ReportDirection::kY);
    pResult->addItem("Z", (int) ReportDirection::kZ);
    return pResult;
}
