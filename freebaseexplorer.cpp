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
    m_gettingScheme = true;
    setEnabled(false);
    schema.clear();
//    schema.append(Tuple("Name", "name"));
//    schema.append(Tuple("Mid", "mid"));

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

#include <QTimer>

void FreebaseExplorer::
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
            tuple.name = map["name"].toString();
            tuple.property = map["id"].toString().split('/').last();
            schema.append(tuple);
        }

        metaObject()->invokeMethod(this, "search"); //;-)

    } else {
        webView()->setHtml( createHtml(data) );
    }

    setEnabled(true);
}

QString FreebaseExplorer::createHtml(const QVariantMap & map)
{
    QString html("<html><body>");

    QVariantList results( map["q0"].toMap()["result"].toList() );
    if( results.isEmpty() )
    {
        html += "<p><i>Empty result</i></p></body></html>";
        return html;
    }

    QVariant value;

    foreach(const QVariant & v, results)
    {
        QVariantMap vMap( v.toMap() );

        QString name(vMap["name"].toString());
        QString mid( vMap["mid"].toString() );

        {
            QVariantMap key( vMap["key"].toMap() );
            QString wiki( key["namespace"].toString().startsWith("/wikipedia") ? key["value"].toString() : QString() );

            QString freebase = mid.isEmpty() ? QString() : QString(
                "<a href=\"http://www.freebase.com/view%1\">"
                "<img src=\"http://www.freebase.com/favicon.ico\" alt=\"Freebase\" hspace=\"2\"/>Freebase</a>"
            ).arg(mid);

            QString wikipedia = wiki.isEmpty() ? QString() : QString(
                "<a href=\"http://en.wikipedia.org/wiki/index.html?curid=%1\">"
                "<img src=\"http://en.wikipedia.org/favicon.ico\" alt=\"Wiki\" hspace=\"2\"/>Wiki</a>"
            ).arg(wiki);

            html += QString("<p>%1 %2</p>").arg(freebase, wikipedia);
        }

        html += QString("<p>%1</p>").arg(name);

        foreach( const Tuple & t, schema )
        {
            value = vMap[t.property];
            if( value.isNull() )
                continue;

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
                        strValue.append("<li>").append(var.toString()).append("</li>");
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

                    strValue = mid.isEmpty() ? name : QString("</a href=\"%1\">%2</a>").arg(mid, name);
                }
            } else strValue = value.toString();

            if( strValue.isEmpty() )
                continue;

            html += QString("<p><b>%1:</b> %2</p>").arg(t.name, strValue);
        }
    }

    html += "</body></html>";
    return html;
}

void FreebaseExplorer::getInfo(const QString & id, const QString & type)
{
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
