#ifndef _NCAL_TODO_H_
#define _NCAL_TODO_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include <nepomuk2/simpleresource.h>

#include "ncal/unionofalarmeventtodo.h"
#include "ncal/unionofeventfreebusyjournaltodo.h"
#include "ncal/unionofalarmeventjournaltodo.h"
#include "ncal/unionofeventjournaltodo.h"
#include "ncal/unionofeventtodo.h"
#include "nie/informationelement.h"
#include "ncal/unionoftimezoneobservanceeventfreebusytimezonetodo.h"
#include "ncal/unionofeventjournaltimezonetodo.h"
#include "ncal/unionoftimezoneobservanceeventjournaltimezonetodo.h"
#include "ncal/unionofalarmeventfreebusytodo.h"
#include "ncal/unionofalarmeventfreebusyjournaltodo.h"
#include "ncal/unionoftimezoneobservanceeventfreebusyjournaltimezonetodo.h"

namespace Nepomuk2 {
namespace NCAL {
/**
 * Provide a grouping of calendar properties that describe a to-do. 
 */
class Todo : public virtual NCAL::UnionOfAlarmEventTodo, public virtual NCAL::UnionOfEventFreebusyJournalTodo, public virtual NCAL::UnionOfAlarmEventJournalTodo, public virtual NCAL::UnionOfEventJournalTodo, public virtual NCAL::UnionOfEventTodo, public virtual NIE::InformationElement, public virtual NCAL::UnionOfTimezoneObservanceEventFreebusyTimezoneTodo, public virtual NCAL::UnionOfEventJournalTimezoneTodo, public virtual NCAL::UnionOfTimezoneObservanceEventJournalTimezoneTodo, public virtual NCAL::UnionOfAlarmEventFreebusyTodo, public virtual NCAL::UnionOfAlarmEventFreebusyJournalTodo, public virtual NCAL::UnionOfTimezoneObservanceEventFreebusyJournalTimezoneTodo
{
public:
    Todo(const QUrl& uri = QUrl())
      : SimpleResource(uri), NCAL::UnionParentClass(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Todo", QUrl::StrictMode)), NCAL::UnionOfAlarmEventTodo(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Todo", QUrl::StrictMode)), NCAL::UnionOfEventFreebusyJournalTodo(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Todo", QUrl::StrictMode)), NCAL::UnionOfAlarmEventJournalTodo(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Todo", QUrl::StrictMode)), NCAL::UnionOfEventJournalTodo(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Todo", QUrl::StrictMode)), NCAL::UnionOfEventTodo(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Todo", QUrl::StrictMode)), NIE::InformationElement(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Todo", QUrl::StrictMode)), NCAL::UnionOfTimezoneObservanceEventFreebusyTimezoneTodo(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Todo", QUrl::StrictMode)), NCAL::UnionOfEventJournalTimezoneTodo(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Todo", QUrl::StrictMode)), NCAL::UnionOfTimezoneObservanceEventJournalTimezoneTodo(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Todo", QUrl::StrictMode)), NCAL::UnionOfAlarmEventFreebusyTodo(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Todo", QUrl::StrictMode)), NCAL::UnionOfAlarmEventFreebusyJournalTodo(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Todo", QUrl::StrictMode)), NCAL::UnionOfTimezoneObservanceEventFreebusyJournalTimezoneTodo(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Todo", QUrl::StrictMode)) {
    }

    Todo(const SimpleResource& res)
      : SimpleResource(res), NCAL::UnionParentClass(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Todo", QUrl::StrictMode)), NCAL::UnionOfAlarmEventTodo(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Todo", QUrl::StrictMode)), NCAL::UnionOfEventFreebusyJournalTodo(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Todo", QUrl::StrictMode)), NCAL::UnionOfAlarmEventJournalTodo(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Todo", QUrl::StrictMode)), NCAL::UnionOfEventJournalTodo(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Todo", QUrl::StrictMode)), NCAL::UnionOfEventTodo(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Todo", QUrl::StrictMode)), NIE::InformationElement(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Todo", QUrl::StrictMode)), NCAL::UnionOfTimezoneObservanceEventFreebusyTimezoneTodo(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Todo", QUrl::StrictMode)), NCAL::UnionOfEventJournalTimezoneTodo(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Todo", QUrl::StrictMode)), NCAL::UnionOfTimezoneObservanceEventJournalTimezoneTodo(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Todo", QUrl::StrictMode)), NCAL::UnionOfAlarmEventFreebusyTodo(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Todo", QUrl::StrictMode)), NCAL::UnionOfAlarmEventFreebusyJournalTodo(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Todo", QUrl::StrictMode)), NCAL::UnionOfTimezoneObservanceEventFreebusyJournalTimezoneTodo(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Todo", QUrl::StrictMode)) {
    }

    Todo& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Todo", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#completed. 
     * This property defines the date and time that a to-do was actually 
     * completed. Inspired by RFC 2445 sec. 4.8.2.1. Note that the 
     * RFC allows ONLY UTC time values for this property. 
     */
    QDateTime completed() const {
        QDateTime value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#completed", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#completed", QUrl::StrictMode)).first().value<QDateTime>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#completed. 
     * This property defines the date and time that a to-do was actually 
     * completed. Inspired by RFC 2445 sec. 4.8.2.1. Note that the 
     * RFC allows ONLY UTC time values for this property. 
     */
    void setCompleted(const QDateTime& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#completed", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#completed. 
     * This property defines the date and time that a to-do was actually 
     * completed. Inspired by RFC 2445 sec. 4.8.2.1. Note that the 
     * RFC allows ONLY UTC time values for this property. 
     */
    void addCompleted(const QDateTime& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#completed", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#due. 
     * This property defines the date and time that a to-do is expected 
     * to be completed. Inspired by RFC 2445 sec. 4.8.2.3 
     */
    QUrl due() const {
        QUrl value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#due", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#due", QUrl::StrictMode)).first().value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#due. 
     * This property defines the date and time that a to-do is expected 
     * to be completed. Inspired by RFC 2445 sec. 4.8.2.3 
     */
    void setDue(const QUrl& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#due", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#due. 
     * This property defines the date and time that a to-do is expected 
     * to be completed. Inspired by RFC 2445 sec. 4.8.2.3 
     */
    void addDue(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#due", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#todoStatus. 
     * Defines the overall status or confirmation for a todo. Based 
     * on the STATUS property defined in RFC 2445 sec. 4.8.1.11. 
     */
    QUrl todoStatus() const {
        QUrl value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#todoStatus", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#todoStatus", QUrl::StrictMode)).first().value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#todoStatus. 
     * Defines the overall status or confirmation for a todo. Based 
     * on the STATUS property defined in RFC 2445 sec. 4.8.1.11. 
     */
    void setTodoStatus(const QUrl& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#todoStatus", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#todoStatus. 
     * Defines the overall status or confirmation for a todo. Based 
     * on the STATUS property defined in RFC 2445 sec. 4.8.1.11. 
     */
    void addTodoStatus(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#todoStatus", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#percentComplete. 
     * This property is used by an assignee or delegatee of a to-do to 
     * convey the percent completion of a to-do to the Organizer. Inspired 
     * by RFC 2445 sec. 4.8.1.8 
     */
    QList<qint64> percentCompletes() const {
        QList<qint64> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#percentComplete", QUrl::StrictMode)))
            value << v.value<qint64>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#percentComplete. 
     * This property is used by an assignee or delegatee of a to-do to 
     * convey the percent completion of a to-do to the Organizer. Inspired 
     * by RFC 2445 sec. 4.8.1.8 
     */
    void setPercentCompletes(const QList<qint64>& value) {
        QVariantList values;
        foreach(const qint64& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#percentComplete", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#percentComplete. 
     * This property is used by an assignee or delegatee of a to-do to 
     * convey the percent completion of a to-do to the Organizer. Inspired 
     * by RFC 2445 sec. 4.8.1.8 
     */
    void addPercentComplete(const qint64& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#percentComplete", QUrl::StrictMode), value);
    }

protected:
    Todo(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NCAL::UnionParentClass(uri, type), NCAL::UnionOfAlarmEventTodo(uri, type), NCAL::UnionOfEventFreebusyJournalTodo(uri, type), NCAL::UnionOfAlarmEventJournalTodo(uri, type), NCAL::UnionOfEventJournalTodo(uri, type), NCAL::UnionOfEventTodo(uri, type), NIE::InformationElement(uri, type), NCAL::UnionOfTimezoneObservanceEventFreebusyTimezoneTodo(uri, type), NCAL::UnionOfEventJournalTimezoneTodo(uri, type), NCAL::UnionOfTimezoneObservanceEventJournalTimezoneTodo(uri, type), NCAL::UnionOfAlarmEventFreebusyTodo(uri, type), NCAL::UnionOfAlarmEventFreebusyJournalTodo(uri, type), NCAL::UnionOfTimezoneObservanceEventFreebusyJournalTimezoneTodo(uri, type) {
    }
    Todo(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NCAL::UnionParentClass(res, type), NCAL::UnionOfAlarmEventTodo(res, type), NCAL::UnionOfEventFreebusyJournalTodo(res, type), NCAL::UnionOfAlarmEventJournalTodo(res, type), NCAL::UnionOfEventJournalTodo(res, type), NCAL::UnionOfEventTodo(res, type), NIE::InformationElement(res, type), NCAL::UnionOfTimezoneObservanceEventFreebusyTimezoneTodo(res, type), NCAL::UnionOfEventJournalTimezoneTodo(res, type), NCAL::UnionOfTimezoneObservanceEventJournalTimezoneTodo(res, type), NCAL::UnionOfAlarmEventFreebusyTodo(res, type), NCAL::UnionOfAlarmEventFreebusyJournalTodo(res, type), NCAL::UnionOfTimezoneObservanceEventFreebusyJournalTimezoneTodo(res, type) {
    }
};
}
}

#endif
