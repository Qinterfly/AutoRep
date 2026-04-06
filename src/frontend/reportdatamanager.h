#ifndef REPORTDATAMANAGER_H
#define REPORTDATAMANAGER_H

#include <QWidget>

namespace Frontend
{

class GraphDataEditor : public QWidget
{
    Q_OBJECT

public:
    GraphDataEditor(QWidget* pParent = nullptr);
    virtual ~GraphDataEditor() = default;
};

class ReportDataManager : public QWidget
{
    Q_OBJECT

public:
    ReportDataManager(QWidget* pParent = nullptr);
    virtual ~ReportDataManager() = default;
};

}

#endif // REPORTDATAMANAGER_H
