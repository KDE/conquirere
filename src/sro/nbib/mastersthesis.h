#ifndef _NBIB_MASTERSTHESIS_H_
#define _NBIB_MASTERSTHESIS_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include "dms-copy/simpleresource.h"

#include "nbib/thesis.h"

namespace Nepomuk {
namespace NBIB {
/**
 * A Master's thesis. 
 */
class MastersThesis : public virtual NBIB::Thesis
{
public:
    MastersThesis(const QUrl& uri = QUrl())
      : SimpleResource(uri), NBIB::Publication(uri, QUrl::fromEncoded("http://www.example.com/nbib#MastersThesis", QUrl::StrictMode)), NBIB::Thesis(uri, QUrl::fromEncoded("http://www.example.com/nbib#MastersThesis", QUrl::StrictMode)) {
    }

    MastersThesis(const SimpleResource& res)
      : SimpleResource(res), NBIB::Publication(res, QUrl::fromEncoded("http://www.example.com/nbib#MastersThesis", QUrl::StrictMode)), NBIB::Thesis(res, QUrl::fromEncoded("http://www.example.com/nbib#MastersThesis", QUrl::StrictMode)) {
    }

    MastersThesis& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.example.com/nbib#MastersThesis", QUrl::StrictMode));
        return *this;
    }

protected:
    MastersThesis(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NBIB::Publication(uri, type), NBIB::Thesis(uri, type) {
    }
    MastersThesis(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NBIB::Publication(res, type), NBIB::Thesis(res, type) {
    }
};
}
}

#endif
