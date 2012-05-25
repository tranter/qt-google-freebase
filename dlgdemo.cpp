#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QDate>
#include <QDesktopServices>
#include <QWebFrame>

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

    connect(ui->webView->page()->networkAccessManager(),
              SIGNAL(sslErrors(QNetworkReply*, const QList<QSslError> & )),
              this,
              SLOT(sslErrorHandler(QNetworkReply*, const QList<QSslError> & )));
    connect(ui->webView,SIGNAL(linkClicked(QUrl)),this,SLOT(onLinkClicked(QUrl)));
    connect(ui->pushButtonBack,SIGNAL(clicked()),this,SLOT(onBack()));
}

DlgDemo::~DlgDemo()
{
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
        QString strHtml = createHtmlForPerson(m_pManager->getJsonData().toMap());
        ui->webView->setHtml(strHtml);
//        QWebPage* page = new QWebPage(this);
//        page->mainFrame()->setHtml(strHtml);
//        ui->webView->setPage(page);
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
    QString type = "/people/person";
    query += QString(",\"type\":\"%1\", \"key\":[{}], \"*\":null}").arg(type);
    qDebug() << "QUERY: " << query;
    list << query;

    query = "{";
    if (id.startsWith("/m/")) {
        query += "\"mid\":\"" + id + "\"";
    } else {
        query += "\"id\":\"" + id + "\"";
    }
    type = "/people/deceased_person";
    query += QString(",\"type\":\"%1\", \"key\":[{}], \"*\":null}").arg(type);
    qDebug() << "QUERY: " << query;
    list << query;

    query = "{";
    if (id.startsWith("/m/")) {
        query += "\"mid\":\"" + id + "\"";
    } else {
        query += "\"id\":\"" + id + "\"";
    }
    type = "/book/author";
    query += QString(",\"type\":\"%1\", \"key\":[{}], \"*\":null}").arg(type);
    qDebug() << "QUERY: " << query;
    list << query;

    query = "[{";
    if (id.startsWith("/m/")) {
        query += "\"mid\":\"" + id + "\"";
    } else {
        query += "\"id\":\"" + id + "\"";
    }
    type = "/people/person";
    query += QString(",\"type\":\"%1\",\"/type/reflect/any_reverse\":[{\"mid\":null,\"name\":null,\"type|=\":[\"/people/person\"]}]}]")
            .arg(type);
    qDebug() << "QUERY: " << query;
    list << query;

    query = "[{";
    if (id.startsWith("/m/")) {
        query += "\"mid\":\"" + id + "\"";
    } else {
        query += "\"id\":\"" + id + "\"";
    }
    type = "/people/person";
    query += QString(",\"type\":\"%1\",\"/type/reflect/any_reverse\":[{\"mid\":null,\"name\":null,\"type|=\":[\"/event/event\"]}]}]")
            .arg(type);
    qDebug() << "QUERY: " << query;
    list << query;

    m_pManager->runMqlQueryMultiple(list);
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
    QVariantMap mapLocal =  map["q0"].toMap()["result"].toMap();
    QString key, text;
    strHtml += QString("<img src=\"https://usercontent.googleapis.com/freebase/v1-sandbox/image%1?maxheight=400&maxwidth=200\" align=\"right\">")
            .arg(mapLocal["mid"].toString());

    // Referencies
    QString s = findNamespaceValue("/wikipedia/en_id",map);
    if (!s.isEmpty()) {
        strHtml += QString("<a href=\"http://www.freebase.com/view%1\"><img src=\"http://www.freebase.com/favicon.ico\" alt=\"Freebase\" hspace=\"2\"/>Freebase</a>")
                .arg(mapLocal["mid"].toString());
        strHtml += QString("<a href=\"http://en.wikipedia.org/wiki/index.html?curid=%1\"><img src=\"http://en.wikipedia.org/favicon.ico\" alt=\"Wiki\" hspace=\"2\"/>Wiki</a>")
                .arg(s);
    }

    strHtml += "<p><i><u>General Info</u></i></p>";
    ///people/person
    key = "name";
    text = "Name:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QVariant::List) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else if (t != QVariant::Invalid) {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }
    key = "date_of_birth";
    text = "Date of birth:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QVariant::List) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else if (t != QVariant::Invalid) {
            QDate date = QDate::fromString(mapLocal[key].toString(), Qt::ISODate);
            strHtml += "<p><i>" + text + " </i><b>" + date.toString("MMM d, yyyy") + "</b></p>";
        }
    }
    mapLocal =  map["q1"].toMap()["result"].toMap();
    key = "date_of_death";
    text = "Date of death:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QVariant::List) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else if (t != QVariant::Invalid) {
            QDate date = QDate::fromString(mapLocal[key].toString(), Qt::ISODate);
            strHtml += "<p><i>" + text + " </i><b>" + date.toString("MMM d, yyyy") + "</b></p>";
        }
    }
    key = "place_of_death";
    text = "Place of death:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QVariant::List) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else if (t != QVariant::Invalid) {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }
    key = "place_of_burial";
    text = "Place of bural:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QVariant::List) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else if (t != QVariant::Invalid) {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }
    key = "cause_of_death";
    text = "Cause of death:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QVariant::List) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else if (t != QVariant::Invalid) {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }

    //Living person
    strHtml += "<p><i><u>Person Info</u></i></p>";
    ///people/person
    mapLocal =  map["q0"].toMap()["result"].toMap();
    key = "place_of_birth";
    text = "Place of birth:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QVariant::List) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else if (t != QVariant::Invalid) {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }
    key = "profession";
    text = "Profession:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QVariant::List) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else if (t != QVariant::Invalid) {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }
    key = "nationality";
    text = "Nationality:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QVariant::List) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else if (t != QVariant::Invalid) {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }
    key = "ethnicity";
    text = "Ethnicity:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QVariant::List) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else if (t != QVariant::Invalid) {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }
    key = "religion";
    text = "Religion:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QVariant::List) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else if (t != QVariant::Invalid) {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }
    key = "children";
    text = "Children:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QVariant::List) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else if (t != QVariant::Invalid) {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }
    key = "parents";
    text = "Parents:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QVariant::List) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else if (t != QVariant::Invalid) {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }
    key = "quotations";
    text = "Quotations:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QVariant::List) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else if (t != QVariant::Invalid) {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }

    //Books author
    strHtml += "<p><i><u>Books Info</u></i></p>";
    ///books/author
    mapLocal =  map["q2"].toMap()["result"].toMap();
    key = "works_written";
    text = "Works written:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QVariant::List) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else if (t != QVariant::Invalid) {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }

    // Reference's links
    // Persons
    lst = map["q3"].toMap()["result"].toList();
    if (!lst.isEmpty()) {
        strHtml += "<p><i><u>People Referencies</u></i></p>";
        mapLocal = lst[0].toMap();
        key = "/type/reflect/any_reverse";
        text = "Referencies:";
        if (mapLocal.contains(key)) {
            QVariant::Type t = mapLocal[key].type();
            if (t == QVariant::List) {
                lst = mapLocal[key].toList();
                if (!lst.isEmpty()) {
                    strHtml += "<p><i>" + text + "</i></p><ul>";
                    foreach (QVariant item, lst) {
                        strHtml += "<li><a href=\""+item.toMap()["mid"].toString()+"\">" + item.toMap()["name"].toString()+ "</a></li>";
                    }
                    strHtml += "</ul>";
                }
            } else if (t != QVariant::Invalid) {
                strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
            }
        }
    }
    // Events
    lst = map["q4"].toMap()["result"].toList();
    if (!lst.isEmpty()) {
        strHtml += "<p><i><u>Event Referencies</u></i></p>";
        mapLocal = lst[0].toMap();
        key = "/type/reflect/any_reverse";
        text = "Referencies:";
        if (mapLocal.contains(key)) {
            QVariant::Type t = mapLocal[key].type();
            if (t == QVariant::List) {
                lst = mapLocal[key].toList();
                if (!lst.isEmpty()) {
                    strHtml += "<p><i>" + text + "</i></p><ul>";
                    foreach (QVariant item, lst) {
                        strHtml += "<li><a href=\""+item.toMap()["mid"].toString()+"\">" + item.toMap()["name"].toString()+ "</a></li>";
                    }
                    strHtml += "</ul>";
                }
            } else if (t != QVariant::Invalid) {
                strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
            }
        }
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

void DlgDemo::onLinkClicked(const QUrl& url)
{
    qDebug() << Q_FUNC_INFO << " url=" << url;
    if (url.toString().contains("http://")) {
        QDesktopServices::openUrl(url);
    } else if (url.toString().startsWith("/m/")) {
        getPersonalInfo(url.toString());
    }
}

void DlgDemo::onBack()
{
    qDebug() << Q_FUNC_INFO;
    ui->webView->back();
}
