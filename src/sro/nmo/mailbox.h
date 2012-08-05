#ifndef _NMO_MAILBOX_H_
#define _NMO_MAILBOX_H_

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
 * A mailbox - container for MailboxDataObjects. 
 */
class Mailbox : public virtual NIE::InformationElement
{
public:
    Mailbox(const QUrl& uri = QUrl())
      : SimpleResource(uri), NIE::InformationElement(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#Mailbox", QUrl::StrictMode)) {
    }

    Mailbox(const SimpleResource& res)
      : SimpleResource(res), NIE::InformationElement(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#Mailbox", QUrl::StrictMode)) {
    }

    Mailbox& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#Mailbox", QUrl::StrictMode));
        return *this;
    }

protected:
    Mailbox(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NIE::InformationElement(uri, type) {
    }
    Mailbox(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NIE::InformationElement(res, type) {
    }
};
}
}

#endif
