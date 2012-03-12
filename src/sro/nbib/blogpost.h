#ifndef _NBIB_BLOGPOST_H_
#define _NBIB_BLOGPOST_H_

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
 * Reference the post in a web log (blog) 
 */
class BlogPost : public virtual NBIB::Article
{
public:
    BlogPost(const QUrl& uri = QUrl())
      : SimpleResource(uri), NIE::InformationElement(uri, QUrl::fromEncoded("http://www.example.com/nbib#BlogPost", QUrl::StrictMode)), NBIB::Publication(uri, QUrl::fromEncoded("http://www.example.com/nbib#BlogPost", QUrl::StrictMode)), NBIB::Article(uri, QUrl::fromEncoded("http://www.example.com/nbib#BlogPost", QUrl::StrictMode)) {
    }

    BlogPost(const SimpleResource& res)
      : SimpleResource(res), NIE::InformationElement(res, QUrl::fromEncoded("http://www.example.com/nbib#BlogPost", QUrl::StrictMode)), NBIB::Publication(res, QUrl::fromEncoded("http://www.example.com/nbib#BlogPost", QUrl::StrictMode)), NBIB::Article(res, QUrl::fromEncoded("http://www.example.com/nbib#BlogPost", QUrl::StrictMode)) {
    }

    BlogPost& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.example.com/nbib#BlogPost", QUrl::StrictMode));
        return *this;
    }

protected:
    BlogPost(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NIE::InformationElement(uri, type), NBIB::Publication(uri, type), NBIB::Article(uri, type) {
    }
    BlogPost(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NIE::InformationElement(res, type), NBIB::Publication(res, type), NBIB::Article(res, type) {
    }
};
}
}

#endif
