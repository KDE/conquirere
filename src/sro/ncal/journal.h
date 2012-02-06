#ifndef _NCAL_JOURNAL_H_
#define _NCAL_JOURNAL_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include "dms-copy/simpleresource.h"

#include "ncal/unionofeventfreebusyjournaltodo.h"
#include "ncal/unionofalarmeventjournaltodo.h"
#include "ncal/unionofeventjournaltodo.h"
#include "ncal/unionofeventjournaltimezonetodo.h"
#include "ncal/unionoftimezoneobservanceeventjournaltimezonetodo.h"
#include "ncal/unionoftimezoneobservanceeventfreebusyjournaltimezonetodo.h"
#include "ncal/unionofalarmeventfreebusyjournaltodo.h"

namespace Nepomuk {
namespace NCAL {
/**
 * Provide a grouping of component properties that describe a 
 * journal entry. 
 */
class Journal : public virtual NCAL::UnionOfEventFreebusyJournalTodo, public virtual NCAL::UnionOfAlarmEventJournalTodo, public virtual NCAL::UnionOfEventJournalTodo, public virtual NCAL::UnionOfEventJournalTimezoneTodo, public virtual NCAL::UnionOfTimezoneObservanceEventJournalTimezoneTodo, public virtual NCAL::UnionOfTimezoneObservanceEventFreebusyJournalTimezoneTodo, public virtual NCAL::UnionOfAlarmEventFreebusyJournalTodo
{
public:
    Journal(const QUrl& uri = QUrl())
      : SimpleResource(uri), NCAL::UnionParentClass(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Journal", QUrl::StrictMode)), NCAL::UnionOfEventFreebusyJournalTodo(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Journal", QUrl::StrictMode)), NCAL::UnionOfAlarmEventJournalTodo(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Journal", QUrl::StrictMode)), NCAL::UnionOfEventJournalTodo(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Journal", QUrl::StrictMode)), NCAL::UnionOfEventJournalTimezoneTodo(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Journal", QUrl::StrictMode)), NCAL::UnionOfTimezoneObservanceEventJournalTimezoneTodo(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Journal", QUrl::StrictMode)), NCAL::UnionOfTimezoneObservanceEventFreebusyJournalTimezoneTodo(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Journal", QUrl::StrictMode)), NCAL::UnionOfAlarmEventFreebusyJournalTodo(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Journal", QUrl::StrictMode)) {
    }

    Journal(const SimpleResource& res)
      : SimpleResource(res), NCAL::UnionParentClass(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Journal", QUrl::StrictMode)), NCAL::UnionOfEventFreebusyJournalTodo(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Journal", QUrl::StrictMode)), NCAL::UnionOfAlarmEventJournalTodo(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Journal", QUrl::StrictMode)), NCAL::UnionOfEventJournalTodo(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Journal", QUrl::StrictMode)), NCAL::UnionOfEventJournalTimezoneTodo(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Journal", QUrl::StrictMode)), NCAL::UnionOfTimezoneObservanceEventJournalTimezoneTodo(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Journal", QUrl::StrictMode)), NCAL::UnionOfTimezoneObservanceEventFreebusyJournalTimezoneTodo(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Journal", QUrl::StrictMode)), NCAL::UnionOfAlarmEventFreebusyJournalTodo(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Journal", QUrl::StrictMode)) {
    }

    Journal& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Journal", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#journalStatus. 
     * Defines the overall status or confirmation for a journal entry. 
     * Based on the STATUS property defined in RFC 2445 sec. 4.8.1.11. 
     */
    QUrl journalStatus() const {
        QUrl value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#journalStatus", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#journalStatus", QUrl::StrictMode)).first().value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#journalStatus. 
     * Defines the overall status or confirmation for a journal entry. 
     * Based on the STATUS property defined in RFC 2445 sec. 4.8.1.11. 
     */
    void setJournalStatus(const QUrl& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#journalStatus", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#journalStatus. 
     * Defines the overall status or confirmation for a journal entry. 
     * Based on the STATUS property defined in RFC 2445 sec. 4.8.1.11. 
     */
    void addJournalStatus(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#journalStatus", QUrl::StrictMode), value);
    }

protected:
    Journal(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NCAL::UnionParentClass(uri, type), NCAL::UnionOfEventFreebusyJournalTodo(uri, type), NCAL::UnionOfAlarmEventJournalTodo(uri, type), NCAL::UnionOfEventJournalTodo(uri, type), NCAL::UnionOfEventJournalTimezoneTodo(uri, type), NCAL::UnionOfTimezoneObservanceEventJournalTimezoneTodo(uri, type), NCAL::UnionOfTimezoneObservanceEventFreebusyJournalTimezoneTodo(uri, type), NCAL::UnionOfAlarmEventFreebusyJournalTodo(uri, type) {
    }
    Journal(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NCAL::UnionParentClass(res, type), NCAL::UnionOfEventFreebusyJournalTodo(res, type), NCAL::UnionOfAlarmEventJournalTodo(res, type), NCAL::UnionOfEventJournalTodo(res, type), NCAL::UnionOfEventJournalTimezoneTodo(res, type), NCAL::UnionOfTimezoneObservanceEventJournalTimezoneTodo(res, type), NCAL::UnionOfTimezoneObservanceEventFreebusyJournalTimezoneTodo(res, type), NCAL::UnionOfAlarmEventFreebusyJournalTodo(res, type) {
    }
};
}
}

#endif
