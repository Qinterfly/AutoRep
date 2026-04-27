#ifndef REPORTTEXTENGINE_H
#define REPORTTEXTENGINE_H

#include <QHash>
#include <QString>

#include "reportinterface.h"

namespace Backend::Core
{

class ReportTextEngine : public ISerializable
{
public:
    ReportTextEngine();
    ~ReportTextEngine() = default;

    bool isEmpty() const;
    int numVariables() const;
    QList<QString> keys() const;
    bool contains(QString const& rawKey) const;
    QString getValue(QString const& rawKey) const;

    void clearVariables();
    bool removeVariable(QString const& rawKey);
    bool addVariable(QString const& rawKey);
    void setVariable(QString const& rawKey, QString const& value);
    void setVariable(QString const& rawKey, double value);
    void setReplacement(QString const& value, QString const& replacement);

    QString process(QString const& input) const;

    QJsonObject toJson() const override;
    void fromJson(QJsonObject const& obj) override;

private:
    QHash<QString, QString> mVariables;
    QHash<QString, QString> mReplacements;
};

}

#endif // REPORTTEXTENGINE_H
