#ifndef _NBIB_DICTIONARY_H_
#define _NBIB_DICTIONARY_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include "dms-copy/simpleresource.h"

#include "nbib/book.h"

namespace Nepomuk {
namespace NBIB {
/**
 * A dictionary 
 */
class Dictionary : public virtual NBIB::Book
{
public:
    Dictionary(const QUrl& uri = QUrl())
      : SimpleResource(uri), NIE::InformationElement(uri, QUrl::fromEncoded("http://www.example.com/nbib#Dictionary", QUrl::StrictMode)), NBIB::Publication(uri, QUrl::fromEncoded("http://www.example.com/nbib#Dictionary", QUrl::StrictMode)), NBIB::Book(uri, QUrl::fromEncoded("http://www.example.com/nbib#Dictionary", QUrl::StrictMode)) {
    }

    Dictionary(const SimpleResource& res)
      : SimpleResource(res), NIE::InformationElement(res, QUrl::fromEncoded("http://www.example.com/nbib#Dictionary", QUrl::StrictMode)), NBIB::Publication(res, QUrl::fromEncoded("http://www.example.com/nbib#Dictionary", QUrl::StrictMode)), NBIB::Book(res, QUrl::fromEncoded("http://www.example.com/nbib#Dictionary", QUrl::StrictMode)) {
    }

    Dictionary& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.example.com/nbib#Dictionary", QUrl::StrictMode));
        return *this;
    }

protected:
    Dictionary(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NIE::InformationElement(uri, type), NBIB::Publication(uri, type), NBIB::Book(uri, type) {
    }
    Dictionary(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NIE::InformationElement(res, type), NBIB::Publication(res, type), NBIB::Book(res, type) {
    }
};
}
}

#endif
