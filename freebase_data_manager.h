#ifndef FREEBASE_DATA_MANAGER_H
#define FREEBASE_DATA_MANAGER_H

#include <QObject>
#include <QVariant>
#include <QSslError>
#include <QPixmap>

class QNetworkAccessManager;
class QNetworkReply;

#define REQ_LOGIN 1
#define REQ_MQL 2
#define REQ_SEARCH 3
#define REQ_TEXT 4
#define REQ_IMAGE 5

class freebase_data_manager : public QObject
{
    Q_OBJECT

public:
    freebase_data_manager(QObject* parent = 0);

    void requestUserEmail(const QString& access_token);
    QString getUserEmail() {return m_strUserEmail;}
    QString getReplyStr() {return m_strReply;}
    QString getRichTextReplyStr() {return m_strRichTextReply;}

    void loginFreebase();

    void runMqlQuery(const QString& query);
    void runSearchQuery(const QString& query, const QString& limit="20", const QString& start="0");
    void runWriteQuery(const QString& query, const QString& access_token, const QString& key);
    void runTextQuery(const QString& query);
    void runImageQuery(const QString& query, int maxHeight=4096, int maxWidth=4096);
    QVariant& getJsonData() {return m_jsonReply;}

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

private:
    QString m_strUserEmail;
    QString m_strReply;
    QString m_strRichTextReply;
    QVariant m_jsonReply;
    QNetworkAccessManager* m_pNetworkAccessManager;
};

#endif // FREEBASE_DATA_MANAGER_H
