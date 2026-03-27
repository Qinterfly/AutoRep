#ifndef REPORTEDITOR_H
#define REPORTEDITOR_H

#include <QWidget>

QT_FORWARD_DECLARE_CLASS(QSettings)

namespace Frontend
{

class ReportEditor : public QWidget
{
    Q_OBJECT

public:
    ReportEditor(QSettings& settings, QWidget* pParent = nullptr);
    virtual ~ReportEditor() = default;

private:
    void createContent();

private:
    QSettings& mSettings;
};

}

#endif // REPORTEDITOR_H
