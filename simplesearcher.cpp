#include "simplesearcher.h"
#include "ui_simplesearcher.h"
#include "simplesearcherhistory.h"

#include "freebase_data_manager.h"
#include "schemeexplorerdialog.h"

#include <QJson/Serializer>

#include <QNetworkReply>
#include <QDebug>

#include <QSettings>
#include <QDesktopServices>
#include <QKeyEvent>



SimpleSearcher::SimpleSearcher(QWidget *p) :
    QWidget(p),
    ui(new Ui::SimpleSearcher),
    m_delegateMQLrequest(false),
    m_history(new SimpleSearcherHistory(this)),
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

    connect(ui->prevPageButton,     SIGNAL(clicked()),  m_history, SLOT(loadPreviousData()));
    connect(ui->nextPageButton,     SIGNAL(clicked()),  m_history, SLOT(loadNextData()));

    connect(ui->findButton,         SIGNAL(clicked()),          this,       SLOT(search()));
    connect(ui->searchLineEdit,     SIGNAL(returnPressed()),    this,       SLOT(search()));

    connect(ui->resultComboBox,     SIGNAL(currentIndexChanged(int)),   this, SLOT(showPosition(int)));

    connect(ui->schemeTypeButton,   SIGNAL(clicked()),                  this, SLOT(addSchemeType()));
    connect(ui->typeComboBox,       SIGNAL(currentIndexChanged(int)),   this, SLOT(onCurrentTypeChanged(int)));
}

SimpleSearcher::~SimpleSearcher()
{
    delete ui;
}

void SimpleSearcher::showTypeWidgets(bool v)
{
    ui->labelType->setVisible(v);
    ui->typeComboBox->setVisible(v);
    ui->schemeTypeButton->setVisible(v);
    ui->findButton->setToolButtonStyle(v ? Qt::ToolButtonTextUnderIcon : Qt::ToolButtonTextBesideIcon);
}

void SimpleSearcher::onLinkClicked(const QUrl & url)
{
    qDebug() << Q_FUNC_INFO << " url=" << url;
    if( url.toString().contains("http://") )
        QDesktopServices::openUrl(url);
}

void SimpleSearcher::setAwaitingMode()
{
    setEnabled(false);
    QApplication::setOverrideCursor(Qt::WaitCursor);
}

void SimpleSearcher::dropAwaitingMode()
{
    QApplication::restoreOverrideCursor();
    setEnabled(true);
}

void SimpleSearcher::onJsonReady(int rt)
{
    QVariantMap jsonMap( m_pManager->getJsonData().toMap() );

    if (rt == freebase_data_manager::REQ_SEARCH)
    {
        ui->resultComboBox->blockSignals(true);
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
        ui->resultComboBox->blockSignals(false);

        if( list.size() )
            showPosition(0);
    }
    else if (rt == freebase_data_manager::REQ_MQL)
    {
        qDebug() << "Some info from MQL arrived";

        if( m_delegateMQLrequest && delegatedRequest(jsonMap) )
        {
            dropAwaitingMode();
            return;
        }

        ui->webView->setHtml( createHtml(jsonMap) );

        m_history->saveData();
    }

    dropAwaitingMode();
}

void SimpleSearcher::addSchemeType()
{
    SchemeExplorerDialog dialog(this);
    dialog.setWindowTitle("Add scheme type");
    dialog.setSelectionMode(SchemeExplorer::TYPES);
    dialog.resize(600, 400);

    if( dialog.exec() != QDialog::Accepted )
        return;

    QVariantList list( dialog.selectedData() );

    if( list.size() < 2 )
        return;

    addSchemeType( list.at(0).toString(), list.at(1), false );
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
    QString query = ui->searchLineEdit->text().trimmed();
    QString type = getCurrentType();

    ui->webView->setHtml("<html/>");

    if( query.isEmpty() || type.isEmpty() )
        return;

    setAwaitingMode();
    m_pManager->runSearchQuery(query+"&type="+type, ui->limitComboBox->currentText(), "0");
}

void SimpleSearcher::showPosition(int pos)
{
    getInfo(
        ui->resultComboBox->itemData(pos).toString(),
        getCurrentType()
    );
}

void SimpleSearcher::onCurrentTypeChanged(int idx)
{
    emit currentSchemaTypeChanged( ui->typeComboBox->itemData(idx).toString() );
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
    setAwaitingMode();

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

int SimpleSearcher::addItemToResultsList(const QString & item, const QVariant & itemData, bool blockSignals)
{
    int pos = ui->resultComboBox->count();
    if(blockSignals) ui->resultComboBox->blockSignals(true);
    ui->resultComboBox->addItem(item, itemData);
    ui->resultComboBox->setCurrentIndex(pos);
    if(blockSignals) ui->resultComboBox->blockSignals(false);
    return pos;
}

void SimpleSearcher::addSchemeType(const QString & item, const QVariant & itemData, bool blockSignals)
{
    int count = ui->typeComboBox->count();
    for(int i(0); i < count; ++i)
    {
        if( ui->typeComboBox->itemData(i).toString() == itemData )
        {
            if( blockSignals )
            {
                ui->typeComboBox->blockSignals(true);
                ui->typeComboBox->setCurrentIndex(i);
                ui->typeComboBox->blockSignals(false);
            }
            else {
                if( i == ui->typeComboBox->currentIndex() )
                    // we want notify changes cause blockSignals == false
                    onCurrentTypeChanged(i);
                else
                    ui->typeComboBox->setCurrentIndex(i);
            }

            return;
        }
    }

    if( blockSignals ) ui->typeComboBox->blockSignals(true);

    ui->typeComboBox->addItem( QString("%1 (%2)").arg( item, itemData.toString()), itemData);
    ui->typeComboBox->setItemData(count, item, Qt::UserRole+1);
    ui->typeComboBox->setCurrentIndex(count);

    if( blockSignals ) ui->typeComboBox->blockSignals(false);
}

void SimpleSearcher::setSearchText(const QString & text)
{
    ui->searchLineEdit->setText(text);
}

void SimpleSearcher::writeSettings(const QString & companyName, const QString & appName)
{
    QSettings settings(companyName, appName);
    settings.beginGroup( objectName() );

    int count = ui->typeComboBox->count();

    QStringList items;
    for(int i( 10 < count ? count-10 : 0 ); i < count; ++i)
    {
        items << ui->typeComboBox->itemData(i,Qt::UserRole+1).toString();
        items << ui->typeComboBox->itemData(i).toString();
    }

    settings.setValue("items", items);

    settings.endGroup();
}

void SimpleSearcher::readSettings(const QString & companyName, const QString & appName)
{
    QSettings settings(companyName, appName);
    settings.beginGroup( objectName() );

    QStringList items = settings.value("items").toStringList();

    if( items.isEmpty() || items.count() & 1 )
        return;

    int count = items.size();
    for(int i(0); i < count; i += 2)
        addSchemeType( items.at(i), items.at(i+1) );

    settings.endGroup();
}
