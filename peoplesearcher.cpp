#include "peoplesearcher.h"
#include "htmlgenerators.h"

#include "freebase_data_manager.h"

#include <QJson/Serializer>

PeopleSearcher::PeopleSearcher(QWidget * p) :
    SimpleSearcher(p)
{
    showTypeWidgets(false);
}

QString PeopleSearcher::getCurrentType() const
{
    return "/people/person";
}

QString PeopleSearcher::createHtml(const QVariantMap & map)
{
    return HtmlGenerators::createHtmlForPerson(map);
}

void PeopleSearcher::getInfo(const QString & id, const QString & /*type*/)
{
    QString type = getCurrentType();
    QStringList list;

    QVariantMap query;
    if (id.startsWith("/m/"))
        query["mid"] = id;
    else
        query["id"] = id;

    query["type"] = type;
    query["key"] = QVariantList();
    query["*"] = QVariant();

    list << QJson::Serializer().serialize(query);
    //-------------------------------------------


    query["type"] = "/people/deceased_person";
    list << QJson::Serializer().serialize(query);
    //-------------------------------------------


    query["type"] = "/book/author";
    list << QJson::Serializer().serialize(query);
    //-------------------------------------------


    query.remove("*");
    query.remove("key");
    query["type"] = "/people/person";

    QVariantMap sub;
    sub["mid"] = QVariant();
    sub["name"] = QVariant();
    sub["type|="] = QVariantList() << "/people/person";

    query["/type/reflect/any_reverse"] = sub;

    list << QJson::Serializer().serialize( QVariantList() << QVariant(query) );
    //-------------------------------------------


    sub["type|="] = QVariantList() << "/event/event";
    query["/type/reflect/any_reverse"] = sub;

    list << QJson::Serializer().serialize( QVariantList() << QVariant(query) );
    //-------------------------------------------

    m_pManager->runMqlQueryMultiple(list);
}
