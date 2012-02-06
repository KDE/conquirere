#ifndef _NFO_ICON_H_
#define _NFO_ICON_H_

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
 * An Icon (regardless of whether it's a raster or a vector icon. 
 * A resource representing an icon could have two types (Icon and 
 * Raster, or Icon and Vector) if required. 
 */
class Icon : public virtual NFO::Image
{
public:
    Icon(const QUrl& uri = QUrl())
      : SimpleResource(uri), NFO::Media(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Icon", QUrl::StrictMode)), NFO::Visual(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Icon", QUrl::StrictMode)), NFO::Image(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Icon", QUrl::StrictMode)) {
    }

    Icon(const SimpleResource& res)
      : SimpleResource(res), NFO::Media(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Icon", QUrl::StrictMode)), NFO::Visual(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Icon", QUrl::StrictMode)), NFO::Image(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Icon", QUrl::StrictMode)) {
    }

    Icon& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Icon", QUrl::StrictMode));
        return *this;
    }

protected:
    Icon(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NFO::Media(uri, type), NFO::Visual(uri, type), NFO::Image(uri, type) {
    }
    Icon(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NFO::Media(res, type), NFO::Visual(res, type), NFO::Image(res, type) {
    }
};
}
}

#endif
