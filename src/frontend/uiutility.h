
#ifndef UIUTILITY_H
#define UIUTILITY_H

#include <QDir>
#include <QFont>


QT_FORWARD_DECLARE_CLASS(QTableWidgetItem);
QT_FORWARD_DECLARE_CLASS(QSettings)
QT_FORWARD_DECLARE_CLASS(QToolBar)
QT_FORWARD_DECLARE_CLASS(QComboBox)
QT_FORWARD_DECLARE_CLASS(QListWidgetItem)

class vtkColor3d;
class QCPScatterStyle;

namespace Frontend::Utility
{

// Text
QFont getFont();
QFont getMonospaceFont();

// Ui
QColor getColor(vtkColor3d color);
vtkColor3d getColor(QColor color);
void setShortcutHints(QToolBar* pToolBar);
int getRepeatedIndex(int index, int size);
int showSaveDialog(QWidget* pWidget, QString const& title, QString const& message);

// File
void modifyFileSuffix(QString& pathFile, QString const& expectedSuffix);
QDir getLastDirectory(QSettings const& settings);
QString getLastPathFile(QSettings const& settings);
void setLastPathFile(QSettings& settings, QString const& pathFile);

// Widgets
void setIndexByKey(QComboBox* pComboBox, int key);
QTableWidgetItem* createTableItem(double value, Qt::AlignmentFlag alignment = Qt::AlignCenter);
QTableWidgetItem* createTableItem(std::vector<double> const& values, Qt::AlignmentFlag alignment = Qt::AlignCenter);
QTableWidgetItem* createTableItem(QString const& text, Qt::AlignmentFlag alignment = Qt::AlignCenter);
QDialog* showAsDialog(QWidget* pWidget, QString const& title = QString(), QWidget* pParent = nullptr, bool isModal = false);

// Icons
QIcon getIcon(QCPScatterStyle const& style, QSize const& size, bool isLine, bool isMarker);
}

#endif // UIUTILITY_H
