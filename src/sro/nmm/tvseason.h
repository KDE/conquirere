#ifndef _NMM_TVSEASON_H_
#define _NMM_TVSEASON_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include <nepomuk2/simpleresource.h>

#include "nie/informationelement.h"

namespace Nepomuk2 {
namespace NMM {
/**
 * A season of a TV Show 
 */
class TVSeason : public virtual NIE::InformationElement
{
public:
    TVSeason(const QUrl& uri = QUrl())
      : SimpleResource(uri), NIE::InformationElement(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2009/02/19/nmm#TVSeason", QUrl::StrictMode)) {
    }

    TVSeason(const SimpleResource& res)
      : SimpleResource(res), NIE::InformationElement(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2009/02/19/nmm#TVSeason", QUrl::StrictMode)) {
    }

    TVSeason& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2009/02/19/nmm#TVSeason", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2009/02/19/nmm#seasonOf. 
     * Relates a TV Season to its series 
     */
    QUrl seasonOf() const {
        QUrl value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2009/02/19/nmm#seasonOf", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2009/02/19/nmm#seasonOf", QUrl::StrictMode)).first().value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2009/02/19/nmm#seasonOf. 
     * Relates a TV Season to its series 
     */
    void setSeasonOf(const QUrl& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2009/02/19/nmm#seasonOf", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2009/02/19/nmm#seasonOf. 
     * Relates a TV Season to its series 
     */
    void addSeasonOf(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2009/02/19/nmm#seasonOf", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2009/02/19/nmm#hasSeasonEpisode. 
     * Relates a TV Show season to its episodes 
     */
    QList<QUrl> seasonEpisodes() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2009/02/19/nmm#hasSeasonEpisode", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2009/02/19/nmm#hasSeasonEpisode. 
     * Relates a TV Show season to its episodes 
     */
    void setSeasonEpisodes(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2009/02/19/nmm#hasSeasonEpisode", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2009/02/19/nmm#hasSeasonEpisode. 
     * Relates a TV Show season to its episodes 
     */
    void addSeasonEpisode(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2009/02/19/nmm#hasSeasonEpisode", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2009/02/19/nmm#seasonNumber. 
     * The number of a season 
     */
    qint64 seasonNumber() const {
        qint64 value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2009/02/19/nmm#seasonNumber", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2009/02/19/nmm#seasonNumber", QUrl::StrictMode)).first().value<qint64>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2009/02/19/nmm#seasonNumber. 
     * The number of a season 
     */
    void setSeasonNumber(const qint64& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2009/02/19/nmm#seasonNumber", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2009/02/19/nmm#seasonNumber. 
     * The number of a season 
     */
    void addSeasonNumber(const qint64& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2009/02/19/nmm#seasonNumber", QUrl::StrictMode), value);
    }

protected:
    TVSeason(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NIE::InformationElement(uri, type) {
    }
    TVSeason(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NIE::InformationElement(res, type) {
    }
};
}
}

#endif
