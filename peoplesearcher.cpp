#include "peoplesearcher.h"
#include "htmlgenerators.h"

#include "freebase_data_manager.h"

#include <QUrl>
#include <QDesktopServices>
#include <QWebView>

#include <QJson/Serializer>

#include <QDebug>

PeopleSearcher::PeopleSearcher(QWidget * p) :
    SimpleSearcher(p)
{
    showTypeWidgets(false);
    delegateRequests();
}

QString PeopleSearcher::getCurrentType() const
{
    return "/people/person";
}

QString PeopleSearcher::createHtml(const QVariantMap & map)
{
    return HtmlGenerators::createHtmlForPerson(map);
}

void PeopleSearcher::delegatedRequest(const QVariantMap & jsonMap)
{
    QVariantMap key = jsonMap["q0"].toMap()["result"].toMap()["key"].toMap();

    if( ! key.isEmpty() && key["namespace"].toString() == "/wikipedia/en_id" )
    {
        QString url = "http://en.wikipedia.org/wiki/index.html?curid=" + key["value"].toString();
        QDesktopServices::openUrl(QUrl(url));
        return;
    }

    webView()->setHtml( createHtml(jsonMap) );
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
    query["key"] = QVariantList() << QVariantMap(); // "key": [{}]
    query["*"] = QVariant(); // "*": null


    list << QJson::Serializer().serialize(query);
    //-------------------------------------------


    query["type"] = "/people/deceased_person";
    list << QJson::Serializer().serialize(query);
    //-------------------------------------------


    query["type"] = "/book/author";
    list << QJson::Serializer().serialize(query);
    //-------------------------------------------


    query["type"] = "/event/event";
    list << QJson::Serializer().serialize(query);
//    //-------------------------------------------


    query.remove("*");
    query.remove("key");
    query["type"] = type;

    QVariantMap sub;
    sub["mid"] = QVariant();
    sub["name"] = QVariant();
    sub["type|="] = QVariantList() << type;

    query["/type/reflect/any_reverse"] = QVariantList() << sub;

    list << QJson::Serializer().serialize( QVariantList() << QVariant(query) );
    //-------------------------------------------


    sub["type|="] = QVariantList() << "/event/event";
    query["/type/reflect/any_reverse"] = QVariantList() << sub;

    list << QJson::Serializer().serialize( QVariantList() << QVariant(query) );
    //-------------------------------------------

    m_pManager->runMqlQueryMultiple(list);
}

void PeopleSearcher::onLinkClicked(const QUrl & url)
{
    qDebug() << Q_FUNC_INFO << " url=" << url;
    if (url.toString().contains("http://"))
    {
        QDesktopServices::openUrl(url);
    }
    else if (url.toString().startsWith("/m/"))
    {
        appendToHistory( url.toString() );
        getInfo(url.toString(), getCurrentType());
    }
    else if (url.toString().startsWith("event@"))
    {
        QStringList list = url.toString().split("@");
        //getEventReferencies(list[1]);
        appendToHistory( list[1] );
        getInfo(list[1], getCurrentType());
    }
}

void PeopleSearcher::getEventReferencies(const QString& mid)
{
//    qDebug() << Q_FUNC_INFO;

    QVariantMap key;
    key["namespace"] = "/wikipedia/en_id";
    key["value"] = QVariant();

    QVariantMap query;
    query["mid"] = mid;
    query["key"] = key;

    m_pManager->runMqlQueryMultiple( QStringList( QJson::Serializer().serialize(query) ) );
}
