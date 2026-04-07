#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QListWidget>

#include "reportdataeditor.h"
#include "reportdocument.h"
#include "reportitem.h"
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

GraphReportDataEditor::GraphReportDataEditor(QWidget* pParent)
    : ReportDataEditor(pParent)
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
        QString name = curve.name.isEmpty() ? tr("Curve %1").arg(1 + i) : curve.name;
        if (curve.points.size() == 1)
            name = curve.points.first().name();
        QListWidgetItem* pCurve = new QListWidgetItem(name);
        mpCurveList->addItem(pCurve);
    }
    if (iCurve >= 0 && iCurve < numCurves)
        mpCurveList->setCurrentRow(iCurve);

    // Set the point list
    QSignalBlocker blockerPointList(mpPointList);
    mpPointList->clear();
    mpPointList->setVisible(pItem->subType == GraphReportItem::kModeshape);
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
    pLayout->addLayout(createCurveLayout());
    setLayout(pLayout);
}

//! Specify the connections between widgets
void GraphReportDataEditor::createConnections()
{
    connect(mpCurveList, &QListWidget::currentItemChanged, this, &GraphReportDataEditor::refresh);
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

//! Create the layout of curve widgets
QLayout* GraphReportDataEditor::createCurveLayout()
{
    // Create the widgets
    mpCurveList = new QListWidget;
    mpPointList = new QListWidget;

    // Initialize the widgets
    mpCurveList->setFont(font());
    mpPointList->setFont(font());
    mpCurveList->setSelectionMode(QListWidget::SingleSelection);
    mpPointList->setSelectionMode(QListWidget::NoSelection);

    // Combine the widgets
    QHBoxLayout* pLayout = new QHBoxLayout;
    pLayout->addWidget(mpCurveList);
    pLayout->addWidget(mpPointList);

    return pLayout;
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
