#ifndef _NCAL_FREEBUSYTYPE_H_
#define _NCAL_FREEBUSYTYPE_H_

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
 * Type of a Freebusy indication. This class has been introduced 
 * to serve as a limited set of values for the ncal:fbtype property. 
 * See the documentation of ncal:fbtype for details. 
 */
class FreebusyType : public virtual Nepomuk::SimpleResource
{
public:
    FreebusyType(const QUrl& uri = QUrl())
      : SimpleResource(uri) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#FreebusyType", QUrl::StrictMode));
    }

    FreebusyType(const SimpleResource& res)
      : SimpleResource(res) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#FreebusyType", QUrl::StrictMode));
    }

    FreebusyType& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#FreebusyType", QUrl::StrictMode));
        return *this;
    }

protected:
    FreebusyType(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri) {
        addType(type);
    }
    FreebusyType(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res) {
        addType(type);
    }
};
}
}

#endif
