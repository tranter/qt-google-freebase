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

void freebase_data_manager::runQuery(const QString& query, const QString& key)
{
    QString url = QString("https://www.googleapis.com/freebase/v1/mqlread?query=%1").arg(query);
    m_pNetworkAccessManager->get(QNetworkRequest(QUrl(url)));
}

void freebase_data_manager::replyFinished(QNetworkReply *reply)
{
    QString json = reply->readAll();
    QString url = reply->url().toString();

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

    if (url.contains("userinfo")) {
        m_strUserEmail = result.toMap()["email"].toString();
        emit sigUserEmailReady();
        return;
    } else if (url.contains("mqlread")) {
        m_strReply = modifyReply(json);
        m_jsonReply = result;
        emit sigMqlReplyReady();
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
