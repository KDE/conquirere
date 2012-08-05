#ifndef _NBIB_BILL_H_
#define _NBIB_BILL_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include <nepomuk2/simpleresource.h>

#include "nbib/legislation.h"

namespace Nepomuk2 {
namespace NBIB {
/**
 * Draft legislation presented for discussion to a legal body. 
 */
class Bill : public virtual NBIB::Legislation
{
public:
    Bill(const QUrl& uri = QUrl())
      : SimpleResource(uri), NIE::InformationElement(uri, QUrl::fromEncoded("http://www.example.com/nbib#Bill", QUrl::StrictMode)), NBIB::Publication(uri, QUrl::fromEncoded("http://www.example.com/nbib#Bill", QUrl::StrictMode)), NBIB::LegalDocument(uri, QUrl::fromEncoded("http://www.example.com/nbib#Bill", QUrl::StrictMode)), NBIB::Legislation(uri, QUrl::fromEncoded("http://www.example.com/nbib#Bill", QUrl::StrictMode)) {
    }

    Bill(const SimpleResource& res)
      : SimpleResource(res), NIE::InformationElement(res, QUrl::fromEncoded("http://www.example.com/nbib#Bill", QUrl::StrictMode)), NBIB::Publication(res, QUrl::fromEncoded("http://www.example.com/nbib#Bill", QUrl::StrictMode)), NBIB::LegalDocument(res, QUrl::fromEncoded("http://www.example.com/nbib#Bill", QUrl::StrictMode)), NBIB::Legislation(res, QUrl::fromEncoded("http://www.example.com/nbib#Bill", QUrl::StrictMode)) {
    }

    Bill& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.example.com/nbib#Bill", QUrl::StrictMode));
        return *this;
    }

protected:
    Bill(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NIE::InformationElement(uri, type), NBIB::Publication(uri, type), NBIB::LegalDocument(uri, type), NBIB::Legislation(uri, type) {
    }
    Bill(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NIE::InformationElement(res, type), NBIB::Publication(res, type), NBIB::LegalDocument(res, type), NBIB::Legislation(res, type) {
    }
};
}
}

#endif
