#include "simplesearcher.h"
#include "ui_simplesearcher.h"

#include "freebase_data_manager.h"

#include <QJson/Serializer>

#include <QDebug>

SimpleSearcher::SimpleSearcher(QWidget *p) :
    QWidget(p),
    ui(new Ui::SimpleSearcher),
    m_currentResults(0),
    m_resultsCount(0),
    m_historyPos(-1),
    m_history(),
    m_pManager(new freebase_data_manager(this))
{
    ui->setupUi(this);
    connect(m_pManager,SIGNAL(sigJsonReady(int)),this,SLOT(onJsonReady(int)));

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
        ui->prevButton->setEnabled(false);
        ui->forwButton->setEnabled(false);
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

        if( jsonMap.contains("hits") )
        {
            m_resultsCount = jsonMap["hits"].toInt();

            ui->forwButton->setEnabled(
                m_currentResults + ui->limitComboBox->currentText().toInt() < m_resultsCount
            );
            ui->prevButton->setEnabled(
                0 <= m_currentResults - ui->limitComboBox->currentText().toInt()
            );
        } else m_resultsCount = 0;

        if( 0 < m_resultsCount )
            showPosition(0);
    }
    else if (rt == freebase_data_manager::REQ_MQL)
    {
        qDebug() << "Some info from MQL arrived";
        ui->webView->setHtml( createHtml(jsonMap) );
    }
}

void SimpleSearcher::on_addTypeButton_clicked()
{
    qDebug() << "Not implemented";
}

void SimpleSearcher::on_backButton_clicked()
{
    --m_historyPos;

    getInfo(
        m_history[m_historyPos].first,
        getCurrentType()
    );
    ui->resultComboBox->setCurrentIndex(m_history[m_historyPos].second);

    ui->backButton->setEnabled( 0 < m_historyPos );
    ui->nextButton->setEnabled(true);
}

void SimpleSearcher::on_nextButton_clicked()
{
    ++m_historyPos;

    getInfo(
        m_history[m_historyPos].first,
        getCurrentType()
    );
    ui->resultComboBox->setCurrentIndex(m_history[m_historyPos].second);

    ui->nextButton->setEnabled( m_historyPos < m_history.count()-1 );
    ui->backButton->setEnabled(true);
}

void SimpleSearcher::search(SearchSwitch s)
{
    qDebug() << Q_FUNC_INFO;
    QString query = ui->searchLineEdit->text().trimmed();
    ui->webView->setHtml("<html/>");

    if( ! query.isEmpty() )
    {
        int limit = ui->limitComboBox->currentText().toInt();
        switch(s)
        {
        case NEW:
            m_historyPos = -1;
            m_history.clear();
            m_currentResults = 0;
            break;

        case FORWARD:
            if( limit + m_currentResults < m_resultsCount )
                m_currentResults += limit;
            ui->forwButton->setEnabled( m_currentResults + limit < m_resultsCount );
            break;

        case PREVIOUS:
            if( 0 <= m_currentResults - limit )
                m_currentResults -= limit;
            ui->prevButton->setEnabled( 0 <= m_currentResults - limit );
            break;
        // ...
        }

        qDebug() << m_resultsCount << m_currentResults << limit;

        QString type = getCurrentType();
        m_pManager->runSearchQuery(query+"&type="+type, ui->limitComboBox->currentText(), QString::number(m_currentResults));
    }
}

void SimpleSearcher::showPosition(int pos)
{
    qDebug() << Q_FUNC_INFO;

    m_history << QPair<QString, int>(ui->resultComboBox->itemData(pos).toString(), pos);
    m_historyPos = m_history.count()-1;
    ui->backButton->setEnabled(m_historyPos);
    ui->nextButton->setEnabled(false);

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

