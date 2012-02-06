#ifndef _NMM_TVSERIES_H_
#define _NMM_TVSERIES_H_

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
 * A TV Series has multiple seasons and episodes 
 */
class TVSeries : public virtual Nepomuk::SimpleResource
{
public:
    TVSeries(const QUrl& uri = QUrl())
      : SimpleResource(uri) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2009/02/19/nmm#TVSeries", QUrl::StrictMode));
    }

    TVSeries(const SimpleResource& res)
      : SimpleResource(res) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2009/02/19/nmm#TVSeries", QUrl::StrictMode));
    }

    TVSeries& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2009/02/19/nmm#TVSeries", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2009/02/19/nmm#hasEpisode. 
     * A TVSeries has many episodes 
     */
    QList<QUrl> episodes() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2009/02/19/nmm#hasEpisode", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2009/02/19/nmm#hasEpisode. 
     * A TVSeries has many episodes 
     */
    void setEpisodes(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2009/02/19/nmm#hasEpisode", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2009/02/19/nmm#hasEpisode. 
     * A TVSeries has many episodes 
     */
    void addEpisode(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2009/02/19/nmm#hasEpisode", QUrl::StrictMode), value);
    }

protected:
    TVSeries(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri) {
        addType(type);
    }
    TVSeries(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res) {
        addType(type);
    }
};
}
}

#endif
