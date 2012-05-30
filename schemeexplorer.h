#ifndef SCHEMEEXPLORER_H
#define SCHEMEEXPLORER_H

#include <QWidget>

#include <QModelIndex>

namespace Ui {
class SchemeExplorer;
}

class SchemeExplorer : public QWidget
{
    Q_OBJECT
    
public:
    enum DataType { NONE, DOMAINS, TYPES, PROPERTIES };

    explicit SchemeExplorer(QWidget *parent = 0);
    ~SchemeExplorer();
    
    QString selectedId() const;
    QVariantList selectedData() const { return m_selectedData; }

public slots:
    void selectionMode(DataType type = NONE);

    void loadDomains();
    void loadTypes(const QString & domain);
    void loadProperties(const QString & type);

signals:
    void idSelected(const QString &);
    void closeClicked();

private slots:
    void updateData(int);
    void doubleClicked(const QModelIndex &);
    void showError(const QString &);
    void previous();
    void onClose();

private:
    Ui::SchemeExplorer * ui;
    class freebase_data_manager * m_dataManager;
    class TypeTableModel * m_model;

    int m_requestedType;
    DataType m_selection;
    QString m_currentId;

    QVariantList m_selectedData;
};

#endif // SCHEMEEXPLORER_H
