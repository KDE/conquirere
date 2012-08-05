#ifndef _NBIB_TECHREPORT_H_
#define _NBIB_TECHREPORT_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include <nepomuk2/simpleresource.h>

#include "nbib/report.h"

namespace Nepomuk2 {
namespace NBIB {
/**
 * A report published by a school or other institution, usually 
 * numbered within a series. 
 */
class Techreport : public virtual NBIB::Report
{
public:
    Techreport(const QUrl& uri = QUrl())
      : SimpleResource(uri), NIE::InformationElement(uri, QUrl::fromEncoded("http://www.example.com/nbib#Techreport", QUrl::StrictMode)), NBIB::Publication(uri, QUrl::fromEncoded("http://www.example.com/nbib#Techreport", QUrl::StrictMode)), NBIB::Report(uri, QUrl::fromEncoded("http://www.example.com/nbib#Techreport", QUrl::StrictMode)) {
    }

    Techreport(const SimpleResource& res)
      : SimpleResource(res), NIE::InformationElement(res, QUrl::fromEncoded("http://www.example.com/nbib#Techreport", QUrl::StrictMode)), NBIB::Publication(res, QUrl::fromEncoded("http://www.example.com/nbib#Techreport", QUrl::StrictMode)), NBIB::Report(res, QUrl::fromEncoded("http://www.example.com/nbib#Techreport", QUrl::StrictMode)) {
    }

    Techreport& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.example.com/nbib#Techreport", QUrl::StrictMode));
        return *this;
    }

protected:
    Techreport(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NIE::InformationElement(uri, type), NBIB::Publication(uri, type), NBIB::Report(uri, type) {
    }
    Techreport(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NIE::InformationElement(res, type), NBIB::Publication(res, type), NBIB::Report(res, type) {
    }
};
}
}

#endif
