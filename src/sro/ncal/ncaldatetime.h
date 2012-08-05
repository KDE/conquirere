#ifndef _NCAL_NCALDATETIME_H_
#define _NCAL_NCALDATETIME_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include <nepomuk2/simpleresource.h>

#include "ncal/ncaltimeentity.h"

namespace Nepomuk2 {
namespace NCAL {
/**
 * 
 */
class NcalDateTime : public virtual NCAL::NcalTimeEntity
{
public:
    NcalDateTime(const QUrl& uri = QUrl())
      : SimpleResource(uri), NCAL::NcalTimeEntity(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#NcalDateTime", QUrl::StrictMode)) {
    }

    NcalDateTime(const SimpleResource& res)
      : SimpleResource(res), NCAL::NcalTimeEntity(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#NcalDateTime", QUrl::StrictMode)) {
    }

    NcalDateTime& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#NcalDateTime", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#date. 
     * Date an instance of NcalDateTime refers to. It was conceived 
     * to express values in DATE datatype specified in RFC 2445 4.3.4 
     */
    QDate date() const {
        QDate value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#date", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#date", QUrl::StrictMode)).first().value<QDate>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#date. 
     * Date an instance of NcalDateTime refers to. It was conceived 
     * to express values in DATE datatype specified in RFC 2445 4.3.4 
     */
    void setDate(const QDate& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#date", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#date. 
     * Date an instance of NcalDateTime refers to. It was conceived 
     * to express values in DATE datatype specified in RFC 2445 4.3.4 
     */
    void addDate(const QDate& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#date", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#ncalTimezone. 
     * The timezone instance that should be used to interpret an NcalDateTime. 
     * The purpose of this property is similar to the TZID parameter 
     * specified in RFC 2445 sec. 4.2.19 
     */
    QUrl ncalTimezone() const {
        QUrl value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#ncalTimezone", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#ncalTimezone", QUrl::StrictMode)).first().value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#ncalTimezone. 
     * The timezone instance that should be used to interpret an NcalDateTime. 
     * The purpose of this property is similar to the TZID parameter 
     * specified in RFC 2445 sec. 4.2.19 
     */
    void setNcalTimezone(const QUrl& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#ncalTimezone", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#ncalTimezone. 
     * The timezone instance that should be used to interpret an NcalDateTime. 
     * The purpose of this property is similar to the TZID parameter 
     * specified in RFC 2445 sec. 4.2.19 
     */
    void addNcalTimezone(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#ncalTimezone", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#dateTime. 
     * Representation of a date an instance of NcalDateTime actually 
     * refers to. It's purpose is to express values in DATE-TIME datatype, 
     * as defined in RFC 2445 sec. 4.3.5 
     */
    QDateTime dateTime() const {
        QDateTime value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#dateTime", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#dateTime", QUrl::StrictMode)).first().value<QDateTime>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#dateTime. 
     * Representation of a date an instance of NcalDateTime actually 
     * refers to. It's purpose is to express values in DATE-TIME datatype, 
     * as defined in RFC 2445 sec. 4.3.5 
     */
    void setDateTime(const QDateTime& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#dateTime", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#dateTime. 
     * Representation of a date an instance of NcalDateTime actually 
     * refers to. It's purpose is to express values in DATE-TIME datatype, 
     * as defined in RFC 2445 sec. 4.3.5 
     */
    void addDateTime(const QDateTime& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#dateTime", QUrl::StrictMode), value);
    }

protected:
    NcalDateTime(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NCAL::NcalTimeEntity(uri, type) {
    }
    NcalDateTime(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NCAL::NcalTimeEntity(res, type) {
    }
};
}
}

#endif
