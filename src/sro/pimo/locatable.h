#ifndef _PIMO_LOCATABLE_H_
#define _PIMO_LOCATABLE_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include <nepomuk2/simpleresource.h>

#include "pimo/thing.h"

namespace Nepomuk2 {
namespace PIMO {
/**
 * Things that can be at a location. Abstract class, use it as a superclass 
 * of things that can be placed in physical space. 
 */
class Locatable : public virtual PIMO::Thing
{
public:
    Locatable(const QUrl& uri = QUrl())
      : SimpleResource(uri), PIMO::ClassOrThingOrPropertyOrAssociation(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Locatable", QUrl::StrictMode)), PIMO::ClassOrThing(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Locatable", QUrl::StrictMode)), PIMO::Thing(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Locatable", QUrl::StrictMode)) {
    }

    Locatable(const SimpleResource& res)
      : SimpleResource(res), PIMO::ClassOrThingOrPropertyOrAssociation(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Locatable", QUrl::StrictMode)), PIMO::ClassOrThing(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Locatable", QUrl::StrictMode)), PIMO::Thing(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Locatable", QUrl::StrictMode)) {
    }

    Locatable& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Locatable", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#hasLocation. 
     * The subject thing is currently located at the object location. 
     */
    QUrl location() const {
        QUrl value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#hasLocation", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#hasLocation", QUrl::StrictMode)).first().value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#hasLocation. 
     * The subject thing is currently located at the object location. 
     */
    void setLocation(const QUrl& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#hasLocation", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#hasLocation. 
     * The subject thing is currently located at the object location. 
     */
    void addLocation(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#hasLocation", QUrl::StrictMode), value);
    }

protected:
    Locatable(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), PIMO::ClassOrThingOrPropertyOrAssociation(uri, type), PIMO::ClassOrThing(uri, type), PIMO::Thing(uri, type) {
    }
    Locatable(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), PIMO::ClassOrThingOrPropertyOrAssociation(res, type), PIMO::ClassOrThing(res, type), PIMO::Thing(res, type) {
    }
};
}
}

#endif
