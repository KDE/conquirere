#ifndef _NCAL_EVENT_H_
#define _NCAL_EVENT_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include "dms-copy/simpleresource.h"

#include "ncal/unionofeventtodo.h"
#include "ncal/unionofalarmeventtodo.h"
#include "ncal/unionofeventfreebusyjournaltodo.h"
#include "ncal/unionofeventfreebusy.h"
#include "ncal/unionofalarmeventjournaltodo.h"
#include "ncal/unionofeventjournaltodo.h"
#include "ncal/unionofeventjournaltimezonetodo.h"
#include "ncal/unionofalarmeventfreebusytodo.h"
#include "ncal/unionoftimezoneobservanceeventfreebusytimezonetodo.h"
#include "ncal/unionoftimezoneobservanceeventjournaltimezonetodo.h"
#include "ncal/unionoftimezoneobservanceeventfreebusyjournaltimezonetodo.h"
#include "ncal/unionofalarmeventfreebusyjournaltodo.h"

namespace Nepomuk {
namespace NCAL {
/**
 * Provide a grouping of component properties that describe an 
 * event. 
 */
class Event : public virtual NCAL::UnionOfEventTodo, public virtual NCAL::UnionOfAlarmEventTodo, public virtual NCAL::UnionOfEventFreebusyJournalTodo, public virtual NCAL::UnionOfEventFreebusy, public virtual NCAL::UnionOfAlarmEventJournalTodo, public virtual NCAL::UnionOfEventJournalTodo, public virtual NCAL::UnionOfEventJournalTimezoneTodo, public virtual NCAL::UnionOfAlarmEventFreebusyTodo, public virtual NCAL::UnionOfTimezoneObservanceEventFreebusyTimezoneTodo, public virtual NCAL::UnionOfTimezoneObservanceEventJournalTimezoneTodo, public virtual NCAL::UnionOfTimezoneObservanceEventFreebusyJournalTimezoneTodo, public virtual NCAL::UnionOfAlarmEventFreebusyJournalTodo
{
public:
    Event(const QUrl& uri = QUrl())
      : SimpleResource(uri), NCAL::UnionParentClass(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Event", QUrl::StrictMode)), NCAL::UnionOfEventTodo(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Event", QUrl::StrictMode)), NCAL::UnionOfAlarmEventTodo(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Event", QUrl::StrictMode)), NCAL::UnionOfEventFreebusyJournalTodo(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Event", QUrl::StrictMode)), NCAL::UnionOfEventFreebusy(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Event", QUrl::StrictMode)), NCAL::UnionOfAlarmEventJournalTodo(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Event", QUrl::StrictMode)), NCAL::UnionOfEventJournalTodo(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Event", QUrl::StrictMode)), NCAL::UnionOfEventJournalTimezoneTodo(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Event", QUrl::StrictMode)), NCAL::UnionOfAlarmEventFreebusyTodo(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Event", QUrl::StrictMode)), NCAL::UnionOfTimezoneObservanceEventFreebusyTimezoneTodo(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Event", QUrl::StrictMode)), NCAL::UnionOfTimezoneObservanceEventJournalTimezoneTodo(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Event", QUrl::StrictMode)), NCAL::UnionOfTimezoneObservanceEventFreebusyJournalTimezoneTodo(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Event", QUrl::StrictMode)), NCAL::UnionOfAlarmEventFreebusyJournalTodo(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Event", QUrl::StrictMode)) {
    }

    Event(const SimpleResource& res)
      : SimpleResource(res), NCAL::UnionParentClass(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Event", QUrl::StrictMode)), NCAL::UnionOfEventTodo(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Event", QUrl::StrictMode)), NCAL::UnionOfAlarmEventTodo(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Event", QUrl::StrictMode)), NCAL::UnionOfEventFreebusyJournalTodo(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Event", QUrl::StrictMode)), NCAL::UnionOfEventFreebusy(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Event", QUrl::StrictMode)), NCAL::UnionOfAlarmEventJournalTodo(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Event", QUrl::StrictMode)), NCAL::UnionOfEventJournalTodo(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Event", QUrl::StrictMode)), NCAL::UnionOfEventJournalTimezoneTodo(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Event", QUrl::StrictMode)), NCAL::UnionOfAlarmEventFreebusyTodo(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Event", QUrl::StrictMode)), NCAL::UnionOfTimezoneObservanceEventFreebusyTimezoneTodo(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Event", QUrl::StrictMode)), NCAL::UnionOfTimezoneObservanceEventJournalTimezoneTodo(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Event", QUrl::StrictMode)), NCAL::UnionOfTimezoneObservanceEventFreebusyJournalTimezoneTodo(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Event", QUrl::StrictMode)), NCAL::UnionOfAlarmEventFreebusyJournalTodo(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Event", QUrl::StrictMode)) {
    }

    Event& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Event", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#transp. 
     * Defines whether an event is transparent or not to busy time searches. 
     * Inspired by RFC 2445 sec.4.8.2.7. Values for this property 
     * can be chosen from a limited vocabulary. To express this a TimeTransparency 
     * class has been introduced. 
     */
    QUrl transp() const {
        QUrl value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#transp", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#transp", QUrl::StrictMode)).first().value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#transp. 
     * Defines whether an event is transparent or not to busy time searches. 
     * Inspired by RFC 2445 sec.4.8.2.7. Values for this property 
     * can be chosen from a limited vocabulary. To express this a TimeTransparency 
     * class has been introduced. 
     */
    void setTransp(const QUrl& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#transp", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#transp. 
     * Defines whether an event is transparent or not to busy time searches. 
     * Inspired by RFC 2445 sec.4.8.2.7. Values for this property 
     * can be chosen from a limited vocabulary. To express this a TimeTransparency 
     * class has been introduced. 
     */
    void addTransp(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#transp", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#eventStatus. 
     * Defines the overall status or confirmation for an Event. Based 
     * on the STATUS property defined in RFC 2445 sec. 4.8.1.11. 
     */
    QUrl eventStatus() const {
        QUrl value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#eventStatus", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#eventStatus", QUrl::StrictMode)).first().value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#eventStatus. 
     * Defines the overall status or confirmation for an Event. Based 
     * on the STATUS property defined in RFC 2445 sec. 4.8.1.11. 
     */
    void setEventStatus(const QUrl& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#eventStatus", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#eventStatus. 
     * Defines the overall status or confirmation for an Event. Based 
     * on the STATUS property defined in RFC 2445 sec. 4.8.1.11. 
     */
    void addEventStatus(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#eventStatus", QUrl::StrictMode), value);
    }

protected:
    Event(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NCAL::UnionParentClass(uri, type), NCAL::UnionOfEventTodo(uri, type), NCAL::UnionOfAlarmEventTodo(uri, type), NCAL::UnionOfEventFreebusyJournalTodo(uri, type), NCAL::UnionOfEventFreebusy(uri, type), NCAL::UnionOfAlarmEventJournalTodo(uri, type), NCAL::UnionOfEventJournalTodo(uri, type), NCAL::UnionOfEventJournalTimezoneTodo(uri, type), NCAL::UnionOfAlarmEventFreebusyTodo(uri, type), NCAL::UnionOfTimezoneObservanceEventFreebusyTimezoneTodo(uri, type), NCAL::UnionOfTimezoneObservanceEventJournalTimezoneTodo(uri, type), NCAL::UnionOfTimezoneObservanceEventFreebusyJournalTimezoneTodo(uri, type), NCAL::UnionOfAlarmEventFreebusyJournalTodo(uri, type) {
    }
    Event(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NCAL::UnionParentClass(res, type), NCAL::UnionOfEventTodo(res, type), NCAL::UnionOfAlarmEventTodo(res, type), NCAL::UnionOfEventFreebusyJournalTodo(res, type), NCAL::UnionOfEventFreebusy(res, type), NCAL::UnionOfAlarmEventJournalTodo(res, type), NCAL::UnionOfEventJournalTodo(res, type), NCAL::UnionOfEventJournalTimezoneTodo(res, type), NCAL::UnionOfAlarmEventFreebusyTodo(res, type), NCAL::UnionOfTimezoneObservanceEventFreebusyTimezoneTodo(res, type), NCAL::UnionOfTimezoneObservanceEventJournalTimezoneTodo(res, type), NCAL::UnionOfTimezoneObservanceEventFreebusyJournalTimezoneTodo(res, type), NCAL::UnionOfAlarmEventFreebusyJournalTodo(res, type) {
    }
};
}
}

#endif
