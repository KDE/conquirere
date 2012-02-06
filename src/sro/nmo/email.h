#ifndef _NMO_EMAIL_H_
#define _NMO_EMAIL_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include "dms-copy/simpleresource.h"

#include "nmo/message.h"

namespace Nepomuk {
namespace NMO {
/**
 * An email. 
 */
class Email : public virtual NMO::Message
{
public:
    Email(const QUrl& uri = QUrl())
      : SimpleResource(uri), NMO::Message(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#Email", QUrl::StrictMode)) {
    }

    Email(const SimpleResource& res)
      : SimpleResource(res), NMO::Message(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#Email", QUrl::StrictMode)) {
    }

    Email& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#Email", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#contentMimeType. 
     * Key used to store the MIME type of the content of an object when 
     * it is different from the object's main MIME type. This value 
     * can be used, for example, to model an e-mail message whose mime 
     * type is"message/rfc822", but whose content has type "text/html". 
     * If not specified, the MIME type of the content defaults to the 
     * value specified by the 'mimeType' property. 
     */
    QString contentMimeType() const {
        QString value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#contentMimeType", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#contentMimeType", QUrl::StrictMode)).first().value<QString>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#contentMimeType. 
     * Key used to store the MIME type of the content of an object when 
     * it is different from the object's main MIME type. This value 
     * can be used, for example, to model an e-mail message whose mime 
     * type is"message/rfc822", but whose content has type "text/html". 
     * If not specified, the MIME type of the content defaults to the 
     * value specified by the 'mimeType' property. 
     */
    void setContentMimeType(const QString& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#contentMimeType", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#contentMimeType. 
     * Key used to store the MIME type of the content of an object when 
     * it is different from the object's main MIME type. This value 
     * can be used, for example, to model an e-mail message whose mime 
     * type is"message/rfc822", but whose content has type "text/html". 
     * If not specified, the MIME type of the content defaults to the 
     * value specified by the 'mimeType' property. 
     */
    void addContentMimeType(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#contentMimeType", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#emailBcc. 
     * A Contact that is to receive a bcc of the email. A Bcc (blind carbon 
     * copy) is a copy of an email message sent to a recipient whose email 
     * address does not appear in the message. 
     */
    QList<QUrl> emailBccs() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#emailBcc", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#emailBcc. 
     * A Contact that is to receive a bcc of the email. A Bcc (blind carbon 
     * copy) is a copy of an email message sent to a recipient whose email 
     * address does not appear in the message. 
     */
    void setEmailBccs(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#emailBcc", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#emailBcc. 
     * A Contact that is to receive a bcc of the email. A Bcc (blind carbon 
     * copy) is a copy of an email message sent to a recipient whose email 
     * address does not appear in the message. 
     */
    void addEmailBcc(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#emailBcc", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#to. 
     * deprecated in favor of nmo:emailTo 
     */
    QList<QUrl> tos() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#to", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#to. 
     * deprecated in favor of nmo:emailTo 
     */
    void setTos(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#to", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#to. 
     * deprecated in favor of nmo:emailTo 
     */
    void addTo(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#to", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#emailCc. 
     * A Contact that is to receive a cc of the email. A cc (carbon copy) 
     * is a copy of an email message whose recipient appears on the recipient 
     * list, so that all other recipients are aware of it. 
     */
    QList<QUrl> emailCcs() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#emailCc", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#emailCc. 
     * A Contact that is to receive a cc of the email. A cc (carbon copy) 
     * is a copy of an email message whose recipient appears on the recipient 
     * list, so that all other recipients are aware of it. 
     */
    void setEmailCcs(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#emailCc", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#emailCc. 
     * A Contact that is to receive a cc of the email. A cc (carbon copy) 
     * is a copy of an email message whose recipient appears on the recipient 
     * list, so that all other recipients are aware of it. 
     */
    void addEmailCc(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#emailCc", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#cc. 
     * deprecated in favor of nmo:emailCc 
     */
    QList<QUrl> ccs() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#cc", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#cc. 
     * deprecated in favor of nmo:emailCc 
     */
    void setCcs(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#cc", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#cc. 
     * deprecated in favor of nmo:emailCc 
     */
    void addCc(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#cc", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#bcc. 
     * deprecated in favor of nmo:emailBcc 
     */
    QList<QUrl> bccs() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#bcc", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#bcc. 
     * deprecated in favor of nmo:emailBcc 
     */
    void setBccs(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#bcc", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#bcc. 
     * deprecated in favor of nmo:emailBcc 
     */
    void addBcc(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#bcc", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#emailTo. 
     * The primary intended recipient of an email. 
     */
    QList<QUrl> emailTos() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#emailTo", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#emailTo. 
     * The primary intended recipient of an email. 
     */
    void setEmailTos(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#emailTo", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#emailTo. 
     * The primary intended recipient of an email. 
     */
    void addEmailTo(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#emailTo", QUrl::StrictMode), value);
    }

protected:
    Email(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NMO::Message(uri, type) {
    }
    Email(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NMO::Message(res, type) {
    }
};
}
}

#endif
