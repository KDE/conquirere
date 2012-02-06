#ifndef _NCAL_TIMETRANSPARENCY_H_
#define _NCAL_TIMETRANSPARENCY_H_

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
 * Time transparency. Introduced to provide a way to express the 
 * limited vocabulary for the values of ncal:transp property. 
 * See documentation of ncal:transp for details. 
 */
class TimeTransparency : public virtual Nepomuk::SimpleResource
{
public:
    TimeTransparency(const QUrl& uri = QUrl())
      : SimpleResource(uri) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#TimeTransparency", QUrl::StrictMode));
    }

    TimeTransparency(const SimpleResource& res)
      : SimpleResource(res) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#TimeTransparency", QUrl::StrictMode));
    }

    TimeTransparency& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#TimeTransparency", QUrl::StrictMode));
        return *this;
    }

protected:
    TimeTransparency(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri) {
        addType(type);
    }
    TimeTransparency(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res) {
        addType(type);
    }
};
}
}

#endif
