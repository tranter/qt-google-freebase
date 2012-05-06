#ifndef FORM_H
#define FORM_H

#include <QWidget>

#include "freebase_data_manager.h"

namespace Ui {
    class Form;
}

class QSettings;
class OAuth2;
class MainWindow;
class QGraphicsScene;

class Form : public QWidget
{
    Q_OBJECT
    
public:
    explicit Form(QWidget *parent = 0);
    ~Form();
    
    void startLogin(bool bForce); //Show login dialog even if there is gotten already access_token.
    void startFreebaseLogin();
    void listDomains();

private slots:
    void onLoginDone();
    void onErrorOccured(const QString& error);
    void onUserEmailReady();
    void onMqlReplyReady();
    void onImageReady(const QPixmap& px);

    void onBtnRunClicked();
    void onBtnClearClicked();
    void onBtnTextGoClicked();
    void onBtnImageGoClicked();

    void onTabQueryTabChanged(int pos);

private:
    Ui::Form *ui;
    MainWindow* m_pMain;
    OAuth2* m_pOAuth2;
    freebase_data_manager* m_pManager;
    QSettings* m_pSettings;
    QString m_strCompanyName;
    QString m_strAppName;
    QGraphicsScene* m_pScene;

    void saveSettings();
    int indexTabQueryByName(const QString& name);
    int indexTabReplyByName(const QString& name);
    void clearReplyImage();
};

#endif // FORM_H
