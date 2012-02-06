#ifndef _NBIB_DOCUMENTSTATUS_H_
#define _NBIB_DOCUMENTSTATUS_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include "dms-copy/simpleresource.h"

namespace Nepomuk {
namespace NBIB {
/**
 * The status of the publication of a document. 
 */
class DocumentStatus : public virtual Nepomuk::SimpleResource
{
public:
    DocumentStatus(const QUrl& uri = QUrl())
      : SimpleResource(uri) {
        addType(QUrl::fromEncoded("http://www.example.com/nbib#DocumentStatus", QUrl::StrictMode));
    }

    DocumentStatus(const SimpleResource& res)
      : SimpleResource(res) {
        addType(QUrl::fromEncoded("http://www.example.com/nbib#DocumentStatus", QUrl::StrictMode));
    }

    DocumentStatus& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.example.com/nbib#DocumentStatus", QUrl::StrictMode));
        return *this;
    }

protected:
    DocumentStatus(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri) {
        addType(type);
    }
    DocumentStatus(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res) {
        addType(type);
    }
};
}
}

#endif
