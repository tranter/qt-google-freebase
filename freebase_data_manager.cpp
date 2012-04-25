#include <QJson/Parser>
#include <QNetworkReply>
#include <QNetworkAccessManager>

#include "freebase_data_manager.h"

freebase_data_manager::freebase_data_manager(QObject* parent) : QObject(parent)
{
    m_pNetworkAccessManager = new QNetworkAccessManager(this);

    connect(m_pNetworkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
}

void freebase_data_manager::getUserEmail(const QString& access_token)
{
    QString query = QString("https://www.googleapis.com/oauth2/v1/userinfo"
                            "?access_token=%1")
            .arg(access_token);
    m_pNetworkAccessManager->get(QNetworkRequest(QUrl(query)));
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
        emit sigErrorOccured("Empty blogs list: probably you don't create any one.");
        return;
    }

    if (url.contains("userinfo")) {
        m_strUserEmail = result.toMap()["email"].toString();
        emit sigUserEmailReady();
        return;
    }
}
