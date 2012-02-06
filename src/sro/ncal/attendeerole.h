#ifndef _NCAL_ATTENDEEROLE_H_
#define _NCAL_ATTENDEEROLE_H_

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
 * A role the attendee is going to play during an event. This class 
 * has been introduced to express the limited vocabulary for the 
 * values of ncal:role property. Please refer to the documentation 
 * of ncal:role for details. 
 */
class AttendeeRole : public virtual Nepomuk::SimpleResource
{
public:
    AttendeeRole(const QUrl& uri = QUrl())
      : SimpleResource(uri) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#AttendeeRole", QUrl::StrictMode));
    }

    AttendeeRole(const SimpleResource& res)
      : SimpleResource(res) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#AttendeeRole", QUrl::StrictMode));
    }

    AttendeeRole& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#AttendeeRole", QUrl::StrictMode));
        return *this;
    }

protected:
    AttendeeRole(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri) {
        addType(type);
    }
    AttendeeRole(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res) {
        addType(type);
    }
};
}
}

#endif
