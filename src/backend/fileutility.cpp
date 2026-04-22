#include <QColor>
#include <QFont>
#include <QJsonObject>
#include <QPageLayout>
#include <QRect>

#include "fileutility.h"

namespace Backend::Utility
{

//! Open a file and check its extension
QSharedPointer<QFile> openFile(QString const& pathFile, QString const& expectedSuffix, QIODevice::OpenModeFlag const& mode)
{
    // Check if the output file has the correct extension
    QFileInfo info(pathFile);
    if (info.suffix() != expectedSuffix)
    {
        qWarning() << QObject::tr("Unknown extension was specified for the file: %1").arg(pathFile);
        return nullptr;
    }

    // Open the file for the specified mode
    QSharedPointer<QFile> pFile(new QFile(pathFile));
    if (!pFile->open(mode))
    {
        qWarning() << QObject::tr("Could not open the file: %1").arg(pathFile);
        return nullptr;
    }
    return pFile;
}

QJsonValue toJson(QUuid const& id)
{
    return id.toString();
}

QJsonValue toJson(QRect const& rect)
{
    QJsonObject obj;
    obj["x"] = rect.x();
    obj["y"] = rect.y();
    obj["width"] = rect.width();
    obj["height"] = rect.height();
    return obj;
}

QJsonValue toJson(QFont const& font)
{
    QJsonObject obj;
    obj["family"] = font.family();
    obj["pointSize"] = font.pointSize();
    obj["pixelSize"] = font.pixelSize();
    obj["bold"] = font.bold();
    obj["italic"] = font.italic();
    obj["underline"] = font.underline();
    obj["strikeOut"] = font.strikeOut();
    obj["weight"] = font.weight();
    obj["styleHint"] = static_cast<int>(font.styleHint());
    return obj;
}

QJsonValue toJson(QPageLayout const& layout)
{
    QJsonObject obj;

    // Page
    QSizeF s = layout.pageSize().size(QPageSize::Millimeter);
    obj["width"] = s.width();
    obj["height"] = s.height();
    obj["orientation"] = (int) layout.orientation();
    obj["units"] = (int) layout.units();
    obj["mode"] = (int) layout.mode();

    // Margins
    QMarginsF m = layout.margins();
    QJsonObject margins;
    margins["left"] = m.left();
    margins["top"] = m.top();
    margins["right"] = m.right();
    margins["bottom"] = m.bottom();
    obj["margins"] = margins;

    return obj;
}

QJsonValue toJson(Qt::Alignment const& align)
{
    return align.toInt();
}

QJsonValue toJson(QColor const& color)
{
    return color.name(QColor::HexArgb);
}

QJsonValue toJson(QPair<double, double> const& pair)
{
    QJsonObject obj;
    obj["first"] = pair.first;
    obj["second"] = pair.second;
    return obj;
}

QJsonValue toJson(QByteArray const& data)
{
    return QString::fromLatin1(data.toBase64());
}
}
