#ifndef _NCAL_ATTACHMENT_H_
#define _NCAL_ATTACHMENT_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include <nepomuk2/simpleresource.h>

#include "nfo/attachment.h"

namespace Nepomuk2 {
namespace NCAL {
/**
 * An object attached to a calendar entity. This class has been 
 * introduced to serve as a structured value of the ncal:attach 
 * property. See the documentation of ncal:attach for details. 
 */
class Attachment : public virtual NFO::Attachment
{
public:
    Attachment(const QUrl& uri = QUrl())
      : SimpleResource(uri), NIE::DataObject(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Attachment", QUrl::StrictMode)), NFO::FileDataObject(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Attachment", QUrl::StrictMode)), NFO::EmbeddedFileDataObject(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Attachment", QUrl::StrictMode)), NFO::Attachment(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Attachment", QUrl::StrictMode)) {
    }

    Attachment(const SimpleResource& res)
      : SimpleResource(res), NIE::DataObject(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Attachment", QUrl::StrictMode)), NFO::FileDataObject(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Attachment", QUrl::StrictMode)), NFO::EmbeddedFileDataObject(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Attachment", QUrl::StrictMode)), NFO::Attachment(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Attachment", QUrl::StrictMode)) {
    }

    Attachment& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Attachment", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#fmttype. 
     * To specify the content type of a referenced object. Inspired 
     * by RFC 2445 sec. 4.2.8. The value of this property should be an 
     * IANA-registered content type (e.g. application/binary) 
     */
    QString fmttype() const {
        QString value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#fmttype", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#fmttype", QUrl::StrictMode)).first().value<QString>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#fmttype. 
     * To specify the content type of a referenced object. Inspired 
     * by RFC 2445 sec. 4.2.8. The value of this property should be an 
     * IANA-registered content type (e.g. application/binary) 
     */
    void setFmttype(const QString& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#fmttype", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#fmttype. 
     * To specify the content type of a referenced object. Inspired 
     * by RFC 2445 sec. 4.2.8. The value of this property should be an 
     * IANA-registered content type (e.g. application/binary) 
     */
    void addFmttype(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#fmttype", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#attachmentContent. 
     * The uri of the attachment. Created to express the actual value 
     * of the ATTACH property defined in RFC 2445 sec. 4.8.1.1. This 
     * property expresses the BINARY datatype of that property. see 
     * ncal:attachmentUri for the URI datatype. 
     */
    QStringList attachmentContents() const {
        QStringList value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#attachmentContent", QUrl::StrictMode)))
            value << v.value<QString>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#attachmentContent. 
     * The uri of the attachment. Created to express the actual value 
     * of the ATTACH property defined in RFC 2445 sec. 4.8.1.1. This 
     * property expresses the BINARY datatype of that property. see 
     * ncal:attachmentUri for the URI datatype. 
     */
    void setAttachmentContents(const QStringList& value) {
        QVariantList values;
        foreach(const QString& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#attachmentContent", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#attachmentContent. 
     * The uri of the attachment. Created to express the actual value 
     * of the ATTACH property defined in RFC 2445 sec. 4.8.1.1. This 
     * property expresses the BINARY datatype of that property. see 
     * ncal:attachmentUri for the URI datatype. 
     */
    void addAttachmentContent(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#attachmentContent", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#encoding. 
     * To specify an alternate inline encoding for the property value. 
     * Inspired by RFC 2445 sec. 4.2.7. Originally this property had 
     * a limited vocabulary. ('8BIT' and 'BASE64'). The terms of this 
     * vocabulary have been expressed as instances of the AttachmentEncoding 
     * class 
     */
    QUrl encoding() const {
        QUrl value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#encoding", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#encoding", QUrl::StrictMode)).first().value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#encoding. 
     * To specify an alternate inline encoding for the property value. 
     * Inspired by RFC 2445 sec. 4.2.7. Originally this property had 
     * a limited vocabulary. ('8BIT' and 'BASE64'). The terms of this 
     * vocabulary have been expressed as instances of the AttachmentEncoding 
     * class 
     */
    void setEncoding(const QUrl& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#encoding", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#encoding. 
     * To specify an alternate inline encoding for the property value. 
     * Inspired by RFC 2445 sec. 4.2.7. Originally this property had 
     * a limited vocabulary. ('8BIT' and 'BASE64'). The terms of this 
     * vocabulary have been expressed as instances of the AttachmentEncoding 
     * class 
     */
    void addEncoding(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#encoding", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#attachmentUri. 
     * The uri of the attachment. Created to express the actual value 
     * of the ATTACH property defined in RFC 2445 sec. 4.8.1.1. This 
     * property expresses the default URI datatype of that property. 
     * see ncal:attachmentContents for the BINARY datatype. 
     */
    QUrl attachmentUri() const {
        QUrl value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#attachmentUri", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#attachmentUri", QUrl::StrictMode)).first().value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#attachmentUri. 
     * The uri of the attachment. Created to express the actual value 
     * of the ATTACH property defined in RFC 2445 sec. 4.8.1.1. This 
     * property expresses the default URI datatype of that property. 
     * see ncal:attachmentContents for the BINARY datatype. 
     */
    void setAttachmentUri(const QUrl& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#attachmentUri", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#attachmentUri. 
     * The uri of the attachment. Created to express the actual value 
     * of the ATTACH property defined in RFC 2445 sec. 4.8.1.1. This 
     * property expresses the default URI datatype of that property. 
     * see ncal:attachmentContents for the BINARY datatype. 
     */
    void addAttachmentUri(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#attachmentUri", QUrl::StrictMode), value);
    }

protected:
    Attachment(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NIE::DataObject(uri, type), NFO::FileDataObject(uri, type), NFO::EmbeddedFileDataObject(uri, type), NFO::Attachment(uri, type) {
    }
    Attachment(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NIE::DataObject(res, type), NFO::FileDataObject(res, type), NFO::EmbeddedFileDataObject(res, type), NFO::Attachment(res, type) {
    }
};
}
}

#endif
