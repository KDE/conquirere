#ifndef _NBIB_NEWSPAPER_H_
#define _NBIB_NEWSPAPER_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include "dms-copy/simpleresource.h"

#include "nbib/series.h"

namespace Nepomuk {
namespace NBIB {
/**
 * A Newspaper which publish several NewspaperIssue containing 
 * several articles 
 */
class Newspaper : public virtual NBIB::Series
{
public:
    Newspaper(const QUrl& uri = QUrl())
      : SimpleResource(uri), NIE::InformationElement(uri, QUrl::fromEncoded("http://www.example.com/nbib#Newspaper", QUrl::StrictMode)), NBIB::Series(uri, QUrl::fromEncoded("http://www.example.com/nbib#Newspaper", QUrl::StrictMode)) {
    }

    Newspaper(const SimpleResource& res)
      : SimpleResource(res), NIE::InformationElement(res, QUrl::fromEncoded("http://www.example.com/nbib#Newspaper", QUrl::StrictMode)), NBIB::Series(res, QUrl::fromEncoded("http://www.example.com/nbib#Newspaper", QUrl::StrictMode)) {
    }

    Newspaper& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.example.com/nbib#Newspaper", QUrl::StrictMode));
        return *this;
    }

protected:
    Newspaper(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NIE::InformationElement(uri, type), NBIB::Series(uri, type) {
    }
    Newspaper(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NIE::InformationElement(res, type), NBIB::Series(res, type) {
    }
};
}
}

#endif
