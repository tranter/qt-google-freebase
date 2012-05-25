#include <QDebug>
#include <QSettings>
#include <QMessageBox>
#include <QStatusBar>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QWebFrame>

#include "form.h"
#include "ui_form.h"
#include "oauth2.h"
#include "mainwindow.h"
#include "treejsonmodel.h"
#include "treejsonitem.h"
#include "dlgdemo.h"
#include "dlgdemocountry.h"

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

    m_listSplitterSave << 250;
    m_listSplitterSave << 300;
    ui->splitter->setSizes(m_listSplitterSave);
    connect(ui->splitter,SIGNAL(splitterMoved(int,int)),this,SLOT(onSplitterMoved(int,int)));

    m_pMain = (MainWindow*)parent;
    m_pMain->showStatusText("Uknown user email");
    m_pOAuth2 = new OAuth2(this);

    m_strCompanyName = "YOU_COMPANY_NAME_HERE";
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

    connect(m_pManager, SIGNAL(sigMqlReplyReady(int)),this,SLOT(onMqlReplyReady(int)));
    connect(m_pManager, SIGNAL(sigSearchReplyReady(int)),this,SLOT(onSearchReplyReady(int)));
    connect(m_pManager, SIGNAL(sigJsonReady(int)),this,SLOT(onJsonReady(int)));
    connect(m_pManager, SIGNAL(sigImageReady(QPixmap,int)),this,SLOT(onImageReady(QPixmap,int)));

    connect(ui->btnRun,SIGNAL(clicked()),this,SLOT(onBtnRunClicked()));
    connect(ui->btnClear,SIGNAL(clicked()),this,SLOT(onBtnClearClicked()));
    connect(ui->pushButtonDomainsList,SIGNAL(clicked()),this,SLOT(listDomains()));

    initSuggestPage();
    connect(ui->webViewImage->page()->networkAccessManager(),
              SIGNAL(sslErrors(QNetworkReply*, const QList<QSslError> & )),
              this,
              SLOT(sslImageErrorHandler(QNetworkReply*, const QList<QSslError> & )));

    ui->tabReply->setCurrentIndex(indexTabReplyByName("Json"));

    m_pModel = new TreeJsonModel(QVariant(),ui->treeMqlReply);
    ui->treeMqlReply->setModel(m_pModel);
    ui->treeMqlReply->setHeaderHidden(false);
    connect(ui->treeMqlReply,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(onTreeGoToItem(QModelIndex)));

    ui->webViewText->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    connect(ui->webViewText->page()->networkAccessManager(),SIGNAL(sslErrors(QNetworkReply*, const QList<QSslError>& )),
              this,SLOT(sslTextErrorHandler(QNetworkReply*, const QList<QSslError>& )));
    connect(ui->webViewText,SIGNAL(linkClicked(QUrl)),this,SLOT(onTextLinkClicked(QUrl)));

    m_historyMax = 20;
    m_historyPos["MQL Request"] = 0;
    m_historyPos["Search Request"] = 0;
    m_history["MQL Request"].insert(0,ui->editMqlQuery->toPlainText());
    m_history["Search Request"].insert(0,ui->editSearchQuery->toPlainText()+"@"+ui->editSearchFilter->toPlainText());

    m_requestForDomainList = false;
}

