#ifndef TREEJSONITEM_H
#define TREEJSONITEM_H

#include <QList>
#include <QVariant>

class TreeJsonItem
{
public:
    TreeJsonItem(const QList<QVariant>& data, TreeJsonItem* parent = 0);
    ~TreeJsonItem();

    void clear();
    void clearChildren();
    void appendChild(TreeJsonItem* child);
    TreeJsonItem* child(int row);

    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    void setData(int column, const QVariant& data);
    int row() const;
    TreeJsonItem* parent();

private:
    QList<TreeJsonItem*> m_itemChildren;
    QList<QVariant> m_itemData;
    TreeJsonItem* m_itemParent;
};

#endif // TREEJSONITEM_H
