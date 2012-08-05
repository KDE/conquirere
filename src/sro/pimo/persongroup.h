#ifndef _PIMO_PERSONGROUP_H_
#define _PIMO_PERSONGROUP_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include <nepomuk2/simpleresource.h>

#include "pimo/agent.h"
#include "pimo/collection.h"

namespace Nepomuk2 {
namespace PIMO {
/**
 * A group of Persons. They are connected to each other by sharing 
 * a common attribute, for example they all belong to the same organization 
 * or have a common interest. Refer to pimo:Collection for more 
 * information about defining collections. 
 */
class PersonGroup : public virtual PIMO::Agent, public virtual PIMO::Collection
{
public:
    PersonGroup(const QUrl& uri = QUrl())
      : SimpleResource(uri), PIMO::ClassOrThingOrPropertyOrAssociation(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#PersonGroup", QUrl::StrictMode)), PIMO::ClassOrThing(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#PersonGroup", QUrl::StrictMode)), PIMO::Thing(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#PersonGroup", QUrl::StrictMode)), PIMO::Agent(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#PersonGroup", QUrl::StrictMode)), PIMO::Collection(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#PersonGroup", QUrl::StrictMode)) {
    }

    PersonGroup(const SimpleResource& res)
      : SimpleResource(res), PIMO::ClassOrThingOrPropertyOrAssociation(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#PersonGroup", QUrl::StrictMode)), PIMO::ClassOrThing(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#PersonGroup", QUrl::StrictMode)), PIMO::Thing(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#PersonGroup", QUrl::StrictMode)), PIMO::Agent(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#PersonGroup", QUrl::StrictMode)), PIMO::Collection(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#PersonGroup", QUrl::StrictMode)) {
    }

    PersonGroup& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#PersonGroup", QUrl::StrictMode));
        return *this;
    }

protected:
    PersonGroup(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), PIMO::ClassOrThingOrPropertyOrAssociation(uri, type), PIMO::ClassOrThing(uri, type), PIMO::Thing(uri, type), PIMO::Agent(uri, type), PIMO::Collection(uri, type) {
    }
    PersonGroup(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), PIMO::ClassOrThingOrPropertyOrAssociation(res, type), PIMO::ClassOrThing(res, type), PIMO::Thing(res, type), PIMO::Agent(res, type), PIMO::Collection(res, type) {
    }
};
}
}

#endif
