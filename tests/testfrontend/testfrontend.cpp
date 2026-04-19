#include <config.h>

#include "fileutility.h"
#include "reportdesigner.h"
#include "reportworkspace.h"
#include "sessioneditor.h"
#include "testfrontend.h"

using namespace Tests;
using namespace Backend;
using namespace Backend::Core;
using namespace Frontend;

TestFrontend::TestFrontend()
{
    mpMainWindow = new MainWindow;
    mpSessionEditor = mpMainWindow->sessionEditor();
    mpReportWorkspace = mpMainWindow->reportWorkspace();
}

//! Open a project
void TestFrontend::openProject()
{
    QString pathFile = Utility::combineFilePath(INPUT_DIR, "MC-21PoslePV.lms");
    QVERIFY(mpSessionEditor->openProject(pathFile));
    mpMainWindow->show();
}

//! Add response bundles from the project
void TestFrontend::addResponseBundles()
{
    QString pathFile = Utility::combineFilePath(INPUT_DIR, "MC-21Bundle.txt");

    // Set the response location in the project
    QStringList bundlePaths;
    bundlePaths.push_back("Section2/Отч 6,7 СВКД 60Н 3,47Гц/ResponsesSpectra/");
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

//! Set the imaginary/real page of the report
void TestFrontend::setImRePage()
{
    // Plottable settings
    QStringList points = {"En:3p2", "En:3p7"};
    QList<QColor> colors = {Qt::red, Qt::blue};
    QList<ReportMarkerShape> markerShapes = {ReportMarkerShape::kPlus, ReportMarkerShape::kDisc};

    // Get the designer
    ReportDesigner* pDesigner = mpReportWorkspace->designer(0);
    QVERIFY(pDesigner);

    // Get the items
    ReportPage& page = pDesigner->page();
    GraphReportItem* pImag = (GraphReportItem*) page.get(0);
    TextReportItem* pTitle = (TextReportItem*) page.get(2);

    // Add the curves
    int numPoints = points.size();
    for (int i = 0; i != numPoints; ++i)
    {
        GraphReportCurve& curve = pImag->addPoint(points[i]);
        curve.lineColor = colors[i];
        curve.markerShape = markerShapes[i];
    }

    // Set the title
    pTitle->text = "Симметричные вертикальные колебания двигателей\nf = ${FREQ} Гц\nВозбуждение с двигателей, F = ${FORCE} Н";

    // Refresh the page
    pDesigner->refresh();

    // Select the first item
    pDesigner->selectItem(0);
}

//! Set the multi imaginary/real page of the report
void TestFrontend::setMultiImRePage()
{
    // Plottable settings
    QString point = "En:3p7";

    // Get the designer
    ReportDesigner* pDesigner = mpReportWorkspace->designer(1);
    QVERIFY(pDesigner);

    // Get the items
    ReportPage& page = pDesigner->page();
    GraphReportItem* pImag = (GraphReportItem*) page.get(0);
    TextReportItem* pTitle = (TextReportItem*) page.get(2);

    // Add the curves
    pImag->addPoint(point);

    // Set the title
    pTitle->text = "Симметричные вертикальные колебания двигателей\nВозбуждение с двигателей\nТочка ${POINT}";

    // Refresh the page
    pDesigner->refresh();

    // Select the first item
    pDesigner->selectItem(0);
}

//! Set the freq imaginary/real page of the report
void TestFrontend::setFreqAmpPage()
{
    QStringList points = {"En:3p2", "En:3p7", "En:3p10", "En:3p15"};
    QList<QColor> colors = {Qt::red, Qt::green, Qt::blue, Qt::black};
    QList<ReportMarkerShape> markerShapes = {ReportMarkerShape::kPlus, ReportMarkerShape::kDisc, ReportMarkerShape::kStar,
                                             ReportMarkerShape::kTriangle};

    // Get the designer
    ReportDesigner* pDesigner = mpReportWorkspace->designer(2);
    QVERIFY(pDesigner);

    // Get the items
    ReportPage& page = pDesigner->page();
    GraphReportItem* pAmp = (GraphReportItem*) page.get(0);
    TextReportItem* pTitle = (TextReportItem*) page.get(1);

    // Add the curves
    int numPoints = points.size();
    for (int i = 0; i != numPoints; ++i)
    {
        GraphReportCurve& curve = pAmp->addPoint(points[i]);
        curve.lineColor = colors[i];
        curve.markerShape = markerShapes[i];
    }

    // Set the title
    pTitle->text = "Симметричные вертикальные колебания двигателей\nВозбуждение с двигателей";

    // Refresh the page
    pDesigner->refresh();

    // Select the first item
    pDesigner->selectItem(0);
}

//! Make up the report
void TestFrontend::buildReport()
{
    // TODO
}

//! Write the report to a file
void TestFrontend::writeReport()
{
    QString pathFile = Utility::combineFilePath(OUTPUT_DIR, "MC-21.pdf");
    mpReportWorkspace->print(pathFile);
}

TestFrontend::~TestFrontend()
{
    QTest::qWait(50000);
    mpMainWindow->deleteLater();
}

QTEST_MAIN(TestFrontend)
