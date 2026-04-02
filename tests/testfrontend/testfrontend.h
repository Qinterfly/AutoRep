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
    void writeReport();

private:
    Frontend::MainWindow* mpMainWindow;
    Frontend::SessionEditor* mpSessionEditor;
    Frontend::ReportEditor* mpReportEditor;
};

}

#endif // TESTFRONTEND_H
