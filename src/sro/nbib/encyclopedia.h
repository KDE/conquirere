#ifndef _NBIB_ENCYCLOPEDIA_H_
#define _NBIB_ENCYCLOPEDIA_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include <nepomuk2/simpleresource.h>

#include "nbib/collection.h"

namespace Nepomuk2 {
namespace NBIB {
/**
 * A encyclopedia 
 */
class Encyclopedia : public virtual NBIB::Collection
{
public:
    Encyclopedia(const QUrl& uri = QUrl())
      : SimpleResource(uri), NIE::InformationElement(uri, QUrl::fromEncoded("http://www.example.com/nbib#Encyclopedia", QUrl::StrictMode)), NBIB::Publication(uri, QUrl::fromEncoded("http://www.example.com/nbib#Encyclopedia", QUrl::StrictMode)), NBIB::Collection(uri, QUrl::fromEncoded("http://www.example.com/nbib#Encyclopedia", QUrl::StrictMode)) {
    }

    Encyclopedia(const SimpleResource& res)
      : SimpleResource(res), NIE::InformationElement(res, QUrl::fromEncoded("http://www.example.com/nbib#Encyclopedia", QUrl::StrictMode)), NBIB::Publication(res, QUrl::fromEncoded("http://www.example.com/nbib#Encyclopedia", QUrl::StrictMode)), NBIB::Collection(res, QUrl::fromEncoded("http://www.example.com/nbib#Encyclopedia", QUrl::StrictMode)) {
    }

    Encyclopedia& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.example.com/nbib#Encyclopedia", QUrl::StrictMode));
        return *this;
    }

protected:
    Encyclopedia(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NIE::InformationElement(uri, type), NBIB::Publication(uri, type), NBIB::Collection(uri, type) {
    }
    Encyclopedia(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NIE::InformationElement(res, type), NBIB::Publication(res, type), NBIB::Collection(res, type) {
    }
};
}
}

#endif
