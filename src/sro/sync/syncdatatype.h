#ifndef _SYNC_SYNCDATATYPE_H_
#define _SYNC_SYNCDATATYPE_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include "dms-copy/simpleresource.h"

namespace Nepomuk {
namespace SYNC {
/**
 * What kind of syncdata we represent here. Help to find deleted 
 * resource thus if a ServerSyncData that represents an attachment 
 * but has no valid resource set fpr sync:attachment means we deleted 
 * it locally. On the next online storage sync we remove it also 
 * from the server. When the resource is valid we can check if the 
 * etag changed and update it 
 */
class SyncDataType : public virtual Nepomuk::SimpleResource
{
public:
    SyncDataType(const QUrl& uri = QUrl())
      : SimpleResource(uri) {
        addType(QUrl::fromEncoded("http://www.example.com/sync#SyncDataType", QUrl::StrictMode));
    }

    SyncDataType(const SimpleResource& res)
      : SimpleResource(res) {
        addType(QUrl::fromEncoded("http://www.example.com/sync#SyncDataType", QUrl::StrictMode));
    }

    SyncDataType& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.example.com/sync#SyncDataType", QUrl::StrictMode));
        return *this;
    }

protected:
    SyncDataType(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri) {
        addType(type);
    }
    SyncDataType(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res) {
        addType(type);
    }
};
}
}

#endif
