#ifndef _PIMO_PERSONROLE_H_
#define _PIMO_PERSONROLE_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include "dms-copy/simpleresource.h"

#include "pimo/association.h"

namespace Nepomuk {
namespace PIMO {
/**
 * A person takes a certain role in a given context. The role can 
 * be that of "a mentor or another person" or "giving a talk at a meeting", 
 * etc. 
 */
class PersonRole : public virtual PIMO::Association
{
public:
    PersonRole(const QUrl& uri = QUrl())
      : SimpleResource(uri), PIMO::ClassOrThingOrPropertyOrAssociation(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#PersonRole", QUrl::StrictMode)), PIMO::Association(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#PersonRole", QUrl::StrictMode)) {
    }

    PersonRole(const SimpleResource& res)
      : SimpleResource(res), PIMO::ClassOrThingOrPropertyOrAssociation(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#PersonRole", QUrl::StrictMode)), PIMO::Association(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#PersonRole", QUrl::StrictMode)) {
    }

    PersonRole& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#PersonRole", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#roleContext. 
     * The context where the role-holder impersonates this role. 
     * For example, the company where a person is employed. 
     */
    QList<QUrl> roleContexts() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#roleContext", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#roleContext. 
     * The context where the role-holder impersonates this role. 
     * For example, the company where a person is employed. 
     */
    void setRoleContexts(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#roleContext", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#roleContext. 
     * The context where the role-holder impersonates this role. 
     * For example, the company where a person is employed. 
     */
    void addRoleContext(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#roleContext", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#roleHolder. 
     * the person taking the role 
     */
    QList<QUrl> roleHolders() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#roleHolder", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#roleHolder. 
     * the person taking the role 
     */
    void setRoleHolders(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#roleHolder", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#roleHolder. 
     * the person taking the role 
     */
    void addRoleHolder(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#roleHolder", QUrl::StrictMode), value);
    }

protected:
    PersonRole(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), PIMO::ClassOrThingOrPropertyOrAssociation(uri, type), PIMO::Association(uri, type) {
    }
    PersonRole(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), PIMO::ClassOrThingOrPropertyOrAssociation(res, type), PIMO::Association(res, type) {
    }
};
}
}

#endif
