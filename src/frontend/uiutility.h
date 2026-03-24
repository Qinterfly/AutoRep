
#ifndef UIUTILITY_H
#define UIUTILITY_H

#include <QFont>

QT_FORWARD_DECLARE_CLASS(QTableWidgetItem);

namespace Frontend::Utility
{

// Text
QFont getFont();
QFont getMonospaceFont();

// Widgets
QTableWidgetItem* createTableItem(double value, Qt::AlignmentFlag alignment = Qt::AlignCenter);
QTableWidgetItem* createTableItem(std::vector<double> const& values, Qt::AlignmentFlag alignment = Qt::AlignCenter);
QTableWidgetItem* createTableItem(QString const& text, Qt::AlignmentFlag alignment = Qt::AlignCenter);
QDialog* showAsDialog(QWidget* pWidget, QString const& title = QString(), QWidget* pParent = nullptr, bool isModal = false);
}

#endif // UIUTILITY_H
