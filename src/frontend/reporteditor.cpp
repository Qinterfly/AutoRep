#include <QSettings>

#include "reporteditor.h"
#include "uiutility.h"

using namespace Frontend;

ReportEditor::ReportEditor(QSettings& settings, QWidget* pParent)
    : QWidget(pParent)
    , mSettings(settings)
{
    setFont(Utility::getFont());
    createContent();
}

//! Create all the widgets
void ReportEditor::createContent()
{
    // TODO
}
