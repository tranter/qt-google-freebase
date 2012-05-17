#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

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

    m_pScene = new QGraphicsScene();
    ui->graphicsViewDemo->setScene(m_pScene);

    connect(ui->pushButtonGoDemo,SIGNAL(clicked()),this,SLOT(startSearch()));
    connect(ui->comboBoxDemo,SIGNAL(currentIndexChanged(QString)),this,SLOT(onItemSelected(QString)));
    connect(m_pManager,SIGNAL(sigJsonReady(int)),this,SLOT(onJsonReady(int)));
    connect(m_pManager,SIGNAL(sigImageReady(QPixmap,int)),this,SLOT(onImageReady(QPixmap,int)));
//    connect(m_pManager,SIGNAL(sigMqlReplyReady()),this,SLOT(onMqlReady(int)));
}

DlgDemo::~DlgDemo()
{
    delete m_pScene;
    delete m_pManager;
    delete ui;
}

void DlgDemo::startSearch()
{
    qDebug() << Q_FUNC_INFO;
    QString query = ui->lineEditInputDemo->text();
    if (!query.isEmpty()) {
        clearOldData();
        m_pManager->runSearchQuery(query+"&type=/people/person",ui->comboBoxLimit->currentText(),ui->lineEditStart->text());
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
            }
        }
        ui->comboBoxDemo->addItem("Select item");
        ui->comboBoxDemo->addItems(m_mapMids.keys());
        ui->comboBoxDemo->setMaxCount(m_mapMids.keys().size()+1);
        ui->labelCount->setText(m_pManager->getJsonData().toMap().contains("hits") ?
                                    m_pManager->getJsonData().toMap()["hits"].toString() : "No Items"
                                    );
    } else if (rt == REQ_MQL) {
        qDebug() << "Some info from MQL arrived";
        QVariantMap map = m_pManager->getJsonData().toMap()["result"].toMap();
        if (map.contains("name")) {
            qDebug() << "name=" << map["name"].toString();
            ui->labelName->setText(map["name"].toString());
        }
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
    int height = ui->graphicsViewDemo->size().height();
    int width = ui->graphicsViewDemo->size().width();
    clearReplyImage();
    m_pManager->runImageQuery(mid,height,width);
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
    QList<QGraphicsItem*> list = ui->graphicsViewDemo->scene()->items();
    foreach (QGraphicsItem* p, list) {
        ui->graphicsViewDemo->scene()->removeItem(p);
    }
}

void DlgDemo::onImageReady(const QPixmap& image, const int rt)
{
    qDebug() << Q_FUNC_INFO;
    QGraphicsPixmapItem *item = new QGraphicsPixmapItem(image);
    m_pScene->addItem(item);
    ui->graphicsViewDemo->fitInView(item,Qt::KeepAspectRatio);
}

void DlgDemo::clearOldData()
{
    ui->labelCount->clear();
    ui->labelName->setText("...");
    ui->comboBoxDemo->clear();
    clearReplyImage();
}
