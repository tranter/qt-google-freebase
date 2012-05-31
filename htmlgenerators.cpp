
#include "htmlgenerators.h"

#include <QDebug>
#include <QDate>
#include <QUrl>

QString HtmlGenerators::createHtmlForPerson(const QVariantMap& map)
{
    QString strHtml = QString("<html><body>");
    QVariantList lst;

    //Person
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

    ///people/person
    if(mapLocal.contains("name"))
    {
        strHtml += makeHtmlRecordForPerson(mapLocal,"name","Name:");
    }
    key = "date_of_birth";
    text = "Date of birth:";
    if (mapLocal.contains(key))
    {
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

    //Deceased person
    mapLocal =  map["q1"].toMap()["result"].toMap();
    key = "date_of_death";
    text = "Date of death:";
    if (mapLocal.contains(key))
    {
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
        }
        else if (t != QVariant::Invalid)
        {
            QDate date = QDate::fromString(mapLocal[key].toString(), Qt::ISODate);
            strHtml += "<p><i>" + text + " </i><b>" + date.toString("MMM d, yyyy") + "</b></p>";
        }
    }
    strHtml += makeHtmlRecordForPerson(mapLocal,"place_of_death","Place of death:");
    strHtml += makeHtmlRecordForPerson(mapLocal,"place_of_burial","Place of burial:");
    strHtml += makeHtmlRecordForPerson(mapLocal,"cause_of_death","Cause of death:");

    //Living person
    ///people/person
    mapLocal =  map["q0"].toMap()["result"].toMap();
    strHtml += makeHtmlRecordForPerson(mapLocal,"place_of_birth","Place of birth:");
    strHtml += makeHtmlRecordForPerson(mapLocal,"profession","Profession:");
    strHtml += makeHtmlRecordForPerson(mapLocal,"nationality","Nationality:");
    strHtml += makeHtmlRecordForPerson(mapLocal,"ethnicity","Ethnicity:");
    strHtml += makeHtmlRecordForPerson(mapLocal,"religion","Religion:");
    strHtml += makeHtmlRecordForPerson(mapLocal,"children","Children:");
    strHtml += makeHtmlRecordForPerson(mapLocal,"parents","Parents:");
    strHtml += makeHtmlRecordForPerson(mapLocal,"quotations","Quotations:");

    //Books author
    ///books/author
    mapLocal =  map["q2"].toMap()["result"].toMap();
    strHtml += makeHtmlRecordForPerson(mapLocal,"works_written","Works written:");


    ///event/event
    mapLocal =  map["q3"].toMap()["result"].toMap();
    strHtml += makeHtmlRecordForPerson(mapLocal,"name","Name:");
    key = "start_date";
    text = "Start date:";
    if(mapLocal.contains(key))
    {
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
        }
        else if (t != QVariant::Invalid)
        {
            QDate date = QDate::fromString(mapLocal[key].toString(), Qt::ISODate);
            strHtml += "<p><i>" + text + " </i><b>" + date.toString("MMM d, yyyy") + "</b></p>";
        }
    }
    key = "end_date";
    text = "End date:";
    if(mapLocal.contains(key))
    {
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
        }
        else if (t != QVariant::Invalid)
        {
            QDate date = QDate::fromString(mapLocal[key].toString(), Qt::ISODate);
            strHtml += "<p><i>" + text + " </i><b>" + date.toString("MMM d, yyyy") + "</b></p>";
        }
    }
    strHtml += makeHtmlRecordForPerson(mapLocal,"locations","Locations:");


    // Reference's links
    // Persons
    lst = map["q4"].toMap()["result"].toList();
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
//                        strHtml += "<li><a href=\""+item.toMap()["mid"].toString()+"\">" + item.toMap()["name"].toString()+ "</a></li>";
                        strHtml +=
                                "<li><a href=\"data@"
                                + item.toMap()["mid"].toString()
                                + "@" + item.toMap()["name"].toString()
                                + "\">"
                                + item.toMap()["name"].toString()
                                + "</a></li>";
                    }
                    strHtml += "</ul>";
                }
            } else if (t != QVariant::Invalid) {
                strHtml += "<p><i>" + text + " </i><b>"+mapLocal[key].toString()+"</b></p>";
            }
        }
    }
    // Events
    lst = map["q5"].toMap()["result"].toList();
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
                        strHtml +=
                                "<li><a href=\"data@"
                                + item.toMap()["mid"].toString()
                                + "@" + item.toMap()["name"].toString()
                                + "\">"
                                + item.toMap()["name"].toString()
                                + "</a></li>";
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

QString HtmlGenerators::makeHtmlRecordForPerson(const QVariantMap& map, const QString& key, const QString& text)
{
    QString ret;
    if (map.contains(key)) {
        QVariant::Type t = map[key].type();
        if (t == QVariant::List) {
            QVariantList lst = map[key].toList();
            if (!lst.isEmpty()) {
                ret += "<p><i>" + text + "</i></p><ul>";
                foreach (QVariant item, lst) {
                    ret += "<li><b>"+item.toString()+"</b></li>";
                }
                ret += "</ul>";
            }
        } else if (t != QVariant::Invalid) {
            ret += "<p><i>" + text + " </i><b>"+map[key].toString()+"</b></p>";
        }
    }
    return ret;
}
