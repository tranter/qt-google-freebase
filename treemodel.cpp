#include <QtGui>

#include "treeitem.h"
#include "treemodel.h"

TreeModel::TreeModel(const QVariant &data, QObject *parent) : QAbstractItemModel(parent)
{
    QList<QVariant> rootData;
//    rootData << "Title" << "Summary";
    rootItem = new TreeItem(rootData);
    setupModelData(data, rootItem);
}

TreeModel::~TreeModel()
{
    delete rootItem;
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    return item->data(index.column());
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent)
            const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

void TreeModel::setupModelData(const QVariant& data, TreeItem *parent)
{
//    QMetaType::Type type = (QMetaType::Type)data.type();
//    TreeItem* itemCurrent = new TreeItem(*(new QList<QVariant>()),parent);
//    QList<QVariant> listData;

//    if (type == QMetaType::QVariant) {
//        listData.append(data);
//        itemCurrent->setItemData(listData);
//        parent->appendChild(itemCurrent);
//    } else if (type == QMetaType::QVariantList) {
//        QList<QVariant> list = data.toList();
//        foreach (QVariant var,list) {
//            if (QMetaType::QVariant == (QMetaType::Type)var.type()) {
//                listData.append(var);
//            } else {
//                setupModelData(var,itemCurrent);
//            }
//        }
//        itemCurrent->setItemData(listData);
//        parent->appendChild(itemCurrent);
//    } else if (type == QMetaType::QVariantMap) {
//        QVariantMap map = data.toMap();
//        QVariantMap::const_iterator it;
//        for (it=map.constBegin();it<map.end();it++) {
//            if (QMetaType::QVariant == (QMetaType::Type)(it->type())) {
//                listData.append(it->key);
//            } else {
//                setupModelData(var,itemCurrent);
//            }
//        }
//    }
}
