#include "htmlgenerators.h"

#include <QDebug>
#include <QDate>

QString HtmlGenerators::createHtmlForPerson(const QVariantMap& map)
{
    QString strHtml = QString("<html><body>");
    QVariantList lst;
    QVariantMap mapLocal =  map["q0"].toMap()["result"].toMap();
    QString key, text;
    strHtml += QString("<img src=\"https://usercontent.googleapis.com/freebase/v1-sandbox/image%1?maxheight=400&maxwidth=200\" align=\"right\">")
            .arg(mapLocal["mid"].toString());

    // Referencies
    QString s = findNamespaceValue("/wikipedia/en_id",map);
    if (!s.isEmpty()) {
        strHtml += QString("<a href=\"http://www.freebase.com/view%1\"><img src=\"http://www.freebase.com/favicon.ico\" alt=\"Freebase\" hspace=\"2\"/>Freebase</a>")
                .arg(mapLocal["mid"].toString());
        strHtml += QString("<a href=\"http://en.wikipedia.org/wiki/index.html?curid=%1\"><img src=\"http://en.wikipedia.org/favicon.ico\" alt=\"Wiki\" hspace=\"2\"/>Wiki</a>")
                .arg(s);
    }

    strHtml += "<p><i><u>General Info</u></i></p>";
    ///people/person
    key = "name";
    text = "Name:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QVariant::List) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else if (t != QVariant::Invalid) {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }
    key = "date_of_birth";
    text = "Date of birth:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QVariant::List) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else if (t != QVariant::Invalid) {
            QDate date = QDate::fromString(mapLocal[key].toString(), Qt::ISODate);
            strHtml += "<p><i>" + text + " </i><b>" + date.toString("MMM d, yyyy") + "</b></p>";
        }
    }
    mapLocal =  map["q1"].toMap()["result"].toMap();
    key = "date_of_death";
    text = "Date of death:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QVariant::List) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else if (t != QVariant::Invalid) {
            QDate date = QDate::fromString(mapLocal[key].toString(), Qt::ISODate);
            strHtml += "<p><i>" + text + " </i><b>" + date.toString("MMM d, yyyy") + "</b></p>";
        }
    }
    key = "place_of_death";
    text = "Place of death:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QVariant::List) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else if (t != QVariant::Invalid) {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }
    key = "place_of_burial";
    text = "Place of bural:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QVariant::List) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else if (t != QVariant::Invalid) {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }
    key = "cause_of_death";
    text = "Cause of death:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QVariant::List) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else if (t != QVariant::Invalid) {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }

    //Living person
    strHtml += "<p><i><u>Person Info</u></i></p>";
    ///people/person
    mapLocal =  map["q0"].toMap()["result"].toMap();
    key = "place_of_birth";
    text = "Place of birth:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QVariant::List) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else if (t != QVariant::Invalid) {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }
    key = "profession";
    text = "Profession:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QVariant::List) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else if (t != QVariant::Invalid) {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }
    key = "nationality";
    text = "Nationality:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QVariant::List) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else if (t != QVariant::Invalid) {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }
    key = "ethnicity";
    text = "Ethnicity:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QVariant::List) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else if (t != QVariant::Invalid) {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }
    key = "religion";
    text = "Religion:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QVariant::List) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else if (t != QVariant::Invalid) {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }
    key = "children";
    text = "Children:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QVariant::List) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else if (t != QVariant::Invalid) {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }
    key = "parents";
    text = "Parents:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QVariant::List) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else if (t != QVariant::Invalid) {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }
    key = "quotations";
    text = "Quotations:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QVariant::List) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else if (t != QVariant::Invalid) {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }

    //Books author
    ///books/author
    mapLocal =  map["q2"].toMap()["result"].toMap();
    key = "works_written";
    text = "Works written:";
    if (mapLocal.contains(key)) {
        strHtml += "<p><i><u>Books Info</u></i></p>";
        QVariant::Type t = mapLocal[key].type();
        if (t == QVariant::List) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else if (t != QVariant::Invalid) {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }

    // Reference's links
    // Persons
    lst = map["q3"].toMap()["result"].toList();
    if (!lst.isEmpty()) {
        strHtml += "<p><i><u>People Referencies</u></i></p>";
        mapLocal = lst[0].toMap();
        key = "/type/reflect/any_reverse";
        text = "Referencies:";
        if (mapLocal.contains(key)) {
            QVariant::Type t = mapLocal[key].type();
            if (t == QVariant::List) {
                lst = mapLocal[key].toList();
                if (!lst.isEmpty()) {
                    strHtml += "<p></p><ul>";
                    foreach (QVariant item, lst) {
                        strHtml += "<li><a href=\""+item.toMap()["mid"].toString()+"\">" + item.toMap()["name"].toString()+ "</a></li>";
                    }
                    strHtml += "</ul>";
                }
            } else if (t != QVariant::Invalid) {
                strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
            }
        }
    }
    // Events
    lst = map["q4"].toMap()["result"].toList();
    if (!lst.isEmpty()) {
        strHtml += "<p><i><u>Event Referencies</u></i></p>";
        mapLocal = lst[0].toMap();
        key = "/type/reflect/any_reverse";
        text = "Referencies:";
        if (mapLocal.contains(key)) {
            QVariant::Type t = mapLocal[key].type();
            if (t == QVariant::List) {
                lst = mapLocal[key].toList();
                if (!lst.isEmpty()) {
                    strHtml += "<p></p><ul>";
                    foreach (QVariant item, lst) {
                        strHtml += "<li><a href=\""+item.toMap()["mid"].toString()+"\">" + item.toMap()["name"].toString()+ "</a></li>";
                    }
                    strHtml += "</ul>";
                }
            } else if (t != QVariant::Invalid) {
                strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
            }
        }
    }

    strHtml += "</body></html>";
    return strHtml;
}

