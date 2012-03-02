#ifndef _NBIB_BRIEF_H_
#define _NBIB_BRIEF_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include "dms-copy/simpleresource.h"

#include "nbib/legalcasedocument.h"

namespace Nepomuk {
namespace NBIB {
/**
 * A written argument submitted to a court. 
 */
class Brief : public virtual NBIB::LegalCaseDocument
{
public:
    Brief(const QUrl& uri = QUrl())
      : SimpleResource(uri), NIE::InformationElement(uri, QUrl::fromEncoded("http://www.example.com/nbib#Brief", QUrl::StrictMode)), NBIB::Publication(uri, QUrl::fromEncoded("http://www.example.com/nbib#Brief", QUrl::StrictMode)), NBIB::LegalDocument(uri, QUrl::fromEncoded("http://www.example.com/nbib#Brief", QUrl::StrictMode)), NBIB::LegalCaseDocument(uri, QUrl::fromEncoded("http://www.example.com/nbib#Brief", QUrl::StrictMode)) {
    }

    Brief(const SimpleResource& res)
      : SimpleResource(res), NIE::InformationElement(res, QUrl::fromEncoded("http://www.example.com/nbib#Brief", QUrl::StrictMode)), NBIB::Publication(res, QUrl::fromEncoded("http://www.example.com/nbib#Brief", QUrl::StrictMode)), NBIB::LegalDocument(res, QUrl::fromEncoded("http://www.example.com/nbib#Brief", QUrl::StrictMode)), NBIB::LegalCaseDocument(res, QUrl::fromEncoded("http://www.example.com/nbib#Brief", QUrl::StrictMode)) {
    }

    Brief& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.example.com/nbib#Brief", QUrl::StrictMode));
        return *this;
    }

protected:
    Brief(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NIE::InformationElement(uri, type), NBIB::Publication(uri, type), NBIB::LegalDocument(uri, type), NBIB::LegalCaseDocument(uri, type) {
    }
    Brief(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NIE::InformationElement(res, type), NBIB::Publication(res, type), NBIB::LegalDocument(res, type), NBIB::LegalCaseDocument(res, type) {
    }
};
}
}

#endif
