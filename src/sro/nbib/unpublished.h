#ifndef _NBIB_UNPUBLISHED_H_
#define _NBIB_UNPUBLISHED_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include <nepomuk2/simpleresource.h>

#include "nbib/publication.h"

namespace Nepomuk2 {
namespace NBIB {
/**
 * A document having an author and title, but not formally published. 
 */
class Unpublished : public virtual NBIB::Publication
{
public:
    Unpublished(const QUrl& uri = QUrl())
      : SimpleResource(uri), NIE::InformationElement(uri, QUrl::fromEncoded("http://www.example.com/nbib#Unpublished", QUrl::StrictMode)), NBIB::Publication(uri, QUrl::fromEncoded("http://www.example.com/nbib#Unpublished", QUrl::StrictMode)) {
    }

    Unpublished(const SimpleResource& res)
      : SimpleResource(res), NIE::InformationElement(res, QUrl::fromEncoded("http://www.example.com/nbib#Unpublished", QUrl::StrictMode)), NBIB::Publication(res, QUrl::fromEncoded("http://www.example.com/nbib#Unpublished", QUrl::StrictMode)) {
    }

    Unpublished& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.example.com/nbib#Unpublished", QUrl::StrictMode));
        return *this;
    }

protected:
    Unpublished(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NIE::InformationElement(uri, type), NBIB::Publication(uri, type) {
    }
    Unpublished(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NIE::InformationElement(res, type), NBIB::Publication(res, type) {
    }
};
}
}

#endif
