#ifndef _NBIB_COLLECTION_H_
#define _NBIB_COLLECTION_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include <nepomuk2/simpleresource.h>

#include "publication.h"

namespace Nepomuk2 {
namespace NBIB {
/**
 * A collection of different publications. 
 */
class Collection : public virtual NBIB::Publication
{
public:
    Collection(const QUrl& uri = QUrl())
      : SimpleResource(uri), NIE::InformationElement(uri, QUrl::fromEncoded("http://www.example.com/nbib#Collection", QUrl::StrictMode)), NBIB::Publication(uri, QUrl::fromEncoded("http://www.example.com/nbib#Collection", QUrl::StrictMode)) {
    }

    Collection(const SimpleResource& res)
      : SimpleResource(res), NIE::InformationElement(res, QUrl::fromEncoded("http://www.example.com/nbib#Collection", QUrl::StrictMode)), NBIB::Publication(res, QUrl::fromEncoded("http://www.example.com/nbib#Collection", QUrl::StrictMode)) {
    }

    Collection& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.example.com/nbib#Collection", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.example.com/nbib#article. Connects 
     * an article to its collection (article in Newspaper, Proceedings, 
     * Journalissue) 
     */
    QList<QUrl> articles() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.example.com/nbib#article", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#article. Connects 
     * an article to its collection (article in Newspaper, Proceedings, 
     * Journalissue) 
     */
    void setArticles(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#article", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#article. 
     * Connects an article to its collection (article in Newspaper, 
     * Proceedings, Journalissue) 
     */
    void addArticle(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#article", QUrl::StrictMode), value);
    }

protected:
    Collection(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NIE::InformationElement(uri, type), NBIB::Publication(uri, type) {
    }
    Collection(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NIE::InformationElement(res, type), NBIB::Publication(res, type) {
    }
};
}
}

#endif
