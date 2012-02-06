#ifndef _NCAL_UNIONOFEVENTFREEBUSY_H_
#define _NCAL_UNIONOFEVENTFREEBUSY_H_

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
class UnionOfEventFreebusy : public virtual NCAL::UnionParentClass
{
public:
    UnionOfEventFreebusy(const QUrl& uri = QUrl())
      : SimpleResource(uri), NCAL::UnionParentClass(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#UnionOfEventFreebusy", QUrl::StrictMode)) {
    }

    UnionOfEventFreebusy(const SimpleResource& res)
      : SimpleResource(res), NCAL::UnionParentClass(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#UnionOfEventFreebusy", QUrl::StrictMode)) {
    }

    UnionOfEventFreebusy& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#UnionOfEventFreebusy", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#dtend. 
     * This property specifies the date and time that a calendar component 
     * ends. Inspired by RFC 2445 sec. 4.8.2.2 
     */
    QUrl dtend() const {
        QUrl value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#dtend", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#dtend", QUrl::StrictMode)).first().value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#dtend. 
     * This property specifies the date and time that a calendar component 
     * ends. Inspired by RFC 2445 sec. 4.8.2.2 
     */
    void setDtend(const QUrl& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#dtend", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#dtend. 
     * This property specifies the date and time that a calendar component 
     * ends. Inspired by RFC 2445 sec. 4.8.2.2 
     */
    void addDtend(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#dtend", QUrl::StrictMode), value);
    }

protected:
    UnionOfEventFreebusy(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NCAL::UnionParentClass(uri, type) {
    }
    UnionOfEventFreebusy(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NCAL::UnionParentClass(res, type) {
    }
};
}
}

#endif
