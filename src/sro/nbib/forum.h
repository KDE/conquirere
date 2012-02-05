#ifndef _NBIB_FORUM_H_
#define _NBIB_FORUM_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include <nepomuk/simpleresource.h>

#include "nbib/collection.h"

namespace Nepomuk {
namespace NBIB {
/**
 * Reference the forum in which several ForumPost can be found 
 */
class Forum : public virtual NBIB::Collection
{
public:
    Forum(const QUrl& uri = QUrl())
      : SimpleResource(uri), NBIB::Publication(uri, QUrl::fromEncoded("http://www.example.com/nbib#Forum", QUrl::StrictMode)), NBIB::Collection(uri, QUrl::fromEncoded("http://www.example.com/nbib#Forum", QUrl::StrictMode)) {
    }

    Forum(const SimpleResource& res)
      : SimpleResource(res), NBIB::Publication(res, QUrl::fromEncoded("http://www.example.com/nbib#Forum", QUrl::StrictMode)), NBIB::Collection(res, QUrl::fromEncoded("http://www.example.com/nbib#Forum", QUrl::StrictMode)) {
    }

    Forum& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.example.com/nbib#Forum", QUrl::StrictMode));
        return *this;
    }

protected:
    Forum(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NBIB::Publication(uri, type), NBIB::Collection(uri, type) {
    }
    Forum(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NBIB::Publication(res, type), NBIB::Collection(res, type) {
    }
};
}
}

#endif
