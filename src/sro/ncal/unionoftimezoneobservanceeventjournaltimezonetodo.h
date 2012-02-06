#ifndef _NCAL_UNIONOFTIMEZONEOBSERVANCEEVENTJOURNALTIMEZONETODO_H_
#define _NCAL_UNIONOFTIMEZONEOBSERVANCEEVENTJOURNALTIMEZONETODO_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include "dms-copy/simpleresource.h"

#include "ncal/unionparentclass.h"

namespace Nepomuk {
namespace NCAL {
/**
 * 
 */
class UnionOfTimezoneObservanceEventJournalTimezoneTodo : public virtual NCAL::UnionParentClass
{
public:
    UnionOfTimezoneObservanceEventJournalTimezoneTodo(const QUrl& uri = QUrl())
      : SimpleResource(uri), NCAL::UnionParentClass(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#UnionOfTimezoneObservanceEventJournalTimezoneTodo", QUrl::StrictMode)) {
    }

    UnionOfTimezoneObservanceEventJournalTimezoneTodo(const SimpleResource& res)
      : SimpleResource(res), NCAL::UnionParentClass(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#UnionOfTimezoneObservanceEventJournalTimezoneTodo", QUrl::StrictMode)) {
    }

    UnionOfTimezoneObservanceEventJournalTimezoneTodo& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#UnionOfTimezoneObservanceEventJournalTimezoneTodo", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#rrule. 
     * This property defines a rule or repeating pattern for recurring 
     * events, to-dos, or time zone definitions. sec. 4.8.5.4 
     */
    QList<QUrl> rrules() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#rrule", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#rrule. 
     * This property defines a rule or repeating pattern for recurring 
     * events, to-dos, or time zone definitions. sec. 4.8.5.4 
     */
    void setRrules(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#rrule", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#rrule. 
     * This property defines a rule or repeating pattern for recurring 
     * events, to-dos, or time zone definitions. sec. 4.8.5.4 
     */
    void addRrule(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#rrule", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#rdate. 
     * This property defines the list of date/times for a recurrence 
     * set. Inspired by RFC 2445 sec. 4.8.5.3. Note that RFC allows 
     * both DATE, DATE-TIME and PERIOD values for this property. That's 
     * why the range has been set to NcalTimeEntity. 
     */
    QList<QUrl> rdates() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#rdate", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#rdate. 
     * This property defines the list of date/times for a recurrence 
     * set. Inspired by RFC 2445 sec. 4.8.5.3. Note that RFC allows 
     * both DATE, DATE-TIME and PERIOD values for this property. That's 
     * why the range has been set to NcalTimeEntity. 
     */
    void setRdates(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#rdate", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#rdate. 
     * This property defines the list of date/times for a recurrence 
     * set. Inspired by RFC 2445 sec. 4.8.5.3. Note that RFC allows 
     * both DATE, DATE-TIME and PERIOD values for this property. That's 
     * why the range has been set to NcalTimeEntity. 
     */
    void addRdate(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#rdate", QUrl::StrictMode), value);
    }

protected:
    UnionOfTimezoneObservanceEventJournalTimezoneTodo(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NCAL::UnionParentClass(uri, type) {
    }
    UnionOfTimezoneObservanceEventJournalTimezoneTodo(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NCAL::UnionParentClass(res, type) {
    }
};
}
}

#endif
