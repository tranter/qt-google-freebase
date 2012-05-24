#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QDate>
#include <QDesktopServices>

#include "dlgdemocountry.h"
#include "ui_dlgdemocountry.h"
#include "freebase_data_manager.h"

DlgDemoCountry::DlgDemoCountry(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgDemoCountry)
{
    ui->setupUi(this);

    ui->comboBoxLimit->setCurrentIndex(3);
    ui->webView->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);

    m_pManager = new freebase_data_manager(this);

    connect(ui->pushButtonGoDemo,SIGNAL(clicked()),this,SLOT(startSearch()));
    connect(ui->comboBoxDemo,SIGNAL(currentIndexChanged(QString)),this,SLOT(onItemSelected()));
    connect(m_pManager,SIGNAL(sigJsonReady(int)),this,SLOT(onJsonReady(int)));

    connect(ui->webView->page()->networkAccessManager(),
              SIGNAL(sslErrors(QNetworkReply*, const QList<QSslError> & )),
              this,
              SLOT(sslErrorHandler(QNetworkReply*, const QList<QSslError> & )));
    connect(ui->webView,SIGNAL(linkClicked(QUrl)),this,SLOT(onLinkClicked(QUrl)));
}

DlgDemoCountry::~DlgDemoCountry()
{
    delete m_pManager;
    delete ui;
}

void DlgDemoCountry::startSearch()
{
    qDebug() << Q_FUNC_INFO;
    QString query = ui->lineEditInputDemo->text();
    if (!query.isEmpty()) {
        clearOldData();
        m_pManager->runSearchQuery(query+"&type=/location/country",ui->comboBoxLimit->currentText(),0);
    }
}

void DlgDemoCountry::onJsonReady(const int rt)
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
//        QVariantMap map = m_pManager->getJsonData().toMap()["q0"].toMap()["result"].toMap();
//        QString strHtml = createHtmlForCountry(map);
        QString strHtml = createHtmlForCountry(m_pManager->getJsonData().toMap());
        ui->webView->setHtml(strHtml);
    }
}

void DlgDemoCountry::onItemSelected()
{
    QString str = ui->comboBoxDemo->currentText();
    qDebug() << Q_FUNC_INFO << " str=" << str;
    if (str != "Select item" && !str.isEmpty())
    {
        getCountryInfo(m_mapMids[str]);
    }
}

void DlgDemoCountry::getImage(const QString& mid)
{
    //int height = ui->graphicsViewDemo->size().height();
    //int width = ui->graphicsViewDemo->size().width();
    //clearReplyImage();
    //m_pManager->runImageQuery(mid,height,width);
}

void DlgDemoCountry::getCountryInfo(const QString& id)
{
    QStringList list;
    QString query = "{";
    if (id.startsWith("/m/"))
    {
        query += "\"mid\":\"" + id + "\"";
    }
    else
    {
        query += "\"id\":\"" + id + "\"";
    }
    QString type = "/location/country";
    query += QString(",\"type\":\"%1\", \"key\":[{}], \"*\":null}").arg(type);
    qDebug() << "QUERY: " << query;
    list << query;

    query = "{";
    if (id.startsWith("/m/")) {
        query += "\"mid\":\"" + id + "\"";
    } else {
        query += "\"id\":\"" + id + "\"";
    }
    type = "/location/location";
    query += QString(",\"type\":\"%1\", \"key\":[{}], \"*\":null}").arg(type);
    qDebug() << "QUERY: " << query;
    list << query;

    m_pManager->runMqlQueryMultiple(list);
}



void DlgDemoCountry::clearOldData()
{
    ui->labelCount->clear();
    //ui->labelName->setText("...");
    ui->comboBoxDemo->clear();
    //clearReplyImage();
}

QString DlgDemoCountry::createHtmlForCountry(const QVariantMap& map)
{
    qDebug() << Q_FUNC_INFO;
    QString strHtml = QString("<html><body>");
    QVariantList lst;
    QVariantMap mapLocal;
    QString key, text;
    strHtml += QString("<img src=\"https://usercontent.googleapis.com/freebase/v1-sandbox/image%1?maxheight=400&maxwidth=200\" align=\"right\">")
            .arg(map["q0"].toMap()["result"].toMap()["mid"].toString());

    // Referencies
    QString s = findNamespaceValue("/wikipedia/en_id",map);
    if (!s.isEmpty()) {
        strHtml += QString("<a href=\"http://www.freebase.com/view%1\"><img src=\"http://www.freebase.com/favicon.ico\" alt=\"Freebase\" hspace=\"2\"/>Freebase</a>")
                .arg(map["q0"].toMap()["result"].toMap()["mid"].toString());
        strHtml += QString("<a href=\"http://en.wikipedia.org/wiki/index.html?curid=%1\"><img src=\"http://en.wikipedia.org/favicon.ico\" alt=\"Wiki\" hspace=\"2\"/>Wiki</a>")
                .arg(s);
    }

    strHtml += "<p><i><u>General Info</u></i></p>";
    ///location/country
    mapLocal =  map["q0"].toMap()["result"].toMap();
    key = "name";
    text = "Name:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QMetaType::QVariantList) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }
    key = "capital";
    text = "Capital:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QMetaType::QVariantList) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }
    key = "administrative_divisions";
    text = "Administrative Divisions:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QMetaType::QVariantList) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }
    key = "calling_code";
    text = "Calling code:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QMetaType::QVariantList) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }
    key = "currency_used";
    text = "Currency used:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QMetaType::QVariantList) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }
    key = "fifa_code";
    text = "FIFA code:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QMetaType::QVariantList) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }
    key = "form_of_government";
    text = "Form of government:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QMetaType::QVariantList) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }
    key = "internet_tld";
    text = "Internet domains:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QMetaType::QVariantList) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }
    key = "languages_spoken";
    text = "Language spoken:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QMetaType::QVariantList) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }
    key = "official_language";
    text = "Official languages:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QMetaType::QVariantList) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }

    strHtml += "<p><i><u>Location Info</u></i></p>";
    ///location/location
    mapLocal =  map["q1"].toMap()["result"].toMap();
    key = "area";
    text = "Area (km2):";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QMetaType::QVariantList) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }
    key = "containedby";
    text = "Contained by:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QMetaType::QVariantList) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }
    key = "time_zones";
    text = "Time zones:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QMetaType::QVariantList) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }

    strHtml += "</body></html>";
    return strHtml;
}

void DlgDemoCountry::sslErrorHandler(QNetworkReply* qnr, const QList<QSslError> & errlist)
{

  #if DEBUG_BUYIT
  qDebug() << "---frmBuyIt::sslErrorHandler: ";
  // show list of all ssl errors
  foreach (QSslError err, errlist)
    qDebug() << "ssl error: " << err;
  #endif

   qnr->ignoreSslErrors();
}

QString DlgDemoCountry::findNamespaceValue(const QString& ns, const QVariantMap& map)
{
    QString ret;
    QVariantList list = map["q0"].toMap()["result"].toMap()["key"].toList();
    foreach (QVariant item, list) {
        QVariantMap m = item.toMap();
        if (item.toMap()["namespace"].toString() == ns) {
            ret = item.toMap()["value"].toString();
            break;
        }
    }

    return ret;
}

void DlgDemoCountry::onLinkClicked(const QUrl& url)
{
    qDebug() << Q_FUNC_INFO << " url=" << url;
    QDesktopServices::openUrl(url);
}
