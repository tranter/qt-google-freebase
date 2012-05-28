#ifndef COUNTRYSEARCHER_H
#define COUNTRYSEARCHER_H

#include "simplesearcher.h"

class CountrySearcher : public SimpleSearcher
{
public:
    CountrySearcher(QWidget * parent = 0);

private:
    QString getCurrentType() const;
    QString createHtml(const QVariantMap & map);
    void getInfo(const QString & id, const QString & /*type = "/location/country"*/);
};

#endif // COUNTRYSEARCHER_H
