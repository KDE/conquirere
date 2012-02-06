#ifndef _NCAL_NCALTIMEENTITY_H_
#define _NCAL_NCALTIMEENTITY_H_

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
 * A time entity. Conceived as a common superclass for NcalDateTime 
 * and NcalPeriod. According to RFC 2445 both DateTime and Period 
 * can be interpreted in different timezones. The first case is 
 * explored in many properties. The second case is theoretically 
 * possible in ncal:rdate property. Therefore the timezone properties 
 * have been defined at this level. 
 */
class NcalTimeEntity : public virtual Nepomuk::SimpleResource
{
public:
    NcalTimeEntity(const QUrl& uri = QUrl())
      : SimpleResource(uri) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#NcalTimeEntity", QUrl::StrictMode));
    }

    NcalTimeEntity(const SimpleResource& res)
      : SimpleResource(res) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#NcalTimeEntity", QUrl::StrictMode));
    }

    NcalTimeEntity& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#NcalTimeEntity", QUrl::StrictMode));
        return *this;
    }

protected:
    NcalTimeEntity(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri) {
        addType(type);
    }
    NcalTimeEntity(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res) {
        addType(type);
    }
};
}
}

#endif
