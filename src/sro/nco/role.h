#ifndef _NCO_ROLE_H_
#define _NCO_ROLE_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include <nepomuk/simpleresource.h>

namespace Nepomuk {
namespace NCO {
/**
 * A role played by a contact. Contacts that denote people, can 
 * have many roles (e.g. see the hasAffiliation property and Affiliation 
 * class). Contacts that denote Organizations or other Agents 
 * usually have one role. Each role can introduce additional contact 
 * media. 
 */
class Role : public virtual Nepomuk::SimpleResource
{
public:
    Role(const QUrl& uri = QUrl())
      : SimpleResource(uri) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#Role", QUrl::StrictMode));
    }

    Role(const SimpleResource& res)
      : SimpleResource(res) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#Role", QUrl::StrictMode));
    }

    Role& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#Role", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/03/22/nco#hasIMAccount. 
     * Indicates that an Instant Messaging account owned by an entity 
     * represented by this contact. 
     */
    QList<QUrl> iMAccounts() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#hasIMAccount", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/03/22/nco#hasIMAccount. 
     * Indicates that an Instant Messaging account owned by an entity 
     * represented by this contact. 
     */
    void setIMAccounts(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#hasIMAccount", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/03/22/nco#hasIMAccount. 
     * Indicates that an Instant Messaging account owned by an entity 
     * represented by this contact. 
     */
    void addIMAccount(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#hasIMAccount", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/03/22/nco#end. 
     * End datetime for the role, such as: the datetime of leaving a 
     * project or organization, datetime of ending employment, datetime 
     * of divorce. If absent or set to a date in the future, the role is 
     * currently active. 
     */
    QDateTime end() const {
        QDateTime value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#end", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#end", QUrl::StrictMode)).first().value<QDateTime>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/03/22/nco#end. 
     * End datetime for the role, such as: the datetime of leaving a 
     * project or organization, datetime of ending employment, datetime 
     * of divorce. If absent or set to a date in the future, the role is 
     * currently active. 
     */
    void setEnd(const QDateTime& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#end", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/03/22/nco#end. 
     * End datetime for the role, such as: the datetime of leaving a 
     * project or organization, datetime of ending employment, datetime 
     * of divorce. If absent or set to a date in the future, the role is 
     * currently active. 
     */
    void addEnd(const QDateTime& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#end", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/03/22/nco#foafUrl. 
     * The URL of the FOAF file. 
     */
    QList<QUrl> foafUrls() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#foafUrl", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/03/22/nco#foafUrl. 
     * The URL of the FOAF file. 
     */
    void setFoafUrls(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#foafUrl", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/03/22/nco#foafUrl. 
     * The URL of the FOAF file. 
     */
    void addFoafUrl(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#foafUrl", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/03/22/nco#start. 
     * Start datetime for the role, such as: the datetime of joining 
     * a project or organization, datetime of starting employment, 
     * datetime of marriage 
     */
    QDateTime start() const {
        QDateTime value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#start", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#start", QUrl::StrictMode)).first().value<QDateTime>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/03/22/nco#start. 
     * Start datetime for the role, such as: the datetime of joining 
     * a project or organization, datetime of starting employment, 
     * datetime of marriage 
     */
    void setStart(const QDateTime& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#start", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/03/22/nco#start. 
     * Start datetime for the role, such as: the datetime of joining 
     * a project or organization, datetime of starting employment, 
     * datetime of marriage 
     */
    void addStart(const QDateTime& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#start", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/03/22/nco#hasPostalAddress. 
     * The default Address for a Contact. An equivalent of the 'ADR' 
     * property as defined in RFC 2426 Sec. 3.2.1. 
     */
    QList<QUrl> postalAddresses() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#hasPostalAddress", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/03/22/nco#hasPostalAddress. 
     * The default Address for a Contact. An equivalent of the 'ADR' 
     * property as defined in RFC 2426 Sec. 3.2.1. 
     */
    void setPostalAddresses(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#hasPostalAddress", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/03/22/nco#hasPostalAddress. 
     * The default Address for a Contact. An equivalent of the 'ADR' 
     * property as defined in RFC 2426 Sec. 3.2.1. 
     */
    void addPostalAddress(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#hasPostalAddress", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/03/22/nco#hasPhoneNumber. 
     * A number for telephony communication with the object represented 
     * by this Contact. An equivalent of the 'TEL' property defined 
     * in RFC 2426 Sec. 3.3.1 
     */
    QList<QUrl> phoneNumbers() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#hasPhoneNumber", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/03/22/nco#hasPhoneNumber. 
     * A number for telephony communication with the object represented 
     * by this Contact. An equivalent of the 'TEL' property defined 
     * in RFC 2426 Sec. 3.3.1 
     */
    void setPhoneNumbers(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#hasPhoneNumber", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/03/22/nco#hasPhoneNumber. 
     * A number for telephony communication with the object represented 
     * by this Contact. An equivalent of the 'TEL' property defined 
     * in RFC 2426 Sec. 3.3.1 
     */
    void addPhoneNumber(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#hasPhoneNumber", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/03/22/nco#blogUrl. 
     * A Blog url. 
     */
    QList<QUrl> blogUrls() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#blogUrl", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/03/22/nco#blogUrl. 
     * A Blog url. 
     */
    void setBlogUrls(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#blogUrl", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/03/22/nco#blogUrl. 
     * A Blog url. 
     */
    void addBlogUrl(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#blogUrl", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/03/22/nco#url. 
     * A uniform resource locator associated with the given role of 
     * a Contact. Inspired by the 'URL' property defined in RFC 2426 
     * Sec. 3.6.8. 
     */
    QList<QUrl> urls() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#url", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/03/22/nco#url. 
     * A uniform resource locator associated with the given role of 
     * a Contact. Inspired by the 'URL' property defined in RFC 2426 
     * Sec. 3.6.8. 
     */
    void setUrls(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#url", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/03/22/nco#url. 
     * A uniform resource locator associated with the given role of 
     * a Contact. Inspired by the 'URL' property defined in RFC 2426 
     * Sec. 3.6.8. 
     */
    void addUrl(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#url", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/03/22/nco#websiteUrl. 
     * A url of a website. 
     */
    QList<QUrl> websiteUrls() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#websiteUrl", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/03/22/nco#websiteUrl. 
     * A url of a website. 
     */
    void setWebsiteUrls(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#websiteUrl", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/03/22/nco#websiteUrl. 
     * A url of a website. 
     */
    void addWebsiteUrl(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#websiteUrl", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/03/22/nco#hasContactMedium. 
     * A superProperty for all properties linking a Contact to an instance 
     * of a contact medium. 
     */
    QList<QUrl> contactMediums() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#hasContactMedium", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/03/22/nco#hasContactMedium. 
     * A superProperty for all properties linking a Contact to an instance 
     * of a contact medium. 
     */
    void setContactMediums(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#hasContactMedium", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/03/22/nco#hasContactMedium. 
     * A superProperty for all properties linking a Contact to an instance 
     * of a contact medium. 
     */
    void addContactMedium(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#hasContactMedium", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/03/22/nco#hasEmailAddress. 
     * An address for electronic mail communication with the object 
     * specified by this contact. An equivalent of the 'EMAIL' property 
     * as defined in RFC 2426 Sec. 3.3.1. 
     */
    QList<QUrl> emailAddresses() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#hasEmailAddress", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/03/22/nco#hasEmailAddress. 
     * An address for electronic mail communication with the object 
     * specified by this contact. An equivalent of the 'EMAIL' property 
     * as defined in RFC 2426 Sec. 3.3.1. 
     */
    void setEmailAddresses(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#hasEmailAddress", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/03/22/nco#hasEmailAddress. 
     * An address for electronic mail communication with the object 
     * specified by this contact. An equivalent of the 'EMAIL' property 
     * as defined in RFC 2426 Sec. 3.3.1. 
     */
    void addEmailAddress(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#hasEmailAddress", QUrl::StrictMode), value);
    }

protected:
    Role(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri) {
        addType(type);
    }
    Role(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res) {
        addType(type);
    }
};
}
}

#endif
