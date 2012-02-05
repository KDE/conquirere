#ifndef _NBIB_MANUAL_H_
#define _NBIB_MANUAL_H_

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
 * Technical documentation. 
 */
class Manual : public virtual NBIB::Publication
{
public:
    Manual(const QUrl& uri = QUrl())
      : SimpleResource(uri), NBIB::Publication(uri, QUrl::fromEncoded("http://www.example.com/nbib#Manual", QUrl::StrictMode)) {
    }

    Manual(const SimpleResource& res)
      : SimpleResource(res), NBIB::Publication(res, QUrl::fromEncoded("http://www.example.com/nbib#Manual", QUrl::StrictMode)) {
    }

    Manual& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.example.com/nbib#Manual", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.example.com/nbib#edition. The 
     * edition of a book, long form (such as 'first' or 'second'). 
     */
    QString edition() const {
        QString value;
        if(contains(QUrl::fromEncoded("http://www.example.com/nbib#edition", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/nbib#edition", QUrl::StrictMode)).first().value<QString>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#edition. The 
     * edition of a book, long form (such as 'first' or 'second'). 
     */
    void setEdition(const QString& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#edition", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#edition. 
     * The edition of a book, long form (such as 'first' or 'second'). 
     */
    void addEdition(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#edition", QUrl::StrictMode), value);
    }

protected:
    Manual(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NBIB::Publication(uri, type) {
    }
    Manual(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NBIB::Publication(res, type) {
    }
};
}
}

#endif
