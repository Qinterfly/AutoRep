#include <QObject>

#include "reportdefaults.h"
#include "reportitem.h"

#include "reportdocument.h"

using namespace Backend::Core;

//! Create curves
QList<GraphReportCurve> ReportDefaults::curves()
{
    QList<GraphReportCurve> result;
    result.emplaceBack("red", ReportMarkerShape::kCircle, true);
    result.emplaceBack("green", ReportMarkerShape::kCircle, false);
    result.emplaceBack("blue", ReportMarkerShape::kSquare, true);
    result.emplaceBack("orange", ReportMarkerShape::kSquare, false);
    result.emplaceBack("cyan", ReportMarkerShape::kTriangle, true);
    result.emplaceBack("magenta", ReportMarkerShape::kTriangle, false);
    result.emplaceBack("gray", ReportMarkerShape::kTriangleInverted, true);
    result.emplaceBack("purple", ReportMarkerShape::kTriangleInverted, false);
    result.emplaceBack("brown", ReportMarkerShape::kCross);
    result.emplaceBack("chocolate", ReportMarkerShape::kPlus);
    result.emplaceBack("olive", ReportMarkerShape::kStar);
    result.emplaceBack("steelblue", ReportMarkerShape::kCrossSquare);
    result.emplaceBack("firebrick", ReportMarkerShape::kPlusSquare);
    return result;
}

//! Create a document with page preset
ReportDocument ReportDefaults::document()
{
    ReportDocument result;
    result.add(ReportDefaults::imRePage());
    result.add(ReportDefaults::multiImRePage());
    result.add(ReportDefaults::freqAmpPage());
    result.add(ReportDefaults::modeshapePage());
    return result;
}

//! Create a page with imaginary and real parts of a spectrum
ReportPage ReportDefaults::imRePage()
{
    ReportPage page(QObject::tr("Im-Re"));

    // Create an imaginary graph
    GraphReportItem* pImag = new GraphReportItem;
    pImag->name = QObject::tr("Imaginary");
    pImag->rect = QRect(25, 35, 150, 110);
    pImag->subType = GraphReportItem::kImag;
    pImag->responseDir = ReportDirection::kY;
    pImag->unit = "m/s^2";
    pImag->xLabel = QObject::tr("f, Hz");
    pImag->yLabel = QObject::tr("a, ${UNIT}");
    pImag->showBundleFreq = true;

    // Create a real graph
    GraphReportItem* pReal = new GraphReportItem;
    pReal->name = QObject::tr("Real");
    pReal->rect = QRect(25, 150, 150, 110);
    pReal->subType = GraphReportItem::kReal;
    pReal->responseDir = ReportDirection::kY;
    pReal->unit = "m/s^2";
    pReal->link = pImag->id;
    pReal->xLabel = QObject::tr("f, Hz");
    pReal->yLabel = QObject::tr("a, ${UNIT}");
    pReal->showBundleFreq = true;

    // Create title
    TextReportItem* pTitle = new TextReportItem;
    pTitle->name = QObject::tr("Title");
    pTitle->rect = QRect(25, 10, 150, 20);
    pTitle->text = QObject::tr("Mode name\nf = ${FREQ} Hz\nExcitation F = ${FORCE} N");

    // Create the caption
    TextReportItem* pCaption = new TextReportItem;
    pCaption->name = QObject::tr("Caption");
    pCaption->rect = QRect(65, 265, 80, 10);
    pCaption->text = QObject::tr("Figure X.YY");

    // Create the page number
    TextReportItem* pNumber = new TextReportItem;
    pNumber->name = QObject::tr("Page");
    pNumber->rect = QRect(100, 280, 10, 10);
    pNumber->text = QObject::tr("PP");

    // Combine
    page.add(pImag);
    page.add(pReal);
    page.add(pTitle);
    page.add(pCaption);
    page.add(pNumber);

    return page;
}

