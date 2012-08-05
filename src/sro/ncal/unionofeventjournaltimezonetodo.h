#ifndef _NCAL_UNIONOFEVENTJOURNALTIMEZONETODO_H_
#define _NCAL_UNIONOFEVENTJOURNALTIMEZONETODO_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include <nepomuk2/simpleresource.h>

#include "ncal/unionparentclass.h"

namespace Nepomuk2 {
namespace NCAL {
/**
 * 
 */
class UnionOfEventJournalTimezoneTodo : public virtual NCAL::UnionParentClass
{
public:
    UnionOfEventJournalTimezoneTodo(const QUrl& uri = QUrl())
      : SimpleResource(uri), NCAL::UnionParentClass(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#UnionOfEventJournalTimezoneTodo", QUrl::StrictMode)) {
    }

    UnionOfEventJournalTimezoneTodo(const SimpleResource& res)
      : SimpleResource(res), NCAL::UnionParentClass(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#UnionOfEventJournalTimezoneTodo", QUrl::StrictMode)) {
    }

    UnionOfEventJournalTimezoneTodo& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#UnionOfEventJournalTimezoneTodo", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#recurrenceId. 
     * This property is used in conjunction with the "UID" and "SEQUENCE" 
     * property to identify a specific instance of a recurring "VEVENT", 
     * "VTODO" or "VJOURNAL" calendar component. The property value 
     * is the effective value of the "DTSTART" property of the recurrence 
     * instance. Inspired by the RFC 2445 sec. 4.8.4.4 
     */
    QUrl recurrenceId() const {
        QUrl value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#recurrenceId", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#recurrenceId", QUrl::StrictMode)).first().value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#recurrenceId. 
     * This property is used in conjunction with the "UID" and "SEQUENCE" 
     * property to identify a specific instance of a recurring "VEVENT", 
     * "VTODO" or "VJOURNAL" calendar component. The property value 
     * is the effective value of the "DTSTART" property of the recurrence 
     * instance. Inspired by the RFC 2445 sec. 4.8.4.4 
     */
    void setRecurrenceId(const QUrl& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#recurrenceId", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#recurrenceId. 
     * This property is used in conjunction with the "UID" and "SEQUENCE" 
     * property to identify a specific instance of a recurring "VEVENT", 
     * "VTODO" or "VJOURNAL" calendar component. The property value 
     * is the effective value of the "DTSTART" property of the recurrence 
     * instance. Inspired by the RFC 2445 sec. 4.8.4.4 
     */
    void addRecurrenceId(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#recurrenceId", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#exdate. 
     * This property defines the list of date/time exceptions for 
     * a recurring calendar component. Inspired by RFC 2445 sec. 4.8.5.1 
     */
    QList<QUrl> exdates() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#exdate", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#exdate. 
     * This property defines the list of date/time exceptions for 
     * a recurring calendar component. Inspired by RFC 2445 sec. 4.8.5.1 
     */
    void setExdates(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#exdate", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#exdate. 
     * This property defines the list of date/time exceptions for 
     * a recurring calendar component. Inspired by RFC 2445 sec. 4.8.5.1 
     */
    void addExdate(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#exdate", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#lastModified. 
     * The property specifies the date and time that the information 
     * associated with the calendar component was last revised in 
     * the calendar store. Note: This is analogous to the modification 
     * date and time for a file in the file system. Inspired by RFC 2445 
     * sec. 4.8.7.3. Note that the RFC allows ONLY UTC time values for 
     * this property. 
     */
    QDateTime lastModified() const {
        QDateTime value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#lastModified", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#lastModified", QUrl::StrictMode)).first().value<QDateTime>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#lastModified. 
     * The property specifies the date and time that the information 
     * associated with the calendar component was last revised in 
     * the calendar store. Note: This is analogous to the modification 
     * date and time for a file in the file system. Inspired by RFC 2445 
     * sec. 4.8.7.3. Note that the RFC allows ONLY UTC time values for 
     * this property. 
     */
    void setLastModified(const QDateTime& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#lastModified", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#lastModified. 
     * The property specifies the date and time that the information 
     * associated with the calendar component was last revised in 
     * the calendar store. Note: This is analogous to the modification 
     * date and time for a file in the file system. Inspired by RFC 2445 
     * sec. 4.8.7.3. Note that the RFC allows ONLY UTC time values for 
     * this property. 
     */
    void addLastModified(const QDateTime& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#lastModified", QUrl::StrictMode), value);
    }

protected:
    UnionOfEventJournalTimezoneTodo(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NCAL::UnionParentClass(uri, type) {
    }
    UnionOfEventJournalTimezoneTodo(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NCAL::UnionParentClass(res, type) {
    }
};
}
}

#endif
