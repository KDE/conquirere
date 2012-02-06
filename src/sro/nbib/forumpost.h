#ifndef _NBIB_FORUMPOST_H_
#define _NBIB_FORUMPOST_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include "dms-copy/simpleresource.h"

#include "nbib/article.h"

namespace Nepomuk {
namespace NBIB {
/**
 * Reference the post in a forum or other electronic list 
 */
class ForumPost : public virtual NBIB::Article
{
public:
    ForumPost(const QUrl& uri = QUrl())
      : SimpleResource(uri), NBIB::Publication(uri, QUrl::fromEncoded("http://www.example.com/nbib#ForumPost", QUrl::StrictMode)), NBIB::Article(uri, QUrl::fromEncoded("http://www.example.com/nbib#ForumPost", QUrl::StrictMode)) {
    }

    ForumPost(const SimpleResource& res)
      : SimpleResource(res), NBIB::Publication(res, QUrl::fromEncoded("http://www.example.com/nbib#ForumPost", QUrl::StrictMode)), NBIB::Article(res, QUrl::fromEncoded("http://www.example.com/nbib#ForumPost", QUrl::StrictMode)) {
    }

    ForumPost& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.example.com/nbib#ForumPost", QUrl::StrictMode));
        return *this;
    }

protected:
    ForumPost(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NBIB::Publication(uri, type), NBIB::Article(uri, type) {
    }
    ForumPost(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NBIB::Publication(res, type), NBIB::Article(res, type) {
    }
};
}
}

#endif
