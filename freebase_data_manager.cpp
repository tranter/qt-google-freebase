#include <QDebug>
#include <QJson/Parser>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QPixmap>

#include "freebase_data_manager.h"

freebase_data_manager::freebase_data_manager(QObject* parent) : QObject(parent)
{
//    m_webServer = "https://www.googleapis.com/freebase/v1-sandbox";
//    m_webServer = "http://api.freebase.com/api/service";
    m_pNetworkAccessManager = new QNetworkAccessManager(this);
    connect(m_pNetworkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
    connect(m_pNetworkAccessManager,SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)),this,SLOT(onSslError(QNetworkReply*,QList<QSslError>)));
}

void freebase_data_manager::requestUserEmail(const QString& access_token)
{
    QString query = QString("https://www.googleapis.com/oauth2/v1/userinfo"
                            "?access_token=%1")
            .arg(access_token);
    m_pNetworkAccessManager->get(QNetworkRequest(QUrl(query)));
}

void freebase_data_manager::loginFreebase()
{
    QString url = "http://api.sandbox-freebase.com/api/account/login?username=<login>&password=<pwd>";

    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded'");

    m_pNetworkAccessManager->post(request,QByteArray());
}

void freebase_data_manager::runMqlQuery(const QString& query)
{
//    QString url = QString("%1/mqlread?query={\"query\":%2}")
//            .arg(m_webServer, normalyzeString(query));
//    m_pNetworkAccessManager->get(QNetworkRequest(QUrl(url)));
    QStringList list;
    list << query;
    runMqlQueryMultiple(list);
}

void freebase_data_manager::runMqlQueryMultiple(const QStringList& queries)
{
    QString convert = prepareConvert(queries);
    QString url = QString("http://api.freebase.com/api/service/mqlread?queries={%1}")
            .arg(normalyzeString(convert));
    m_pNetworkAccessManager->get(QNetworkRequest(QUrl(url)));
}

void freebase_data_manager::runSearchQuery(const QString& query, const QString& limit, const QString& start)
{
    QString s = QString("https://www.googleapis.com/freebase/v1-sandbox/search?query=%1&start=%2&limit=%3&indent=true")
            .arg(query,start,limit);
    m_pNetworkAccessManager->get(QNetworkRequest(QUrl(s)));
}

void freebase_data_manager::runWriteQuery(const QString& query, const QString& access_token, const QString& key)
{
    QString s = QString("https://www.googleapis.com/freebase/v1-sandbox/mqlwrite?key=%1").arg(access_token);
    QByteArray content = query.toLatin1();

    QNetworkRequest request;
    request.setUrl(QUrl(s));
    request.setRawHeader("GData-Version", "3.0");
    request.setRawHeader("Authorization", (QString("OAuth %1").arg(access_token)).toLatin1());
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");

    request.setRawHeader("Content-Length", QString::number(content.size()).toLatin1());
    request.setRawHeader("X-Metaweb-Request", "True");
    request.setRawHeader("X-Requested-With", "True");

    m_pNetworkAccessManager->post(request,content);
}

void freebase_data_manager::runTextQuery(const QString& query)
{
    QString url = QString("https://www.googleapis.com/freebase/v1-sandbox/text%1").arg(query);
    m_pNetworkAccessManager->get(QNetworkRequest(QUrl(url)));
}

void freebase_data_manager::runImageQuery(const QString& query, int maxHeight, int maxWidth)
{
    QString url = QString("https://usercontent.googleapis.com/freebase/v1-sandbox/image%1?maxheight=%2&maxwidth=%3&mode=fit")
            .arg(query,QString::number(maxHeight),QString::number(maxWidth));
    m_pNetworkAccessManager->get(QNetworkRequest(QUrl(url)));
}

