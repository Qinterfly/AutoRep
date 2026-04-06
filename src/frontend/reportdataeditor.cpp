#include "reportdataeditor.h"
#include "reportdocument.h"

using namespace Backend::Core;
using namespace Frontend;

ReportDataEditor::ReportDataEditor(ReportItem* pItem, QWidget* pParent)
    : QWidget(pParent)
    , mpItem(pItem)
{
}

GraphReportDataEditor::GraphReportDataEditor(ReportItem* pItem, QWidget* pParent)
    : ReportDataEditor(pItem, pParent)
{
}

ReportDataEditor::Type GraphReportDataEditor::type() const
{
    return ReportDataEditor::kGraph;
}

void GraphReportDataEditor::refresh()
{
    // TODO
}
