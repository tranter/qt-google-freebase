#ifndef PEOPLESEARCHER_H
#define PEOPLESEARCHER_H

#include "simplesearcher.h"

class PeopleSearcher : public SimpleSearcher
{
public:
    explicit PeopleSearcher(QWidget * parent = 0);

private:
    QString createHtml(const QVariantMap & map);
    void getInfo(const QString & id, const QString & /*type = "/people/person"*/);
    bool delegatedRequest(const QVariantMap & );
    void onLinkClicked(const QUrl &);
};

#endif // PEOPLESEARCHER_H
