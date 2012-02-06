#ifndef _NMM_MOVIE_H_
#define _NMM_MOVIE_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include "dms-copy/simpleresource.h"

namespace Nepomuk {
namespace NMM {
/**
 * A Movie 
 */
class Movie : public virtual Nepomuk::SimpleResource
{
public:
    Movie(const QUrl& uri = QUrl())
      : SimpleResource(uri) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2009/02/19/nmm#Movie", QUrl::StrictMode));
    }

    Movie(const SimpleResource& res)
      : SimpleResource(res) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2009/02/19/nmm#Movie", QUrl::StrictMode));
    }

    Movie& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2009/02/19/nmm#Movie", QUrl::StrictMode));
        return *this;
    }

protected:
    Movie(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri) {
        addType(type);
    }
    Movie(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res) {
        addType(type);
    }
};
}
}

#endif
