#ifndef _NFO_VECTORIMAGE_H_
#define _NFO_VECTORIMAGE_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include "dms-copy/simpleresource.h"

#include "nfo/image.h"

namespace Nepomuk {
namespace NFO {
/**
 * 
 */
class VectorImage : public virtual NFO::Image
{
public:
    VectorImage(const QUrl& uri = QUrl())
      : SimpleResource(uri), NFO::Media(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#VectorImage", QUrl::StrictMode)), NFO::Visual(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#VectorImage", QUrl::StrictMode)), NFO::Image(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#VectorImage", QUrl::StrictMode)) {
    }

    VectorImage(const SimpleResource& res)
      : SimpleResource(res), NFO::Media(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#VectorImage", QUrl::StrictMode)), NFO::Visual(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#VectorImage", QUrl::StrictMode)), NFO::Image(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#VectorImage", QUrl::StrictMode)) {
    }

    VectorImage& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#VectorImage", QUrl::StrictMode));
        return *this;
    }

protected:
    VectorImage(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NFO::Media(uri, type), NFO::Visual(uri, type), NFO::Image(uri, type) {
    }
    VectorImage(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NFO::Media(res, type), NFO::Visual(res, type), NFO::Image(res, type) {
    }
};
}
}

#endif