void freebase_data_manager::replyFinished(QNetworkReply *reply)
{
    qDebug() << Q_FUNC_INFO;

    QByteArray json = reply->readAll();
    QString url = reply->url().toString();

    qDebug() << "URL=" << url;
//    qDebug() << "json=" << json;

//    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(reply->error()) {
        emit sigErrorOccured(QString("reply->error()=%1,\nerrmessage=%2,\njson=%3").arg(reply->error()).arg(reply->errorString(),QString(json)));
        return;
    }

    if (json.length() == 0) {
        return;
    }

    QJson::Parser parser;
    bool ok;
    // json is a QString containing the data to convert
    QVariant result;
    if (url.contains("https://www.googleapis.com/oauth2/v1/userinfo")) {
        result = parser.parse(json,&ok);
        if (!ok) {
            emit sigErrorOccured(QString("Cannot convert to QJson object: %1").arg(QString(json)));
            return;
        }
        if (result.toMap().contains("error")) {
            emit sigErrorOccured(result.toMap()["error"].toMap()["message"].toString());
            return;
        }
        m_strUserEmail = result.toMap()["email"].toString();
        emit sigUserEmailReady();
        return;
    } else if (url.contains("login")) {
        result = parser.parse(json,&ok);
        if (!ok) {
            emit sigErrorOccured(QString("Cannot convert to QJson object: %1").arg(QString(json)));
            return;
        }
        if (result.toMap().contains("error")) {
            emit sigErrorOccured(result.toMap()["error"].toMap()["message"].toString());
            return;
        }
        m_jsonReply = result;
        m_strReply = modifyReply(json);
        emit sigMqlReplyReady(REQ_LOGIN);
        return;
    } else if (url.contains("/mqlread?query")) {
        result = parser.parse(json,&ok);
        if (!ok) {
            emit sigErrorOccured(QString("Cannot convert to QJson object: %1").arg(QString(json)));
            return;
        }
        if (result.toMap().contains("error")) {
            emit sigErrorOccured(result.toMap()["error"].toMap()["message"].toString());
            return;
        }
        m_jsonReply = result;
        m_strReply = modifyReply(json);
        m_strRichTextReply = modifyTextReply();
        emit sigMqlReplyReady(REQ_MQL);
        emit sigJsonReady(REQ_MQL);
        return;
    } else if (url.contains("/mqlread?queries")) {
        result = parser.parse(json,&ok);
        if (!ok) {
            emit sigErrorOccured(QString("Cannot convert to QJson object: %1").arg(QString(json)));
            return;
        }
        if (result.toMap().contains("error")) {
            emit sigErrorOccured(result.toMap()["error"].toMap()["message"].toString());
            return;
        }
        m_jsonReply = result;
        m_strReply = modifyReply(json);
        m_strRichTextReply = modifyTextReply();
        emit sigMqlReplyReady(REQ_MQL);
        emit sigJsonReady(REQ_MQL);
        return;
    } else if (url.contains("/search")) {
        result = parser.parse(json,&ok);
        if (!ok) {
            emit sigErrorOccured(QString("Cannot convert to QJson object: %1").arg(QString(json)));
            return;
        }
        if (result.toMap().contains("error")) {
            emit sigErrorOccured(result.toMap()["error"].toMap()["message"].toString());
            return;
        }
        m_jsonReply = result;
        m_strReply = modifyReply(json);
        m_strRichTextReply = modifySearchReply();
        emit sigSearchReplyReady(REQ_SEARCH);
        emit sigJsonReady(REQ_SEARCH);
        return;
    } else if (url.contains("/text")) {
        result = parser.parse(json,&ok);
        if (!ok) {
            emit sigErrorOccured(QString("Cannot convert to QJson object: %1").arg(QString(json)));
            return;
        }
        if (result.toMap().contains("error")) {
            emit sigErrorOccured(result.toMap()["error"].toMap()["message"].toString());
            return;
        }
        m_jsonReply = result;
        m_strReply = modifyReply(json);
        m_strRichTextReply = modifyTextReply();
        emit sigMqlReplyReady(REQ_TEXT);
        emit sigJsonReady(REQ_TEXT);
        return;
    } else if (url.contains("https://usercontent.googleapis.com/freebase/v1-sandbox/image")) {
        m_strReply = "";
        QByteArray arr(json);
        QPixmap px;
        px.loadFromData(arr);
        emit sigImageReady(px,REQ_IMAGE);
        return;
    }
}

