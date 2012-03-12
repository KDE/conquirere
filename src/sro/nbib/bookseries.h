#ifndef _NBIB_BOOKSERIES_H_
#define _NBIB_BOOKSERIES_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include "dms-copy/simpleresource.h"

#include "nbib/series.h"

namespace Nepomuk {
namespace NBIB {
/**
 * The series of a book. 
 */
class BookSeries : public virtual NBIB::Series
{
public:
    BookSeries(const QUrl& uri = QUrl())
      : SimpleResource(uri), NIE::InformationElement(uri, QUrl::fromEncoded("http://www.example.com/nbib#BookSeries", QUrl::StrictMode)), NBIB::Series(uri, QUrl::fromEncoded("http://www.example.com/nbib#BookSeries", QUrl::StrictMode)) {
    }

    BookSeries(const SimpleResource& res)
      : SimpleResource(res), NIE::InformationElement(res, QUrl::fromEncoded("http://www.example.com/nbib#BookSeries", QUrl::StrictMode)), NBIB::Series(res, QUrl::fromEncoded("http://www.example.com/nbib#BookSeries", QUrl::StrictMode)) {
    }

    BookSeries& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.example.com/nbib#BookSeries", QUrl::StrictMode));
        return *this;
    }

protected:
    BookSeries(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NIE::InformationElement(uri, type), NBIB::Series(uri, type) {
    }
    BookSeries(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NIE::InformationElement(res, type), NBIB::Series(res, type) {
    }
};
}
}

#endif
