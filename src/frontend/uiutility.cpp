#include <QSettings>
#include <QTableWidgetItem>
#include <QToolBar>

#include <vtkColor.h>

#include "uiconstants.h"
#include "uiutility.h"

namespace Frontend::Utility
{

//! Get a default font
QFont getFont()
{
    QString fontName = "Roboto";
    uint fontSize = 12;
#ifdef Q_OS_WIN
    fontName = "Cambria";
    fontSize = 12;
#endif
    return QFont(fontName, fontSize);
}

//! Get a default monospace font
QFont getMonospaceFont()
{
    QString fontName = "RobotoMono";
    uint fontSize = 12;
#ifdef Q_OS_WIN
    fontName = "monofur";
    fontSize = 14;
#endif
    return QFont(fontName, fontSize);
}

//! Convert a VTK color to Qt one
QColor getColor(vtkColor3d color)
{
    return QColor::fromRgbF(color[0], color[1], color[2]);
}

//! Convert a Qt color to Vtk one
vtkColor3d getColor(QColor color)
{
    return vtkColor3d(color.redF(), color.greenF(), color.blueF());
}

//! Add shortcurt hints to all items contained in a tool bar
void setShortcutHints(QToolBar* pToolBar)
{
    QList<QAction*> actions = pToolBar->actions();
    int numActions = actions.size();
    for (int i = 0; i != numActions; ++i)
    {
        QAction* pAction = actions[i];
        QKeySequence shortcut = pAction->shortcut();
        if (shortcut.isEmpty())
            continue;
        pAction->setToolTip(QString("%1 (%2)").arg(pAction->toolTip(), shortcut.toString()));
    }
}

//! Get circular index
int getRepeatedIndex(int index, int size)
{
    if (index >= size)
        return index % size;
    return index;
}

//! Substitute a file suffix to the expected one, if necessary
void modifyFileSuffix(QString& pathFile, QString const& expectedSuffix)
{
    QFileInfo info(pathFile);
    QString currentSuffix = info.suffix();
    if (currentSuffix.isEmpty())
        pathFile.append(QString(".%1").arg(expectedSuffix));
    else if (currentSuffix != expectedSuffix)
        pathFile.replace(currentSuffix, expectedSuffix);
}

//! Retrieve last used directory
QDir getLastDirectory(QSettings const& settings)
{
    return QFileInfo(getLastPathFile(settings)).dir();
}

//! Retrieve last used path file
QString getLastPathFile(QSettings const& settings)
{
    return settings.value(Constants::Settings::skLastPathFile, QString()).toString();
}

//! Set last used path file
void setLastPathFile(QSettings& settings, QString const& pathFile)
{
    settings.setValue(Constants::Settings::skLastPathFile, pathFile);
}

//! Create table widget item associated with a double value
QTableWidgetItem* createTableItem(double value, Qt::AlignmentFlag alignment)
{
    QString text = QString::number(value);
    QTableWidgetItem* pItem = new QTableWidgetItem(text);
    pItem->setTextAlignment(alignment);
    return pItem;
};

//! Create table widget item associated with double values
QTableWidgetItem* createTableItem(std::vector<double> const& values, Qt::AlignmentFlag alignment)
{
    QString text;
    QTextStream stream(&text);
    int numValues = values.size();
    for (int i = 0; i != numValues; ++i)
    {
        if (i > 0)
            stream << " ";
        stream << values[i];
    }
    return createTableItem(text, alignment);
}

//! Create table widget item associated with a string value
QTableWidgetItem* createTableItem(QString const& text, Qt::AlignmentFlag alignment)
{
    QTableWidgetItem* pItem = new QTableWidgetItem(text);
    pItem->setTextAlignment(alignment);
    return pItem;
}
}
