#ifndef REPORTEDITOR_H
#define REPORTEDITOR_H

#include <QWidget>

#include "reportdocument.h"

QT_FORWARD_DECLARE_CLASS(QSettings)

namespace Frontend
{

class CustomTabWidget;

class ReportEditor : public QWidget
{
    Q_OBJECT

public:
    ReportEditor(QSettings& settings, QWidget* pParent = nullptr);
    virtual ~ReportEditor() = default;

    QSize sizeHint() const;

    bool exportReport(QString const& pathFile);

private:
    void createContent();
    void initialize();
    void refresh();

private:
    QSettings& mSettings;
    ReportDocument mDocument;
    CustomTabWidget* mpTabWidget;
};

}

#endif // REPORTEDITOR_H
