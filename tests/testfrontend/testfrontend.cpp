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

//! Set the modeshape page of the report
void TestFrontend::setModeshapePage()
{
    // Get the designer
    ReportDesigner* pDesigner = mpReportWorkspace->designer(3);
    QVERIFY(pDesigner);

    // Get the items
    ReportPage& page = pDesigner->page();
    GraphReportItem* pFus = (GraphReportItem*) page.get(0);
    GraphReportItem* pWing = (GraphReportItem*) page.get(1);
    GraphReportItem* pHStab = (GraphReportItem*) page.get(2);
    GraphReportItem* pVStab = (GraphReportItem*) page.get(3);
    TextReportItem* pTitle = (TextReportItem*) page.get(4);

    // clang-format off
    
    // Add the fuselage
    pFus->addCurve(GraphReportCurve({"F:5p1", "F:5p4", "F:5p7", "F:5p14", "F:4p25", "F:4p22", "F:4p19"}, "black", ReportMarkerShape::kCircle, true));
    pFus->addCurve(GraphReportCurve({"F:5p2", "F:5p5", "F:5p8", "F:5p15", "F:4p26", "F:4p23", "F:4p20"}, "black", ReportMarkerShape::kCircle, false));

    pFus->addCurve(GraphReportCurve({"OOSh:3p17"}, "red", ReportMarkerShape::kSquare, true));
    pFus->addCurve(GraphReportCurve({"OOSh:3p20"}, "red", ReportMarkerShape::kSquare, false));

    pFus->addCurve(GraphReportCurve({"OOSh:3p21"}, "green", ReportMarkerShape::kTriangle, true));
    pFus->addCurve(GraphReportCurve({"OOSh:3p24"}, "green", ReportMarkerShape::kTriangle, false));

    pFus->addCurve(GraphReportCurve({"POSh:5p10"}, "blue", ReportMarkerShape::kTriangleInverted, true));
    pFus->addCurve(GraphReportCurve({"POSh:5p13"}, "blue", ReportMarkerShape::kTriangleInverted, false));

    // Add the wing
    pWing->addCurve(GraphReportCurve({"W:1p2", "W:1p6", "W:1p11", "W:1p14", "W:1p17", "W:1p22"}, "black", ReportMarkerShape::kCircle, true));
    pWing->addCurve(GraphReportCurve({"W:1p3", "W:1p7", "W:1p12", "W:1p15", "W:1p18", "W:1p23"}, "black", ReportMarkerShape::kCircle, false));
    pWing->addCurve(GraphReportCurve({"W:2p2", "W:2p6", "W:2p11", "W:2p14", "W:2p17", "W:2p22"}, "black", ReportMarkerShape::kCircle, true));
    pWing->addCurve(GraphReportCurve({"W:2p3", "W:2p7", "W:2p12", "W:2p15", "W:2p18", "W:2p23"}, "black", ReportMarkerShape::kCircle, false));
    
    pWing->addCurve(GraphReportCurve({"En:3p2", "En:3p7"}, "red", ReportMarkerShape::kSquare, true));
    pWing->addCurve(GraphReportCurve({"En:3p4", "En:3p8"}, "red", ReportMarkerShape::kSquare, false));
    pWing->addCurve(GraphReportCurve({"En:3p15", "En:3p10"}, "red", ReportMarkerShape::kSquare, true));
    pWing->addCurve(GraphReportCurve({"En:3p16", "En:3p12"}, "red", ReportMarkerShape::kSquare, false));

    pWing->addCurve(GraphReportCurve({"Ail:1p4", "Ail:1p8"}, "green", ReportMarkerShape::kTriangle, true));
    pWing->addCurve(GraphReportCurve({"Ail:2p4", "Ail:2p8"}, "green", ReportMarkerShape::kTriangle, true));

    pWing->addCurve(GraphReportCurve({"Fl:1p9", "Fl:1p19", "Fl:1p20", "Fl:1p24"}, "blue", ReportMarkerShape::kTriangleInverted, true));
    pWing->addCurve(GraphReportCurve({"Fl:2p9", "Fl:2p19", "Fl:2p20", "Fl:2p24"}, "blue", ReportMarkerShape::kTriangleInverted, true));

    // Add the horizontal stabilizer
    pHStab->addCurve(GraphReportCurve({"GO:4p8", "GO:4p10", "GO:4p11"}, "black", ReportMarkerShape::kCircle, true));
    pHStab->addCurve(GraphReportCurve({"GO:4p14", "GO:4p16", "GO:4p17"}, "black", ReportMarkerShape::kCircle, true));

    pHStab->addCurve(GraphReportCurve({"El:4p9", "El:4p12"}, "red", ReportMarkerShape::kSquare, true));
    pHStab->addCurve(GraphReportCurve({"El:4p15", "El:4p18"}, "red", ReportMarkerShape::kSquare, true));

    // Add the vertical stabilizer
    pVStab->addCurve(GraphReportCurve({"VO:4p2", "VO:4p4", "VO:4p5"}, "black", ReportMarkerShape::kCircle, true));
    pVStab->addCurve(GraphReportCurve({"Rd:4p3", "Rd:4p6"}, "red", ReportMarkerShape::kSquare, true));

    // clang-format on

    // Set the title
    pTitle->text = "Симметричные вертикальные колебания двигателей\nВозбуждение с двигателей";
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
