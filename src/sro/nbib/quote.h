#ifndef _NBIB_QUOTE_H_
#define _NBIB_QUOTE_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include "dms-copy/simpleresource.h"

#include "nbib/documentpart.h"

namespace Nepomuk {
namespace NBIB {
/**
 * A quote in a document 
 */
class Quote : public virtual NBIB::DocumentPart
{
public:
    Quote(const QUrl& uri = QUrl())
      : SimpleResource(uri), NIE::InformationElement(uri, QUrl::fromEncoded("http://www.example.com/nbib#Quote", QUrl::StrictMode)), NBIB::DocumentPart(uri, QUrl::fromEncoded("http://www.example.com/nbib#Quote", QUrl::StrictMode)) {
    }

    Quote(const SimpleResource& res)
      : SimpleResource(res), NIE::InformationElement(res, QUrl::fromEncoded("http://www.example.com/nbib#Quote", QUrl::StrictMode)), NBIB::DocumentPart(res, QUrl::fromEncoded("http://www.example.com/nbib#Quote", QUrl::StrictMode)) {
    }

    Quote& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.example.com/nbib#Quote", QUrl::StrictMode));
        return *this;
    }

protected:
    Quote(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NIE::InformationElement(uri, type), NBIB::DocumentPart(uri, type) {
    }
    Quote(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NIE::InformationElement(res, type), NBIB::DocumentPart(res, type) {
    }
};
}
}

#endif
