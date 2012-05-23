#ifndef DLGDEMO_H
#define DLGDEMO_H

#include <QDialog>
#include <QMap>
#include <QPixmap>
#include <QVariantMap>
#include <QNetworkReply>
#include <QSslError>

namespace Ui {
class DlgDemo;
}

class freebase_data_manager;

class DlgDemo : public QDialog
{
    Q_OBJECT
    
public:
    explicit DlgDemo(QWidget *parent = 0);
    ~DlgDemo();

private slots:
    void startSearch();
    void onJsonReady(const int rt);
    void onItemSelected();
    void sslErrorHandler(QNetworkReply* qnr, const QList<QSslError> & errlist);
    void onLinkClicked(const QUrl& url);

private:
    void getImage(const QString& mid);
    void getPersonalInfo(const QString& id);
    void clearOldData();
    QString createHtmlForPerson(const QVariantMap& map);
    QString findNamespaceValue(const QString& ns, const QVariantMap& map);


private:
    Ui::DlgDemo *ui;
    freebase_data_manager* m_pManager;
    QMap<QString,QString> m_mapMids;
};

#endif // DLGDEMO_H
