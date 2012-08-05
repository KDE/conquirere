#ifndef _PIMO_BUILDING_H_
#define _PIMO_BUILDING_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include <nepomuk2/simpleresource.h>

#include "pimo/location.h"

namespace Nepomuk2 {
namespace PIMO {
/**
 * A structure that has a roof and walls and stands more or less permanently 
 * in one place; "there was a three-story building on the corner"; 
 * "it was an imposing edifice". (Definition from SUMO). 
 */
class Building : public virtual PIMO::Location
{
public:
    Building(const QUrl& uri = QUrl())
      : SimpleResource(uri), PIMO::ClassOrThingOrPropertyOrAssociation(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Building", QUrl::StrictMode)), PIMO::ClassOrThing(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Building", QUrl::StrictMode)), PIMO::Thing(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Building", QUrl::StrictMode)), PIMO::Location(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Building", QUrl::StrictMode)) {
    }

    Building(const SimpleResource& res)
      : SimpleResource(res), PIMO::ClassOrThingOrPropertyOrAssociation(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Building", QUrl::StrictMode)), PIMO::ClassOrThing(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Building", QUrl::StrictMode)), PIMO::Thing(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Building", QUrl::StrictMode)), PIMO::Location(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Building", QUrl::StrictMode)) {
    }

    Building& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Building", QUrl::StrictMode));
        return *this;
    }

protected:
    Building(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), PIMO::ClassOrThingOrPropertyOrAssociation(uri, type), PIMO::ClassOrThing(uri, type), PIMO::Thing(uri, type), PIMO::Location(uri, type) {
    }
    Building(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), PIMO::ClassOrThingOrPropertyOrAssociation(res, type), PIMO::ClassOrThing(res, type), PIMO::Thing(res, type), PIMO::Location(res, type) {
    }
};
}
}

#endif
