#include "treejsonitem.h"

TreeJsonItem::TreeJsonItem(const QList<QVariant>& data, TreeJsonItem* parent)
{
    m_itemParent = parent;
    m_itemData = data;
}

TreeJsonItem::~TreeJsonItem()
{
    clear();
}

void TreeJsonItem::clear()
{
    foreach (TreeJsonItem* pItem,m_itemChildren) {
        delete pItem;
    }
    m_itemChildren.clear();
    m_itemData.clear();
}

void TreeJsonItem::clearChildren()
{
    foreach (TreeJsonItem* pItem,m_itemChildren) {
        delete pItem;
    }
    m_itemChildren.clear();
}

void TreeJsonItem::appendChild(TreeJsonItem* item)
{
    m_itemChildren.append(item);
}

TreeJsonItem* TreeJsonItem::child(int row)
{
    return m_itemChildren.value(row);
}

int TreeJsonItem::childCount() const
{
    return m_itemChildren.count();
}

int TreeJsonItem::columnCount() const
{
    return m_itemData.count();
}

QVariant TreeJsonItem::data(int column) const
{
    return m_itemData.value(column);
}

void TreeJsonItem::setData(int column, const QVariant& data)
{
    if (column >= 0 && column < m_itemData.size()) {
        m_itemData[column] = data;
    }
}

TreeJsonItem* TreeJsonItem::parent()
{
    return m_itemParent;
}
/** \brief Return row index of current node in it's parent children list
 *
 */
int TreeJsonItem::row() const
{
    if (m_itemParent)
        return m_itemParent->m_itemChildren.indexOf(const_cast<TreeJsonItem*>(this));

    return 0;
}
