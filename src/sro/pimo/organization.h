#ifndef _PIMO_ORGANIZATION_H_
#define _PIMO_ORGANIZATION_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include <nepomuk2/simpleresource.h>

#include "pimo/agent.h"
#include "pimo/locatable.h"

namespace Nepomuk2 {
namespace PIMO {
/**
 * An administrative and functional structure (as a business 
 * or a political party). (Definition from Merriam-Webster) 
 */
class Organization : public virtual PIMO::Agent, public virtual PIMO::Locatable
{
public:
    Organization(const QUrl& uri = QUrl())
      : SimpleResource(uri), PIMO::ClassOrThingOrPropertyOrAssociation(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Organization", QUrl::StrictMode)), PIMO::ClassOrThing(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Organization", QUrl::StrictMode)), PIMO::Thing(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Organization", QUrl::StrictMode)), PIMO::Agent(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Organization", QUrl::StrictMode)), PIMO::Locatable(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Organization", QUrl::StrictMode)) {
    }

    Organization(const SimpleResource& res)
      : SimpleResource(res), PIMO::ClassOrThingOrPropertyOrAssociation(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Organization", QUrl::StrictMode)), PIMO::ClassOrThing(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Organization", QUrl::StrictMode)), PIMO::Thing(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Organization", QUrl::StrictMode)), PIMO::Agent(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Organization", QUrl::StrictMode)), PIMO::Locatable(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Organization", QUrl::StrictMode)) {
    }

    Organization& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Organization", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#hasOrganizationMember. 
     * The subject organization has the object person or organization 
     * (Agent) as a member. 
     */
    QList<QUrl> organizationMembers() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#hasOrganizationMember", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#hasOrganizationMember. 
     * The subject organization has the object person or organization 
     * (Agent) as a member. 
     */
    void setOrganizationMembers(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#hasOrganizationMember", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#hasOrganizationMember. 
     * The subject organization has the object person or organization 
     * (Agent) as a member. 
     */
    void addOrganizationMember(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#hasOrganizationMember", QUrl::StrictMode), value);
    }

protected:
    Organization(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), PIMO::ClassOrThingOrPropertyOrAssociation(uri, type), PIMO::ClassOrThing(uri, type), PIMO::Thing(uri, type), PIMO::Agent(uri, type), PIMO::Locatable(uri, type) {
    }
    Organization(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), PIMO::ClassOrThingOrPropertyOrAssociation(res, type), PIMO::ClassOrThing(res, type), PIMO::Thing(res, type), PIMO::Agent(res, type), PIMO::Locatable(res, type) {
    }
};
}
}

#endif
