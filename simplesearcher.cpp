#include "simplesearcher.h"
#include "ui_simplesearcher.h"

#include "freebase_data_manager.h"

#include <QJson/Serializer>

#include <QNetworkReply>
#include <QDebug>

SimpleSearcher::SimpleSearcher(QWidget *p) :
    QWidget(p),
    ui(new Ui::SimpleSearcher),
    m_historyPos(-1),
    m_delegateMQLrequest(false),
    m_history(),
    m_pManager(new freebase_data_manager(this))
{
    ui->setupUi(this);

    ui->webView->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);

    connect(m_pManager,SIGNAL(sigJsonReady(int)),this,SLOT(onJsonReady(int)));

    connect(ui->webView->page()->networkAccessManager(),
              SIGNAL(sslErrors(QNetworkReply*, const QList<QSslError> & )),
              this,
              SLOT(sslErrorHandler(QNetworkReply*, const QList<QSslError> & )));
    connect(ui->webView,SIGNAL(linkClicked(QUrl)),this,SLOT(onLinkClicked(QUrl)));

    connect(ui->prevPageButton, SIGNAL(clicked()), this, SLOT(previousPage()));
    connect(ui->nextPageButton, SIGNAL(clicked()), this, SLOT(nextPage()));
    connect(ui->findButton,     SIGNAL(clicked()), this, SLOT(search()));
    connect(ui->searchLineEdit, SIGNAL(returnPressed()), this, SLOT(search()));
    connect(ui->resultComboBox, SIGNAL(activated(int)), this, SLOT(showPosition(int)));

    //ui->typeComboBox->addItem(tr("Person (/people/person)"), "/people/person");
    //showTypeWidgets(false);
}

SimpleSearcher::~SimpleSearcher()
{
    delete ui;
}

void SimpleSearcher::showTypeWidgets(bool v)
{
    ui->labelType->setVisible(v);
    ui->typeComboBox->setVisible(v);
    ui->addTypeButton->setVisible(v);
    ui->findButton->setToolButtonStyle(v ? Qt::ToolButtonTextUnderIcon : Qt::ToolButtonTextBesideIcon);
}

void SimpleSearcher::onJsonReady(int rt)
{
    qDebug() << Q_FUNC_INFO << " rt=" << rt;

    QVariantMap jsonMap( m_pManager->getJsonData().toMap() );

    if (rt == freebase_data_manager::REQ_SEARCH)
    {
        ui->resultComboBox->clear();

        QString mapKey;
        QVariantList list = jsonMap.contains("result") ? jsonMap["result"].toList() : QVariantList();

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
                ui->resultComboBox->addItem( mapKey, map["mid"] );
            }
        }

        //if( jsonMap.contains("hits") )
        //{
            //m_resultsCount = jsonMap["hits"].toInt();
        //} else m_resultsCount = 0;

        if( list.size() )
            showPosition(0);
    }
    else if (rt == freebase_data_manager::REQ_MQL)
    {
        qDebug() << "Some info from MQL arrived";

        if(m_delegateMQLrequest)
        {
            delegatedRequest(jsonMap);
            return;
        }

        ui->webView->setHtml( createHtml(jsonMap) );
    }
}

void SimpleSearcher::on_addTypeButton_clicked()
{
    qDebug() << "Not implemented";
}

void SimpleSearcher::previousPage()
{
    --m_historyPos;

    getInfo(
        m_history[m_historyPos].first,
        getCurrentType()
    );
    int pos = m_history[m_historyPos].second;
    if( -1 < pos && pos < ui->resultComboBox->count() )
        ui->resultComboBox->setCurrentIndex(pos);
    else
        ui->resultComboBox->setCurrentIndex(-1);

    ui->prevPageButton->setEnabled( 0 < m_historyPos );
    ui->nextPageButton->setEnabled(true);
}

void SimpleSearcher::nextPage()
{
    ++m_historyPos;

    getInfo(
        m_history[m_historyPos].first,
        getCurrentType()
    );
    int pos = m_history[m_historyPos].second;
    if( -1 < pos && pos < ui->resultComboBox->count() )
        ui->resultComboBox->setCurrentIndex(pos);
    else
        ui->resultComboBox->setCurrentIndex(-1);

    ui->nextPageButton->setEnabled( m_historyPos < m_history.count()-1 );
    ui->prevPageButton->setEnabled(true);
}

void SimpleSearcher::sslErrorHandler(QNetworkReply* qnr, const QList<QSslError> & /*errlist*/)
{

  #if DEBUG_BUYIT
  qDebug() << "---frmBuyIt::sslErrorHandler: ";
  // show list of all ssl errors
  foreach (QSslError err, errlist)
    qDebug() << "ssl error: " << err;
  #endif

   qnr->ignoreSslErrors();
}

void SimpleSearcher::search()
{
    qDebug() << Q_FUNC_INFO;
    QString query = ui->searchLineEdit->text().trimmed();
    ui->webView->setHtml("<html/>");

    if( ! query.isEmpty() )
    {
        m_historyPos = -1;
        m_history.clear();

        QString type = getCurrentType();
        m_pManager->runSearchQuery(query+"&type="+type, ui->limitComboBox->currentText(), "0");
    }
}

void SimpleSearcher::showPosition(int pos)
{
    qDebug() << Q_FUNC_INFO;

    appendToHistory(ui->resultComboBox->itemData(pos).toString(), pos);

    getInfo(
        m_history.last().first,
        getCurrentType()
    );
}

QString SimpleSearcher::getCurrentType() const
{
    return ui->typeComboBox->itemData( ui->typeComboBox->currentIndex() ).toString();
}

QString SimpleSearcher::createHtml(const QVariantMap & /*map*/)
{
    return "<html/>";
}

void SimpleSearcher::getInfo(const QString & id, const QString & type)
{
    QVariantMap query;
    if (id.startsWith("/m/"))
        query["mid"] = id;
    else
        query["id"] = id;

    query["type"] = type;
    query["key"] = QVariantList();
    query["*"] = QVariant();

    m_pManager->runMqlQueryMultiple( QStringList( QJson::Serializer().serialize(query) ) );
}

QWebView * SimpleSearcher::webView() const { return ui->webView; }


void SimpleSearcher::appendToHistory(const QString & value, int pos)
{
    if( m_historyPos != m_history.count()-1 )
    {
        int count = m_history.count()-m_historyPos;
        for(int i(0); i < count; ++i)
            m_history.removeLast();
    }

    m_history << HistoryNode(value, pos);
    m_historyPos = m_history.count()-1;
    ui->prevPageButton->setEnabled(m_historyPos);
    ui->nextPageButton->setEnabled(false);
}