QString HtmlGenerators::createHtmlForCountry(const QVariantMap & map)
{
    qDebug() << Q_FUNC_INFO;
    QString strHtml = QString("<html><body>");
    QVariantList lst;
    QVariantMap mapLocal =  map["q0"].toMap()["result"].toMap();
    QString key, text;
    strHtml += QString("<img src=\"https://usercontent.googleapis.com/freebase/v1-sandbox/image%1?maxheight=400&maxwidth=200\" align=\"right\">")
            .arg(mapLocal["mid"].toString());

    // Referencies
    QString s = findNamespaceValue("/wikipedia/en_id",map);
    if (!s.isEmpty()) {
        strHtml += QString("<a href=\"http://www.freebase.com/view%1\"><img src=\"http://www.freebase.com/favicon.ico\" alt=\"Freebase\" hspace=\"2\"/>Freebase</a>")
                .arg(mapLocal["mid"].toString());
        strHtml += QString("<a href=\"http://en.wikipedia.org/wiki/index.html?curid=%1\"><img src=\"http://en.wikipedia.org/favicon.ico\" alt=\"Wiki\" hspace=\"2\"/>Wiki</a>")
                .arg(s);
    }

    strHtml += "<p><i><u>General Info</u></i></p>";
    ///location/country
    key = "name";
    text = "Name:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QVariant::List) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else if (t != QVariant::Invalid) {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }
    key = "capital";
    text = "Capital:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QVariant::List) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else if (t != QVariant::Invalid) {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }
    key = "administrative_divisions";
    text = "Administrative Divisions:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QVariant::List) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else if (t != QVariant::Invalid) {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }
    key = "calling_code";
    text = "Calling code:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QVariant::List) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else if (t != QVariant::Invalid) {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }
    key = "currency_used";
    text = "Currency used:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QVariant::List) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else if (t != QVariant::Invalid) {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }
    key = "fifa_code";
    text = "FIFA code:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QVariant::List) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else if (t != QVariant::Invalid) {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }
    key = "form_of_government";
    text = "Form of government:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QVariant::List) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else if (t != QVariant::Invalid) {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }
    key = "internet_tld";
    text = "Internet domains:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QVariant::List) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else if (t != QVariant::Invalid) {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }
    key = "languages_spoken";
    text = "Language spoken:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QVariant::List) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else if (t != QVariant::Invalid) {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }
    key = "official_language";
    text = "Official languages:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QVariant::List) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else if (t != QVariant::Invalid) {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }

    strHtml += "<p><i><u>Location Info</u></i></p>";
    ///location/location
    mapLocal =  map["q1"].toMap()["result"].toMap();
    key = "area";
    text = "Area (km2):";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QVariant::List) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else if (t != QVariant::Invalid) {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }
    key = "containedby";
    text = "Contained by:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QVariant::List) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else if (t != QVariant::Invalid) {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }
    key = "time_zones";
    text = "Time zones:";
    if (mapLocal.contains(key)) {
        QVariant::Type t = mapLocal[key].type();
        if (t == QVariant::List) {
            lst = mapLocal[key].toList();
            if (!lst.isEmpty()) {
                strHtml += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    strHtml += "<li><b>"+item.toString()+"</b></li>";
                }
                strHtml += "</ul>";
            }
        } else if (t != QVariant::Invalid) {
            strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
        }
    }

    strHtml += "</body></html>";
    return strHtml;
}

QString HtmlGenerators::findNamespaceValue(const QString& ns, const QVariantMap& map)
{
    QVariantList list = map["q0"].toMap()["result"].toMap()["key"].toList();
    foreach (QVariant item, list) {
        QVariantMap m = item.toMap();
        if( m["namespace"].toString() == ns )
            return m["value"].toString();
    }

    return QString();
}
