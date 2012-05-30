#ifndef FREEBASE_DATA_MANAGER_H
#define FREEBASE_DATA_MANAGER_H

#include <QObject>
#include <QVariant>
#include <QSslError>
#include <QPixmap>

class QNetworkAccessManager;
class QNetworkReply;

class freebase_data_manager : public QObject
{
    Q_OBJECT

public:

    enum RequestStatus {
        REQ_LOGIN, REQ_MQL, REQ_SEARCH, REQ_TEXT, REQ_IMAGE
    };

    freebase_data_manager(QObject* parent = 0);

    void requestUserEmail(const QString& access_token);
    QString getUserEmail() {return m_strUserEmail;}
    QString getReplyStr() {return m_strReply;}
    QString getRichTextReplyStr() {return m_strRichTextReply;}

    void loginFreebase();

    void runMqlQuery(const QString& query);
    void runMqlQueryMultiple(const QStringList& queries);
    void runSearchQuery(const QString& query, const QString& limit="20", const QString& start="0");
    void runWriteQuery(const QString& query, const QString& access_token, const QString& key);
    void runTextQuery(const QString& query);
    void runImageQuery(const QString& query, int maxHeight=4096, int maxWidth=4096);
    const QVariant & getJsonData() const { return m_jsonReply; }

signals:
    void sigErrorOccured(const QString& error);
    void sigUserEmailReady();
    void sigMqlReplyReady(const int rt);
    void sigSearchReplyReady(const int rt);
    void sigImageReady(const QPixmap& px,const int rt);
    void sigJsonReady(const int rt);

private slots:
    void replyFinished(QNetworkReply*);
    void onSslError(QNetworkReply* reply,QList<QSslError> listErr);

private:
    QString modifyReply(const QString& reply);
    QString modifyTextReply();
    QString modifySearchReply();
    QString normalyzeString(const QString& str);
    QString prepareConvert(const QStringList& list);

private:
    QString m_strUserEmail;
    QString m_strReply;
    QString m_strRichTextReply;
    QVariant m_jsonReply;
    QNetworkAccessManager* m_pNetworkAccessManager;
};

#endif // FREEBASE_DATA_MANAGER_H
