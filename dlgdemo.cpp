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
    connect(ui->comboBoxDemo,SIGNAL(currentIndexChanged(QString)),this,SLOT(onItemSelected()));
    connect(m_pManager,SIGNAL(sigJsonReady(int)),this,SLOT(onJsonReady(int)));
    connect(ui->deceasedCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onItemSelected()));

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

void DlgDemo::onItemSelected()
{
    QString str = ui->comboBoxDemo->currentText();
    qDebug() << Q_FUNC_INFO << " str=" << str;
    if (str != "Select item" && !str.isEmpty())
    {
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
    if(!ui->deceasedCheckBox->isChecked())
    {
        query += ",\"type\":\"/people/person\",\"*\":null}";
    }
    else
    {
        query += ",\"type\":\"/people/deceased_person\",\"*\":null}";
    }
    qDebug() << "QUERY: " << query;
    m_pManager->runMqlQuery(query);
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
    QVariantList lst;
    strHtml += QString("<img src=\"https://usercontent.googleapis.com/freebase/v1-sandbox/image%1?maxheight=400&maxwidth=200\">").arg(map["mid"].toString());
    strHtml += "<P><u>Name</u>: <b>" + map["name"].toString() + "</b>\n\n";

    //Deceased person
    if(!map["date_of_death"].toString().isEmpty())
    {
        QDate date = QDate::fromString(map["date_of_death"].toString(), Qt::ISODate);
        strHtml += "<P><u>Date of death</u>: <b>" + date.toString("d MMM yyyy") + "</b>\n\n";
    }
    if(!map["place_of_death"].toString().isEmpty())
    {
        strHtml += "<P><u>Place of death</u>: <b>" + map["place_of_death"].toString() + "</b>\n\n";
    }
    lst = map["place_of_burial"].toList();
    if(!lst.isEmpty())
    {
        strHtml += "<P><u>Place of burial</u>: <b>" + lst[0].toString() + "</b>\n\n";
    }
    lst = map["cause_of_death"].toList();
    if(!lst.isEmpty())
    {
        QString str;
        for(int i = 0; i < lst.count(); ++i)
        {
            str += lst[i].toString();
            if(i < lst.count()-1)
                str += ", ";
        }
        strHtml += "<P><u>Cause of death</u>: <b>" + str + "</b>\n\n";
    }


    //Living person
    if(!map["date_of_birth"].toString().isEmpty())
    {
        QDate date = QDate::fromString(map["date_of_birth"].toString(), Qt::ISODate);
        strHtml += "<P><u>Date of birth</u>: <b>" + date.toString("d MMM yyyy") + "</b>\n\n";
    }
    if(!map["place_of_birth"].toString().isEmpty())
    {
        strHtml += "<P><u>Place of birth</u>: <b>" + map["place_of_birth"].toString() + "</b>\n\n";
    }
    lst = map["profession"].toList();
    if(!lst.isEmpty())
    {
        QString str;
        for(int i = 0; i < lst.count(); ++i)
        {
            str += lst[i].toString();
            if(i < lst.count()-1)
                str += ", ";
        }
        strHtml += "<P><u>Profession</u>: <b>" + str + "</b>\n\n";
    }
    lst = map["nationality"].toList();
    if(!lst.isEmpty())
    {
        strHtml += "<P><u>Nationality</u>: <b>" + lst[0].toString() + "</b>\n\n";
    }
    lst = map["ethnicity"].toList();
    if(!lst.isEmpty())
    {
        QString str;
        for(int i = 0; i < lst.count(); ++i)
        {
            str += lst[i].toString();
            if(i < lst.count()-1)
                str += ", ";
        }
        strHtml += "<P><u>Ethnicity</u>: <b>" + str + "</b>\n\n";
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
