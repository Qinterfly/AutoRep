#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QInputDialog>
#include <QLabel>
#include <QToolBar>

#include <customvariantpropertymanager.h>
#include <qttreepropertybrowser.h>
#include <qtvariantproperty.h>

#include "constants.h"
#include "customplot.h"
#include "geometryview.h"
#include "reportdataeditor.h"
#include "reportdefaults.h"
#include "reportdocument.h"
#include "reportitem.h"
#include "uiconstants.h"
#include "uiutility.h"

using namespace Backend::Constants;
using namespace Backend::Core;
using namespace Frontend;

// Constants
int const skCurveRole = Qt::UserRole + 1;
int const skPointRole = skCurveRole + 1;

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

GraphReportDataEditor::GraphReportDataEditor(GeometryView* pGeometryView, ReportPage const& page, QWidget* pParent)
    : ReportDataEditor(pParent)
    , mpGeometryView(pGeometryView)
    , mPage(page)
{
    setFont(Utility::getFont());
    createContent();
    createConnections();
}

//! Update the widgets state
void GraphReportDataEditor::refresh()
{
    refreshHeader();
    refreshTree();
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
    pLayout->addLayout(createTreeLayout());
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
    connect(mpLinkSelector, &QComboBox::currentIndexChanged, this, &GraphReportDataEditor::processHeaderChanged);

    // List
    connect(mpCurveTree, &QTreeWidget::itemSelectionChanged, this, &GraphReportDataEditor::processTreeSelected);
    connect(mpCurveTree, &QTreeWidget::itemDoubleClicked, this, &GraphReportDataEditor::editSelectedCurve);
}

//! Create the layout of header widgets
QLayout* GraphReportDataEditor::createHeaderLayout()
{
    // Create the widgets
    mpSubTypeSelector = new QComboBox;
    mpCoordDirSelector = createDirComboBox();
    mpResponseDirSelector = createDirComboBox();
    mpUnitSelector = new QComboBox;
    mpLinkSelector = new QComboBox;

    // Initialize the widgets
    mpSubTypeSelector->addItem(QString(), GraphReportItem::kNone);
    mpSubTypeSelector->addItem(tr("Re"), GraphReportItem::kReal);
    mpSubTypeSelector->addItem(tr("Im"), GraphReportItem::kImag);
    mpSubTypeSelector->addItem(tr("Multi Re"), GraphReportItem::kMultiReal);
    mpSubTypeSelector->addItem(tr("Multi Im"), GraphReportItem::kMultiImag);
    mpSubTypeSelector->addItem(tr("Freq Amp"), GraphReportItem::kFreqAmp);
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
    pLayout->addWidget(new QLabel(tr("Link: ")), 4, 0);
    pLayout->addWidget(mpLinkSelector, 4, 1);
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
    pToolBar->addAction(QIcon(":/icons/data-remove.svg"), tr("Remove object"), Qt::SHIFT | Qt::Key_D, this,
                        &GraphReportDataEditor::removeSelected);
    Utility::setShortcutHints(pToolBar);
    return pToolBar;
}

//! Create the layout of curve widgets
QLayout* GraphReportDataEditor::createTreeLayout()
{
    // Constants
    QSize const kIconSize(32, 32);

    // Create the lists
    mpCurveTree = new QTreeWidget;

    // Initialize the lists
    mpCurveTree->setFont(font());
    mpCurveTree->setSelectionMode(QListWidget::SingleSelection);
    mpCurveTree->setIconSize(kIconSize);
    mpCurveTree->setHeaderHidden(true);
    mpCurveTree->setColumnCount(1);

    // Combine the widgets
    QVBoxLayout* pLayout = new QVBoxLayout;
    pLayout->addWidget(mpCurveTree);

    return pLayout;
}

//! Update the header widgets
void GraphReportDataEditor::refreshHeader()
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

    // Set the link
    QSignalBlocker blockerLink(mpLinkSelector);
    mpLinkSelector->clear();
    int numItems = mPage.count();
    mpLinkSelector->addItem(QString(), QUuid());
    for (int i = 0; i != numItems; ++i)
    {
        ReportItem const* pAnotherItem = mPage.get(i);
        if (pAnotherItem->type() != ReportItem::kGraph)
            continue;
        if (pItem->id == pAnotherItem->id)
            continue;
        mpLinkSelector->addItem(pAnotherItem->name, pAnotherItem->id);
        if (pItem->link == pAnotherItem->id)
            mpLinkSelector->setCurrentIndex(mpLinkSelector->count() - 1);
    }
}

