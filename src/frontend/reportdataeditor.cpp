#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QInputDialog>
#include <QLabel>
#include <QListWidget>
#include <QToolBar>

#include "geometryview.h"
#include "reportdataeditor.h"
#include "reportdocument.h"
#include "reportitem.h"
#include "uiconstants.h"
#include "uiutility.h"

using namespace Backend::Core;
using namespace Frontend;

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
        if (mpUnitSelector->itemText(i) == pItem->unit)
        {
            iUnit = i;
            break;
        }
    }
    mpUnitSelector->setCurrentIndex(iUnit);

    // Set the coordinate direction
    QSignalBlocker blockerCoordDir(mpCoordDirSelector);
    Utility::setIndexByKey(mpCoordDirSelector, pItem->coordDir);

    // Set the response direction
    QSignalBlocker blockerResponseDir(mpResponseDirSelector);
    Utility::setIndexByKey(mpResponseDirSelector, pItem->responseDir);

    // Set the curve list
    QSignalBlocker blockerCurveList(mpCurveList);
    int iCurve = mpCurveList->currentRow();
    mpCurveList->clear();
    int numCurves = pItem->curves.size();
    for (int i = 0; i != numCurves; ++i)
    {
        GraphReportCurve const& curve = pItem->curves[i];
        QString name = curve.name;
        if (name.isEmpty())
            name = curve.points.size() == 1 ? curve.points.first().name() : tr("Curve %1").arg(1 + i);
        QListWidgetItem* pCurve = new QListWidgetItem(name);
        mpCurveList->addItem(pCurve);
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
    connect(mpCurveList, &QListWidget::currentItemChanged, this, &GraphReportDataEditor::processCurveSelected);
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
    mpUnitSelector->addItems({QString(), "m/s^2", "(m/s^2)/N"});

    // Combine the widgets
    QGridLayout* pLayout = new QGridLayout;
    pLayout->addWidget(new QLabel(tr("Type: ")), 0, 0);
    pLayout->addWidget(mpSubTypeSelector, 0, 1);
    pLayout->addWidget(new QLabel(tr("Unit: ")), 0, 2);
    pLayout->addWidget(mpUnitSelector, 0, 3);
    pLayout->addWidget(new QLabel(tr("Coord dir: ")), 1, 0);
    pLayout->addWidget(mpCoordDirSelector, 1, 1);
    pLayout->addWidget(new QLabel(tr("Response dir: ")), 1, 2);
    pLayout->addWidget(mpResponseDirSelector, 1, 3);
    pLayout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Preferred), 0, 4);

    return pLayout;
}

//! Create a set of actions to manipulate curves
QWidget* GraphReportDataEditor::createToolBar()
{
    QToolBar* pToolBar = new QToolBar;
    pToolBar->setIconSize(Constants::Size::skToolBarIcon);
    pToolBar->addAction(QIcon(":/icons/data-add.svg"), tr("Add curve"), this, &GraphReportDataEditor::addCurve);
    pToolBar->addAction(QIcon(":/icons/data-edit.svg"), tr("Edit curve"), this, &GraphReportDataEditor::editSelectedCurve);
    pToolBar->addAction(QIcon(":/icons/data-rename.svg"), tr("Rename curve"), this, &GraphReportDataEditor::renameSelectedCurve);
    pToolBar->addAction(QIcon(":/icons/data-remove.svg"), tr("Remove curve"), this, &GraphReportDataEditor::removeSelectedCurve);
    Utility::setShortcutHints(pToolBar);
    return pToolBar;
}

//! Create the layout of curve widgets
QLayout* GraphReportDataEditor::createCurveLayout()
{
    // Constants
    QMargins const kMargins(3, 3, 3, 3);

    // Create the lists
    mpCurveList = new QListWidget;
    mpPointList = new QListWidget;

    // Initialize the lists
    mpCurveList->setFont(font());
    mpPointList->setFont(font());
    mpCurveList->setSelectionMode(QListWidget::SingleSelection);
    mpPointList->setSelectionMode(QListWidget::NoSelection);

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

    // Add the curve
    QList<GraphReportPoint> selectedPoints = getSelectedPoints();
    if (!selectedPoints.isEmpty())
    {
        if (pItem->isMultiPointCurve())
        {
            pItem->curves.push_back(selectedPoints);
        }
        else
        {
            for (GraphReportPoint const& p : std::as_const(selectedPoints))
            {
                GraphReportCurve curve({p});
                pItem->curves.push_back(curve);
            }
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

//! Replace the selected curve with the new one
void GraphReportDataEditor::editSelectedCurve()
{
    // Retrieve the graph item
    if (!mpItem)
        return;
    GraphReportItem* pItem = (GraphReportItem*) mpItem;

    // Replace the current curve
    QList<GraphReportPoint> selectedPoints = getSelectedPoints();
    int iCurve = mpCurveList->currentRow();
    if (iCurve >= 0 && iCurve < pItem->curves.size())
    {
        if (!selectedPoints.isEmpty())
        {
            if (pItem->isMultiPointCurve())
                pItem->curves[iCurve] = GraphReportCurve(selectedPoints);
            else
                pItem->curves[iCurve] = GraphReportCurve({selectedPoints.first()});
            qInfo() << tr("Curve is edited");
        }
        else
        {
            qWarning() << tr("Cannot edit the curve, since there are no selected points");
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
    pItem->unit = mpUnitSelector->currentText();

    // Update the content
    refresh();

    // Finish up the editing
    emit edited();
}

//! Helper function to create a combobox with predefined directions
QComboBox* createDirComboBox()
{
    QComboBox* pResult = new QComboBox;
    pResult->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    pResult->addItem(QString(), ReportDirection::kNone);
    pResult->addItem("X", ReportDirection::kX);
    pResult->addItem("Y", ReportDirection::kY);
    pResult->addItem("Z", ReportDirection::kZ);
    return pResult;
}
