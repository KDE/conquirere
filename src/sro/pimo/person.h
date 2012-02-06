#ifndef _PIMO_PERSON_H_
#define _PIMO_PERSON_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include "dms-copy/simpleresource.h"

#include "pimo/agent.h"
#include "pimo/locatable.h"

namespace Nepomuk {
namespace PIMO {
/**
 * Represents a person. Either living, dead, real or imaginary. 
 * (Definition from foaf:Person) 
 */
class Person : public virtual PIMO::Agent, public virtual PIMO::Locatable
{
public:
    Person(const QUrl& uri = QUrl())
      : SimpleResource(uri), PIMO::ClassOrThingOrPropertyOrAssociation(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Person", QUrl::StrictMode)), PIMO::ClassOrThing(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Person", QUrl::StrictMode)), PIMO::Thing(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Person", QUrl::StrictMode)), PIMO::Agent(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Person", QUrl::StrictMode)), PIMO::Locatable(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Person", QUrl::StrictMode)) {
    }

    Person(const SimpleResource& res)
      : SimpleResource(res), PIMO::ClassOrThingOrPropertyOrAssociation(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Person", QUrl::StrictMode)), PIMO::ClassOrThing(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Person", QUrl::StrictMode)), PIMO::Thing(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Person", QUrl::StrictMode)), PIMO::Agent(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Person", QUrl::StrictMode)), PIMO::Locatable(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Person", QUrl::StrictMode)) {
    }

    Person& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Person", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#jabberId. 
     * Jabber-ID of the user. Used to communicate amongst peers in 
     * the social scenario of the semantic desktop. Use the xmpp node 
     * identifier as specified by RFC3920, see http://www.xmpp.org/specs/rfc3920.html#addressing-node. 
     * The format is the same as e-mail addresses: username@hostname. 
     */
    QString jabberId() const {
        QString value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#jabberId", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#jabberId", QUrl::StrictMode)).first().value<QString>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#jabberId. 
     * Jabber-ID of the user. Used to communicate amongst peers in 
     * the social scenario of the semantic desktop. Use the xmpp node 
     * identifier as specified by RFC3920, see http://www.xmpp.org/specs/rfc3920.html#addressing-node. 
     * The format is the same as e-mail addresses: username@hostname. 
     */
    void setJabberId(const QString& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#jabberId", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#jabberId. 
     * Jabber-ID of the user. Used to communicate amongst peers in 
     * the social scenario of the semantic desktop. Use the xmpp node 
     * identifier as specified by RFC3920, see http://www.xmpp.org/specs/rfc3920.html#addressing-node. 
     * The format is the same as e-mail addresses: username@hostname. 
     */
    void addJabberId(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#jabberId", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#attends. 
     * A person attends a social event. 
     */
    QList<QUrl> attendses() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#attends", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#attends. 
     * A person attends a social event. 
     */
    void setAttendses(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#attends", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#attends. 
     * A person attends a social event. 
     */
    void addAttends(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#attends", QUrl::StrictMode), value);
    }

protected:
    Person(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), PIMO::ClassOrThingOrPropertyOrAssociation(uri, type), PIMO::ClassOrThing(uri, type), PIMO::Thing(uri, type), PIMO::Agent(uri, type), PIMO::Locatable(uri, type) {
    }
    Person(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), PIMO::ClassOrThingOrPropertyOrAssociation(res, type), PIMO::ClassOrThing(res, type), PIMO::Thing(res, type), PIMO::Agent(res, type), PIMO::Locatable(res, type) {
    }
};
}
}

#endif
