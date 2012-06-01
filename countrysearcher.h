#ifndef COUNTRYSEARCHER_H
#define COUNTRYSEARCHER_H

#include "simplesearcher.h"

class CountrySearcher : public SimpleSearcher
{
public:
    CountrySearcher(QWidget * parent = 0);

private:
    QString createHtml(const QVariantMap & map);

};

#endif // COUNTRYSEARCHER_H
