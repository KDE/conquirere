#ifndef _NBIB_JOURNALISSUE_H_
#define _NBIB_JOURNALISSUE_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include <nepomuk2/simpleresource.h>

#include "collection.h"

namespace Nepomuk2 {
namespace NBIB {
/**
 * A journal issue for serious, scholarly publications 
 */
class JournalIssue : public virtual NBIB::Collection
{
public:
    JournalIssue(const QUrl& uri = QUrl())
      : SimpleResource(uri), NIE::InformationElement(uri, QUrl::fromEncoded("http://www.example.com/nbib#JournalIssue", QUrl::StrictMode)), NBIB::Publication(uri, QUrl::fromEncoded("http://www.example.com/nbib#JournalIssue", QUrl::StrictMode)), NBIB::Collection(uri, QUrl::fromEncoded("http://www.example.com/nbib#JournalIssue", QUrl::StrictMode)) {
    }

    JournalIssue(const SimpleResource& res)
      : SimpleResource(res), NIE::InformationElement(res, QUrl::fromEncoded("http://www.example.com/nbib#JournalIssue", QUrl::StrictMode)), NBIB::Publication(res, QUrl::fromEncoded("http://www.example.com/nbib#JournalIssue", QUrl::StrictMode)), NBIB::Collection(res, QUrl::fromEncoded("http://www.example.com/nbib#JournalIssue", QUrl::StrictMode)) {
    }

    JournalIssue& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.example.com/nbib#JournalIssue", QUrl::StrictMode));
        return *this;
    }

protected:
    JournalIssue(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NIE::InformationElement(uri, type), NBIB::Publication(uri, type), NBIB::Collection(uri, type) {
    }
    JournalIssue(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NIE::InformationElement(res, type), NBIB::Publication(res, type), NBIB::Collection(res, type) {
    }
};
}
}

#endif
