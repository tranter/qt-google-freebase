#include <QDebug>
#include <QJson/Parser>
#include <QNetworkReply>
#include <QNetworkAccessManager>

#include "freebase_data_manager.h"

freebase_data_manager::freebase_data_manager(QObject* parent) : QObject(parent)
{
    m_pNetworkAccessManager = new QNetworkAccessManager(this);
    connect(m_pNetworkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
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
//    request.setRawHeader("GData-Version", "3.0");
//    request.setRawHeader("Authorization", (QString("OAuth %1").arg(access_token)).toLatin1());
//    request.setRawHeader("Content-Type", "application/atom+xml");
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded'");
//    request.setRawHeader("X-Metaweb-Request", "True");
//    request.setRawHeader("Content-Length", QString::number(content.size()).toLatin1());

//    m_pNetworkAccessManager->post(request,content);
    m_pNetworkAccessManager->post(request,QByteArray());
}

void freebase_data_manager::runMqlQuery(const QString& query)
{
    qDebug() << Q_FUNC_INFO << ", query=" << query;
    QString url = QString("https://www.googleapis.com/freebase/v1-sandbox/mqlread?query=%1&cost=true").arg(query);
    m_pNetworkAccessManager->get(QNetworkRequest(QUrl(url)));
}

void freebase_data_manager::runSearchQuery(const QString& query)
{
    qDebug() << Q_FUNC_INFO << ", query=" << query;
    QString s = QString("https://www.googleapis.com/freebase/v1-sandbox/search?query=%1&indent=true").arg(query);
    m_pNetworkAccessManager->get(QNetworkRequest(QUrl(s)));
}

//void freebase_data_manager::runWriteQuery(const QString& query, const QString& access_token)
//{
//    qDebug() << Q_FUNC_INFO << ", query=" << query;
////    QString s = QString("https://www.googleapis.com/freebase/v1-sandbox/mqlwrite?query=%1&indent=true&access_token=%2").arg(query,access_token);
//    QString s = QString("https://www.googleapis.com/freebase/v1-sandbox/mqlwrite?query=%1&indent=true").arg(query);

//    QNetworkRequest request;
//    request.setUrl(QUrl(s));
//    request.setRawHeader("GData-Version", "3.0");
//    request.setRawHeader("Authorization", (QString("OAuth %1").arg(access_token)).toLatin1());

//    m_pNetworkAccessManager->post(request,QByteArray());
//}

//void freebase_data_manager::runWriteQuery(const QString& query, const QString& access_token)
//{
//    qDebug() << Q_FUNC_INFO << ", query=" << query;
//    QString s = "https://www.googleapis.com/freebase/v1-sandbox/mqlwrite";
////    QString s = "https://www.googleapis.com/freebase/v1-sandbox/mqlwrite?key=%1".arg(access_token);
//    QByteArray content = "query="+query.toLatin1();
//    qDebug() << "content=" << content;

//    QNetworkRequest request;
//    request.setUrl(QUrl(s));
//    request.setRawHeader("GData-Version", "3.0");
//    request.setRawHeader("Authorization", (QString("OAuth %1").arg(access_token)).toLatin1());
//    request.setRawHeader("Content-Type", "application/atom+xml");
//    request.setRawHeader("Content-Length", QString::number(content.size()).toLatin1());

//    m_pNetworkAccessManager->post(request,content);
//}

//void freebase_data_manager::runWriteQuery(const QString& query, const QString& key)
//{
//    qDebug() << Q_FUNC_INFO << ", query=" << query;
//    QString s = QString("https://www.googleapis.com/freebase/v1-sandbox/mqlwrite?key=%1").arg(key);
//    QByteArray content = "query="+query.toLatin1();
//    qDebug() << "content=" << content;

//    QNetworkRequest request;
//    request.setUrl(QUrl(s));
////    request.setRawHeader("GData-Version", "3.0");
////    request.setRawHeader("Authorization", (QString("OAuth %1").arg(access_token)).toLatin1());
////    request.setRawHeader("Content-Type", "application/atom+xml");
//    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded'");
//    request.setRawHeader("X-Metaweb-Request", "True");
//    request.setRawHeader("Content-Length", QString::number(content.size()).toLatin1());

//    m_pNetworkAccessManager->post(request,content);
//}

//void freebase_data_manager::runWriteQuery(const QString& query, const QString& access_token, const QString& key)
//{
//    qDebug() << Q_FUNC_INFO << ", query=" << query;
//    QString s = QString("https://www.googleapis.com/freebase/v1-sandbox/mqlwrite?query=%1").arg(query);//?access_token=%1").arg(access_token);
////    QString s = QString("https://www.googleapis.com/freebase/v1-sandbox/mqlwrite?key=%1").arg(access_token);
////    QByteArray content = "query="+query.toLatin1();
//    QByteArray content = query.toLatin1();
//    qDebug() << "content=" << content;

//    QNetworkRequest request;
//    request.setUrl(QUrl(s));
////    request.setRawHeader("GData-Version", "3.0");
////    request.setRawHeader("Authorization", (QString("OAuth %1").arg(access_token)).toLatin1());
////    request.setRawHeader("Content-Type", "application/atom+xml");
////    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");

//    request.setRawHeader("Content-Type", "application/json");
////    request.setRawHeader("Content-Length", QString::number(content.size()).toLatin1());
////    request.setRawHeader("X-Metaweb-Request", "True");
////    request.setRawHeader("X-Requested-With", "True");
//    request.setRawHeader("Authorization", (QString("OAuth %1").arg(access_token)).toLatin1());

//    m_pNetworkAccessManager->get(request);//,content);
//}

void freebase_data_manager::runWriteQuery(const QString& query, const QString& access_token, const QString& key)
{
    qDebug() << Q_FUNC_INFO << ", query=" << query;
//    QString s = QString("https://www.googleapis.com/freebase/v1-sandbox/mqlwrite?query=%1").arg(query);//?access_token=%1").arg(access_token);
    QString s = QString("https://www.googleapis.com/freebase/v1-sandbox/mqlwrite?key=%1").arg(access_token);
//    QByteArray content = "query="+query.toLatin1();
    QByteArray content = query.toLatin1();
    qDebug() << "content=" << content;

    QNetworkRequest request;
    request.setUrl(QUrl(s));
    request.setRawHeader("GData-Version", "3.0");
    request.setRawHeader("Authorization", (QString("OAuth %1").arg(access_token)).toLatin1());
//    request.setRawHeader("Content-Type", "application/atom+xml");
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");

//    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("Content-Length", QString::number(content.size()).toLatin1());
    request.setRawHeader("X-Metaweb-Request", "True");
    request.setRawHeader("X-Requested-With", "True");
//    request.setRawHeader("Authorization", (QString("OAuth %1").arg(access_token)).toLatin1());

//    m_pNetworkAccessManager->get(request);//,content);
    m_pNetworkAccessManager->post(request,content);
}

void freebase_data_manager::runTextQuery(const QString& query)
{
    qDebug() << Q_FUNC_INFO << ", query=" << query;
    QString url = QString("https://www.googleapis.com/freebase/v1-sandbox/text%1").arg(query);
    m_pNetworkAccessManager->get(QNetworkRequest(QUrl(url)));
}

void freebase_data_manager::runImageQuery(const QString& query)
{
    qDebug() << Q_FUNC_INFO << ", query=" << query;
    QString url = QString("https://usercontent.googleapis.com/freebase/v1-sandbox/text%1").arg(query);
    m_pNetworkAccessManager->get(QNetworkRequest(QUrl(url)));
}

void freebase_data_manager::replyFinished(QNetworkReply *reply)
{
    QString json = reply->readAll();
    QString url = reply->url().toString();

    qDebug() << "url:\n" << url;
    qDebug() << "json:\n" << json;

    if(reply->error())
    {
        qDebug() << "ERROR" << reply->error() << reply->errorString();
        emit sigErrorOccured(QString("reply->error(): %1").arg(json));
        return;
    }

    if (json.length() == 0) {
        return;
    }

    QJson::Parser parser;

    bool ok;

    // json is a QString containing the data to convert
    QVariant result = parser.parse (json.toLatin1(), &ok);

    if (!ok) {
        emit sigErrorOccured(QString("Cannot convert to QJson object: %1").arg(json));
        return;
    }

    if (result.toMap().contains("error")) {
        emit sigErrorOccured(result.toMap()["error"].toMap()["message"].toString());
        return;
    }

    if (url.contains("https://www.googleapis.com/oauth2/v1/userinfo")) {
        m_strUserEmail = result.toMap()["email"].toString();
        emit sigUserEmailReady();
        return;
    } else if (url.contains("login")) {
        m_strReply = modifyReply(json);
        m_jsonReply = result;
        emit sigMqlReplyReady();
        return;
    } else if (url.contains("https://www.googleapis.com/freebase/v1-sandbox/mqlread")) {
        m_strReply = modifyReply(json);
        m_jsonReply = result;
        emit sigMqlReplyReady();
        return;
    } else if (url.contains("https://www.googleapis.com/freebase/v1-sandbox/search")) {
        m_strReply = modifyReply(json);
        m_jsonReply = result;
        emit sigMqlReplyReady();
        return;
    } else if (url.contains("https://www.googleapis.com/freebase/v1-sandbox/text")) {
        m_strReply = modifyReply(json);
        m_jsonReply = result;
        emit sigMqlReplyReady();
        return;
    } else if (url.contains("https://usercontent.googleapis.com/freebase/v1-sandbox/image")) {
        //m_strReply = modifyReply(json);
        //m_jsonReply = result;
        //emit sigMqlReplyReady();
        return;
    }
}

QString freebase_data_manager::modifyReply(const QString& reply)
{
    QString ret;
    QString copy = reply;
    int level = 0;
    bool bAdd = false;

    //copy.replace("\t","");

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
