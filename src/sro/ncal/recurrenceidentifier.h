#ifndef _NCAL_RECURRENCEIDENTIFIER_H_
#define _NCAL_RECURRENCEIDENTIFIER_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include "dms-copy/simpleresource.h"

namespace Nepomuk {
namespace NCAL {
/**
 * Recurrence Identifier. Introduced to provide a structure 
 * for the value of ncal:recurrenceId property. See the documentation 
 * of ncal:recurrenceId for details. 
 */
class RecurrenceIdentifier : public virtual Nepomuk::SimpleResource
{
public:
    RecurrenceIdentifier(const QUrl& uri = QUrl())
      : SimpleResource(uri) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#RecurrenceIdentifier", QUrl::StrictMode));
    }

    RecurrenceIdentifier(const SimpleResource& res)
      : SimpleResource(res) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#RecurrenceIdentifier", QUrl::StrictMode));
    }

    RecurrenceIdentifier& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#RecurrenceIdentifier", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#range. 
     * To specify the effective range of recurrence instances from 
     * the instance specified by the recurrence identifier specified 
     * by the property. It is intended to express the RANGE parameter 
     * specified in RFC 2445 sec. 4.2.13. The set of possible values 
     * for this property is limited. See also the documentation for 
     * ncal:recurrenceId for more details. 
     */
    QUrl range() const {
        QUrl value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#range", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#range", QUrl::StrictMode)).first().value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#range. 
     * To specify the effective range of recurrence instances from 
     * the instance specified by the recurrence identifier specified 
     * by the property. It is intended to express the RANGE parameter 
     * specified in RFC 2445 sec. 4.2.13. The set of possible values 
     * for this property is limited. See also the documentation for 
     * ncal:recurrenceId for more details. 
     */
    void setRange(const QUrl& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#range", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#range. 
     * To specify the effective range of recurrence instances from 
     * the instance specified by the recurrence identifier specified 
     * by the property. It is intended to express the RANGE parameter 
     * specified in RFC 2445 sec. 4.2.13. The set of possible values 
     * for this property is limited. See also the documentation for 
     * ncal:recurrenceId for more details. 
     */
    void addRange(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#range", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#recurrenceIdDateTime. 
     * The date and time of a recurrence identifier. Provided to express 
     * the actual value of the ncal:recurrenceId property. See documentation 
     * for ncal:recurrenceId for details. 
     */
    QUrl recurrenceIdDateTime() const {
        QUrl value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#recurrenceIdDateTime", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#recurrenceIdDateTime", QUrl::StrictMode)).first().value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#recurrenceIdDateTime. 
     * The date and time of a recurrence identifier. Provided to express 
     * the actual value of the ncal:recurrenceId property. See documentation 
     * for ncal:recurrenceId for details. 
     */
    void setRecurrenceIdDateTime(const QUrl& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#recurrenceIdDateTime", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#recurrenceIdDateTime. 
     * The date and time of a recurrence identifier. Provided to express 
     * the actual value of the ncal:recurrenceId property. See documentation 
     * for ncal:recurrenceId for details. 
     */
    void addRecurrenceIdDateTime(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#recurrenceIdDateTime", QUrl::StrictMode), value);
    }

protected:
    RecurrenceIdentifier(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri) {
        addType(type);
    }
    RecurrenceIdentifier(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res) {
        addType(type);
    }
};
}
}

#endif
