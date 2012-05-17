#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QDate>

#include "dlgdemo.h"
#include "ui_dlgdemo.h"
#include "freebase_data_manager.h"

DlgDemo::DlgDemo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgDemo)
{
    ui->setupUi(this);
    ui->comboBoxLimit->setCurrentIndex(3);

    m_pManager = new freebase_data_manager(this);

    //m_pScene = new QGraphicsScene();
    //ui->graphicsViewDemo->setScene(m_pScene);

    connect(ui->pushButtonGoDemo,SIGNAL(clicked()),this,SLOT(startSearch()));
    connect(ui->comboBoxDemo,SIGNAL(currentIndexChanged(QString)),this,SLOT(onItemSelected(QString)));
    connect(m_pManager,SIGNAL(sigJsonReady(int)),this,SLOT(onJsonReady(int)));
    connect(m_pManager,SIGNAL(sigImageReady(QPixmap,int)),this,SLOT(onImageReady(QPixmap,int)));
//    connect(m_pManager,SIGNAL(sigMqlReplyReady()),this,SLOT(onMqlReady(int)));

    connect(ui->webView->page()->networkAccessManager(),
              SIGNAL(sslErrors(QNetworkReply*, const QList<QSslError> & )),
              this,
              SLOT(sslErrorHandler(QNetworkReply*, const QList<QSslError> & )));


}

DlgDemo::~DlgDemo()
{
    //delete m_pScene;
    delete m_pManager;
    delete ui;
}

void DlgDemo::startSearch()
{
    qDebug() << Q_FUNC_INFO;
    QString query = ui->lineEditInputDemo->text();
    if (!query.isEmpty()) {
        clearOldData();
        m_pManager->runSearchQuery(query+"&type=/people/person",ui->comboBoxLimit->currentText(),0);
    }
}

void DlgDemo::onJsonReady(const int rt)
{
    qDebug() << Q_FUNC_INFO << " rt=" << rt;
    if (rt == REQ_SEARCH) {
        m_mapMids.clear();
        ui->comboBoxDemo->clear();
        QString mapKey;
        QVariantList list = m_pManager->getJsonData().toMap().contains("result") ?
                    m_pManager->getJsonData().toMap()["result"].toList() : QVariantList();

        QStringList names;
        foreach (QVariant item, list) {
            mapKey = "";
            QVariantMap map = item.toMap();
            if (map.contains("name")) {
                mapKey += map["name"].toString();
            }
            if (map.contains("notable")) {
                mapKey += map["notable"].toMap().contains("name") ? " (" + map["notable"].toMap()["name"].toString() + ")" : "";
            }
            if (map.contains("mid")) {
                m_mapMids[mapKey] = map["mid"].toString();
                names << mapKey;
            }
        }
        ui->comboBoxDemo->addItems(names);
        ui->labelCount->setText(m_pManager->getJsonData().toMap().contains("hits") ?
                                    m_pManager->getJsonData().toMap()["hits"].toString() : "No Items"
                                    );
    }
    else if (rt == REQ_MQL)
    {
        qDebug() << "Some info from MQL arrived";
        QVariantMap map = m_pManager->getJsonData().toMap()["result"].toMap();
        QString strHtml = createHtmlForPerson(map);
        ui->webView->setHtml(strHtml);
    }
}

void DlgDemo::onItemSelected(const QString& str)
{
    qDebug() << Q_FUNC_INFO << " str=" << str;
    if (str != "Select item" && !str.isEmpty()) {
        getImage(m_mapMids[str]);
        getPersonalInfo(m_mapMids[str]);
    }
}

void DlgDemo::getImage(const QString& mid)
{
    //int height = ui->graphicsViewDemo->size().height();
    //int width = ui->graphicsViewDemo->size().width();
    //clearReplyImage();
    //m_pManager->runImageQuery(mid,height,width);
}

