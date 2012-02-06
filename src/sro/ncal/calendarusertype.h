#ifndef _NCAL_CALENDARUSERTYPE_H_
#define _NCAL_CALENDARUSERTYPE_H_

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
 * A calendar user type. This class has been introduced to express 
 * the limited vocabulary for the ncal:cutype property. See documentation 
 * of ncal:cutype for details. 
 */
class CalendarUserType : public virtual Nepomuk::SimpleResource
{
public:
    CalendarUserType(const QUrl& uri = QUrl())
      : SimpleResource(uri) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#CalendarUserType", QUrl::StrictMode));
    }

    CalendarUserType(const SimpleResource& res)
      : SimpleResource(res) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#CalendarUserType", QUrl::StrictMode));
    }

    CalendarUserType& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#CalendarUserType", QUrl::StrictMode));
        return *this;
    }

protected:
    CalendarUserType(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri) {
        addType(type);
    }
    CalendarUserType(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res) {
        addType(type);
    }
};
}
}

#endif
