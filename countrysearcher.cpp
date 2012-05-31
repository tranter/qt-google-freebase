#include "countrysearcher.h"
#include "htmlgenerators.h"

#include "freebase_data_manager.h"

#include <QJson/Serializer>

CountrySearcher::CountrySearcher(QWidget * p) :
    SimpleSearcher(p)
{
    showTypeWidgets(false);
}

QString CountrySearcher::getCurrentType() const
{
    return "/location/country";
}

QString CountrySearcher::createHtml(const QVariantMap & map)
{
    return HtmlGenerators::createHtmlForCountry(map);
}

void CountrySearcher::getInfo(const QString & id, const QString & /*type = "/location/country"*/)
{
    setAwaitingMode();

    QVariantMap query;
    if (id.startsWith("/m/"))
        query["mid"] = id;
    else
        query["id"] = id;

    query["type"] = getCurrentType();
    query["key"] = QVariantList();
    query["*"] = QVariant();

    m_pManager->runMqlQueryMultiple( QStringList( QJson::Serializer().serialize(query) ) );
}
