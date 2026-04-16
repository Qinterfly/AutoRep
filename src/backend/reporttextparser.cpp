#include <QRegularExpression>

#include "reporttextparser.h"

using namespace Backend::Core;

// Helper functions
QString normalizeKey(QString const& rawKey);

ReportTextParser::ReportTextParser()
{
}

//! Check if there are any variables to process
bool ReportTextParser::isEmpty() const
{
    return mVariables.isEmpty();
}

//! Check if there is the variable with the requested name
bool ReportTextParser::contains(QString const& rawKey) const
{
    QString key = normalizeKey(rawKey);
    return mVariables.contains(key);
}

//! Get the variable value
QString ReportTextParser::getValue(QString const& rawKey) const
{
    QString key = normalizeKey(rawKey);
    if (!contains(key))
        return QString();
    QString value = mVariables[key];
    if (mTranslations.contains(value))
        value = mTranslations[value];
    return value;
}

//! Set the variable value
void ReportTextParser::setValue(QString const& rawKey, QString const& value)
{
    QString key = normalizeKey(rawKey);
    mVariables[key] = value;
}

//! Set the variable translation
void ReportTextParser::setTranslation(QString const& value, QString const& translation)
{
    mTranslations[value] = translation;
}

//! Find and replace ${VARIABLE} in text
QString ReportTextParser::process(QString const& input) const
{
    // Matches ${VAR_NAME}
    QRegularExpression const re(R"(\$\{([A-Za-z0-9_]+)\})");

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

//! Helper function to convert key to the expected format
QString normalizeKey(QString const& rawKey)
{
    return rawKey.toUpper();
}