//! Update the hierarchy widgets
void GraphReportDataEditor::refreshTree()
{
    // Cast to the graph item
    if (!mpItem)
        return;
    GraphReportItem* pItem = (GraphReportItem*) mpItem;

    // Set the curve tree
    QSignalBlocker blockerCurveTree(mpCurveTree);
    mpCurveTree->setEnabled(pItem->link.isNull());
    auto [iCurve, iPoint] = getTreeSelected();
    mpCurveTree->clear();
    int numCurves = pItem->curves.size();
    for (int iCurve = 0; iCurve != numCurves; ++iCurve)
    {
        GraphReportCurve const& curve = pItem->curves[iCurve];
        QTreeWidgetItem* pCurveItem = new QTreeWidgetItem;
        pCurveItem->setFont(0, font());

        // Construct the name
        QString name = curve.name;
        if (name.isEmpty())
            name = curve.points.size() == 1 ? curve.points.first().name() : tr("Curve %1").arg(1 + iCurve);
        pCurveItem->setText(0, name);
        pCurveItem->setData(0, skCurveRole, iCurve);
        pCurveItem->setData(0, skPointRole, -1);

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
            QIcon icon = Utility::getIcon(style, mpCurveTree->iconSize(), isLine, isMarker);
            pCurveItem->setIcon(0, icon);
        }

        // Add the points
        int numPoints = curve.points.size();
        for (int iPoint = 0; iPoint != numPoints; ++iPoint)
        {
            QTreeWidgetItem* pPointItem = new QTreeWidgetItem;
            pPointItem->setText(0, curve.points[iPoint].name());
            pPointItem->setFont(0, font());
            pPointItem->setData(0, skCurveRole, iCurve);
            pPointItem->setData(0, skPointRole, iPoint);
            pCurveItem->addChild(pPointItem);
        }

        // Add the item to the tree
        mpCurveTree->addTopLevelItem(pCurveItem);
    }
    setTreeSelected(iCurve, iPoint);
}

