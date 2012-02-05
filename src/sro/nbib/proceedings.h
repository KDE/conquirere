#ifndef _NBIB_PROCEEDINGS_H_
#define _NBIB_PROCEEDINGS_H_

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
 * The proceedings of a conference. 
 */
class Proceedings : public virtual NBIB::Collection
{
public:
    Proceedings(const QUrl& uri = QUrl())
      : SimpleResource(uri), NBIB::Publication(uri, QUrl::fromEncoded("http://www.example.com/nbib#Proceedings", QUrl::StrictMode)), NBIB::Collection(uri, QUrl::fromEncoded("http://www.example.com/nbib#Proceedings", QUrl::StrictMode)) {
    }

    Proceedings(const SimpleResource& res)
      : SimpleResource(res), NBIB::Publication(res, QUrl::fromEncoded("http://www.example.com/nbib#Proceedings", QUrl::StrictMode)), NBIB::Collection(res, QUrl::fromEncoded("http://www.example.com/nbib#Proceedings", QUrl::StrictMode)) {
    }

    Proceedings& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.example.com/nbib#Proceedings", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.example.com/nbib#organization. 
     * The organization that held the conference where the proceedings 
     * comes from 
     */
    QUrl organization() const {
        QUrl value;
        if(contains(QUrl::fromEncoded("http://www.example.com/nbib#organization", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/nbib#organization", QUrl::StrictMode)).first().value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#organization. 
     * The organization that held the conference where the proceedings 
     * comes from 
     */
    void setOrganization(const QUrl& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#organization", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#organization. 
     * The organization that held the conference where the proceedings 
     * comes from 
     */
    void addOrganization(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#organization", QUrl::StrictMode), value);
    }

protected:
    Proceedings(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NBIB::Publication(uri, type), NBIB::Collection(uri, type) {
    }
    Proceedings(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NBIB::Publication(res, type), NBIB::Collection(res, type) {
    }
};
}
}

#endif
