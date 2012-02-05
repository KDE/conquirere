#ifndef _NBIB_BOOKSERIES_H_
#define _NBIB_BOOKSERIES_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include <nepomuk/simpleresource.h>

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
      : SimpleResource(uri), NBIB::Series(uri, QUrl::fromEncoded("http://www.example.com/nbib#BookSeries", QUrl::StrictMode)) {
    }

    BookSeries(const SimpleResource& res)
      : SimpleResource(res), NBIB::Series(res, QUrl::fromEncoded("http://www.example.com/nbib#BookSeries", QUrl::StrictMode)) {
    }

    BookSeries& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.example.com/nbib#BookSeries", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.example.com/nbib#seriesOf. The 
     * book this series belongs to 
     */
    QList<QUrl> seriesOfs() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.example.com/nbib#seriesOf", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#seriesOf. The 
     * book this series belongs to 
     */
    void setSeriesOfs(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#seriesOf", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#seriesOf. 
     * The book this series belongs to 
     */
    void addSeriesOf(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#seriesOf", QUrl::StrictMode), value);
    }

protected:
    BookSeries(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NBIB::Series(uri, type) {
    }
    BookSeries(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NBIB::Series(res, type) {
    }
};
}
}

#endif
