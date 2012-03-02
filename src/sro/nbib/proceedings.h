#ifndef _NBIB_PROCEEDINGS_H_
#define _NBIB_PROCEEDINGS_H_

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
 * The proceedings of a conference. 
 */
class Proceedings : public virtual NBIB::Collection
{
public:
    Proceedings(const QUrl& uri = QUrl())
      : SimpleResource(uri), NIE::InformationElement(uri, QUrl::fromEncoded("http://www.example.com/nbib#Proceedings", QUrl::StrictMode)), NBIB::Publication(uri, QUrl::fromEncoded("http://www.example.com/nbib#Proceedings", QUrl::StrictMode)), NBIB::Collection(uri, QUrl::fromEncoded("http://www.example.com/nbib#Proceedings", QUrl::StrictMode)) {
    }

    Proceedings(const SimpleResource& res)
      : SimpleResource(res), NIE::InformationElement(res, QUrl::fromEncoded("http://www.example.com/nbib#Proceedings", QUrl::StrictMode)), NBIB::Publication(res, QUrl::fromEncoded("http://www.example.com/nbib#Proceedings", QUrl::StrictMode)), NBIB::Collection(res, QUrl::fromEncoded("http://www.example.com/nbib#Proceedings", QUrl::StrictMode)) {
    }

    Proceedings& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.example.com/nbib#Proceedings", QUrl::StrictMode));
        return *this;
    }

protected:
    Proceedings(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NIE::InformationElement(uri, type), NBIB::Publication(uri, type), NBIB::Collection(uri, type) {
    }
    Proceedings(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NIE::InformationElement(res, type), NBIB::Publication(res, type), NBIB::Collection(res, type) {
    }
};
}
}

#endif
