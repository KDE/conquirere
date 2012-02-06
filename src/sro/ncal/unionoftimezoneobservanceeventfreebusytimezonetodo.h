#ifndef _NCAL_UNIONOFTIMEZONEOBSERVANCEEVENTFREEBUSYTIMEZONETODO_H_
#define _NCAL_UNIONOFTIMEZONEOBSERVANCEEVENTFREEBUSYTIMEZONETODO_H_

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
class UnionOfTimezoneObservanceEventFreebusyTimezoneTodo : public virtual NCAL::UnionParentClass
{
public:
    UnionOfTimezoneObservanceEventFreebusyTimezoneTodo(const QUrl& uri = QUrl())
      : SimpleResource(uri), NCAL::UnionParentClass(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#UnionOfTimezoneObservanceEventFreebusyTimezoneTodo", QUrl::StrictMode)) {
    }

    UnionOfTimezoneObservanceEventFreebusyTimezoneTodo(const SimpleResource& res)
      : SimpleResource(res), NCAL::UnionParentClass(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#UnionOfTimezoneObservanceEventFreebusyTimezoneTodo", QUrl::StrictMode)) {
    }

    UnionOfTimezoneObservanceEventFreebusyTimezoneTodo& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#UnionOfTimezoneObservanceEventFreebusyTimezoneTodo", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#dtstart. 
     * This property specifies when the calendar component begins. 
     * Inspired by RFC 2445 sec. 4.8.2.4 
     */
    QUrl dtstart() const {
        QUrl value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#dtstart", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#dtstart", QUrl::StrictMode)).first().value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#dtstart. 
     * This property specifies when the calendar component begins. 
     * Inspired by RFC 2445 sec. 4.8.2.4 
     */
    void setDtstart(const QUrl& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#dtstart", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#dtstart. 
     * This property specifies when the calendar component begins. 
     * Inspired by RFC 2445 sec. 4.8.2.4 
     */
    void addDtstart(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#dtstart", QUrl::StrictMode), value);
    }

protected:
    UnionOfTimezoneObservanceEventFreebusyTimezoneTodo(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NCAL::UnionParentClass(uri, type) {
    }
    UnionOfTimezoneObservanceEventFreebusyTimezoneTodo(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NCAL::UnionParentClass(res, type) {
    }
};
}
}

#endif