QString freebase_data_manager::modifyReply(const QString& reply)
{
    QString ret;
    QString copy = reply;
    int level = 0;
    bool bAdd = false;

    foreach (QChar ch,copy) {
        if (ch == '{' || ch == '[') {
            level++;
        } else if (ch == '}' || ch == ']') {
            level--;
            if (ret.endsWith('\t')) {
                ret.chop(1);
            }
        } else if (ch == '\n') {
            bAdd = true;
        }
        ret += ch;
        if (bAdd) {
            for (int i=0;i<level;i++) {
                ret += '\t';
            }
            bAdd = false;
        }
    }

    return ret;
}

QString freebase_data_manager::modifyTextReply()
{
    QString ret = "<b><p align='justify'>";
    QString str = m_jsonReply.toMap()["result"].toString();
    foreach (QChar ch, str) {
        if (ch == '\n') {
            ret += "</p><p align='justify'>";
        } else {
            ret += ch;
        }
    }

    ret += "</p></b>";
    return ret;
}

QString freebase_data_manager::modifySearchReply()
{
    QString ret("<table>");
    QVariantList list = m_jsonReply.toMap()["result"].toList();
    foreach (QVariant item, list) {
        QVariantMap map = item.toMap();
        ret += "<tr><td style=\"text-align: center\">";
        if (map.contains("mid")) {
            ret += QString("<img src=\"https://usercontent.googleapis.com/freebase/v1-sandbox/image%1\"/>")
                    .arg(map.value("mid").toString());
        }
        ret += "</td><td style=\"vertical-align: middle; padding-left: 10px\">";
        if (map.contains("name")) {
            ret += "<a href='" + map.value("name").toString() + "'>" + map.value("name").toString() + "</a>";
        }
        if (map.contains("notable")) {
            ret += " (" + map.value("notable").toMap().value("name").toString()+")";
        }
        ret += "<br/>";
        if (map.contains("mid")) {
            ret += "<a href='" + map.value("mid").toString() + "'>" + map.value("mid").toString() + "</a>";
        }
        if (map.contains("lang")) {
            ret += ", " + map.value("lang").toString();
        }
        if (map.contains("score")) {
            ret += ", score=" + map.value("score").toString();
        }
        ret += "</td></tr>";
    }
    ret += "</table>";
    return ret;
}

void freebase_data_manager::onSslError(QNetworkReply* reply,QList<QSslError> listErr)
{
    reply->ignoreSslErrors();
}

QString freebase_data_manager::normalyzeString(const QString& str)
{
    QString ret;
    QStringList lines = str.split(QRegExp("[\\r\\n]+"));
    bool inComment = false;
    bool inQuotation = false;

    foreach (QString line,lines) {
        inComment = false;
        inQuotation = false;
        foreach (QChar ch, line) {
            if (!inComment) {
                if (ch == ' ' || ch == '\t') {
                    if (inQuotation) {
                        ret += ch;
                    }
                }
                if (ch != ' ' && ch != '\t' && ch != '#') {
                    ret += ch;
                }
            }
            if (ch == '#') {
                inComment = true;
            }
            if (ch == '"') {
                if (inQuotation) {
                    inQuotation = false;
                } else {
                    inQuotation = true;
                }
            }
        }
    }

    return ret;
}

QString freebase_data_manager::prepareConvert(const QStringList& list)
{
    QString ret;
    int pos = 0;
    foreach (QString item,list) {
        if (pos != 0) {
            ret += ",";
        }
        ret += "\"q"+QString::number(pos++)+"\":{\"query\":"+item+"}";
    }

    return ret;
}
