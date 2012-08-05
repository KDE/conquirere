#ifndef _NCAL_UNIONOFALARMEVENTFREEBUSYTODO_H_
#define _NCAL_UNIONOFALARMEVENTFREEBUSYTODO_H_

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
class UnionOfAlarmEventFreebusyTodo : public virtual NCAL::UnionParentClass
{
public:
    UnionOfAlarmEventFreebusyTodo(const QUrl& uri = QUrl())
      : SimpleResource(uri), NCAL::UnionParentClass(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#UnionOfAlarmEventFreebusyTodo", QUrl::StrictMode)) {
    }

    UnionOfAlarmEventFreebusyTodo(const SimpleResource& res)
      : SimpleResource(res), NCAL::UnionParentClass(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#UnionOfAlarmEventFreebusyTodo", QUrl::StrictMode)) {
    }

    UnionOfAlarmEventFreebusyTodo& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#UnionOfAlarmEventFreebusyTodo", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#duration. 
     * The property specifies a positive duration of time. Inspired 
     * by RFC 2445 sec. 4.8.2.5 
     */
    QUrl duration() const {
        QUrl value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#duration", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#duration", QUrl::StrictMode)).first().value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#duration. 
     * The property specifies a positive duration of time. Inspired 
     * by RFC 2445 sec. 4.8.2.5 
     */
    void setDuration(const QUrl& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#duration", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#duration. 
     * The property specifies a positive duration of time. Inspired 
     * by RFC 2445 sec. 4.8.2.5 
     */
    void addDuration(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#duration", QUrl::StrictMode), value);
    }

protected:
    UnionOfAlarmEventFreebusyTodo(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NCAL::UnionParentClass(uri, type) {
    }
    UnionOfAlarmEventFreebusyTodo(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NCAL::UnionParentClass(res, type) {
    }
};
}
}

#endif
