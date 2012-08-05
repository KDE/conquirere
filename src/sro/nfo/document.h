#ifndef _NFO_DOCUMENT_H_
#define _NFO_DOCUMENT_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include <nepomuk2/simpleresource.h>

#include "nie/informationelement.h"

namespace Nepomuk2 {
namespace NFO {
/**
 * A generic document. A common superclass for all documents on 
 * the desktop. 
 */
class Document : public virtual NIE::InformationElement
{
public:
    Document(const QUrl& uri = QUrl())
      : SimpleResource(uri), NIE::InformationElement(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Document", QUrl::StrictMode)) {
    }

    Document(const SimpleResource& res)
      : SimpleResource(res), NIE::InformationElement(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Document", QUrl::StrictMode)) {
    }

    Document& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Document", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.example.com/nbib#publishedAs. 
     * Links a document to its publication information. 
     */
    QList<QUrl> publishedAses() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.example.com/nbib#publishedAs", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#publishedAs. 
     * Links a document to its publication information. 
     */
    void setPublishedAses(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#publishedAs", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#publishedAs. 
     * Links a document to its publication information. 
     */
    void addPublishedAs(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#publishedAs", QUrl::StrictMode), value);
    }

protected:
    Document(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NIE::InformationElement(uri, type) {
    }
    Document(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NIE::InformationElement(res, type) {
    }
};
}
}

#endif
