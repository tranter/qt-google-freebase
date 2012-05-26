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
    connect(ui->pushButtonForward,SIGNAL(clicked()),this,SLOT(onForward()));
    m_state = ST_NONE;
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
        if (m_state == ST_NONE) {
            QString strHtml = createHtmlForPerson(m_pManager->getJsonData().toMap());
            ui->webView->setHtml(strHtml);
        } else {
            m_state = ST_NONE;
            QVariantMap map = m_pManager->getJsonData().toMap()["q0"].toMap();
            if (map.contains("result")) {
                QVariantList list = map["result"].toList();
                if (list.size() > 0) {
                    if (list[0].toMap().contains("key")) {
                        QString url = "http://en.wikipedia.org/wiki/index.html?curid="+list[0].toMap()["key"].toMap()["value"].toString();
                        QDesktopServices::openUrl(QUrl(url));
                    }
                }
            }
        }
    }
}

void DlgDemo::onItemSelected()
{
    QString str = ui->comboBoxDemo->currentText();
    qDebug() << Q_FUNC_INFO << " str=" << str;
    if (str != "Select item" && !str.isEmpty())
    {
        addMidToStack(m_mapMids[str]);
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

//    query = "{";
//    if (id.startsWith("/m/")) {
//        query += "\"mid\":\"" + id + "\"";
//    } else {
//        query += "\"id\":\"" + id + "\"";
//    }
//    type = "/event/event";
//    query += QString(",\"type\":\"%1\", \"key\":[{}], \"*\":null}").arg(type);
//    qDebug() << "QUERY: " << query;
//    list << query;


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
    strHtml += makeHtmlRecord(mapLocal,"name","Name:");
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
    strHtml += makeHtmlRecord(mapLocal,"place_of_death","Place of death:");
    strHtml += makeHtmlRecord(mapLocal,"place_of_burial","Place of burial:");
    strHtml += makeHtmlRecord(mapLocal,"cause_of_death","Cause of death:");

    //Living person
    strHtml += "<p><i><u>Person Info</u></i></p>";
    ///people/person
    mapLocal =  map["q0"].toMap()["result"].toMap();
    strHtml += makeHtmlRecord(mapLocal,"place_of_birth","Place of birth:");
    strHtml += makeHtmlRecord(mapLocal,"profession","Profession:");
    strHtml += makeHtmlRecord(mapLocal,"nationality","Nationality:");
    strHtml += makeHtmlRecord(mapLocal,"ethnicity","Ethnicity:");
    strHtml += makeHtmlRecord(mapLocal,"religion","Religion:");
    strHtml += makeHtmlRecord(mapLocal,"children","Children:");
    strHtml += makeHtmlRecord(mapLocal,"parents","Parents:");
    strHtml += makeHtmlRecord(mapLocal,"quotations","Quotations:");

    //Books author
    ///books/author
    mapLocal =  map["q2"].toMap()["result"].toMap();
    strHtml += makeHtmlRecord(mapLocal,"works_written","Works written:");

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
                    strHtml += "<p></p><ul>";
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
                    strHtml += "<p></p><ul>";
                    foreach (QVariant item, lst) {
                        strHtml += "<li><a href=\"event@"+item.toMap()["mid"].toString()+"\">" + item.toMap()["name"].toString()+ "</a></li>";
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
        addMidToStack(url.toString());
    } else if (url.toString().startsWith("event@")) {
        QStringList list = url.toString().split("@");
        getEventReferencies(list[1]);
//        addMidToStack(url.toString());
    }
}

void DlgDemo::onBack()
{
    qDebug() << Q_FUNC_INFO;
    //ui->webView->back();
    if(m_nMidPosition > 0)
    {
        //m_midsUndoStack.removeLast();
        --m_nMidPosition;
        getPersonalInfo(m_midsUndoStack[m_nMidPosition]);
    }
}

void DlgDemo::onForward()
{
    qDebug() << Q_FUNC_INFO;
    //ui->webView->back();
    if(m_nMidPosition < m_midsUndoStack.count()-1)
    {
        //m_midsUndoStack.removeLast();
        ++m_nMidPosition;
        getPersonalInfo(m_midsUndoStack[m_nMidPosition]);
    }
}

void DlgDemo::addMidToStack(const QString& mid)
{
    if(!m_midsUndoStack.isEmpty())
    {
        while(m_nMidPosition < m_midsUndoStack.count()-1)
        {
            m_midsUndoStack.removeLast();
        }
        m_midsUndoStack.append(mid);
        ++m_nMidPosition;
    }
    else
    {
        m_midsUndoStack.append(mid);
        m_nMidPosition = 0;
    }
    getPersonalInfo(m_midsUndoStack[m_nMidPosition]);
}

QString DlgDemo::makeHtmlRecord(const QVariantMap& map, const QString& key, const QString& text)
{
    QString ret;
    if (map.contains(key)) {
        QVariant::Type t = map[key].type();
        if (t == QVariant::List) {
            QVariantList lst = map[key].toList();
            if (!lst.isEmpty()) {
                ret += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    ret += "<li><b>"+item.toString()+"</b></li>";
                }
                ret += "</ul>";
            }
        } else if (t != QVariant::Invalid) {
            ret += "<p><i>" + text + " </i><b>"+map[key].toString()+"</b></p>";
        }
    }
    return ret;
}

void DlgDemo::getEventReferencies(const QString& mid)
{
    qDebug() << Q_FUNC_INFO;
    QStringList list;
    QString query = "[{";
    query += "\"mid\":\"" + mid + "\"";
    query += QString(",\"key\":{\"namespace\":\"/wikipedia/en_id\", \"value\":null}}]");
    qDebug() << "QUERY: " << query;
    list << query;

    m_state = ST_REF;
    m_pManager->runMqlQueryMultiple(list);
}
