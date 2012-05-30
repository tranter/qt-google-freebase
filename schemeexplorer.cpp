#include "schemeexplorer.h"
#include "ui_schemeexplorer.h"

#include "freebase_data_manager.h"

#include <QJson/Serializer>

#include <QAbstractTableModel>
#include <QMessageBox>

class TypeTableModel : public QAbstractTableModel
{
public:
    TypeTableModel(QObject * o = 0);

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    int columnCount(const QModelIndex & parent = QModelIndex()) const;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    QString data(int row, int column) const;
    QVariantList data(int row) const;

    void setData(const QList<QVariantList> &, int t = SchemeExplorer::DOMAINS);
    int currentDataType() const { return m_currentType; }

private:
    QList<QVariantList> m_data;
    int m_currentType;
};

TypeTableModel::
TypeTableModel(QObject * o) :
    QAbstractTableModel(o)
{
    m_currentType = SchemeExplorer::DOMAINS;
}

int TypeTableModel::
rowCount(const QModelIndex & /*parent*/) const
{
    return m_data.size();
}

int TypeTableModel::
columnCount(const QModelIndex & /*parent*/) const
{
    return m_currentType == SchemeExplorer::PROPERTIES ? 3 : 2;
}

QVariant TypeTableModel::
headerData(int section, Qt::Orientation orientation, int role) const
{
    if( orientation == Qt::Vertical || role != Qt::DisplayRole )
        return QVariant();

    switch(section)
    {
    case 0: return tr("Name");
    case 1: return tr("ID");
//    case 2: return tr("Expected type name");
//    case 3: return tr("Expected type id");
    case 2: return tr("Defualt value");
    }

    return QVariant();
}

QVariant TypeTableModel::
data(const QModelIndex & index, int role) const
{
    return ! index.isValid() || role != Qt::DisplayRole
        ? QVariant()
        : m_data[index.row()][index.column()];
}

QString TypeTableModel::
data(int row, int column) const
{
    return -1 < row && row < m_data.size() && -1 < column && column < 2
            ? m_data[row][column].toString()
            : QString();
}

QVariantList TypeTableModel::
data(int row) const
{
    return -1 < row && row < m_data.size() ? m_data[row] : QVariantList();
}

void TypeTableModel::
setData(const QList<QVariantList> & dataList, int t)
{
    beginResetModel();

    m_currentType = t;
    m_data.clear();
    m_data = dataList;

    endResetModel();
}

/********************************************************************/

SchemeExplorer::
SchemeExplorer(QWidget * p) :
    QWidget(p),
    ui(new Ui::SchemeExplorer)
{
    ui->setupUi(this);
    m_dataManager = new freebase_data_manager(this);
    m_model = new TypeTableModel(ui->tableView);

    m_selection = NONE;

    ui->tableView->setModel(m_model);

    connect(ui->tableView, SIGNAL(doubleClicked(QModelIndex)),  this, SLOT(doubleClicked(QModelIndex)));
    connect(m_dataManager, SIGNAL(sigMqlReplyReady(int)),       this, SLOT(updateData(int)));
    connect(m_dataManager, SIGNAL(sigErrorOccured(QString)),    this, SLOT(showError(QString)));

    connect(ui->previousButton, SIGNAL(clicked()), this, SLOT(previous()));
    connect(ui->closeButton,    SIGNAL(clicked()), this, SLOT(onClose()));

    loadDomains();
}

SchemeExplorer::
~SchemeExplorer()
{
    delete ui;
}

QString SchemeExplorer::
selectedId() const
{
    return m_selectedData.size() > 1 ? m_selectedData.at(1).toString() : QString();
}

void SchemeExplorer::
selectionMode(DataType type)
{
    m_selection = type;
}

void SchemeExplorer::
loadDomains()
{
    m_requestedType = DOMAINS;
    m_dataManager->runMqlQuery(
        "[{\n\t\"id\": null,\n\t\"name\": null,\n\t\"sort\": \"name\",\n\t\"type\": \"/type/domain\","
        "\n\t\"!/freebase/domain_category/domains\": {\n\t\t\"id\": \"/category/commons\"\n\t}\n}]"
    );
}

