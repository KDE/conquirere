#ifndef _NBIB_STANDARD_H_
#define _NBIB_STANDARD_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include <nepomuk/simpleresource.h>

#include "nbib/publication.h"

namespace Nepomuk {
namespace NBIB {
/**
 * A document describing a standard 
 */
class Standard : public virtual NBIB::Publication
{
public:
    Standard(const QUrl& uri = QUrl())
      : SimpleResource(uri), NBIB::Publication(uri, QUrl::fromEncoded("http://www.example.com/nbib#Standard", QUrl::StrictMode)) {
    }

    Standard(const SimpleResource& res)
      : SimpleResource(res), NBIB::Publication(res, QUrl::fromEncoded("http://www.example.com/nbib#Standard", QUrl::StrictMode)) {
    }

    Standard& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.example.com/nbib#Standard", QUrl::StrictMode));
        return *this;
    }

protected:
    Standard(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NBIB::Publication(uri, type) {
    }
    Standard(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NBIB::Publication(res, type) {
    }
};
}
}

#endif
