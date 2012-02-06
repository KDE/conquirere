#ifndef _NBIB_MAGAZINISSUE_H_
#define _NBIB_MAGAZINISSUE_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include "dms-copy/simpleresource.h"

#include "nbib/collection.h"

namespace Nepomuk {
namespace NBIB {
/**
 * A periodical of magazine Articles. A magazine is a publication 
 * that is issued periodically, usually bound in a paper cover, 
 * and typically contains essays, stories, poems, etc., by many 
 * writers, and often photographs and drawings, frequently specializing 
 * in a particular subject or area, as hobbies, news, or sports. 
 */
class MagazinIssue : public virtual NBIB::Collection
{
public:
    MagazinIssue(const QUrl& uri = QUrl())
      : SimpleResource(uri), NBIB::Publication(uri, QUrl::fromEncoded("http://www.example.com/nbib#MagazinIssue", QUrl::StrictMode)), NBIB::Collection(uri, QUrl::fromEncoded("http://www.example.com/nbib#MagazinIssue", QUrl::StrictMode)) {
    }

    MagazinIssue(const SimpleResource& res)
      : SimpleResource(res), NBIB::Publication(res, QUrl::fromEncoded("http://www.example.com/nbib#MagazinIssue", QUrl::StrictMode)), NBIB::Collection(res, QUrl::fromEncoded("http://www.example.com/nbib#MagazinIssue", QUrl::StrictMode)) {
    }

    MagazinIssue& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.example.com/nbib#MagazinIssue", QUrl::StrictMode));
        return *this;
    }

protected:
    MagazinIssue(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NBIB::Publication(uri, type), NBIB::Collection(uri, type) {
    }
    MagazinIssue(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NBIB::Publication(res, type), NBIB::Collection(res, type) {
    }
};
}
}

#endif
