#ifndef _NBIB_NEWSPAPERISSUE_H_
#define _NBIB_NEWSPAPERISSUE_H_

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
 * A single issue of a published Newspaper 
 */
class NewspaperIssue : public virtual NBIB::Collection
{
public:
    NewspaperIssue(const QUrl& uri = QUrl())
      : SimpleResource(uri), NIE::InformationElement(uri, QUrl::fromEncoded("http://www.example.com/nbib#NewspaperIssue", QUrl::StrictMode)), NBIB::Publication(uri, QUrl::fromEncoded("http://www.example.com/nbib#NewspaperIssue", QUrl::StrictMode)), NBIB::Collection(uri, QUrl::fromEncoded("http://www.example.com/nbib#NewspaperIssue", QUrl::StrictMode)) {
    }

    NewspaperIssue(const SimpleResource& res)
      : SimpleResource(res), NIE::InformationElement(res, QUrl::fromEncoded("http://www.example.com/nbib#NewspaperIssue", QUrl::StrictMode)), NBIB::Publication(res, QUrl::fromEncoded("http://www.example.com/nbib#NewspaperIssue", QUrl::StrictMode)), NBIB::Collection(res, QUrl::fromEncoded("http://www.example.com/nbib#NewspaperIssue", QUrl::StrictMode)) {
    }

    NewspaperIssue& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.example.com/nbib#NewspaperIssue", QUrl::StrictMode));
        return *this;
    }

protected:
    NewspaperIssue(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NIE::InformationElement(uri, type), NBIB::Publication(uri, type), NBIB::Collection(uri, type) {
    }
    NewspaperIssue(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NIE::InformationElement(res, type), NBIB::Publication(res, type), NBIB::Collection(res, type) {
    }
};
}
}

#endif
