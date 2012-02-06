#ifndef _NMO_MAILBOX_H_
#define _NMO_MAILBOX_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include "dms-copy/simpleresource.h"

namespace Nepomuk {
namespace NMO {
/**
 * A mailbox - container for MailboxDataObjects. 
 */
class Mailbox : public virtual Nepomuk::SimpleResource
{
public:
    Mailbox(const QUrl& uri = QUrl())
      : SimpleResource(uri) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#Mailbox", QUrl::StrictMode));
    }

    Mailbox(const SimpleResource& res)
      : SimpleResource(res) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#Mailbox", QUrl::StrictMode));
    }

    Mailbox& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#Mailbox", QUrl::StrictMode));
        return *this;
    }

protected:
    Mailbox(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri) {
        addType(type);
    }
    Mailbox(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res) {
        addType(type);
    }
};
}
}

#endif
