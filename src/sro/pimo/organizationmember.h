#ifndef _PIMO_ORGANIZATIONMEMBER_H_
#define _PIMO_ORGANIZATIONMEMBER_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include "dms-copy/simpleresource.h"

#include "pimo/personrole.h"

namespace Nepomuk {
namespace PIMO {
/**
 * The role of one or multiple persons being a member in one or multiple 
 * organizations. Use pimo:organization and pimo:roleHolder 
 * to link to the organizations and persons. 
 */
class OrganizationMember : public virtual PIMO::PersonRole
{
public:
    OrganizationMember(const QUrl& uri = QUrl())
      : SimpleResource(uri), PIMO::ClassOrThingOrPropertyOrAssociation(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#OrganizationMember", QUrl::StrictMode)), PIMO::Association(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#OrganizationMember", QUrl::StrictMode)), PIMO::PersonRole(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#OrganizationMember", QUrl::StrictMode)) {
    }

    OrganizationMember(const SimpleResource& res)
      : SimpleResource(res), PIMO::ClassOrThingOrPropertyOrAssociation(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#OrganizationMember", QUrl::StrictMode)), PIMO::Association(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#OrganizationMember", QUrl::StrictMode)), PIMO::PersonRole(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#OrganizationMember", QUrl::StrictMode)) {
    }

    OrganizationMember& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#OrganizationMember", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#organization. 
     * relation to the organization in an OrganizationMember association. 
     */
    QList<QUrl> organizations() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#organization", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#organization. 
     * relation to the organization in an OrganizationMember association. 
     */
    void setOrganizations(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#organization", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#organization. 
     * relation to the organization in an OrganizationMember association. 
     */
    void addOrganization(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#organization", QUrl::StrictMode), value);
    }

protected:
    OrganizationMember(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), PIMO::ClassOrThingOrPropertyOrAssociation(uri, type), PIMO::Association(uri, type), PIMO::PersonRole(uri, type) {
    }
    OrganizationMember(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), PIMO::ClassOrThingOrPropertyOrAssociation(res, type), PIMO::Association(res, type), PIMO::PersonRole(res, type) {
    }
};
}
}

#endif
