#ifndef _NBIB_LEGISLATION_H_
#define _NBIB_LEGISLATION_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include "dms-copy/simpleresource.h"

#include "nbib/legaldocument.h"

namespace Nepomuk {
namespace NBIB {
/**
 * A legal document proposing or enacting a law or a group of laws. 
 */
class Legislation : public virtual NBIB::LegalDocument
{
public:
    Legislation(const QUrl& uri = QUrl())
      : SimpleResource(uri), NBIB::Publication(uri, QUrl::fromEncoded("http://www.example.com/nbib#Legislation", QUrl::StrictMode)), NBIB::LegalDocument(uri, QUrl::fromEncoded("http://www.example.com/nbib#Legislation", QUrl::StrictMode)) {
    }

    Legislation(const SimpleResource& res)
      : SimpleResource(res), NBIB::Publication(res, QUrl::fromEncoded("http://www.example.com/nbib#Legislation", QUrl::StrictMode)), NBIB::LegalDocument(res, QUrl::fromEncoded("http://www.example.com/nbib#Legislation", QUrl::StrictMode)) {
    }

    Legislation& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.example.com/nbib#Legislation", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.example.com/nbib#codeOfLaw. The 
     * code of law the legislation is contained in 
     */
    QUrl codeOfLaw() const {
        QUrl value;
        if(contains(QUrl::fromEncoded("http://www.example.com/nbib#codeOfLaw", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/nbib#codeOfLaw", QUrl::StrictMode)).first().value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#codeOfLaw. The 
     * code of law the legislation is contained in 
     */
    void setCodeOfLaw(const QUrl& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#codeOfLaw", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#codeOfLaw. 
     * The code of law the legislation is contained in 
     */
    void addCodeOfLaw(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#codeOfLaw", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/nbib#coSponsor. The 
     * co-sponsor [of a bill] 
     */
    QList<QUrl> coSponsors() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.example.com/nbib#coSponsor", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#coSponsor. The 
     * co-sponsor [of a bill] 
     */
    void setCoSponsors(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#coSponsor", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#coSponsor. 
     * The co-sponsor [of a bill] 
     */
    void addCoSponsor(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#coSponsor", QUrl::StrictMode), value);
    }

protected:
    Legislation(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NBIB::Publication(uri, type), NBIB::LegalDocument(uri, type) {
    }
    Legislation(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NBIB::Publication(res, type), NBIB::LegalDocument(res, type) {
    }
};
}
}

#endif
