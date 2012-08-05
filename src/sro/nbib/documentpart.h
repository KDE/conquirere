#ifndef _NBIB_DOCUMENTPART_H_
#define _NBIB_DOCUMENTPART_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include <nepomuk2/simpleresource.h>

#include "nie/informationelement.h"

namespace Nepomuk2 {
namespace NBIB {
/**
 * a part of an document, mostly Chapters but alsow quotes etc. 
 * These don't have to be available as real file extracts but can 
 * also be linked to a real file, if available 
 */
class DocumentPart : public virtual NIE::InformationElement
{
public:
    DocumentPart(const QUrl& uri = QUrl())
      : SimpleResource(uri), NIE::InformationElement(uri, QUrl::fromEncoded("http://www.example.com/nbib#DocumentPart", QUrl::StrictMode)) {
    }

    DocumentPart(const SimpleResource& res)
      : SimpleResource(res), NIE::InformationElement(res, QUrl::fromEncoded("http://www.example.com/nbib#DocumentPart", QUrl::StrictMode)) {
    }

    DocumentPart& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.example.com/nbib#DocumentPart", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.example.com/nbib#documentPartOf. 
     * Connects a part of a document to the publication it is from 
     */
    QUrl documentPartOf() const {
        QUrl value;
        if(contains(QUrl::fromEncoded("http://www.example.com/nbib#documentPartOf", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/nbib#documentPartOf", QUrl::StrictMode)).first().value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#documentPartOf. 
     * Connects a part of a document to the publication it is from 
     */
    void setDocumentPartOf(const QUrl& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#documentPartOf", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#documentPartOf. 
     * Connects a part of a document to the publication it is from 
     */
    void addDocumentPartOf(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#documentPartOf", QUrl::StrictMode), value);
    }

protected:
    DocumentPart(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NIE::InformationElement(uri, type) {
    }
    DocumentPart(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NIE::InformationElement(res, type) {
    }
};
}
}

#endif
