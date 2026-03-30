
#ifndef UIUTILITY_H
#define UIUTILITY_H

#include <QDir>
#include <QFont>

class vtkColor3d;

QT_FORWARD_DECLARE_CLASS(QTableWidgetItem);
QT_FORWARD_DECLARE_CLASS(QSettings)

namespace Frontend::Utility
{

// Text
QFont getFont();
QFont getMonospaceFont();

// Ui
QColor getColor(vtkColor3d color);
vtkColor3d getColor(QColor color);
int getRepeatedIndex(int index, int size);

// File
void modifyFileSuffix(QString& pathFile, QString const& expectedSuffix);
QDir getLastDirectory(QSettings const& settings);
QString getLastPathFile(QSettings const& settings);
void setLastPathFile(QSettings& settings, QString const& pathFile);

// Widgets
QTableWidgetItem* createTableItem(double value, Qt::AlignmentFlag alignment = Qt::AlignCenter);
QTableWidgetItem* createTableItem(std::vector<double> const& values, Qt::AlignmentFlag alignment = Qt::AlignCenter);
QTableWidgetItem* createTableItem(QString const& text, Qt::AlignmentFlag alignment = Qt::AlignCenter);
QDialog* showAsDialog(QWidget* pWidget, QString const& title = QString(), QWidget* pParent = nullptr, bool isModal = false);
}

#endif // UIUTILITY_H
