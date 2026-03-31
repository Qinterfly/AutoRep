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

private:
    Frontend::MainWindow* mpMainWindow;
    Frontend::SessionEditor* mpSessionEditor;
};

}

#endif // TESTFRONTEND_H
