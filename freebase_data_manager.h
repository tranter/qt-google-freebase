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
    freebase_data_manager(QObject* parent = 0);

    void requestUserEmail(const QString& access_token);
    QString getUserEmail() {return m_strUserEmail;}
    QString getReplyStr() {return m_strReply;}

    void loginFreebase();

    void runMqlQuery(const QString& query);
    void runSearchQuery(const QString& query);
    void runWriteQuery(const QString& query, const QString& access_token, const QString& key);
    void runTextQuery(const QString& query);
    void runImageQuery(const QString& query, int maxHeight=4096, int maxWidth=4096);
    QVariant& getJsonData() {return m_jsonReply;}

signals:
    void sigErrorOccured(const QString& error);
    void sigUserEmailReady();
    void sigMqlReplyReady();
    void sigImageReady(const QPixmap& px);

private slots:
    void replyFinished(QNetworkReply*);
    void onSslError(QNetworkReply* reply,QList<QSslError> listErr);

private:
    QString modifyReply(const QString& reply);

private:
    QString m_strUserEmail;
    QString m_strReply;
    QVariant m_jsonReply;
    QNetworkAccessManager* m_pNetworkAccessManager;
};

#endif // FREEBASE_DATA_MANAGER_H
