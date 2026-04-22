
#ifndef FILEUTILITY_H
#define FILEUTILITY_H

#include <QDir>

QT_FORWARD_DECLARE_CLASS(QPageLayout)

namespace Backend::Utility
{

QSharedPointer<QFile> openFile(QString const& pathFile, QString const& expectedSuffix, QIODevice::OpenModeFlag const& mode);

//! Base case for combining a filepath
template<typename T>
QString combineFilePath(T const& value)
{
    return value;
}

//! Combine several components of a filepath, adding slashes if necessary
template<typename T, typename... Args>
QString combineFilePath(T const& first, Args... args)
{
    return QDir(first).filePath(combineFilePath(args...));
}

// Convert to Json
QJsonValue toJson(QUuid const& id);
QJsonValue toJson(QRect const& rect);
QJsonValue toJson(QFont const& font);
QJsonValue toJson(QPageLayout const& layout);
QJsonValue toJson(Qt::Alignment const& align);
QJsonValue toJson(QColor const& color);
QJsonValue toJson(QPair<double, double> const& pair);
QJsonValue toJson(QByteArray const& data);
}

#endif // FILEUTILITY_H
