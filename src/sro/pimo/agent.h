#ifndef _PIMO_AGENT_H_
#define _PIMO_AGENT_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include "dms-copy/simpleresource.h"

#include "pimo/thing.h"

namespace Nepomuk {
namespace PIMO {
/**
 * An agent (eg. person, group, software or physical artifact). 
 * The Agent class is the class of agents; things that do stuff. 
 * A well known sub-class is Person, representing people. Other 
 * kinds of agents include Organization and Group. (inspired 
 * by FOAF). Agent is not a subclass of NAO:Party. 
 */
class Agent : public virtual PIMO::Thing
{
public:
    Agent(const QUrl& uri = QUrl())
      : SimpleResource(uri), PIMO::ClassOrThingOrPropertyOrAssociation(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Agent", QUrl::StrictMode)), PIMO::ClassOrThing(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Agent", QUrl::StrictMode)), PIMO::Thing(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Agent", QUrl::StrictMode)) {
    }

    Agent(const SimpleResource& res)
      : SimpleResource(res), PIMO::ClassOrThingOrPropertyOrAssociation(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Agent", QUrl::StrictMode)), PIMO::ClassOrThing(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Agent", QUrl::StrictMode)), PIMO::Thing(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Agent", QUrl::StrictMode)) {
    }

    Agent& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Agent", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#isOrganizationMemberOf. 
     * The subject person or organozation (Agent) is member of the 
     * object organization. 
     */
    QList<QUrl> isOrganizationMemberOfs() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#isOrganizationMemberOf", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#isOrganizationMemberOf. 
     * The subject person or organozation (Agent) is member of the 
     * object organization. 
     */
    void setIsOrganizationMemberOfs(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#isOrganizationMemberOf", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#isOrganizationMemberOf. 
     * The subject person or organozation (Agent) is member of the 
     * object organization. 
     */
    void addIsOrganizationMemberOf(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#isOrganizationMemberOf", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#createdPimo. 
     * The creator of the Personal Information Model. The human being 
     * whose mental models are represented in the PIMO. 
     */
    QList<QUrl> createdPimos() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#createdPimo", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#createdPimo. 
     * The creator of the Personal Information Model. The human being 
     * whose mental models are represented in the PIMO. 
     */
    void setCreatedPimos(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#createdPimo", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#createdPimo. 
     * The creator of the Personal Information Model. The human being 
     * whose mental models are represented in the PIMO. 
     */
    void addCreatedPimo(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#createdPimo", QUrl::StrictMode), value);
    }

protected:
    Agent(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), PIMO::ClassOrThingOrPropertyOrAssociation(uri, type), PIMO::ClassOrThing(uri, type), PIMO::Thing(uri, type) {
    }
    Agent(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), PIMO::ClassOrThingOrPropertyOrAssociation(res, type), PIMO::ClassOrThing(res, type), PIMO::Thing(res, type) {
    }
};
}
}

#endif
