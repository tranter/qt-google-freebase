#ifndef DLGDEMOCOUNTRY_H
#define DLGDEMOCOUNTRY_H

#include <QDialog>
#include <QMap>
#include <QPixmap>
#include <QVariantMap>
#include <QNetworkReply>
#include <QSslError>

namespace Ui {
class DlgDemoCountry;
}

class freebase_data_manager;

class DlgDemoCountry : public QDialog
{
    Q_OBJECT
    
public:
    explicit DlgDemoCountry(QWidget *parent = 0);
    ~DlgDemoCountry();
private slots:
    void startSearch();
    void onJsonReady(const int rt);
    void onItemSelected();
    void sslErrorHandler(QNetworkReply* qnr, const QList<QSslError> & errlist);
    void onLinkClicked(const QUrl& url);

private:
    void getImage(const QString& mid);
    void getCountryInfo(const QString& id);
    void clearOldData();
    QString createHtmlForCountry(const QVariantMap& map);
    QString findNamespaceValue(const QString& ns, const QVariantMap& map);

private:
    Ui::DlgDemoCountry *ui;
    freebase_data_manager* m_pManager;
    QMap<QString,QString> m_mapMids;
};

#endif // DLGDEMOCOUNTRY_H
