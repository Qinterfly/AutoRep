#include "reportelement.h"
#include "customplot.h"

using namespace Frontend;

IReportElement::IReportElement()
{
}

TextReportElement::TextReportElement()
{
}

void TextReportElement::draw()
{
    // TODO
}

IReportElement::Type TextReportElement::type() const
{
    return IReportElement::kText;
}

QString const& TextReportElement::text()
{
    return mText;
}

void TextReportElement::setText(QString const& text)
{
    mText = text;
}

GraphReportElement::GraphReportElement()
{
    mpPlot = new CustomPlot;
}

void GraphReportElement::draw()
{
    // TODO
}

IReportElement::Type GraphReportElement::type() const
{
    return IReportElement::kGraph;
}
