#ifndef FREEBASEEXPLORER_H
#define FREEBASEEXPLORER_H

#include "simplesearcher.h"

class FreebaseExplorer : public SimpleSearcher
{
    Q_OBJECT
public:
    explicit FreebaseExplorer(QWidget *parent = 0);
    
private slots:
    void getSchema(const QString & id);

private:
    bool delegatedRequest(const QVariantMap & );
    QString createSearchLink(QString type, QString typeName, QString text);
    QString createHtml(const QVariantMap & );
    void getInfo(const QString & id, const QString & type);
    void onLinkClicked(const QUrl &);

private:
    bool m_gettingScheme;

    struct Tuple
    {
        QString name;
        QString property;
        QString type_id;
        QString type_name;
    };

    QString m_currentSchemaTypeId;
    QList<Tuple> schema;
};

#endif // FREEBASEEXPLORER_H
