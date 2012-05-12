#include "treejsonmodel.h"

#include "treejsonitem.h"
#include "treejsonmodel.h"

TreeJsonModel::TreeJsonModel(const QVariant& data, QObject* parent) : QAbstractItemModel(parent)
{
    QList<QVariant> rootData;
    rootData << "Key" << "Value";
    m_rootItem = new TreeJsonItem(rootData);
    setupModelData(data, m_rootItem);
}

TreeJsonModel::~TreeJsonModel()
{
    delete m_rootItem;
}

int TreeJsonModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return static_cast<TreeJsonItem*>(parent.internalPointer())->columnCount();
    else
        return m_rootItem->columnCount();
}

QVariant TreeJsonModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    TreeJsonItem *item = static_cast<TreeJsonItem*>(index.internalPointer());

    return item->data(index.column());
}

Qt::ItemFlags TreeJsonModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant TreeJsonModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return m_rootItem->data(section);

    return QVariant();
}

QModelIndex TreeJsonModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeJsonItem *parentItem;

    if (!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = static_cast<TreeJsonItem*>(parent.internalPointer());

    TreeJsonItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex TreeJsonModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeJsonItem *childItem = static_cast<TreeJsonItem*>(index.internalPointer());
    TreeJsonItem *parentItem = childItem->parent();

    if (parentItem == m_rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}
/** \brief Return children list count of the parent index
 *
 */
int TreeJsonModel::rowCount(const QModelIndex &parent) const
{
    TreeJsonItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = static_cast<TreeJsonItem*>(parent.internalPointer());

    return parentItem->childCount();
}

void TreeJsonModel::setNewModelData(const QVariant& newdata, const QModelIndex& index)
{
    TreeJsonItem* pNode = index.isValid() ? static_cast<TreeJsonItem*>(index.internalPointer()) : NULL;
    if (pNode) {
        beginResetModel();
        pNode->clearChildren();
        setupModelData(newdata,pNode);
        endResetModel();
    } else {
        beginResetModel();
        m_rootItem->clearChildren();
        setupModelData(newdata,m_rootItem);
        endResetModel();
    }
}

void TreeJsonModel::setupModelData(const QVariant& newdata, TreeJsonItem* parent)
{
    QMetaType::Type type = (QMetaType::Type)newdata.type();

    if (type == QMetaType::QVariantMap) {
        QVariantMap map = newdata.toMap();
        QVariantMap::const_iterator it;
        for (it = map.constBegin(); it != map.constEnd(); it++) {
            type = (QMetaType::Type)it.value().type();
            if (type == QMetaType::QString || type == QMetaType::ULongLong) {
                QList<QVariant> value;
                value << it.key() << it.value();
                TreeJsonItem* item = new TreeJsonItem(value,parent);
                parent->appendChild(item);
            } else if (type == QMetaType::QVariantMap) {
                QList<QVariant> value;
                value << it.key() << "{";
                TreeJsonItem* item = new TreeJsonItem(value,parent);
                parent->appendChild(item);
                setupModelData(it.value(),item);
                item->setData(1,QString("{%1}").arg(item->childCount()));
            } else if (type == QMetaType::QVariantList) {
                QList<QVariant> value;
                value << it.key() << "[";
                TreeJsonItem* item = new TreeJsonItem(value,parent);
                parent->appendChild(item);
                setupModelData(it.value(),item);
                item->setData(1,QString("[%1]").arg(item->childCount()));
            }
        }
    } else if (type == QMetaType::QVariantList) {
        QVariantList list = newdata.toList();
        int len = list.size();
        for (int i=0;i<len;i++) {
            type = (QMetaType::Type)list.value(i).type();
            if (type == QMetaType::QString || type == QMetaType::ULongLong) {
                QList<QVariant> value;
                value << i << list.value(i);
                TreeJsonItem* item = new TreeJsonItem(value,parent);
                parent->appendChild(item);
            } else if (type == QMetaType::QVariantMap) {
                QList<QVariant> value;
                value << i << "{";
                TreeJsonItem* item = new TreeJsonItem(value,parent);
                parent->appendChild(item);
                setupModelData(list.value(i),item);
                item->setData(1,QString("{%1}").arg(item->childCount()));
            } else if (type == QMetaType::QVariantList) {
                QList<QVariant> value;
                value << i << "[";
                TreeJsonItem* item = new TreeJsonItem(value,parent);
                parent->appendChild(item);
                setupModelData(list.value(i),item);
                item->setData(1,QString("[%1]").arg(item->childCount()));
            }
        }
    }
}

void TreeJsonModel::clear()
{
    beginResetModel();
    m_rootItem->clearChildren();
    endResetModel();
}
