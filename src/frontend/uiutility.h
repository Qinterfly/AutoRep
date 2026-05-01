
#ifndef UIUTILITY_H
#define UIUTILITY_H

#include <QDir>
#include <QFont>

#include <Eigen/Core>

#include <vtkNew.h>

QT_FORWARD_DECLARE_CLASS(QTableWidgetItem);
QT_FORWARD_DECLARE_CLASS(QSettings)
QT_FORWARD_DECLARE_CLASS(QToolBar)
QT_FORWARD_DECLARE_CLASS(QComboBox)
QT_FORWARD_DECLARE_CLASS(QListWidgetItem)

class QCPScatterStyle;

class vtkColor3d;
class vtkRenderer;
class vtkLookupTable;
class vtkActor;

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
Eigen::Vector3d convert3d(std::vector<double> const& data);
void setIndexByKey(QComboBox* pComboBox, int key);
QTableWidgetItem* createTableItem(double value, Qt::AlignmentFlag alignment = Qt::AlignCenter);
QTableWidgetItem* createTableItem(std::vector<double> const& values, Qt::AlignmentFlag alignment = Qt::AlignCenter);
QTableWidgetItem* createTableItem(QString const& text, Qt::AlignmentFlag alignment = Qt::AlignCenter);
QDialog* showAsDialog(QWidget* pWidget, QString const& title = QString(), QWidget* pParent = nullptr, bool isModal = false);

// Render
vtkSmartPointer<vtkLookupTable> createBlueToRedColorMap();
void setIsometricView(vtkSmartPointer<vtkRenderer> renderer);
void setPlaneView(vtkSmartPointer<vtkRenderer> renderer, int dir, int sign);
void setCustomView(vtkSmartPointer<vtkRenderer> renderer, Eigen::Vector3d const& translation, Eigen::Vector3d const& rotation);
vtkSmartPointer<vtkActor> createCubeActor(Eigen::Vector3d const& position, double length);

// Icons
QIcon getIcon(QCPScatterStyle const& style, QSize const& size, bool isLine, bool isMarker);
}

#endif // UIUTILITY_H
