#ifndef _NBIB_COURTREPORTER_H_
#define _NBIB_COURTREPORTER_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include <nepomuk/simpleresource.h>

#include "nbib/collection.h"

namespace Nepomuk {
namespace NBIB {
/**
 * A collection of legal cases. 
 */
class CourtReporter : public virtual NBIB::Collection
{
public:
    CourtReporter(const QUrl& uri = QUrl())
      : SimpleResource(uri), NBIB::Publication(uri, QUrl::fromEncoded("http://www.example.com/nbib#CourtReporter", QUrl::StrictMode)), NBIB::Collection(uri, QUrl::fromEncoded("http://www.example.com/nbib#CourtReporter", QUrl::StrictMode)) {
    }

    CourtReporter(const SimpleResource& res)
      : SimpleResource(res), NBIB::Publication(res, QUrl::fromEncoded("http://www.example.com/nbib#CourtReporter", QUrl::StrictMode)), NBIB::Collection(res, QUrl::fromEncoded("http://www.example.com/nbib#CourtReporter", QUrl::StrictMode)) {
    }

    CourtReporter& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.example.com/nbib#CourtReporter", QUrl::StrictMode));
        return *this;
    }

protected:
    CourtReporter(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NBIB::Publication(uri, type), NBIB::Collection(uri, type) {
    }
    CourtReporter(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NBIB::Publication(res, type), NBIB::Collection(res, type) {
    }
};
}
}

#endif
