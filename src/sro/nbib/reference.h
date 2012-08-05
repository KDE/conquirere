#ifndef _NBIB_REFERENCE_H_
#define _NBIB_REFERENCE_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include <nepomuk2/simpleresource.h>

#include "nie/informationelement.h"

namespace Nepomuk2 {
namespace NBIB {
/**
 * A bibliographic reference 
 */
class Reference : public virtual NIE::InformationElement
{
public:
    Reference(const QUrl& uri = QUrl())
      : SimpleResource(uri), NIE::InformationElement(uri, QUrl::fromEncoded("http://www.example.com/nbib#Reference", QUrl::StrictMode)) {
    }

    Reference(const SimpleResource& res)
      : SimpleResource(res), NIE::InformationElement(res, QUrl::fromEncoded("http://www.example.com/nbib#Reference", QUrl::StrictMode)) {
    }

    Reference& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.example.com/nbib#Reference", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.example.com/nbib#referencedPart. 
     * The specific part of the document of a reference. Used to reference 
     * a chapter or quote a small portion of a publication 
     */
    QUrl referencedPart() const {
        QUrl value;
        if(contains(QUrl::fromEncoded("http://www.example.com/nbib#referencedPart", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/nbib#referencedPart", QUrl::StrictMode)).first().value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#referencedPart. 
     * The specific part of the document of a reference. Used to reference 
     * a chapter or quote a small portion of a publication 
     */
    void setReferencedPart(const QUrl& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#referencedPart", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#referencedPart. 
     * The specific part of the document of a reference. Used to reference 
     * a chapter or quote a small portion of a publication 
     */
    void addReferencedPart(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#referencedPart", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/nbib#publication. 
     * The publication used for this reference 
     */
    QUrl publication() const {
        QUrl value;
        if(contains(QUrl::fromEncoded("http://www.example.com/nbib#publication", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/nbib#publication", QUrl::StrictMode)).first().value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#publication. 
     * The publication used for this reference 
     */
    void setPublication(const QUrl& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#publication", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#publication. 
     * The publication used for this reference 
     */
    void addPublication(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#publication", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/nbib#citeKey. Used 
     * to identify the reference. 
     */
    QString citeKey() const {
        QString value;
        if(contains(QUrl::fromEncoded("http://www.example.com/nbib#citeKey", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/nbib#citeKey", QUrl::StrictMode)).first().value<QString>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#citeKey. Used 
     * to identify the reference. 
     */
    void setCiteKey(const QString& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#citeKey", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#citeKey. 
     * Used to identify the reference. 
     */
    void addCiteKey(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#citeKey", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/nbib#pages. Page numbers, 
     * separated either by commas or double-hyphens. 
     */
    QString pages() const {
        QString value;
        if(contains(QUrl::fromEncoded("http://www.example.com/nbib#pages", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/nbib#pages", QUrl::StrictMode)).first().value<QString>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#pages. Page numbers, 
     * separated either by commas or double-hyphens. 
     */
    void setPages(const QString& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#pages", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#pages. 
     * Page numbers, separated either by commas or double-hyphens. 
     */
    void addPages(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#pages", QUrl::StrictMode), value);
    }

protected:
    Reference(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NIE::InformationElement(uri, type) {
    }
    Reference(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NIE::InformationElement(res, type) {
    }
};
}
}

#endif