void DlgDemo::getPersonalInfo(const QString& id)
{
    QString query = "{";
    if (id.startsWith("/m/")) {
        query += "\"mid\":\"" + id + "\"";
    } else {
        query += "\"id\":\"" + id + "\"";
    }
    query += ",\"type\":\"/people/person\",\"*\":null}";
    qDebug() << "QUERY: " << query;
    m_pManager->runMqlQuery(query);
}

void DlgDemo::clearReplyImage()
{
    //QList<QGraphicsItem*> list = ui->graphicsViewDemo->scene()->items();
    //foreach (QGraphicsItem* p, list) {
    //    ui->graphicsViewDemo->scene()->removeItem(p);
    //}
}

void DlgDemo::onImageReady(const QPixmap& image, const int rt)
{
    qDebug() << Q_FUNC_INFO;
    //QGraphicsPixmapItem *item = new QGraphicsPixmapItem(image);
    //m_pScene->addItem(item);
    //ui->graphicsViewDemo->fitInView(item,Qt::KeepAspectRatio);
}

void DlgDemo::clearOldData()
{
    ui->labelCount->clear();
    //ui->labelName->setText("...");
    ui->comboBoxDemo->clear();
    //clearReplyImage();
}

QString DlgDemo::createHtmlForPerson(const QVariantMap& map)
{
    QString strHtml = QString("<html><body>");
    strHtml += QString("<img src=\"https://usercontent.googleapis.com/freebase/v1-sandbox/image%1?maxheight=400&maxwidth=200\">").arg(map["mid"].toString());
    strHtml += "<P><u>Name</u>: <b>" + map["name"].toString() + "</b>\n\n";

    if(!map["date_of_birth"].toString().isEmpty())
    {
        QDate date = QDate::fromString(map["date_of_birth"].toString(), Qt::ISODate);
        strHtml += "<P><u>Date of birth</u>: <b>" + date.toString("d MMM yyyy") + "</b>\n\n";
    }
    if(!map["place_of_birth"].toString().isEmpty())
    {
        strHtml += "<P><u>Place of birth</u>: <b>" + map["place_of_birth"].toString() + "</b>\n\n";
    }
    QVariantList lst = map["nationality"].toList();
    if(!lst.isEmpty())
    {
        strHtml += "<P><u>Nationality</u>: <b>" + lst[0].toString() + "</b>\n\n";
    }
    lst = map["religion"].toList();
    if(!lst.isEmpty())
    {
        strHtml += "<P><u>Religion</u>: <b>" + lst[0].toString() + "</b>\n\n";
    }

    lst = map["children"].toList();
    if(!lst.isEmpty())
    {
        QString children;
        for(int i = 0; i < lst.count(); ++i)
        {
            children += lst[i].toString();
            if(i < lst.count()-1)
                children += ", ";
        }
        strHtml += "<P><u>Children</u>: <b>" + children + "</b>\n\n";
    }

    lst = map["parents"].toList();
    if(!lst.isEmpty())
    {
        QString parents;
        for(int i = 0; i < lst.count(); ++i)
        {
            parents += lst[i].toString();
            if(i < lst.count()-1)
                parents += ", ";
        }
        strHtml += "<P><u>Parents</u>: <b>" + parents + "</b>\n\n";
    }

    lst = map["quotations"].toList();
    if(!lst.isEmpty())
    {
        QString str = "<ul>";
        for(int i = 0; i < lst.count(); ++i)
        {
            str += "<li>" + lst[i].toString() + "</li>";
        }
        str += "</ul>";
        strHtml += "<P><u>Quotations</u>:<br>" + str + "\n\n";
    }


    strHtml += "</body></html>";
    return strHtml;
}

void DlgDemo::sslErrorHandler(QNetworkReply* qnr, const QList<QSslError> & errlist)
{

  #if DEBUG_BUYIT
  qDebug() << "---frmBuyIt::sslErrorHandler: ";
  // show list of all ssl errors
  foreach (QSslError err, errlist)
    qDebug() << "ssl error: " << err;
  #endif

   qnr->ignoreSslErrors();
}