//! Add a new curve
void GraphReportDataEditor::addCurve()
{
    // Constants
    QList<GraphReportCurve> const kDefaultCurves = ReportDefaults::curves();

    // Retrieve the graph item
    if (!mpItem)
        return;
    GraphReportItem* pItem = (GraphReportItem*) mpItem;

    // Helper function
    auto createCurve = [pItem, &kDefaultCurves](QList<GraphReportPoint> const& points)
    {
        int iDefaultCurve = Utility::getRepeatedIndex(pItem->curves.count(), kDefaultCurves.size());
        GraphReportCurve curve = kDefaultCurves[iDefaultCurve];
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
        setTreeSelected(pItem->curves.size() - 1);

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
    auto [iCurve, iPoint] = getTreeSelected();
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

    // Retrieve the selected curve
    auto [iCurve, iPoint] = getTreeSelected();
    if (iCurve < 0)
        return;

    // Replace the current curve
    QList<GraphReportPoint> selectedPoints = getSelectedPoints();
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

    // Retrieve the selected curve
    auto [iCurve, iPoint] = getTreeSelected();
    if (iCurve < 0)
        return;
    GraphReportCurve& curve = pItem->curves[iCurve];

    // Create the input dialog
    bool ok = false;
    QString text = QInputDialog::getText(this, tr("Rename curve"), tr("Curve name: "), QLineEdit::Normal, mpCurveTree->currentItem()->text(0),
                                         &ok);
    if (ok && !text.isEmpty())
        curve.name = text;

    // Update the widgets content
    refresh();

    // Finish up the editing
    emit edited();
}

// //! Process removing the current curve
void GraphReportDataEditor::removeSelected()
{
    // Retrieve the graph item
    if (!mpItem)
        return;
    GraphReportItem* pItem = (GraphReportItem*) mpItem;

    // Remove the selected curve
    auto [iCurve, iPoint] = getTreeSelected();
    bool isCurve = iPoint < 0;
    if (iCurve >= 0 && iCurve < pItem->curves.size())
    {
        if (isCurve)
        {
            pItem->curves.remove(iCurve);
            qInfo() << tr("Curve is successfully removed");
        }
        else
        {
            pItem->curves[iCurve].points.remove(iPoint);
            qInfo() << tr("Point is successfully removed");
        }
    }

    // Update the widgets content
    refresh();

    // Select the last curve
    if (isCurve)
        setTreeSelected(pItem->curves.size() - 1);

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

//! Process changing tree selection
void GraphReportDataEditor::processTreeSelected()
{
    // Retrieve the graph item
    if (!mpItem)
        return;
    GraphReportItem* pItem = (GraphReportItem*) mpItem;

    // Update the header
    refreshHeader();

    // Highlight the selected curve
    auto [iCurve, iPoint] = getTreeSelected();
    if (iCurve >= 0 && iCurve < pItem->curves.size())
    {
        GraphReportCurve const& curve = pItem->curves[iCurve];
        if (curve.isEmpty())
            return;
        int numPoints = curve.points.size();
        mpGeometryView->clearSelection();
        bool isCurve = iPoint < 0;
        if (isCurve)
        {
            for (int i = 0; i != numPoints; ++i)
            {
                GraphReportPoint const& point = curve.points[i];
                mpGeometryView->addSelection(point.component, point.node);
            }
        }
        else
        {
            GraphReportPoint const& point = curve.points[iPoint];
            mpGeometryView->addSelection(point.component, point.node);
        }
        mpGeometryView->refresh();
    }
}

//! Retrieve the selected tree entity: curve or point
PairInt GraphReportDataEditor::getTreeSelected()
{
    PairInt const null(-1, -1);
    QTreeWidgetItem* pCurrent = mpCurveTree->currentItem();
    if (!pCurrent)
        return null;
    int iPoint = pCurrent->data(0, skPointRole).toInt();
    int iCurve = pCurrent->data(0, skCurveRole).toInt();
    return {iCurve, iPoint};
}

//! Set the selected tree item
void GraphReportDataEditor::setTreeSelected(int iCurve, int iPoint)
{
    if (!mpItem)
        return;
    GraphReportItem* pItem = (GraphReportItem*) mpItem;
    if (iCurve >= 0 && iCurve < pItem->curves.size())
    {
        QTreeWidgetItem* pCurveItem = mpCurveTree->topLevelItem(iCurve);
        if (pCurveItem && iPoint >= 0)
        {
            pCurveItem->setExpanded(true);
            mpCurveTree->setCurrentItem(pCurveItem->child(iPoint));
        }
        else
        {
            mpCurveTree->setCurrentItem(pCurveItem);
        }
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
    pItem->link = mpLinkSelector->currentData().toUuid();

    // Update the content
    refreshTree();

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

    QtVariantProperty* pMarkerFillProperty = mpManager->addProperty(kMarkerFill, QMetaType::Bool, tr("Marker fill"));
    pMarkerFillProperty->setValue(mCurve.markerFill);
    mpEditor->addProperty(pMarkerFillProperty);

    QtVariantProperty* pMarkerSkipProperty = mpManager->addProperty(kMarkerSkip, QMetaType::Int, tr("Marker skip"));
    pMarkerSkipProperty->setValue(mCurve.markerSkip);
    mpEditor->addProperty(pMarkerSkipProperty);
}

//! Specify connections
void ReportCurvePropertyEditor::createConnections()
{
    connect(mpManager, &CustomVariantPropertyManager::valueChanged, this, &ReportCurvePropertyEditor::setValue);
}

//! Change the plottable property value
void ReportCurvePropertyEditor::setValue(QtProperty* pProperty, QVariant value)
{
    // Constants
    QSet<ReportMarkerShape> kPlainMarkers = {ReportMarkerShape::kDot,         ReportMarkerShape::kCross,       ReportMarkerShape::kPlus,
                                             ReportMarkerShape::kStar,        ReportMarkerShape::kCrossSquare, ReportMarkerShape::kPlusSquare,
                                             ReportMarkerShape::kCrossCircle, ReportMarkerShape::kPlusCircle};
    // Get the property id
    if (!mpManager->contains(pProperty))
        return;
    int id = mpManager->id(pProperty);

    // Set property value
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
        if (kPlainMarkers.contains(mCurve.markerShape))
            mCurve.markerFill = false;
        break;
    case kMarkerSize:
        mCurve.markerSize = value.toInt();
        break;
    case kMarkerFill:
        mCurve.markerFill = value.toBool();
        break;
    case kMarkerSkip:
        mCurve.markerSkip = value.toInt();
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
