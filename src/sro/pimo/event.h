#ifndef _PIMO_EVENT_H_
#define _PIMO_EVENT_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include <nepomuk2/simpleresource.h>

#include "pimo/processconcept.h"

namespace Nepomuk2 {
namespace PIMO {
/**
 * Something that happens An Event is conceived as compact in time. 
 * (Definition from Merriam-Webster) 
 */
class Event : public virtual PIMO::ProcessConcept
{
public:
    Event(const QUrl& uri = QUrl())
      : SimpleResource(uri), PIMO::ClassOrThingOrPropertyOrAssociation(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Event", QUrl::StrictMode)), PIMO::ClassOrThing(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Event", QUrl::StrictMode)), PIMO::Thing(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Event", QUrl::StrictMode)), PIMO::ProcessConcept(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Event", QUrl::StrictMode)) {
    }

    Event(const SimpleResource& res)
      : SimpleResource(res), PIMO::ClassOrThingOrPropertyOrAssociation(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Event", QUrl::StrictMode)), PIMO::ClassOrThing(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Event", QUrl::StrictMode)), PIMO::Thing(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Event", QUrl::StrictMode)), PIMO::ProcessConcept(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Event", QUrl::StrictMode)) {
    }

    Event& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Event", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.example.com/nbib#eventPublication. 
     * the publications that fornm the result of an event 
     */
    QList<QUrl> eventPublications() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.example.com/nbib#eventPublication", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#eventPublication. 
     * the publications that fornm the result of an event 
     */
    void setEventPublications(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#eventPublication", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#eventPublication. 
     * the publications that fornm the result of an event 
     */
    void addEventPublication(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#eventPublication", QUrl::StrictMode), value);
    }

protected:
    Event(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), PIMO::ClassOrThingOrPropertyOrAssociation(uri, type), PIMO::ClassOrThing(uri, type), PIMO::Thing(uri, type), PIMO::ProcessConcept(uri, type) {
    }
    Event(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), PIMO::ClassOrThingOrPropertyOrAssociation(res, type), PIMO::ClassOrThing(res, type), PIMO::Thing(res, type), PIMO::ProcessConcept(res, type) {
    }
};
}
}

#endif
