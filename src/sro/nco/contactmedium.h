#ifndef _NCO_CONTACTMEDIUM_H_
#define _NCO_CONTACTMEDIUM_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include "dms-copy/simpleresource.h"

namespace Nepomuk {
namespace NCO {
/**
 * A superclass for all contact media - ways to contact an entity 
 * represented by a Contact instance. Some of the subclasses of 
 * this class (the various kinds of telephone numbers and postal 
 * addresses) have been inspired by the values of the TYPE parameter 
 * of ADR and TEL properties defined in RFC 2426 sec. 3.2.1. and 
 * 3.3.1 respectively. Each value is represented by an appropriate 
 * subclass with two major exceptions TYPE=home and TYPE=work. 
 * They are to be expressed by the roles these contact media are 
 * attached to i.e. contact media with TYPE=home parameter are 
 * to be attached to the default role (nco:Contact or nco:PersonContact), 
 * whereas media with TYPE=work parameter should be attached 
 * to nco:Affiliation or nco:OrganizationContact. 
 */
class ContactMedium : public virtual Nepomuk::SimpleResource
{
public:
    ContactMedium(const QUrl& uri = QUrl())
      : SimpleResource(uri) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#ContactMedium", QUrl::StrictMode));
    }

    ContactMedium(const SimpleResource& res)
      : SimpleResource(res) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#ContactMedium", QUrl::StrictMode));
    }

    ContactMedium& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#ContactMedium", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/03/22/nco#contactMediumComment. 
     * A comment about the contact medium. (Deprecated in favor of 
     * nie:comment or nao:description - based on the context) 
     */
    QStringList contactMediumComments() const {
        QStringList value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#contactMediumComment", QUrl::StrictMode)))
            value << v.value<QString>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/03/22/nco#contactMediumComment. 
     * A comment about the contact medium. (Deprecated in favor of 
     * nie:comment or nao:description - based on the context) 
     */
    void setContactMediumComments(const QStringList& value) {
        QVariantList values;
        foreach(const QString& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#contactMediumComment", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/03/22/nco#contactMediumComment. 
     * A comment about the contact medium. (Deprecated in favor of 
     * nie:comment or nao:description - based on the context) 
     */
    void addContactMediumComment(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#contactMediumComment", QUrl::StrictMode), value);
    }

protected:
    ContactMedium(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri) {
        addType(type);
    }
    ContactMedium(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res) {
        addType(type);
    }
};
}
}

#endif