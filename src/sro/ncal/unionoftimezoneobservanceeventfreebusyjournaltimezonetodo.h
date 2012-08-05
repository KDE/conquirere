#ifndef _NCAL_UNIONOFTIMEZONEOBSERVANCEEVENTFREEBUSYJOURNALTIMEZONETODO_H_
#define _NCAL_UNIONOFTIMEZONEOBSERVANCEEVENTFREEBUSYJOURNALTIMEZONETODO_H_

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
class UnionOfTimezoneObservanceEventFreebusyJournalTimezoneTodo : public virtual NCAL::UnionParentClass
{
public:
    UnionOfTimezoneObservanceEventFreebusyJournalTimezoneTodo(const QUrl& uri = QUrl())
      : SimpleResource(uri), NCAL::UnionParentClass(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#UnionOfTimezoneObservanceEventFreebusyJournalTimezoneTodo", QUrl::StrictMode)) {
    }

    UnionOfTimezoneObservanceEventFreebusyJournalTimezoneTodo(const SimpleResource& res)
      : SimpleResource(res), NCAL::UnionParentClass(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#UnionOfTimezoneObservanceEventFreebusyJournalTimezoneTodo", QUrl::StrictMode)) {
    }

    UnionOfTimezoneObservanceEventFreebusyJournalTimezoneTodo& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#UnionOfTimezoneObservanceEventFreebusyJournalTimezoneTodo", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#commentAltRep. 
     * Alternate representation of the comment. Introduced to cover 
     * the ALTREP parameter of the COMMENT property. See documentation 
     * of ncal:comment for details. 
     */
    QList<QUrl> commentAltReps() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#commentAltRep", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#commentAltRep. 
     * Alternate representation of the comment. Introduced to cover 
     * the ALTREP parameter of the COMMENT property. See documentation 
     * of ncal:comment for details. 
     */
    void setCommentAltReps(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#commentAltRep", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#commentAltRep. 
     * Alternate representation of the comment. Introduced to cover 
     * the ALTREP parameter of the COMMENT property. See documentation 
     * of ncal:comment for details. 
     */
    void addCommentAltRep(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#commentAltRep", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#comment. 
     * Non-processing information intended to provide a comment 
     * to the calendar user. Inspired by RFC 2445 sec. 4.8.1.4 with 
     * the following reservations: the LANGUAGE parameter has been 
     * discarded. Please use xml:lang literals to express language. 
     * For the ALTREP parameter use the commentAltRep property. 
     */
    QStringList comments() const {
        QStringList value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#comment", QUrl::StrictMode)))
            value << v.value<QString>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#comment. 
     * Non-processing information intended to provide a comment 
     * to the calendar user. Inspired by RFC 2445 sec. 4.8.1.4 with 
     * the following reservations: the LANGUAGE parameter has been 
     * discarded. Please use xml:lang literals to express language. 
     * For the ALTREP parameter use the commentAltRep property. 
     */
    void setComments(const QStringList& value) {
        QVariantList values;
        foreach(const QString& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#comment", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#comment. 
     * Non-processing information intended to provide a comment 
     * to the calendar user. Inspired by RFC 2445 sec. 4.8.1.4 with 
     * the following reservations: the LANGUAGE parameter has been 
     * discarded. Please use xml:lang literals to express language. 
     * For the ALTREP parameter use the commentAltRep property. 
     */
    void addComment(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#comment", QUrl::StrictMode), value);
    }

protected:
    UnionOfTimezoneObservanceEventFreebusyJournalTimezoneTodo(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NCAL::UnionParentClass(uri, type) {
    }
    UnionOfTimezoneObservanceEventFreebusyJournalTimezoneTodo(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NCAL::UnionParentClass(res, type) {
    }
};
}
}

#endif
