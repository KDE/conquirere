#ifndef _NMO_MIMEENTITY_H_
#define _NMO_MIMEENTITY_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include <nepomuk2/simpleresource.h>

#include "nie/informationelement.h"

namespace Nepomuk2 {
namespace NMO {
/**
 * A MIME entity, as defined in RFC2045, Section 2.4. 
 */
class MimeEntity : public virtual NIE::InformationElement
{
public:
    MimeEntity(const QUrl& uri = QUrl())
      : SimpleResource(uri), NIE::InformationElement(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#MimeEntity", QUrl::StrictMode)) {
    }

    MimeEntity(const SimpleResource& res)
      : SimpleResource(res), NIE::InformationElement(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#MimeEntity", QUrl::StrictMode)) {
    }

    MimeEntity& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#MimeEntity", QUrl::StrictMode));
        return *this;
    }

protected:
    MimeEntity(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NIE::InformationElement(uri, type) {
    }
    MimeEntity(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NIE::InformationElement(res, type) {
    }
};
}
}

#endif