//! Create a page with multiple imaginary and real parts of a spectrum
ReportPage ReportDefaults::multiImRePage()
{
    ReportPage page(QObject::tr("Multi Im-Re"));

    // Create an imaginary graph
    GraphReportItem* pImag = new GraphReportItem;
    pImag->name = QObject::tr("Imaginary");
    pImag->rect = QRect(25, 35, 150, 110);
    pImag->subType = GraphReportItem::kMultiImag;
    pImag->responseDir = ReportDirection::kY;
    pImag->unit = "m/s^2";
    pImag->xLabel = QObject::tr("f, Hz");
    pImag->yLabel = QObject::tr("a, ${UNIT}");

    // Create a real graph
    GraphReportItem* pReal = new GraphReportItem;
    pReal->name = QObject::tr("Real");
    pReal->rect = QRect(25, 150, 150, 110);
    pReal->subType = GraphReportItem::kMultiReal;
    pReal->responseDir = ReportDirection::kY;
    pReal->unit = "m/s^2";
    pReal->link = pImag->id;
    pReal->xLabel = QObject::tr("f, Hz");
    pReal->yLabel = QObject::tr("a, ${UNIT}");

    // Create title
    TextReportItem* pTitle = new TextReportItem;
    pTitle->name = QObject::tr("Title");
    pTitle->rect = QRect(25, 10, 150, 20);
    pTitle->text = QObject::tr("Mode name\nExcitation\nPoint ${POINT}");

    // Create the caption
    TextReportItem* pCaption = new TextReportItem;
    pCaption->name = QObject::tr("Caption");
    pCaption->rect = QRect(65, 265, 80, 10);
    pCaption->text = QObject::tr("Figure X.YY");

    // Create the page number
    TextReportItem* pNumber = new TextReportItem;
    pNumber->name = QObject::tr("Page");
    pNumber->rect = QRect(100, 280, 10, 10);
    pNumber->text = QObject::tr("PP");

    // Combine
    page.add(pImag);
    page.add(pReal);
    page.add(pTitle);
    page.add(pCaption);
    page.add(pNumber);

    return page;
}

//! Create a page with freq imaginary and real parts of a spectrum
ReportPage ReportDefaults::freqAmpPage()
{
    ReportPage page(QObject::tr("Freq Amp"));

    // Create an imaginary graph
    GraphReportItem* pAmp = new GraphReportItem;
    pAmp->name = QObject::tr("Amplitude");
    pAmp->rect = QRect(25, 35, 150, 110);
    pAmp->subType = GraphReportItem::kFreqAmp;
    pAmp->responseDir = ReportDirection::kY;
    pAmp->unit = "m";
    pAmp->xLabel = QObject::tr("f, Hz");
    pAmp->yLabel = QObject::tr("a, ${UNIT}");
    pAmp->swapAxes = true;

    // Create a picture
    PictureReportItem* pPic = new PictureReportItem;
    pPic->name = QObject::tr("Picture");
    pPic->rect = QRect(25, 150, 150, 110);

    // Create title
    TextReportItem* pTitle = new TextReportItem;
    pTitle->name = QObject::tr("Title");
    pTitle->rect = QRect(25, 10, 150, 20);
    pTitle->text = QObject::tr("Mode name\nExcitation");

    // Create the caption
    TextReportItem* pCaption = new TextReportItem;
    pCaption->name = QObject::tr("Caption");
    pCaption->rect = QRect(65, 265, 80, 10);
    pCaption->text = QObject::tr("Figure X.YY");

    // Create the page number
    TextReportItem* pNumber = new TextReportItem;
    pNumber->name = QObject::tr("Page");
    pNumber->rect = QRect(100, 280, 10, 10);
    pNumber->text = QObject::tr("PP");

    // Combine
    page.add(pAmp);
    page.add(pPic);
    page.add(pTitle);
    page.add(pCaption);
    page.add(pNumber);

    return page;
}

