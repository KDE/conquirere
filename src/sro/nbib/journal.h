#ifndef _NBIB_JOURNAL_H_
#define _NBIB_JOURNAL_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include <nepomuk2/simpleresource.h>

#include "nbib/series.h"

namespace Nepomuk2 {
namespace NBIB {
/**
 * A Journal which publish single journal issues. 
 */
class Journal : public virtual NBIB::Series
{
public:
    Journal(const QUrl& uri = QUrl())
      : SimpleResource(uri), NIE::InformationElement(uri, QUrl::fromEncoded("http://www.example.com/nbib#Journal", QUrl::StrictMode)), NBIB::Series(uri, QUrl::fromEncoded("http://www.example.com/nbib#Journal", QUrl::StrictMode)) {
    }

    Journal(const SimpleResource& res)
      : SimpleResource(res), NIE::InformationElement(res, QUrl::fromEncoded("http://www.example.com/nbib#Journal", QUrl::StrictMode)), NBIB::Series(res, QUrl::fromEncoded("http://www.example.com/nbib#Journal", QUrl::StrictMode)) {
    }

    Journal& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.example.com/nbib#Journal", QUrl::StrictMode));
        return *this;
    }

protected:
    Journal(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NIE::InformationElement(uri, type), NBIB::Series(uri, type) {
    }
    Journal(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NIE::InformationElement(res, type), NBIB::Series(res, type) {
    }
};
}
}

#endif
