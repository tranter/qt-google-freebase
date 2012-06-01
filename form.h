#ifndef FORM_H
#define FORM_H

#include <QDebug>
#include <QWidget>
#include <QModelIndex>
#include <QWebPage>
#include <QNetworkReply>

namespace Ui {
    class Form;
}

class QNetworkReply;
class QSettings;
class OAuth2;

class TreeJsonModel;
class freebase_data_manager;


class MyWebPage : public QWebPage
{
    Q_OBJECT

public:
    MyWebPage(QObject * o = 0) : QWebPage(o) {
        connect(networkAccessManager(),SIGNAL(sslErrors(QNetworkReply*, const QList<QSslError>& )),
                  this,SLOT(sslErrorHandler(QNetworkReply*, const QList<QSslError>& )));
    }

    QString userAgentForUrl(const QUrl & /*url*/ ) const {
        return "Mozilla/5.0 (Windows NT 6.1; WOW64; rv:6.0.2) Gecko/20100101 Firefox/6.0.2";
    }

    void javaScriptConsoleMessage(const QString& msg, int lineNumber, const QString& sourceID) {
        qDebug() << Q_FUNC_INFO << " lineNumber=" << lineNumber << ", msg=" << msg << ", sourceID=" << sourceID;
    }

private slots:
    void sslErrorHandler(QNetworkReply* qnr, const QList<QSslError>& /*errlist*/) {
        qnr->ignoreSslErrors();
    }
};



class Form : public QWidget
{
    Q_OBJECT
    
public:
    explicit Form(QWidget *parent = 0);
    ~Form();

signals:
    void showStatusText(const QString & text);

public slots:
    void startLogin(bool bForce); //Show login dialog even if there is gotten already access_token.
    void startFreebaseLogin();
    void runQuery();
    void startDlgDemo();
    void startDlgDemoCountry();
    void openFreebaseExplorer();

    void onSuggestData(const QString& name,const QString& id,const QString& mid);
    void onBacwardAction();
    void onForwardAction();

private slots:
    void onLoginDone();
    void onErrorOccured(const QString& error);
    void onUserEmailReady();

    void onMqlReplyReady(const int rt);
    void onSearchReplyReady(const int rt);
    void onJsonReady(const int rt);

    void onBtnRunClicked();
    void onBtnClearClicked();
    void onBtnTextGoClicked();
    void onBtnImageGoClicked();

    void onTabQueryTabChanged(int pos);
    void onSplitterMoved(int pos, int index);

    void onNewPage();

    void sslImageErrorHandler(QNetworkReply* qnr, const QList<QSslError>& errlist);
    void sslTextErrorHandler(QNetworkReply* qnr, const QList<QSslError>& errlist);
    void onTextLinkClicked(const QUrl& url);

private:
    void saveSettings();
    int indexTabQueryByName(const QString& name);
    int indexTabReplyByName(const QString& name);
    void clearTreeJson();
    void initSuggestPage();
    QString createImageHtml();

private:
    Ui::Form *ui;
    OAuth2* m_pOAuth2;
    freebase_data_manager* m_pManager;
    TreeJsonModel* m_pModel;
    QSettings* m_pSettings;

    QString m_strCompanyName;
    QString m_strAppName;

    QList<int> m_listSplitterSave;
    QMap<QString,QList<QString> > m_history;
    QMap<QString,int> m_historyPos;
    int m_historyMax;
};

#endif // FORM_H
