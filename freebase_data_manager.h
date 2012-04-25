#ifndef FREEBASE_DATA_MANAGER_H
#define FREEBASE_DATA_MANAGER_H

#include <QObject>

class QNetworkAccessManager;
class QNetworkReply;

class freebase_data_manager : public QObject
{
    Q_OBJECT

public:
    freebase_data_manager(QObject* parent = 0);

    void getUserEmail(const QString& access_token);
    QString userEmail() {return m_strUserEmail;}

signals:
    void sigErrorOccured(const QString& error);
    void sigUserEmailReady();

private slots:
    void replyFinished(QNetworkReply*);

private:
    QString m_strUserEmail;
    QNetworkAccessManager* m_pNetworkAccessManager;
};

#endif // FREEBASE_DATA_MANAGER_H