void SchemeExplorer::
loadTypes(const QString & id)
{
    m_currentId = id;

    m_requestedType = TYPES;
    m_dataManager->runMqlQuery(
        QString(
            "[{ \"id\": null, \"name\": null, \"sort\": \"name\", "
            "\"type\": \"/type/type\", \"domain\": \"%1\" }]"
        ).arg( id )
    );
}

void SchemeExplorer::
loadProperties(const QString & id)
{
    m_currentId = id;

    m_requestedType = PROPERTIES;
    QVariantMap query;
    query["id"]   = id;
    query["type"] = "/type/type";

    QVariantMap properties;
    properties["id"]   = QVariant();
    properties["name"] = QVariant();

    QVariantMap expected_type;
    expected_type["default_property"] = QVariant();
    expected_type["id"]   = QVariant();
    expected_type["name"] = QVariant();

    properties["expected_type"] = expected_type;

    query["properties"] = QVariantList() << properties;

    m_dataManager->runMqlQuery( QJson::Serializer().serialize(query) );
}

void SchemeExplorer::
updateData(int r)
{
    //qDebug() << m_dataManager->getJsonData();

    ui->previousButton->setEnabled( m_requestedType != DOMAINS );
    QList<QVariantList> dataList;

    if( m_requestedType != PROPERTIES )
    {
        foreach(const QVariant & v, m_dataManager->getJsonData().toMap()["q0"].toMap()["result"].toList())
        {
            QVariantMap map( v.toMap() );
            dataList.append( QVariantList() << map["name"] << map["id"] );
        }
    } else {

//        {
//          "code": "/api/status/ok",
//          "q0": {
//                "code": "/api/status/ok",
//                "result": {
//                      "id": "/music/artist",
//                      "properties": [
//                            {
//                                  "expected_type": {
//                                        "default_property": null,
//                                        "id": "/location/location",
//                                        "name": "Location"
//                                      },
//                                  "id": "/music/artist/origin",
//                                  "name": "Place Musical Career Began"
//                                },

        QVariantMap result( m_dataManager->getJsonData().toMap()["q0"].toMap()["result"].toMap() );
        m_currentId = result["id"].toString();

        QVariantList properties( result["properties"].toList());

        foreach(const QVariant & v, properties)
        {
            QVariantMap map(v.toMap());
            QVariantList list;
            list << map["name"] << map["id"];

            map = map["expected_type"].toMap();
//            list << map["name"] << map["id"];

            QVariant defaultValue( map["default_property"] );
            list << (defaultValue.isNull() ? QVariant("null") : defaultValue);

            dataList.append( list );
        }
    }

    m_model->setData( dataList, m_requestedType );
    setEnabled(true);
}

void SchemeExplorer::
doubleClicked(const QModelIndex & index)
{

    if( m_requestedType == m_selection )
    {
        setEnabled(false);

        QModelIndexList list( ui->tableView->selectionModel()->selectedIndexes() );
        if( list.isEmpty() ) return;

        QString id = m_model->data(list.first().row(), 1);
        if( id.isEmpty() ) return;

        m_selectedData = m_model->data(index.row());
        emit idSelected( m_selectedData.at(1).toString() );
        return;

    } else if( m_requestedType == PROPERTIES )
        return;

    setEnabled(false);

    switch(m_model->currentDataType())
    {
    case DOMAINS: loadTypes( m_model->data(index.row(), 1) ); break;
    case TYPES:   loadProperties( m_model->data(index.row(), 1) );
    }
}

void SchemeExplorer::
showError(const QString & error)
{
    QMessageBox::warning(this, tr("Error"), error);
}

void SchemeExplorer::
previous()
{
    if( m_requestedType == DOMAINS )
        return;

    setEnabled(false);

    switch(m_requestedType)
    {
    case PROPERTIES: loadTypes( m_currentId.left(m_currentId.indexOf('/', 1)) ); break;
    case TYPES:      loadDomains();
    }
}

void SchemeExplorer::
onClose()
{
    close();
    emit closeClicked();
}
