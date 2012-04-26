#include <QStringList>

#include "treeitem.h"

TreeItem::TreeItem(const QList<QVariant> &data, TreeItem *parent)
{
    m_pParent = parent;
    m_itemData = data;
}

TreeItem::~TreeItem()
 {
     qDeleteAll(m_rowChildren);
 }

 void TreeItem::appendChild(TreeItem *item)
 {
     m_rowChildren.append(item);
 }

 TreeItem *TreeItem::child(int row)
 {
     return m_rowChildren.value(row);
 }

 int TreeItem::childCount() const
 {
     return m_rowChildren.count();
 }

 int TreeItem::columnCount() const
 {
//     return m_itemData.count();
     return 0;
 }

 QVariant TreeItem::data(int column) const
 {
//     return m_itemData.value(column);
     return QVariant();
 }

 TreeItem *TreeItem::parent()
 {
     return m_pParent;
 }

 int TreeItem::row() const
 {
     if (m_pParent)
         return m_pParent->m_rowChildren.indexOf(const_cast<TreeItem*>(this));

     return 0;
 }

 void TreeItem::setItemData(const QList<QVariant>& data)
 {
     m_itemData = data;
 }
