#include <QDebug>
#include <QSettings>
#include <QMessageBox>
#include <QStatusBar>
#include <QGraphicsScene>
#include <QGraphicsItem>

#include "form.h"
#include "ui_form.h"
#include "oauth2.h"
#include "mainwindow.h"

Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);

    ui->tabQuery->setCurrentIndex(0);

    int ind = indexTabQueryByName("Write Request");
    if (ind >= 0) {
        ui->tabQuery->removeTab(ind);
    }

    connect(ui->pushButtonTextGo,SIGNAL(clicked()),this,SLOT(onBtnTextGoClicked()));
    connect(ui->pushButtonImageGo,SIGNAL(clicked()),this,SLOT(onBtnImageGoClicked()));
    connect(ui->tabQuery,SIGNAL(currentChanged(int)),this,SLOT(onTabQueryTabChanged(int)));

    ui->tabReply->setCurrentIndex(1);
    ui->tabReply->setTabEnabled(0,false);
    ui->tabReply->removeTab(0);

    QList<int> sizes;
    sizes << 200;
    sizes << 350;
    ui->splitter->setSizes(sizes);

    m_pMain = (MainWindow*)parent;
    m_pMain->showStatusText("Uknown user email");
    m_pOAuth2 = new OAuth2(this);

//    m_strCompanyName = "YOU_COMPANY_NAME_HERE";
    m_strCompanyName = "ICS";
    m_strAppName = "QtFreebase";

    // Load settings
    m_pSettings = new QSettings(m_strCompanyName,m_strAppName);
    m_pOAuth2->setAccessToken(m_pSettings->value("access_token").toString());
    m_pOAuth2->setRefreshToken(m_pSettings->value("refresh_token").toString());
    m_pOAuth2->setSettings(m_pSettings);

    connect(m_pOAuth2, SIGNAL(loginDone()), this, SLOT(onLoginDone()));
    connect(m_pOAuth2, SIGNAL(sigErrorOccured(QString)),this,SLOT(onErrorOccured(QString)));

    m_pManager = new freebase_data_manager(this);
    connect(m_pManager, SIGNAL(sigErrorOccured(QString)),this,SLOT(onErrorOccured(QString)));
    connect(m_pManager, SIGNAL(sigUserEmailReady()),this,SLOT(onUserEmailReady()));
    connect(m_pManager, SIGNAL(sigMqlReplyReady()),this,SLOT(onMqlReplyReady()));
    connect(m_pManager, SIGNAL(sigImageReady(QPixmap)),this,SLOT(onImageReady(QPixmap)));

    connect(ui->btnRun,SIGNAL(clicked()),this,SLOT(onBtnRunClicked()));
    connect(ui->btnClear,SIGNAL(clicked()),this,SLOT(onBtnClearClicked()));

//    QImage img(":/images/login");
//    QPixmap px = QPixmap::fromImage(img);
////    QGraphicsPixmapItem *item = new QGraphicsPixmapItem(px);
    m_pScene = new QGraphicsScene();
////    scene->addItem(item);
//    scene->addPixmap(px);
    ui->graphicsViewImage->setScene(m_pScene);
}

Form::~Form()
{
    delete ui;
}

void Form::startLogin(bool bForce)
{
    //Now we allow to start logging in when m_oauth2.isAuthorized(). User can log in using another Google account.
    m_pOAuth2->startLogin(bForce); //this is a parent widget for a login dialog.
}

void Form::startFreebaseLogin()
{
    m_pManager->loginFreebase();
}

void Form::onErrorOccured(const QString& error)
{
    if(error.indexOf("Invalid Credentials") != -1) {
        startLogin(true);
    } else {
        QMessageBox::warning(this, tr("Some error on server occured"), error);
    }
}
/*! \brief Start function for initialization
 *
 */
void Form::onLoginDone()
{
    m_pManager->requestUserEmail(m_pOAuth2->accessToken());
}

void Form::saveSettings()
{
    m_pSettings->setValue("access_token",m_pOAuth2->accessToken());
    m_pSettings->setValue("refresh_token",m_pOAuth2->getRefreshToken());
}

void Form::onUserEmailReady()
{
    m_pMain->showStatusText(m_pManager->getUserEmail());
}

void Form::onMqlReplyReady()
{
    ui->textMqlReply->setPlainText(m_pManager->getReplyStr());
//    m_pModel->newTreeData(m_pManager->getJsonData());
}

void Form::onImageReady(const QPixmap& px)
{
    qDebug() << Q_FUNC_INFO;
//    m_pScene->addPixmap(px);
    QGraphicsPixmapItem *item = new QGraphicsPixmapItem(px);
    m_pScene->addItem(item);
    ui->graphicsViewImage->fitInView(item,Qt::KeepAspectRatio);
}

void Form::onBtnRunClicked()
{
    ui->textMqlReply->clear();
    clearReplyImage();
    int index = ui->tabQuery->currentIndex();
    if (ui->tabQuery->tabText(index) == "MQL Request") {
        m_pManager->runMqlQuery(ui->editMqlQuery->toPlainText());
    } else if (ui->tabQuery->tabText(index) == "Search Request") {
        QString query = QString("%1&%2").arg(ui->editSearchQuery->toPlainText(),ui->editSearchFilter->toPlainText());
        m_pManager->runSearchQuery(query);
    } else if (ui->tabQuery->tabText(index) == "Write Request") {
        m_pManager->runWriteQuery(ui->editWriteQuery->toPlainText(),m_pOAuth2->accessToken(),m_pOAuth2->getSimpleAPIKey());
    }
}

void Form::onBtnTextGoClicked()
{
    ui->tabReply->setCurrentIndex(indexTabReplyByName("Text"));
    ui->textMqlReply->clear();
    m_pManager->runTextQuery(ui->lineEditText->text());
}

void Form::onBtnImageGoClicked()
{
    ui->tabReply->setCurrentIndex(indexTabReplyByName("Image"));
    clearReplyImage();
    m_pManager->runImageQuery(ui->lineEditImage->text()
                              ,ui->graphicsViewImage->size().height()
                              ,ui->graphicsViewImage->size().width());
}

void Form::onBtnClearClicked()
{
    ui->textMqlReply->clear();
    clearReplyImage();
}

void Form::listDomains()
{
    QString query = "[{\"id\": null,\"name\": null,\"type\": \"/type/domain\",\"!/freebase/domain_category/domains\": {\"id\": \"/category/commons\"}}]";
    m_pManager->runMqlQuery(query);
}

void Form::onTabQueryTabChanged(int pos)
{
    if (pos == indexTabQueryByName("Misc Request")) {
        ui->btnRun->setEnabled(false);
    } else {
        ui->btnRun->setEnabled(true);
    }
}

int Form::indexTabQueryByName(const QString& name)
{
    int ret = -1;
    int len = ui->tabQuery->count();
    for (int i=0;i<len;i++) {
        if (ui->tabQuery->tabText(i) == name) {
            ret = i;
            break;
        }
    }
    return ret;
}

int Form::indexTabReplyByName(const QString& name)
{
    int ret = -1;
    int len = ui->tabReply->count();
    for (int i=0;i<len;i++) {
        if (ui->tabReply->tabText(i) == name) {
            ret = i;
            break;
        }
    }
    return ret;
}

void Form::clearReplyImage()
{
    QList<QGraphicsItem*> list = ui->graphicsViewImage->scene()->items();
    foreach (QGraphicsItem* p, list) {
        ui->graphicsViewImage->scene()->removeItem(p);
    }
}
