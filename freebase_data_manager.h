#ifndef FREEBASE_DATA_MANAGER_H
#define FREEBASE_DATA_MANAGER_H

#include <QObject>
#include <QVariant>

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

    void runMqlQuery(const QString& query);
    void runSearchQuery(const QString& query);
    void runWriteQuery(const QString& query, const QString& access_token);
    QVariant& getJsonData() {return m_jsonReply;}

signals:
    void sigErrorOccured(const QString& error);
    void sigUserEmailReady();
    void sigMqlReplyReady();

private slots:
    void replyFinished(QNetworkReply*);

private:
    QString modifyReply(const QString& reply);

private:
    QString m_strUserEmail;
    QString m_strReply;
    QVariant m_jsonReply;
    QNetworkAccessManager* m_pNetworkAccessManager;
};

#endif // FREEBASE_DATA_MANAGER_H
