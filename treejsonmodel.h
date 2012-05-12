#ifndef TREEJSONMODEL_H
#define TREEJSONMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

class TreeJsonItem;

class TreeJsonModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    TreeJsonModel(const QVariant& data, QObject* parent = 0);
    ~TreeJsonModel();

    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    void setNewModelData(const QVariant& newdata, const QModelIndex& index = QModelIndex());
    void clear();

private:
    void setupModelData(const QVariant& newdata, TreeJsonItem* parent);

    TreeJsonItem* m_rootItem;
};

#endif // TREEJSONMODEL_H
