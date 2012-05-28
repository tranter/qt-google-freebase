#ifndef HTMLGENERATORS_H
#define HTMLGENERATORS_H

#include <QString>
#include <QVariantMap>

namespace HtmlGenerators
{
    QString createHtmlForPerson(const QVariantMap & map);
    QString createHtmlForCountry(const QVariantMap & map);
    QString findNamespaceValue(const QString& ns, const QVariantMap & map);

    QString makeHtmlRecordForPerson(const QVariantMap& map, const QString& key, const QString& text);
}

#endif // HTMLGENERATOR_H
