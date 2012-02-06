#ifndef _NCAL_UNIONOFALARMEVENTTODO_H_
#define _NCAL_UNIONOFALARMEVENTTODO_H_

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
class UnionOfAlarmEventTodo : public virtual NCAL::UnionParentClass
{
public:
    UnionOfAlarmEventTodo(const QUrl& uri = QUrl())
      : SimpleResource(uri), NCAL::UnionParentClass(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#UnionOfAlarmEventTodo", QUrl::StrictMode)) {
    }

    UnionOfAlarmEventTodo(const SimpleResource& res)
      : SimpleResource(res), NCAL::UnionParentClass(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#UnionOfAlarmEventTodo", QUrl::StrictMode)) {
    }

    UnionOfAlarmEventTodo& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#UnionOfAlarmEventTodo", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#trigger. 
     * This property specifies when an alarm will trigger. Inspired 
     * by RFC 2445 sec. 4.8.6.3 Originally the value of this property 
     * could accept two types : duration and date-time. To express 
     * this fact a Trigger class has been introduced. It also has a related 
     * property to account for the RELATED parameter. 
     */
    QList<QUrl> triggers() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#trigger", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#trigger. 
     * This property specifies when an alarm will trigger. Inspired 
     * by RFC 2445 sec. 4.8.6.3 Originally the value of this property 
     * could accept two types : duration and date-time. To express 
     * this fact a Trigger class has been introduced. It also has a related 
     * property to account for the RELATED parameter. 
     */
    void setTriggers(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#trigger", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#trigger. 
     * This property specifies when an alarm will trigger. Inspired 
     * by RFC 2445 sec. 4.8.6.3 Originally the value of this property 
     * could accept two types : duration and date-time. To express 
     * this fact a Trigger class has been introduced. It also has a related 
     * property to account for the RELATED parameter. 
     */
    void addTrigger(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#trigger", QUrl::StrictMode), value);
    }

protected:
    UnionOfAlarmEventTodo(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NCAL::UnionParentClass(uri, type) {
    }
    UnionOfAlarmEventTodo(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NCAL::UnionParentClass(res, type) {
    }
};
}
}

#endif
