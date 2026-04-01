#ifndef REPORTITEM_H
#define REPORTITEM_H

#include <QRect>
#include <QString>

QT_FORWARD_DECLARE_CLASS(QPainter)

namespace Frontend
{

class CustomPlot;

class ReportItem
{
public:
    enum Type
    {
        kText,
        kGraph
    };
    ReportItem();
    virtual ~ReportItem() = default;

    virtual Type type() const = 0;
    virtual ReportItem* clone() const = 0;
    virtual void paint(QPainter* pPainter) = 0;

public:
    QRectF rect;
};

class TextReportItem : public ReportItem
{
public:
    TextReportItem();
    virtual ~TextReportItem() = default;

    Type type() const override;
    virtual ReportItem* clone() const override;
    void paint(QPainter* pPainter) override;

public:
    QString text;
};

class GraphReportItem : public ReportItem
{
public:
    GraphReportItem();
    virtual ~GraphReportItem() = default;

    Type type() const override;
    virtual ReportItem* clone() const override;
    void paint(QPainter* pPainter) override;

public:
    CustomPlot* pPlot;
};
}

#endif // REPORTITEM_H
