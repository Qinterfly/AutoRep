#ifndef TESTFRONTEND_H
#define TESTFRONTEND_H

#include <QTest>

#include "mainwindow.h"

namespace Tests
{

class TestFrontend : public QObject
{
    Q_OBJECT

public:
    TestFrontend();
    virtual ~TestFrontend();

private slots:
    void openProject();
    void addResponseBundles();
    void setImRePage();
    void setMultiImRePage();
    void setFreqAmpPage();
    void buildReport();
    void writeReport();

private:
    Frontend::MainWindow* mpMainWindow;
    Frontend::SessionEditor* mpSessionEditor;
    Frontend::ReportWorkspace* mpReportWorkspace;
};

}

#endif // TESTFRONTEND_H
