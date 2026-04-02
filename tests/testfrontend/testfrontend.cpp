#include <config.h>

#include "fileutility.h"
#include "reporteditor.h"
#include "sessioneditor.h"
#include "testfrontend.h"

using namespace Tests;
using namespace Backend;
using namespace Frontend;

TestFrontend::TestFrontend()
{
    mpMainWindow = new MainWindow;
    mpSessionEditor = mpMainWindow->sessionEditor();
    mpReportEditor = mpMainWindow->reportEditor();
}

//! Open a project
void TestFrontend::openProject()
{
    QString pathFile = Utility::combineFilePath(INPUT_DIR, "MC-21PoslePV.lms");
    QVERIFY(mpSessionEditor->openProject(pathFile));
    mpMainWindow->show();
}

//! Add the response bundles
void TestFrontend::addResponseBundles()
{
    QString pathFile = Utility::combineFilePath(INPUT_DIR, "MC-21Bundle.txt");

    // Set the response location in the project
    QStringList bundlePaths;
    bundlePaths.push_back("Section2/Отч 6,6 СВКД 140Н 3,40Гц/ResponsesSpectra/");
    bundlePaths.push_back("Section2/Отч 6,6 СВКД 180Н 3,37Гц/ResponsesSpectra/");

    // Get names of responses
    QFile file(pathFile);
    QStringList responseNames;
    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&file);
        while (!stream.atEnd())
        {
            QString line = stream.readLine();
            responseNames.push_back(line);
        }
        file.close();
    }

    // Process all the bundles
    int numBundles = bundlePaths.size();
    ResponseEditor* pResponseEditor = mpSessionEditor->responseEditor();
    for (int i = 0; i != numBundles; ++i)
    {
        QString prefix = bundlePaths[i];
        QStringList paths = responseNames;
        for (QString& v : paths)
            v.prepend(prefix);
        QVERIFY(pResponseEditor->addBundle(paths));
    }
    QVERIFY(pResponseEditor->collection().count() == numBundles);
}

void TestFrontend::writeReport()
{
    QString pathFile = Utility::combineFilePath(OUTPUT_DIR, "MC-21.pdf");
    mpReportEditor->writeReport(pathFile);
}

TestFrontend::~TestFrontend()
{
    QTest::qWait(50000);
    mpMainWindow->deleteLater();
}

QTEST_MAIN(TestFrontend)
