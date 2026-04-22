#include <QJsonArray>
#include <QJsonObject>
#include <QRegularExpression>

#include "fileutility.h"
#include "reporttextengine.h"

using namespace Backend::Core;

// Helper functions
QString normalizeKey(QString const& rawKey);

ReportTextEngine::ReportTextEngine()
{
}

//! Check if there are any variables to process
bool ReportTextEngine::isEmpty() const
{
    return mVariables.isEmpty();
}

//! Check if there is the variable with the requested name
bool ReportTextEngine::contains(QString const& rawKey) const
{
    QString key = normalizeKey(rawKey);
    return mVariables.contains(key);
}

//! Retrieve the number of variables
int ReportTextEngine::numVariables() const
{
    return mVariables.count();
}

//! Retrieve the variable keys
QList<QString> ReportTextEngine::keys() const
{
    return mVariables.keys();
}

//! Get the variable value
QString ReportTextEngine::getValue(QString const& rawKey) const
{
    QString key = normalizeKey(rawKey);
    if (!contains(key))
        return QString();
    QString value = mVariables[key];
    if (mReplacements.contains(value))
        value = mReplacements[value];
    return value;
}

//! Remove all the variables
void ReportTextEngine::clearVariables()
{
    mVariables.clear();
}

//! Remove the variable by its key
bool ReportTextEngine::removeVariable(QString const& rawKey)
{
    QString key = normalizeKey(rawKey);
    if (!contains(key))
        return false;
    return mVariables.remove(key);
}

//! Add a variable
bool ReportTextEngine::addVariable(QString const& rawKey)
{
    QString key = normalizeKey(rawKey);
    if (contains(key))
        return false;
    mVariables[key] = QString();
    return true;
}

//! Set the variable value
void ReportTextEngine::setVariable(QString const& rawKey, QString const& value)
{
    QString key = normalizeKey(rawKey);
    mVariables[key] = value;
}

//! Set the variable translation
void ReportTextEngine::setReplacement(QString const& value, QString const& replacement)
{
    mReplacements[value] = replacement;
}

//! Find and replace ${VARIABLE} in text
QString ReportTextEngine::process(QString const& input) const
{
    // Matches ${VAR_NAME}
    QRegularExpression const re(R"(\$\{([^}]+)\})");

    // Check if there are any variables to use
    if (mVariables.isEmpty())
        return input;

    // Allocate the result
    QString result;
    result.reserve(input.size());

    // Loop through all the matches
    QRegularExpressionMatchIterator it = re.globalMatch(input);
    int iLast = 0;
    while (it.hasNext())
    {
        // Obtain the match
        QRegularExpressionMatch match = it.next();

        // Append text before match
        result.append(input.mid(iLast, match.capturedStart() - iLast));

        // Replace or keep original if missing
        QString key = match.captured(1);
        if (contains(key))
            result.append(getValue(key));
        else
            result.append(match.captured(0));
        iLast = match.capturedEnd();
    }

    // Append remaining text
    result.append(input.mid(iLast));

    return result;
}

QJsonObject ReportTextEngine::toJson() const
{
    QJsonObject obj;
    obj["variableKeys"] = QJsonArray::fromStringList(mVariables.keys());
    obj["variableValues"] = QJsonArray::fromStringList(mVariables.values());
    obj["replacementKeys"] = QJsonArray::fromStringList(mReplacements.keys());
    obj["replacementValues"] = QJsonArray::fromStringList(mReplacements.values());
    return obj;
}

void ReportTextEngine::fromJson(QJsonObject const& obj)
{
    // Set variables
    QStringList varKeys, varValues;
    Utility::fromJson(varKeys, obj["variableKeys"]);
    Utility::fromJson(varValues, obj["variableValues"]);
    int numVars = varKeys.size();
    mVariables.clear();
    for (int i = 0; i != numVars; ++i)
        mVariables[varKeys[i]] = varValues[i];

    // Set replacements
    QStringList replaceKeys, replaceValues;
    Utility::fromJson(replaceKeys, obj["replacementKeys"]);
    Utility::fromJson(replaceValues, obj["replacementValues"]);
    int numReplace = replaceKeys.size();
    mReplacements.clear();
    for (int i = 0; i != numReplace; ++i)
        mReplacements[replaceKeys[i]] = replaceValues[i];
}

//! Helper function to convert key to the expected format
QString normalizeKey(QString const& rawKey)
{
    return rawKey.toUpper();
}
