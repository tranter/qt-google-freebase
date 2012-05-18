#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QDate>
#include <QDesktopServices>

#include "dlgdemo.h"
#include "ui_dlgdemo.h"
#include "freebase_data_manager.h"

DlgDemo::DlgDemo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgDemo)
{
    ui->setupUi(this);
    ui->comboBoxLimit->setCurrentIndex(3);
    ui->webView->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);

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
    connect(ui->webView,SIGNAL(linkClicked(QUrl)),this,SLOT(onLinkClicked(QUrl)));
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
        query += ",\"type\":\"/people/person\",\"key\":[{}],\"*\":null}";
    }
    else
    {
        query += ",\"type\":\"/people/deceased_person\",\"key\":[{}],\"*\":null}";
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
    strHtml += QString("<img src=\"https://usercontent.googleapis.com/freebase/v1-sandbox/image%1?maxheight=400&maxwidth=200\" align=\"left\" vspace=10 hspace=10>").arg(map["mid"].toString());

    // Referencies
    QString s = findNamespaceValue("/wikipedia/en_id",map);
    if (!s.isEmpty()) {
        strHtml += QString("<a href=\"http://www.freebase.com/view%1\"><img src=\"http://www.freebase.com/favicon.ico\" alt=\"Freebase\" hspace=\"2\"/>Freebase</a>")
                .arg(map["mid"].toString());
        strHtml += QString("<a href=\"http://en.wikipedia.org/wiki/index.html?curid=%1\"><img src=\"http://en.wikipedia.org/favicon.ico\" alt=\"Wiki\" hspace=\"2\"/>Wiki</a>")
                .arg(s);
    }

    // Name
    strHtml += "<P><u>Name</u>: <b>" + map["name"].toString() + "</b>";

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

QString DlgDemo::findNamespaceValue(const QString& ns, const QVariantMap& map)
{
    QString ret;
    QVariantList list = map["key"].toList();
    foreach (QVariant item, list) {
        QVariantMap m = item.toMap();
        if (item.toMap()["namespace"].toString() == ns) {
            ret = item.toMap()["value"].toString();
            break;
        }
    }

    return ret;
}

void DlgDemo::onLinkClicked(const QUrl& url)
{
    qDebug() << Q_FUNC_INFO << " url=" << url;
    QDesktopServices::openUrl(url);
}
