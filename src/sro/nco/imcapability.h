#ifndef _NCO_IMCAPABILITY_H_
#define _NCO_IMCAPABILITY_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include "dms-copy/simpleresource.h"

namespace Nepomuk {
namespace NCO {
/**
 * Capabilities of a cetain IMAccount. 
 */
class IMCapability : public virtual Nepomuk::SimpleResource
{
public:
    IMCapability(const QUrl& uri = QUrl())
      : SimpleResource(uri) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#IMCapability", QUrl::StrictMode));
    }

    IMCapability(const SimpleResource& res)
      : SimpleResource(res) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#IMCapability", QUrl::StrictMode));
    }

    IMCapability& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#IMCapability", QUrl::StrictMode));
        return *this;
    }

protected:
    IMCapability(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri) {
        addType(type);
    }
    IMCapability(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res) {
        addType(type);
    }
};
}
}

#endif
