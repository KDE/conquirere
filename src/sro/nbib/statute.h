#ifndef _NBIB_STATUTE_H_
#define _NBIB_STATUTE_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include <nepomuk2/simpleresource.h>

#include "nbib/legislation.h"

namespace Nepomuk2 {
namespace NBIB {
/**
 * A bill enacted into law. 
 */
class Statute : public virtual NBIB::Legislation
{
public:
    Statute(const QUrl& uri = QUrl())
      : SimpleResource(uri), NIE::InformationElement(uri, QUrl::fromEncoded("http://www.example.com/nbib#Statute", QUrl::StrictMode)), NBIB::Publication(uri, QUrl::fromEncoded("http://www.example.com/nbib#Statute", QUrl::StrictMode)), NBIB::LegalDocument(uri, QUrl::fromEncoded("http://www.example.com/nbib#Statute", QUrl::StrictMode)), NBIB::Legislation(uri, QUrl::fromEncoded("http://www.example.com/nbib#Statute", QUrl::StrictMode)) {
    }

    Statute(const SimpleResource& res)
      : SimpleResource(res), NIE::InformationElement(res, QUrl::fromEncoded("http://www.example.com/nbib#Statute", QUrl::StrictMode)), NBIB::Publication(res, QUrl::fromEncoded("http://www.example.com/nbib#Statute", QUrl::StrictMode)), NBIB::LegalDocument(res, QUrl::fromEncoded("http://www.example.com/nbib#Statute", QUrl::StrictMode)), NBIB::Legislation(res, QUrl::fromEncoded("http://www.example.com/nbib#Statute", QUrl::StrictMode)) {
    }

    Statute& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.example.com/nbib#Statute", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.example.com/nbib#publicLawNumber. 
     * The public law number of the statute 
     */
    QStringList publicLawNumbers() const {
        QStringList value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.example.com/nbib#publicLawNumber", QUrl::StrictMode)))
            value << v.value<QString>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#publicLawNumber. 
     * The public law number of the statute 
     */
    void setPublicLawNumbers(const QStringList& value) {
        QVariantList values;
        foreach(const QString& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#publicLawNumber", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#publicLawNumber. 
     * The public law number of the statute 
     */
    void addPublicLawNumber(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#publicLawNumber", QUrl::StrictMode), value);
    }

protected:
    Statute(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NIE::InformationElement(uri, type), NBIB::Publication(uri, type), NBIB::LegalDocument(uri, type), NBIB::Legislation(uri, type) {
    }
    Statute(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NIE::InformationElement(res, type), NBIB::Publication(res, type), NBIB::LegalDocument(res, type), NBIB::Legislation(res, type) {
    }
};
}
}

#endif
