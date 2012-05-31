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
    QString createHtml(const QVariantMap & );
    void getInfo(const QString & id, const QString & type);

private:
    bool m_gettingScheme;

    struct Tuple {
        QString name;
        QString property;

        Tuple() {}
        Tuple(QString n, QString p) :
            name(n), property(p) {}
    };

    QList<Tuple> schema;
};

#endif // FREEBASEEXPLORER_H
