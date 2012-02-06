#ifndef _NFO_VIDEO_H_
#define _NFO_VIDEO_H_

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
 * A video file. 
 */
class Video : public virtual NFO::Visual
{
public:
    Video(const QUrl& uri = QUrl())
      : SimpleResource(uri), NFO::Media(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Video", QUrl::StrictMode)), NFO::Visual(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Video", QUrl::StrictMode)) {
    }

    Video(const SimpleResource& res)
      : SimpleResource(res), NFO::Media(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Video", QUrl::StrictMode)), NFO::Visual(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Video", QUrl::StrictMode)) {
    }

    Video& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Video", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#frameCount. 
     * The amount of frames in a video sequence. 
     */
    qint64 frameCount() const {
        qint64 value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#frameCount", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#frameCount", QUrl::StrictMode)).first().value<qint64>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#frameCount. 
     * The amount of frames in a video sequence. 
     */
    void setFrameCount(const qint64& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#frameCount", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#frameCount. 
     * The amount of frames in a video sequence. 
     */
    void addFrameCount(const qint64& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#frameCount", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#frameRate. 
     * Amount of video frames per second. 
     */
    double frameRate() const {
        double value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#frameRate", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#frameRate", QUrl::StrictMode)).first().value<double>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#frameRate. 
     * Amount of video frames per second. 
     */
    void setFrameRate(const double& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#frameRate", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#frameRate. 
     * Amount of video frames per second. 
     */
    void addFrameRate(const double& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#frameRate", QUrl::StrictMode), value);
    }

protected:
    Video(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NFO::Media(uri, type), NFO::Visual(uri, type) {
    }
    Video(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NFO::Media(res, type), NFO::Visual(res, type) {
    }
};
}
}

#endif
