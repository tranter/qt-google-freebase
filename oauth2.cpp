#include <QApplication>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QMessageBox>
#include <QSettings>
#include <QTimer>

#include <QJson/Parser>

#include "logindialog.h"
#include "oauth2.h"

OAuth2::OAuth2(QWidget * p) :
    QObject(p)
{
    m_strEndPoint = "https://accounts.google.com/o/oauth2/auth";
    m_strScope = "https://www.googleapis.com/auth/freebase"
            "+https://www.googleapis.com/auth/userinfo.email"
            ;

    m_strClientID     = "YOUR_CLIENT_ID_HERE";
    m_strClientSecret = "YOUR_CLIENT_SECRET_HERE";
    m_strRedirectURI  = "YOUR_REDIRECT_URI_HERE";
    m_strSimpleAPIKey = "YOUR_SIMPLE_API_KEY_HERE";

    m_pNetworkAccessManager = new QNetworkAccessManager(this);
    connect(m_pNetworkAccessManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));

    m_pLoginDialog = NULL;
    m_pParent = p;
    m_pSettings = NULL;
}

QString OAuth2::loginUrl()
{
    QString str = QString("%1?client_id=%2&redirect_uri=%3&response_type=token&scope=%5")
            .arg(m_strEndPoint,m_strClientID,m_strRedirectURI,m_strScope);
    return str;
}

QString OAuth2::permanentLoginUrl()
{
    QString str = QString("%1?client_id=%2&redirect_uri=%3&response_type=code&scope=%5&approval_prompt=force&access_type=offline").
            arg(m_strEndPoint,m_strClientID,m_strRedirectURI,m_strScope);
    return str;
}

bool OAuth2::isAuthorized()
{
    return m_strAccessToken != "";
}

void OAuth2::startLogin(bool bForce)
{
//    if(m_strClientID == "YOUR_CLIENT_ID_HERE" || m_strRedirectURI == "YOUR_REDIRECT_URI_HERE" ||
//        m_strClientSecret == "YOUR_CLIENT_SECRET_HERE")
//    {
//        QMessageBox::warning(m_pParent, "Warning",
//                             "To work with application you need to register your own application in <b>Google</b>.\n"
//                             "Learn more from <a href='http://code.google.com/p/qt-google-freebase/wiki/HowToRegisterYourApplicationInGoogle'>here</a>");
//        return;
//    }

    if (bForce) {
        if(m_strRefreshToken.isEmpty() || bForce) {
            if (m_pLoginDialog != NULL) {
                delete m_pLoginDialog;
            }
            m_pLoginDialog = new LoginDialog(m_pParent);
            connect(m_pLoginDialog, SIGNAL(accessTokenObtained()), this, SLOT(accessTokenObtained()));
            connect(m_pLoginDialog, SIGNAL(codeObtained()), this, SLOT(codeObtained()));
            m_pLoginDialog->setLoginUrl(permanentLoginUrl());
            m_pLoginDialog->show();
        } else {
            getAccessTokenFromRefreshToken();
        }
    }
}
/*! \brief This slot called only form Google Login dialog in case of response_type=token
 *
 */
void OAuth2::accessTokenObtained()
{
    m_strAccessToken = m_pLoginDialog->accessToken();
    emit loginDone();
    if (m_pLoginDialog != NULL) {
        m_pLoginDialog->deleteLater();
        m_pLoginDialog = NULL;
    }
}
/*! \brief This slot called only form Google Login dialog in case of response_type=code
 *
 */
void OAuth2::codeObtained()
{
    m_strCode = m_pLoginDialog->code();

    QUrl url("https://accounts.google.com/o/oauth2/token");
    QNetworkRequest request;
    request.setUrl(url);
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");

    QString params = "client_id=" + m_strClientID;
    params += "&redirect_uri=" + m_strRedirectURI;
    params += "&client_secret=" + m_strClientSecret;
    params += "&grant_type=authorization_code";
    params += "&code=" + m_strCode;

    m_pNetworkAccessManager->post(request, params.toLatin1());
}

void OAuth2::replyFinished(QNetworkReply* reply)
{
    QString json = reply->readAll();

    QJson::Parser parser;
    bool ok;
    // json is a QString containing the data to convert
    QVariant result = parser.parse (json.toLatin1(), &ok);

    if( !ok )
    {
        emit sigErrorOccured(QString("Cannot convert to QJson object: %1").arg(json));
        return;
    }
    if (result.toMap().contains("error")) {
        emit sigErrorOccured(result.toMap()["error"].toString());
        return;
    }

    QString str = result.toMap()["refresh_token"].toString();
    if (!str.isEmpty() && str != m_strRefreshToken) {
        m_strRefreshToken = str;
        if (m_pSettings != NULL) {
            m_pSettings->setValue("refresh_token",m_strRefreshToken);
        }
    }

    QString prevAccessToken = m_strAccessToken;
    str = result.toMap()["access_token"].toString();
    int expires_in = result.toMap()["expires_in"].toInt();
    if(!str.isEmpty())
    {
        //After 55 min update access_token
        QTimer::singleShot ( (expires_in - 120)*1000, this, SLOT(getAccessTokenFromRefreshToken()) );
    }
    if(!str.isEmpty() && str != prevAccessToken)
    {
        m_strAccessToken = str;
        if (m_pSettings != NULL) {
            m_pSettings->setValue("access_token",m_strAccessToken);
        }
        emit loginDone();
    }
    if (m_pLoginDialog != NULL) {
        delete m_pLoginDialog;
        m_pLoginDialog = NULL;
    }
}

void OAuth2::getAccessTokenFromRefreshToken()
{
    QUrl url("https://accounts.google.com/o/oauth2/token");
    QNetworkRequest request;
    request.setUrl(url);
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");

    QString params = "client_id=" + m_strClientID;
    params += "&client_secret=" + m_strClientSecret;
    params += "&grant_type=refresh_token";
    params += "&refresh_token=" + m_strRefreshToken;

    m_pNetworkAccessManager->post(request, params.toLatin1());
}
