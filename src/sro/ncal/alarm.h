#ifndef _NCAL_ALARM_H_
#define _NCAL_ALARM_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include "dms-copy/simpleresource.h"

#include "ncal/unionofalarmeventfreebusytodo.h"
#include "ncal/unionofalarmeventjournaltodo.h"
#include "ncal/unionofalarmeventfreebusyjournaltodo.h"
#include "ncal/unionofalarmeventtodo.h"

namespace Nepomuk {
namespace NCAL {
/**
 * Provide a grouping of component properties that define an alarm. 
 */
class Alarm : public virtual NCAL::UnionOfAlarmEventFreebusyTodo, public virtual NCAL::UnionOfAlarmEventJournalTodo, public virtual NCAL::UnionOfAlarmEventFreebusyJournalTodo, public virtual NCAL::UnionOfAlarmEventTodo
{
public:
    Alarm(const QUrl& uri = QUrl())
      : SimpleResource(uri), NCAL::UnionParentClass(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Alarm", QUrl::StrictMode)), NCAL::UnionOfAlarmEventFreebusyTodo(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Alarm", QUrl::StrictMode)), NCAL::UnionOfAlarmEventJournalTodo(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Alarm", QUrl::StrictMode)), NCAL::UnionOfAlarmEventFreebusyJournalTodo(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Alarm", QUrl::StrictMode)), NCAL::UnionOfAlarmEventTodo(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Alarm", QUrl::StrictMode)) {
    }

    Alarm(const SimpleResource& res)
      : SimpleResource(res), NCAL::UnionParentClass(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Alarm", QUrl::StrictMode)), NCAL::UnionOfAlarmEventFreebusyTodo(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Alarm", QUrl::StrictMode)), NCAL::UnionOfAlarmEventJournalTodo(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Alarm", QUrl::StrictMode)), NCAL::UnionOfAlarmEventFreebusyJournalTodo(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Alarm", QUrl::StrictMode)), NCAL::UnionOfAlarmEventTodo(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Alarm", QUrl::StrictMode)) {
    }

    Alarm& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Alarm", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#repeat. 
     * This property defines the number of time the alarm should be 
     * repeated, after the initial trigger. Inspired by RFC 2445 sec. 
     * 4.8.6.2 
     */
    qint64 repeat() const {
        qint64 value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#repeat", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#repeat", QUrl::StrictMode)).first().value<qint64>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#repeat. 
     * This property defines the number of time the alarm should be 
     * repeated, after the initial trigger. Inspired by RFC 2445 sec. 
     * 4.8.6.2 
     */
    void setRepeat(const qint64& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#repeat", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#repeat. 
     * This property defines the number of time the alarm should be 
     * repeated, after the initial trigger. Inspired by RFC 2445 sec. 
     * 4.8.6.2 
     */
    void addRepeat(const qint64& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#repeat", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#action. 
     * This property defines the action to be invoked when an alarm 
     * is triggered. Inspired by RFC 2445 sec 4.8.6.1. Originally 
     * this property had a limited set of values. They are expressed 
     * as instances of the AlarmAction class. 
     */
    QList<QUrl> actions() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#action", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#action. 
     * This property defines the action to be invoked when an alarm 
     * is triggered. Inspired by RFC 2445 sec 4.8.6.1. Originally 
     * this property had a limited set of values. They are expressed 
     * as instances of the AlarmAction class. 
     */
    void setActions(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#action", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#action. 
     * This property defines the action to be invoked when an alarm 
     * is triggered. Inspired by RFC 2445 sec 4.8.6.1. Originally 
     * this property had a limited set of values. They are expressed 
     * as instances of the AlarmAction class. 
     */
    void addAction(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#action", QUrl::StrictMode), value);
    }

protected:
    Alarm(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NCAL::UnionParentClass(uri, type), NCAL::UnionOfAlarmEventFreebusyTodo(uri, type), NCAL::UnionOfAlarmEventJournalTodo(uri, type), NCAL::UnionOfAlarmEventFreebusyJournalTodo(uri, type), NCAL::UnionOfAlarmEventTodo(uri, type) {
    }
    Alarm(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NCAL::UnionParentClass(res, type), NCAL::UnionOfAlarmEventFreebusyTodo(res, type), NCAL::UnionOfAlarmEventJournalTodo(res, type), NCAL::UnionOfAlarmEventFreebusyJournalTodo(res, type), NCAL::UnionOfAlarmEventTodo(res, type) {
    }
};
}
}

#endif
