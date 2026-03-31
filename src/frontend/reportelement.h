#ifndef REPORTELEMENT_H
#define REPORTELEMENT_H

#include <QString>

namespace Frontend
{

class CustomPlot;

class IReportElement
{
public:
    enum Type
    {
        kText,
        kGraph
    };
    IReportElement();
    virtual ~IReportElement() = default;

    virtual void draw() = 0;
    virtual Type type() const = 0;
};

class TextReportElement : public IReportElement
{
public:
    TextReportElement();
    virtual ~TextReportElement() = default;

    void draw() override;
    Type type() const override;

    QString const& text();
    void setText(QString const& text);

private:
    QString mText;
};

class GraphReportElement : public IReportElement
{
public:
    GraphReportElement();
    virtual ~GraphReportElement() = default;

    void draw() override;
    Type type() const override;

private:
    CustomPlot* mpPlot;
};
}

#endif // REPORTELEMENT_H
