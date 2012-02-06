#ifndef _NFO_IMAGE_H_
#define _NFO_IMAGE_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include "dms-copy/simpleresource.h"

#include "nfo/visual.h"

namespace Nepomuk {
namespace NFO {
/**
 * A file containing an image. 
 */
class Image : public virtual NFO::Visual
{
public:
    Image(const QUrl& uri = QUrl())
      : SimpleResource(uri), NFO::Media(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Image", QUrl::StrictMode)), NFO::Visual(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Image", QUrl::StrictMode)) {
    }

    Image(const SimpleResource& res)
      : SimpleResource(res), NFO::Media(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Image", QUrl::StrictMode)), NFO::Visual(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Image", QUrl::StrictMode)) {
    }

    Image& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Image", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#verticalResolution. 
     * Vertical resolution of an Image (if printed). Expressed in 
     * DPI 
     */
    qint64 verticalResolution() const {
        qint64 value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#verticalResolution", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#verticalResolution", QUrl::StrictMode)).first().value<qint64>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#verticalResolution. 
     * Vertical resolution of an Image (if printed). Expressed in 
     * DPI 
     */
    void setVerticalResolution(const qint64& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#verticalResolution", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#verticalResolution. 
     * Vertical resolution of an Image (if printed). Expressed in 
     * DPI 
     */
    void addVerticalResolution(const qint64& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#verticalResolution", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#horizontalResolution. 
     * Horizontal resolution of an image (if printed). Expressed 
     * in DPI. 
     */
    qint64 horizontalResolution() const {
        qint64 value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#horizontalResolution", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#horizontalResolution", QUrl::StrictMode)).first().value<qint64>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#horizontalResolution. 
     * Horizontal resolution of an image (if printed). Expressed 
     * in DPI. 
     */
    void setHorizontalResolution(const qint64& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#horizontalResolution", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#horizontalResolution. 
     * Horizontal resolution of an image (if printed). Expressed 
     * in DPI. 
     */
    void addHorizontalResolution(const qint64& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#horizontalResolution", QUrl::StrictMode), value);
    }

protected:
    Image(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NFO::Media(uri, type), NFO::Visual(uri, type) {
    }
    Image(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NFO::Media(res, type), NFO::Visual(res, type) {
    }
};
}
}

#endif
