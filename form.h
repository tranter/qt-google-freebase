#ifndef FORM_H
#define FORM_H

#include <QWidget>

#include "freebase_data_manager.h"

namespace Ui {
    class Form;
}

class QSettings;
class OAuth2;

class Form : public QWidget
{
    Q_OBJECT
    
public:
    explicit Form(QWidget *parent = 0);
    ~Form();
    
    void startLogin(bool bForce); //Show login dialog even if there is gotten already access_token.

private slots:
    void onLoginDone();
    void onErrorOccured(const QString& error);
    void onUserEmailReady();

private:
    Ui::Form *ui;
    OAuth2* m_pOAuth2;
    freebase_data_manager* m_pManager;
    QSettings* m_pSettings;
    QString m_strCompanyName;
    QString m_strAppName;

    void saveSettings();
};

#endif // FORM_H
