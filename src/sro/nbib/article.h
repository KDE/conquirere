#ifndef _NBIB_ARTICLE_H_
#define _NBIB_ARTICLE_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include "dms-copy/simpleresource.h"

#include "nbib/publication.h"

namespace Nepomuk {
namespace NBIB {
/**
 * An article from a journal or magazine. 
 */
class Article : public virtual NBIB::Publication
{
public:
    Article(const QUrl& uri = QUrl())
      : SimpleResource(uri), NIE::InformationElement(uri, QUrl::fromEncoded("http://www.example.com/nbib#Article", QUrl::StrictMode)), NBIB::Publication(uri, QUrl::fromEncoded("http://www.example.com/nbib#Article", QUrl::StrictMode)) {
    }

    Article(const SimpleResource& res)
      : SimpleResource(res), NIE::InformationElement(res, QUrl::fromEncoded("http://www.example.com/nbib#Article", QUrl::StrictMode)), NBIB::Publication(res, QUrl::fromEncoded("http://www.example.com/nbib#Article", QUrl::StrictMode)) {
    }

    Article& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.example.com/nbib#Article", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.example.com/nbib#collection. 
     * The collection this article was published in (for example a 
     * Journalissue, Newspaper or Proceedings 
     */
    QUrl collection() const {
        QUrl value;
        if(contains(QUrl::fromEncoded("http://www.example.com/nbib#collection", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/nbib#collection", QUrl::StrictMode)).first().value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#collection. 
     * The collection this article was published in (for example a 
     * Journalissue, Newspaper or Proceedings 
     */
    void setCollection(const QUrl& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#collection", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#collection. 
     * The collection this article was published in (for example a 
     * Journalissue, Newspaper or Proceedings 
     */
    void addCollection(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#collection", QUrl::StrictMode), value);
    }

protected:
    Article(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NIE::InformationElement(uri, type), NBIB::Publication(uri, type) {
    }
    Article(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NIE::InformationElement(res, type), NBIB::Publication(res, type) {
    }
};
}
}

#endif
