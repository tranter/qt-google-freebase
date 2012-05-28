#ifndef HTMLGENERATORS_H
#define HTMLGENERATORS_H

#include <QString>
#include <QVariantMap>

namespace HtmlGenerators
{
    QString createHtmlForPerson(const QVariantMap & map);
    QString createHtmlForCountry(const QVariantMap & map);
    QString findNamespaceValue(const QString& ns, const QVariantMap & map);
}

#endif // HTMLGENERATOR_H
