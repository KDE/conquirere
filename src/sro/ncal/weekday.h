#ifndef _NCAL_WEEKDAY_H_
#define _NCAL_WEEKDAY_H_

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
 * Day of the week. This class has been created to provide the limited 
 * vocabulary for ncal:byday property. See the documentation 
 * for ncal:byday for details. 
 */
class Weekday : public virtual Nepomuk::SimpleResource
{
public:
    Weekday(const QUrl& uri = QUrl())
      : SimpleResource(uri) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Weekday", QUrl::StrictMode));
    }

    Weekday(const SimpleResource& res)
      : SimpleResource(res) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Weekday", QUrl::StrictMode));
    }

    Weekday& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Weekday", QUrl::StrictMode));
        return *this;
    }

protected:
    Weekday(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri) {
        addType(type);
    }
    Weekday(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res) {
        addType(type);
    }
};
}
}

#endif
