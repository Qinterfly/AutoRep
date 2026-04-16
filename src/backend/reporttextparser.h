#ifndef REPORTTEXTPARSER_H
#define REPORTTEXTPARSER_H

#include <QHash>
#include <QString>

namespace Backend::Core
{

class ReportTextParser
{
public:
    ReportTextParser();
    ~ReportTextParser() = default;

    bool isEmpty() const;
    bool contains(QString const& rawKey) const;
    QString getValue(QString const& rawKey) const;

    void setValue(QString const& rawKey, QString const& value);
    void setTranslation(QString const& value, QString const& translation);

    QString process(QString const& input) const;

private:
    QHash<QString, QString> mVariables;
    QHash<QString, QString> mTranslations;
};

}

#endif // REPORTTEXTPARSER_H
