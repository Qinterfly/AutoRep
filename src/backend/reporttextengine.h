#ifndef REPORTTEXTENGINE_H
#define REPORTTEXTENGINE_H

#include <QHash>
#include <QString>

namespace Backend::Core
{

class ReportTextEngine
{
public:
    ReportTextEngine();
    ~ReportTextEngine() = default;

    bool isEmpty() const;
    bool contains(QString const& rawKey) const;
    QString getValue(QString const& rawKey) const;

    void setVariable(QString const& rawKey, QString const& value);
    void setReplacement(QString const& value, QString const& replacement);

    QString process(QString const& input) const;

private:
    QHash<QString, QString> mVariables;
    QHash<QString, QString> mReplacements;
};

}

#endif // REPORTTEXTENGINE_H
