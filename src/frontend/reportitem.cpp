#include "reportitem.h"
#include "customplot.h"

using namespace Frontend;

ReportItem::ReportItem()
    : rect(0, 0, 0, 0)
{
}

TextReportItem::TextReportItem()
{
}

ReportItem::Type TextReportItem::type() const
{
    return ReportItem::kText;
}

ReportItem* TextReportItem::clone() const
{
    TextReportItem* pResult = new TextReportItem;
    pResult->rect = rect;
    pResult->text = text;
    return pResult;
}

void TextReportItem::paint(QPainter* pPainter)
{
    // TODO
}

GraphReportItem::GraphReportItem()
{
    pPlot = new CustomPlot;
}

ReportItem::Type GraphReportItem::type() const
{
    return ReportItem::kGraph;
}

ReportItem* GraphReportItem::clone() const
{
    GraphReportItem* pResult = new GraphReportItem;
    pResult->rect = rect;
    pResult->pPlot = new CustomPlot;
    return pResult;
}

void GraphReportItem::paint(QPainter* pPainter)
{
    // TODO
}