Form::~Form()
{
    delete m_pModel;
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

void Form::onMqlReplyReady(const int rt)
{
    qDebug() << Q_FUNC_INFO;
    ui->textMqlReply->setPlainText(m_pManager->getReplyStr());
    ui->webViewText->setHtml(m_pManager->getRichTextReplyStr());

    if( m_requestForDomainList )
    {
        QVariantMap map = m_pManager->getJsonData().toMap();
        if( ! map["q0"].isValid() ) return;

        QString html( "<html><head><style type=\"text/css\">a{color:black;text-decoration:none}</style></head><body><table>" );
        QVariantList list = map["q0"].toMap()["result"].toList();
        QString id, name, type;

        QString mqlHref( "<a href=\"mql%1:%2\">%2</a>" );

        foreach(const QVariant & v, list)
        {
            map  = v.toMap();
            id   = map["id"].toString();
            name = map["name"].toString();
            type = map["type"].toString();

            QString link = QString("<a href=\"%1\">%1</a>").arg(id);

            if( type == "/type/domain" )
                link = mqlHref.arg( "Type", id );
            else if( type == "/type/type" )
                link = mqlHref.arg( "Instances", id );

            html += QString("<tr><td><a href=\"%2\">%1</a></td>"
                            "<td>%3</td></tr>").arg(name, id, link);
        }

        html += "</table></body></html>";

        ui->webViewText->setHtml( html );
    }
}

void Form::onSearchReplyReady(const int rt)
{
    qDebug() << Q_FUNC_INFO;
    ui->textMqlReply->setPlainText(m_pManager->getReplyStr());
    ui->webViewText->setHtml(m_pManager->getRichTextReplyStr());
}

void Form::onJsonReady(const int rt)
{
    m_pModel->setNewModelData(m_pManager->getJsonData());
}

void Form::onImageReady(const QPixmap& px, const int rt)
{
//    QGraphicsPixmapItem *item = new QGraphicsPixmapItem(px);
//    m_pScene->addItem(item);
//    ui->graphicsViewImage->fitInView(item,Qt::KeepAspectRatio);
}

void Form::onBtnRunClicked()
{
    m_requestForDomainList = false;

    ui->textMqlReply->clear();
    clearReplyImage();
    int index = ui->tabQuery->currentIndex();
    if (ui->tabQuery->tabText(index) == "MQL Request") {
        m_historyPos["MQL Request"] = 0;
        QString curValue = ui->editMqlQuery->toPlainText();
        QString oldValue = m_history["MQL Request"].value(0);
        if (curValue != oldValue) {
            m_history["MQL Request"].insert(0,curValue);
            if (m_history["MQL Request"].size()>m_historyMax) {
                m_history["MQL Request"].removeLast();
            }
        }
        ui->tabReply->setCurrentIndex(indexTabReplyByName("Json"));
        m_pManager->runMqlQuery(curValue);
    } else if (ui->tabQuery->tabText(index) == "Search Request") {
        m_historyPos["Search Request"] = 0;
        QStringList list = m_history["Search Request"].value(0).split("@");
        if (list.size() == 1) {
            list << "";
        }
        QString curValueQuery = ui->editSearchQuery->toPlainText();
        QString oldValueQuery = list[0];
        QString curValueFilter = ui->editSearchFilter->toPlainText();
        QString oldValueFilter = list[1];
        if (curValueQuery != oldValueQuery || curValueFilter != oldValueFilter) {
            m_history["Search Request"].insert(0,curValueQuery+"@"+curValueFilter);
            if (m_history["Search Request"].size()>m_historyMax) {
                m_history["Search Request"].removeLast();
            }
        }
        ui->tabReply->setCurrentIndex(indexTabReplyByName("Text"));
        QString query = QString("%1&%2").arg(curValueQuery,curValueFilter);
        m_pManager->runSearchQuery(query);
    } else if (ui->tabQuery->tabText(index) == "Write Request") {
        ui->tabReply->setCurrentIndex(indexTabReplyByName("Json"));
        m_pManager->runWriteQuery(ui->editWriteQuery->toPlainText(),m_pOAuth2->accessToken(),m_pOAuth2->getSimpleAPIKey());
    }
}

void Form::onBtnTextGoClicked()
{
    m_requestForDomainList = false;
    ui->tabReply->setCurrentIndex(indexTabReplyByName("Text"));
    ui->textMqlReply->clear();
    m_pManager->runTextQuery(ui->lineEditText->text());
}

void Form::onBtnImageGoClicked()
{
    m_requestForDomainList = false;
//    ui->tabReply->setCurrentIndex(indexTabReplyByName("Image"));
//    clearReplyImage();
//    m_pManager->runImageQuery(ui->lineEditImage->text()
//                              ,ui->graphicsViewImage->size().height()
//                              ,ui->graphicsViewImage->size().width());
    QString strHtml = createImageHtml();
    ui->webViewImage->setHtml(strHtml);
}

void Form::onBtnClearClicked()
{
    m_requestForDomainList = false;
    ui->textMqlReply->clear();
//    ui->textBrowserText->clear();
    ui->webViewText->setHtml(QString());
    ui->webViewImage->setHtml(QString());
    clearReplyImage();
    clearTreeJson();
}

void Form::listDomains()
{
    m_historyPos["MQL Request"] = 0;
    QString curValue = "[{\n\t\"id\": null,\n\t\"name\": null,\n\t\"sort\": \"name\",\n\t\"type\": \"/type/domain\",\n\t\"!/freebase/domain_category/domains\": {\n\t\t\"id\": \"/category/commons\"\n\t}\n}]";
    QString oldValue = m_history["MQL Request"].value(0);
    if (curValue != oldValue) {
        m_history["MQL Request"].insert(0,curValue);
        if (m_history["MQL Request"].size()>m_historyMax) {
            m_history["MQL Request"].removeLast();
        }
    }
    //ui->tabReply->setCurrentIndex(indexTabReplyByName("Json"));
    ui->tabReply->setCurrentIndex(indexTabReplyByName("Text"));
    ui->textMqlReply->clear();
    m_pManager->runMqlQuery(curValue);

    m_requestForDomainList = true;

//    ui->tabReply->setCurrentIndex(indexTabReplyByName("Text"));
//    ui->textMqlReply->clear();
//    QString query = "[{\"id\": null,\"name\": null,\"type\": \"/type/domain\",\"!/freebase/domain_category/domains\": {\"id\": \"/category/commons\"}}]";
//    m_pManager->runMqlQuery(query);
}

void Form::onTabQueryTabChanged(int pos)
{
    if (pos == indexTabQueryByName("Misc Request")) {
        ui->splitter->setSizes(m_listSplitterSave);
        ui->btnRun->setEnabled(false);
    } else if (pos == indexTabQueryByName("Suggest")) {
        ui->btnRun->setEnabled(false);
    } else {
        ui->splitter->setSizes(m_listSplitterSave);
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
//    QList<QGraphicsItem*> list = ui->graphicsViewImage->scene()->items();
//    foreach (QGraphicsItem* p, list) {
//        ui->graphicsViewImage->scene()->removeItem(p);
//    }
}

void Form::clearTreeJson()
{
    m_pModel->clear();
}

void Form::onSplitterMoved(int pos, int index)
{
    if (ui->tabQuery->currentIndex()!=indexTabQueryByName("Suggest")) {
        m_listSplitterSave = ui->splitter->sizes();
    }
}
/** \brief Load MyWebPage as a current page
 *
 *  Main purpose: init Form object in a page's javascript context
 *  Two stage process. First connect signal about new url changed with handler slot.
 */
void Form::initSuggestPage()
{
    QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, true);
    MyWebPage* page = new MyWebPage(ui->webViewSuggest);
    ui->webViewSuggest->setPage((QWebPage*)page);
    QWebFrame* frame = page->mainFrame();
    connect(frame,SIGNAL(javaScriptWindowObjectCleared()),this,SLOT(onNewPage()));

    ui->webViewSuggest->setUrl(QUrl("qrc:/html/suggest-basic.html"));

    MyWebPage* pageImage = new MyWebPage(ui->webViewImage);
    ui->webViewImage->setPage((QWebPage*)pageImage);
}
/** \brief On each new url add Form object to javascript context.
 *
 */
void Form::onNewPage()
{
    QWebFrame* frame = ui->webViewSuggest->page()->mainFrame();
    frame->addToJavaScriptWindowObject("appForm",this);
}

void Form::onSuggestData(const QString& name,const QString& id,const QString& mid)
{
    ui->tabReply->setCurrentIndex(indexTabReplyByName("Text"));
    ui->textMqlReply->clear();
    m_pManager->runSearchQuery(id);

//    clearReplyImage();
//    m_pManager->runImageQuery(id
//        ,ui->graphicsViewImage->size().height()
//        ,ui->graphicsViewImage->size().width());
}

void Form::onTreeGoToItem(const QModelIndex& index)
{
//    if (!index.isValid()) {
//        return;
//    }
//    TreeJsonItem* pNode = static_cast<TreeJsonItem*>(index.internalPointer());
//    TreeJsonItem* pParentNode = index.parent().isValid() ? static_cast<TreeJsonItem*>(index.parent().internalPointer()) : NULL;
//    QString key = pNode->data(0).toString();
//    QString keyParent = pParentNode ? pParentNode->data(0).toString() : "";
//    QString value = pNode->data(1).toString();
//    int height = ui->graphicsViewImage->size().height();
//    int width = ui->graphicsViewImage->size().width();
//    if (value.startsWith("/m/")) {
//        ui->textMqlReply->clear();
//        m_pManager->runSearchQuery(value);

//        clearReplyImage();
//        m_pManager->runImageQuery(value
//            ,height
//            ,width);
//    } else if (key == "guid" || keyParent == "guid") {
//        QString str = "/guid/";
//        str += value;
//        ui->textMqlReply->clear();
//        m_pManager->runSearchQuery(str);

//        clearReplyImage();
//        m_pManager->runImageQuery(str
//            ,height
//            ,width);
//    } else if (key == "id" || keyParent == "id") {
//        ui->textMqlReply->clear();
//        m_pManager->runSearchQuery(value);

//        clearReplyImage();
//        m_pManager->runImageQuery(value
//            ,height
//            ,width);
//    }
}

//void Form::onTextBrowserAnchorClicked(const QUrl& url)
//{
//    QString value = url.toString();
//    qDebug() << Q_FUNC_INFO << " url=" << url;

//    ui->textMqlReply->clear();
//    m_pManager->runSearchQuery(value);
////    int height = ui->graphicsViewImage->size().height();
////    int width = ui->graphicsViewImage->size().width();
////    if (value.startsWith('/')) {
////        clearReplyImage();
////        m_pManager->runImageQuery(value
////            ,height
////            ,width);
////    }
//}

void Form::onTextLinkClicked(const QUrl& url)
{
    qDebug() << Q_FUNC_INFO << " url=" << url;

    QString value = url.toString();
    ui->textMqlReply->clear();

    if( value.startsWith("mqltype:") )
    {
        QString q = QString("[{ \"id\": null, \"name\": null, \"type\": \"/type/type\", \"domain\": \"%1\" }]").arg( value.remove(0, 8) );
        m_pManager->runMqlQuery(q);
        return;
    } else if( value.startsWith("mqlinstances:") ) {
        QString q = QString("[{ \"id\": null, \"name\": null, \"type\": \"%1\" }]").arg( value.remove(0, 13) );
        m_pManager->runMqlQuery(q);
        return;
    }

    m_pManager->runSearchQuery(value);
}

void Form::onBacwardAction()
{
    qDebug() << Q_FUNC_INFO;
    int len;
    QString tabName = ui->tabQuery->tabText(ui->tabQuery->currentIndex());
    if (tabName == "MQL Request") {
        len = m_history["MQL Request"].size();
        if (m_historyPos["MQL Request"] < len-1) {
            m_historyPos["MQL Request"]++;
            ui->editMqlQuery->setPlainText(m_history["MQL Request"].value(m_historyPos["MQL Request"]));
        }
    } else if (tabName == "Search Request") {
        len = m_history["Search Request"].size();
        if (m_historyPos["Search Request"] < len-1) {
            m_historyPos["Search Request"]++;
            QStringList list = m_history["Search Request"].value(m_historyPos["Search Request"]).split("@");
            ui->editSearchQuery->setPlainText(list.value(0));
            ui->editSearchFilter->setPlainText(list.value(1));
        }
    }
}

void Form::onForwardAction()
{
    qDebug() << Q_FUNC_INFO;
    QString tabName = ui->tabQuery->tabText(ui->tabQuery->currentIndex());
    if (tabName == "MQL Request") {
        if (m_historyPos["MQL Request"] > 0) {
            m_historyPos["MQL Request"]--;
            ui->editMqlQuery->setPlainText(m_history["MQL Request"].value(m_historyPos["MQL Request"]));
        }
    } else if (tabName == "Search Request") {
        if (m_historyPos["Search Request"] > 0) {
            m_historyPos["Search Request"]--;
            QStringList list = m_history["Search Request"].value(m_historyPos["Search Request"]).split("@");
            ui->editSearchQuery->setPlainText(list.value(0));
            ui->editSearchFilter->setPlainText(list.value(1));
        }
    }
}

void Form::runQuery()
{
    if (ui->btnRun->isEnabled()) {
        onBtnRunClicked();
    }
}

void Form::startDlgDemo()
{
    qDebug() << Q_FUNC_INFO;

    DlgDemo* pDlg = new DlgDemo(this);
    pDlg->exec();
    delete pDlg;
}

void Form::startDlgDemoCountry()
{
    qDebug() << Q_FUNC_INFO;

    DlgDemoCountry* pDlg = new DlgDemoCountry(this);
    pDlg->exec();
    delete pDlg;
}

void Form::sslImageErrorHandler(QNetworkReply* qnr, const QList<QSslError>& /*errlist*/)
{
    qnr->ignoreSslErrors();
}

void Form::sslTextErrorHandler(QNetworkReply* qnr, const QList<QSslError>& /*errlist*/)
{
    qnr->ignoreSslErrors();
}

QString Form::createImageHtml()
{
    QString ret;
    QString id = ui->lineEditImage->text();
    if (id.isEmpty()) {
        return ret;
    }
    int imageHeight = ui->webViewImage->size().height();
    int imageWidth = ui->webViewImage->size().width();
    ret += "<html>";
    ret += "<script language=\"javascript\">"
            "function fixImage() {"
            "   var image=document.getElementById(\"resizeImage\");"
            "   var w=image.width;"
            "   var h=image.height;"
            "   if (h > w) {"
            "       image.setAttribute(\"height\",\"95%\");"
            "   } else {"
            "       image.setAttribute(\"width\",\"95%\");"
            "   }"
            "}</script>";
    ret += "<body onLoad=\"fixImage();\">";
    ret += QString("<img id=\"resizeImage\" src=\"https://usercontent.googleapis.com/freebase/v1-sandbox/image%1?maxheight=%2&maxwidth=%3&mode=fit\"/>")
            .arg(id,QString::number(imageHeight),QString::number(imageWidth));
    ret += "</body></html>";
    return ret;
}