//! Create a page with a modeshape
ReportPage ReportDefaults::modeshapePage()
{
    ReportPage page(QObject::tr("Modeshape"));
    page.layout.setOrientation(QPageLayout::Landscape);

    // Create an fuselage graph
    GraphReportItem* pFus = new GraphReportItem;
    pFus->name = QObject::tr("Fuselage");
    pFus->rect = QRect(20, 30, 170, 50);
    pFus->subType = GraphReportItem::kModeshape;
    pFus->coordDir = ReportDirection::kX;
    pFus->responseDir = ReportDirection::kY;
    pFus->unit = "m/s^2";
    pFus->xLabel = QObject::tr("${CDIR}, m");
    pFus->yLabel = QObject::tr("${RDIR}; a, ${UNIT}");
    pFus->showLegend = false;

    // Create a wing graph
    GraphReportItem* pWing = new GraphReportItem;
    pWing->name = QObject::tr("Wing");
    pWing->rect = QRect(20, 80, 170, 50);
    pWing->subType = GraphReportItem::kModeshape;
    pWing->coordDir = ReportDirection::kZ;
    pWing->responseDir = ReportDirection::kY;
    pWing->unit = "m/s^2";
    pWing->xLabel = QObject::tr("${CDIR}, m");
    pWing->yLabel = QObject::tr("${RDIR}; a, ${UNIT}");
    pWing->showLegend = false;

    // Create a horizontal stabilizer graph
    GraphReportItem* pHStab = new GraphReportItem;
    pHStab->name = QObject::tr("Hor. stab.");
    pHStab->rect = QRect(45, 130, 120, 50);
    pHStab->subType = GraphReportItem::kModeshape;
    pHStab->coordDir = ReportDirection::kZ;
    pHStab->responseDir = ReportDirection::kY;
    pHStab->unit = "m/s^2";
    pHStab->xLabel = QObject::tr("${CDIR}, m");
    pHStab->yLabel = QObject::tr("${RDIR}; a, ${UNIT}");
    pHStab->showLegend = false;

    // Create a vertical stabilizer graph
    GraphReportItem* pVStab = new GraphReportItem;
    pVStab->name = QObject::tr("Vert. stab.");
    pVStab->rect = QRect(200, 80, 70, 100);
    pVStab->subType = GraphReportItem::kModeshape;
    pVStab->coordDir = ReportDirection::kY;
    pVStab->responseDir = ReportDirection::kZ;
    pVStab->unit = "m/s^2";
    pVStab->xLabel = QObject::tr("${CDIR}, m");
    pVStab->yLabel = QObject::tr("${RDIR}; a, ${UNIT}");
    pVStab->swapAxes = true;
    pVStab->showLegend = false;

    // Create table
    TableReportItem* pTable = new TableReportItem;
    pTable->name = QObject::tr("Table");
    pTable->rect = QRect(205, 30, 60, 45);
    pTable->resize(3, 2);

    // Create title
    TextReportItem* pTitle = new TextReportItem;
    pTitle->name = QObject::tr("Title");
    pTitle->rect = QRect(65, 15, 150, 20);
    pTitle->text = QObject::tr("Mode name\nExcitation");

    // Create the caption
    TextReportItem* pCaption = new TextReportItem;
    pCaption->name = QObject::tr("Caption");
    pCaption->rect = QRect(100, 190, 80, 10);
    pCaption->text = QObject::tr("Figure X.YY");

    // Create the page number
    TextReportItem* pNumber = new TextReportItem;
    pNumber->name = QObject::tr("Page");
    pNumber->rect = QRect(5, 100, 10, 10);
    pNumber->text = QObject::tr("PP");
    pNumber->angle = 90;

    // Combine
    page.add(pFus);
    page.add(pWing);
    page.add(pHStab);
    page.add(pVStab);
    page.add(pTable);
    page.add(pTitle);
    page.add(pCaption);
    page.add(pNumber);

    return page;
}
