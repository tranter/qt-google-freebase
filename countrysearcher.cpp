#include "countrysearcher.h"
#include "htmlgenerators.h"

#include "freebase_data_manager.h"

#include <QJson/Serializer>

CountrySearcher::CountrySearcher(QWidget * p) :
    SimpleSearcher(p)
{
    showTypeWidgets(false);
    addSchemeType(tr("Country"), "/location/country");
}

QString CountrySearcher::createHtml(const QVariantMap & map)
{
    return HtmlGenerators::createHtmlForCountry(map);
}
