#ifndef _NBIB_MAGAZIN_H_
#define _NBIB_MAGAZIN_H_

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
 * A Magazin which publish several MagazinIssue containing several 
 * articles 
 */
class Magazin : public virtual NBIB::Series
{
public:
    Magazin(const QUrl& uri = QUrl())
      : SimpleResource(uri), NBIB::Series(uri, QUrl::fromEncoded("http://www.example.com/nbib#Magazin", QUrl::StrictMode)) {
    }

    Magazin(const SimpleResource& res)
      : SimpleResource(res), NBIB::Series(res, QUrl::fromEncoded("http://www.example.com/nbib#Magazin", QUrl::StrictMode)) {
    }

    Magazin& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.example.com/nbib#Magazin", QUrl::StrictMode));
        return *this;
    }

protected:
    Magazin(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NBIB::Series(uri, type) {
    }
    Magazin(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NBIB::Series(res, type) {
    }
};
}
}

#endif
