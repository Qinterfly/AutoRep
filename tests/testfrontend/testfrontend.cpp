#include <config.h>

#include "testfrontend.h"

using namespace Tests;
using namespace Frontend;

TestFrontend::TestFrontend()
{
    mpMainWindow = new MainWindow;
}

TestFrontend::~TestFrontend()
{
    mpMainWindow->show();
    QTest::qWait(30000);
    mpMainWindow->deleteLater();
}

QTEST_MAIN(TestFrontend)
