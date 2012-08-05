#ifndef _NCAL_FREEBUSY_H_
#define _NCAL_FREEBUSY_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include <nepomuk2/simpleresource.h>

#include "ncal/unionofeventfreebusyjournaltodo.h"
#include "ncal/unionofeventfreebusy.h"
#include "ncal/unionoftimezoneobservanceeventfreebusyjournaltimezonetodo.h"
#include "nie/informationelement.h"
#include "ncal/unionoftimezoneobservanceeventfreebusytimezonetodo.h"
#include "ncal/unionofalarmeventfreebusytodo.h"
#include "ncal/unionofalarmeventfreebusyjournaltodo.h"

namespace Nepomuk2 {
namespace NCAL {
/**
 * Provide a grouping of component properties that describe either 
 * a request for free/busy time, describe a response to a request 
 * for free/busy time or describe a published set of busy time. 
 */
class Freebusy : public virtual NCAL::UnionOfEventFreebusyJournalTodo, public virtual NCAL::UnionOfEventFreebusy, public virtual NCAL::UnionOfTimezoneObservanceEventFreebusyJournalTimezoneTodo, public virtual NIE::InformationElement, public virtual NCAL::UnionOfTimezoneObservanceEventFreebusyTimezoneTodo, public virtual NCAL::UnionOfAlarmEventFreebusyTodo, public virtual NCAL::UnionOfAlarmEventFreebusyJournalTodo
{
public:
    Freebusy(const QUrl& uri = QUrl())
      : SimpleResource(uri), NCAL::UnionParentClass(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Freebusy", QUrl::StrictMode)), NCAL::UnionOfEventFreebusyJournalTodo(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Freebusy", QUrl::StrictMode)), NCAL::UnionOfEventFreebusy(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Freebusy", QUrl::StrictMode)), NCAL::UnionOfTimezoneObservanceEventFreebusyJournalTimezoneTodo(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Freebusy", QUrl::StrictMode)), NIE::InformationElement(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Freebusy", QUrl::StrictMode)), NCAL::UnionOfTimezoneObservanceEventFreebusyTimezoneTodo(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Freebusy", QUrl::StrictMode)), NCAL::UnionOfAlarmEventFreebusyTodo(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Freebusy", QUrl::StrictMode)), NCAL::UnionOfAlarmEventFreebusyJournalTodo(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Freebusy", QUrl::StrictMode)) {
    }

    Freebusy(const SimpleResource& res)
      : SimpleResource(res), NCAL::UnionParentClass(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Freebusy", QUrl::StrictMode)), NCAL::UnionOfEventFreebusyJournalTodo(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Freebusy", QUrl::StrictMode)), NCAL::UnionOfEventFreebusy(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Freebusy", QUrl::StrictMode)), NCAL::UnionOfTimezoneObservanceEventFreebusyJournalTimezoneTodo(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Freebusy", QUrl::StrictMode)), NIE::InformationElement(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Freebusy", QUrl::StrictMode)), NCAL::UnionOfTimezoneObservanceEventFreebusyTimezoneTodo(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Freebusy", QUrl::StrictMode)), NCAL::UnionOfAlarmEventFreebusyTodo(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Freebusy", QUrl::StrictMode)), NCAL::UnionOfAlarmEventFreebusyJournalTodo(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Freebusy", QUrl::StrictMode)) {
    }

    Freebusy& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Freebusy", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#freebusy. 
     * The property defines one or more free or busy time intervals. 
     * Inspired by RFC 2445 sec. 4.8.2.6. Note that the periods specified 
     * by this property can only be expressed with UTC times. Originally 
     * this property could have many comma-separated values. Please 
     * use a separate triple for each value. 
     */
    QList<QUrl> freebusys() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#freebusy", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#freebusy. 
     * The property defines one or more free or busy time intervals. 
     * Inspired by RFC 2445 sec. 4.8.2.6. Note that the periods specified 
     * by this property can only be expressed with UTC times. Originally 
     * this property could have many comma-separated values. Please 
     * use a separate triple for each value. 
     */
    void setFreebusys(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#freebusy", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#freebusy. 
     * The property defines one or more free or busy time intervals. 
     * Inspired by RFC 2445 sec. 4.8.2.6. Note that the periods specified 
     * by this property can only be expressed with UTC times. Originally 
     * this property could have many comma-separated values. Please 
     * use a separate triple for each value. 
     */
    void addFreebusy(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#freebusy", QUrl::StrictMode), value);
    }

protected:
    Freebusy(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NCAL::UnionParentClass(uri, type), NCAL::UnionOfEventFreebusyJournalTodo(uri, type), NCAL::UnionOfEventFreebusy(uri, type), NCAL::UnionOfTimezoneObservanceEventFreebusyJournalTimezoneTodo(uri, type), NIE::InformationElement(uri, type), NCAL::UnionOfTimezoneObservanceEventFreebusyTimezoneTodo(uri, type), NCAL::UnionOfAlarmEventFreebusyTodo(uri, type), NCAL::UnionOfAlarmEventFreebusyJournalTodo(uri, type) {
    }
    Freebusy(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NCAL::UnionParentClass(res, type), NCAL::UnionOfEventFreebusyJournalTodo(res, type), NCAL::UnionOfEventFreebusy(res, type), NCAL::UnionOfTimezoneObservanceEventFreebusyJournalTimezoneTodo(res, type), NIE::InformationElement(res, type), NCAL::UnionOfTimezoneObservanceEventFreebusyTimezoneTodo(res, type), NCAL::UnionOfAlarmEventFreebusyTodo(res, type), NCAL::UnionOfAlarmEventFreebusyJournalTodo(res, type) {
    }
};
}
}

#endif
