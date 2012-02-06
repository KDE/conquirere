#ifndef _NCAL_EVENTSTATUS_H_
#define _NCAL_EVENTSTATUS_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include "dms-copy/simpleresource.h"

namespace Nepomuk {
namespace NCAL {
/**
 * A status of an event. This class has been introduced to express 
 * the limited set of values for the ncal:status property. The 
 * user may use the instances provided with this ontology or create 
 * his/her own. See the documentation for ncal:eventStatus for 
 * details. 
 */
class EventStatus : public virtual Nepomuk::SimpleResource
{
public:
    EventStatus(const QUrl& uri = QUrl())
      : SimpleResource(uri) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#EventStatus", QUrl::StrictMode));
    }

    EventStatus(const SimpleResource& res)
      : SimpleResource(res) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#EventStatus", QUrl::StrictMode));
    }

    EventStatus& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#EventStatus", QUrl::StrictMode));
        return *this;
    }

protected:
    EventStatus(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri) {
        addType(type);
    }
    EventStatus(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res) {
        addType(type);
    }
};
}
}

#endif
