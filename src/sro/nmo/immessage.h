#ifndef _NMO_IMMESSAGE_H_
#define _NMO_IMMESSAGE_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include "dms-copy/simpleresource.h"

#include "nmo/message.h"

namespace Nepomuk {
namespace NMO {
/**
 * A message sent with Instant Messaging software. 
 */
class IMMessage : public virtual NMO::Message
{
public:
    IMMessage(const QUrl& uri = QUrl())
      : SimpleResource(uri), NMO::Message(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#IMMessage", QUrl::StrictMode)) {
    }

    IMMessage(const SimpleResource& res)
      : SimpleResource(res), NMO::Message(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#IMMessage", QUrl::StrictMode)) {
    }

    IMMessage& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#IMMessage", QUrl::StrictMode));
        return *this;
    }

protected:
    IMMessage(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NMO::Message(uri, type) {
    }
    IMMessage(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NMO::Message(res, type) {
    }
};
}
}

#endif
