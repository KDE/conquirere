#ifndef _NCAL_UNIONOFALARMEVENTFREEBUSYJOURNALTODO_H_
#define _NCAL_UNIONOFALARMEVENTFREEBUSYJOURNALTODO_H_

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
class UnionOfAlarmEventFreebusyJournalTodo : public virtual NCAL::UnionParentClass
{
public:
    UnionOfAlarmEventFreebusyJournalTodo(const QUrl& uri = QUrl())
      : SimpleResource(uri), NCAL::UnionParentClass(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#UnionOfAlarmEventFreebusyJournalTodo", QUrl::StrictMode)) {
    }

    UnionOfAlarmEventFreebusyJournalTodo(const SimpleResource& res)
      : SimpleResource(res), NCAL::UnionParentClass(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#UnionOfAlarmEventFreebusyJournalTodo", QUrl::StrictMode)) {
    }

    UnionOfAlarmEventFreebusyJournalTodo& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#UnionOfAlarmEventFreebusyJournalTodo", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#attendee. 
     * The property defines an "Attendee" within a calendar component. 
     * Inspired by RFC 2445 sec. 4.8.4.1. Originally this property 
     * accepted many parameters. The Attendee class has been introduced 
     * to express them all. Note that NCAL is aligned with NCO. The actual 
     * value (of the CAL-ADDRESS type) is expressed as an instance 
     * of nco:Contact. Remember that the CN parameter has been removed 
     * from NCAL. Instead that value should be expressed using nco:fullname 
     * property of the above mentioned nco:Contact instance. The 
     * RFC stated that whenever this property is attached to a Valarm 
     * instance, the Attendee cannot have any parameters apart from 
     * involvedContact. 
     */
    QList<QUrl> attendees() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#attendee", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#attendee. 
     * The property defines an "Attendee" within a calendar component. 
     * Inspired by RFC 2445 sec. 4.8.4.1. Originally this property 
     * accepted many parameters. The Attendee class has been introduced 
     * to express them all. Note that NCAL is aligned with NCO. The actual 
     * value (of the CAL-ADDRESS type) is expressed as an instance 
     * of nco:Contact. Remember that the CN parameter has been removed 
     * from NCAL. Instead that value should be expressed using nco:fullname 
     * property of the above mentioned nco:Contact instance. The 
     * RFC stated that whenever this property is attached to a Valarm 
     * instance, the Attendee cannot have any parameters apart from 
     * involvedContact. 
     */
    void setAttendees(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#attendee", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#attendee. 
     * The property defines an "Attendee" within a calendar component. 
     * Inspired by RFC 2445 sec. 4.8.4.1. Originally this property 
     * accepted many parameters. The Attendee class has been introduced 
     * to express them all. Note that NCAL is aligned with NCO. The actual 
     * value (of the CAL-ADDRESS type) is expressed as an instance 
     * of nco:Contact. Remember that the CN parameter has been removed 
     * from NCAL. Instead that value should be expressed using nco:fullname 
     * property of the above mentioned nco:Contact instance. The 
     * RFC stated that whenever this property is attached to a Valarm 
     * instance, the Attendee cannot have any parameters apart from 
     * involvedContact. 
     */
    void addAttendee(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#attendee", QUrl::StrictMode), value);
    }

protected:
    UnionOfAlarmEventFreebusyJournalTodo(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NCAL::UnionParentClass(uri, type) {
    }
    UnionOfAlarmEventFreebusyJournalTodo(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NCAL::UnionParentClass(res, type) {
    }
};
}
}

#endif
