#include "freebaseexplorer.h"
#include "freebase_data_manager.h"

#include <QJson/Serializer>
#include <QWebView>
#include <QDebug>

#include <QMetaMethod>

FreebaseExplorer::
FreebaseExplorer(QWidget *parent) :
    SimpleSearcher(parent)
{
    m_gettingScheme = false;
    delegateRequests();
    connect(this, SIGNAL(currentSchemaTypeChanged(QString)), this, SLOT(getSchema(QString)));
}

void FreebaseExplorer::
getSchema(const QString & id)
{
    if( m_currentSchemaTypeId == id )
        return;

    m_gettingScheme = true;
    setAwaitingMode();
    schema.clear();
    m_currentSchemaTypeId = id;

    QVariantMap query;
    query["id"]   = id;
    query["type"] = "/type/type";

    QVariantMap properties;
    properties["id"]   = QVariant();
    properties["name"] = QVariant();

    QVariantMap expected_type;
    expected_type["default_property"] = QVariant();
    expected_type["id"]   = QVariant();
    expected_type["name"] = QVariant();

    properties["expected_type"] = expected_type;

    query["properties"] = QVariantList() << properties;

    m_pManager->runMqlQuery( QJson::Serializer().serialize(query) );
}

bool FreebaseExplorer::
delegatedRequest(const QVariantMap & data)
{
    if(m_gettingScheme)
    {
        m_gettingScheme = false;

        QVariantMap result( data["q0"].toMap()["result"].toMap() );

        QVariantList properties( result["properties"].toList());

        foreach(const QVariant & v, properties)
        {
            QVariantMap map(v.toMap());

            Tuple tuple;
            tuple.name      = map["name"].toString();
            tuple.property  = map["id"].toString().split('/').last();

            map = map["expected_type"].toMap();
            tuple.type_id   = map["id"].toString();
            tuple.type_name = map["name"].toString();

            schema.append(tuple);
        }

        search();

        return true;
    }
    return false;
}

QString FreebaseExplorer::createSearchLink(QString type, QString typeName, QString text)
{
//    type.replace(':', "\\:");
//    typeName.replace(':', "\\:");

//    QString _text(text);
//    text.replace(':', "\\:");

    return QString("<a href=\"search:%1:%2:%3\">%3</a>").arg(type, typeName, text);
}

QString FreebaseExplorer::createHtml(const QVariantMap & map)
{
    QVariantList results( map["q0"].toMap()["result"].toList() );
    if( results.isEmpty() )
    {
        return "<html><body><p><i>Empty result</i></p></body></html>";
    }

    QString head(
                "<html><head><style type=\"text/css\">a{color:black}</style>"
                "<script type=\"text/javascript\">"
                "function loadText() {"
                "var req = new XMLHttpRequest();"
                "req.onreadystatechange = function() {"
                "if( req.readyState == 4 && req.status == 200 ) {"
                "var result = eval('(' + req.responseText + ')');"
                "document.getElementById('art').innerHTML = result['result'];"
                "} };"
                "req.open( 'GET', 'https://www.googleapis.com/freebase/v1-sandbox/text"
            "%1', true );"
                "req.send( null ); }"
                "</script></head><body onload=\"loadText()\">"
                "<img src=\"https://usercontent.googleapis.com/freebase/v1-sandbox/image"
            "%1"
                "?maxheight=400&maxwidth=200\" align=\"right\"/>"
                "<p><a href=\"http://www.freebase.com/view"
            "%1"
                "\"><img src=\"http://www.freebase.com/favicon.ico\" alt=\"Freebase\" hspace=\"2\"/>Freebase</a>"
    );

    QVariantMap vMap( results.at(0).toMap() );

    QString name(vMap["name"].toString());
    QString mid( vMap["mid"].toString() );

    QString html( head.arg(mid) );

    {
        QVariantMap key( vMap["key"].toMap() );
        QString wiki( key["namespace"].toString().startsWith("/wikipedia") ? key["value"].toString() : QString() );

        QString wikipedia = wiki.isEmpty() ? QString() : QString(
            "<a href=\"http://en.wikipedia.org/wiki/index.html?curid=%1\">"
            "<img src=\"http://en.wikipedia.org/favicon.ico\" alt=\"Wiki\" hspace=\"2\"/>Wiki</a></p>"
        ).arg(wiki);

        html.append( wikipedia.isEmpty() ? "</p>" : wikipedia );
    }

    html += QString("<h2>%1</h2><p id=\"art\"></p>").arg(name);

    foreach( const Tuple & t, schema )
    {
        QVariant value = vMap[t.property];
        if( value.isNull() )
            continue;

        bool attachedTypeInfo( ! (t.type_id.isEmpty() || t.type_name.isEmpty()) );

        QString strValue;

        if( value.type() == QVariant::List )
        {
            QVariantList list = value.toList();
            if( list.count() )
            {
                int nulls(0);

                strValue.append("<ul>");
                foreach( const QVariant & var, list )
                {
                    if( var.isNull() ) { ++nulls; continue; }
                    strValue.append("<li>").append(
                        attachedTypeInfo ? createSearchLink(t.type_id, t.type_name, var.toString()) : var.toString()
                    ).append("</li>");
                }
                strValue.append("</ul>");

                if( nulls == list.count() )
                    strValue.clear();
            }
        }
        else if( value.type() == QVariant::Map )
        {
            QVariantMap m(value.toMap());
            if( ! m.isEmpty() )
            {
                name = m["name"].toString();
                mid  = m["mid"].toString();

                strValue = mid.isEmpty() ? name : QString("<a href=\"%1\">%2</a>").arg(mid, name);
            }
        } else {
            strValue = attachedTypeInfo
                    ? createSearchLink(t.type_id, t.type_name, value.toString())
                    : value.toString();
        }

        if( strValue.isEmpty() )
            continue;

        html += QString("<p><b>%1:</b> %2</p>").arg(t.name, strValue);
    }

    html += "</body></html>";
    qDebug() << html;
    return html;
}

void FreebaseExplorer::getInfo(const QString & id, const QString & type)
{
    setAwaitingMode();

    QVariantMap query;
    if (id.startsWith("/m/"))
        query["mid"] = id;
    else
        query["id"] = id;

    query["type"] = type;
    query["*"] = QVariant();

//    "key" : {
//      "namespace" : "/wikipedia/en_id",
//      "value" : null
//     }

    QVariantMap key;
    key["namespace"] = "/wikipedia/en_id";
    key["value"] = QVariant();

    query["key"] = key;

    m_pManager->runMqlQuery( QJson::Serializer().serialize( QVariantList() << QVariant(query) ) );
}

void FreebaseExplorer::onLinkClicked(const QUrl & _url)
{
    QString url( _url.toString() );
    if( url.isEmpty() ) return;

    if( ! url.startsWith("search:") )
    {
        SimpleSearcher::onLinkClicked(_url);
        return;
    }

    QStringList searchData( url.split(':') );
    if( searchData.size() != 4 ) {
        qDebug() << "Wrong url:" << url;
        return;
    }

    setSearchText(searchData.last());
    addSchemeType(searchData.at(2), searchData.at(1), false);

    if( searchData.at(1) == m_currentSchemaTypeId )
        search();
}
