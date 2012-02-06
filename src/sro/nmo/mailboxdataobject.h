#ifndef _NMO_MAILBOXDATAOBJECT_H_
#define _NMO_MAILBOXDATAOBJECT_H_

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
 * An entity encountered in a mailbox. Most common interpretations 
 * for such an entity include Message or Folder 
 */
class MailboxDataObject : public virtual Nepomuk::SimpleResource
{
public:
    MailboxDataObject(const QUrl& uri = QUrl())
      : SimpleResource(uri) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#MailboxDataObject", QUrl::StrictMode));
    }

    MailboxDataObject(const SimpleResource& res)
      : SimpleResource(res) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#MailboxDataObject", QUrl::StrictMode));
    }

    MailboxDataObject& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#MailboxDataObject", QUrl::StrictMode));
        return *this;
    }

protected:
    MailboxDataObject(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri) {
        addType(type);
    }
    MailboxDataObject(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res) {
        addType(type);
    }
};
}
}

#endif
