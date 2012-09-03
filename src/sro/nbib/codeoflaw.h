#ifndef _NBIB_CODEOFLAW_H_
#define _NBIB_CODEOFLAW_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include <nepomuk2/simpleresource.h>

#include "collection.h"

namespace Nepomuk2 {
namespace NBIB {
/**
 * A collection of statutes. 
 */
class CodeOfLaw : public virtual NBIB::Collection
{
public:
    CodeOfLaw(const QUrl& uri = QUrl())
      : SimpleResource(uri), NIE::InformationElement(uri, QUrl::fromEncoded("http://www.example.com/nbib#CodeOfLaw", QUrl::StrictMode)), NBIB::Publication(uri, QUrl::fromEncoded("http://www.example.com/nbib#CodeOfLaw", QUrl::StrictMode)), NBIB::Collection(uri, QUrl::fromEncoded("http://www.example.com/nbib#CodeOfLaw", QUrl::StrictMode)) {
    }

    CodeOfLaw(const SimpleResource& res)
      : SimpleResource(res), NIE::InformationElement(res, QUrl::fromEncoded("http://www.example.com/nbib#CodeOfLaw", QUrl::StrictMode)), NBIB::Publication(res, QUrl::fromEncoded("http://www.example.com/nbib#CodeOfLaw", QUrl::StrictMode)), NBIB::Collection(res, QUrl::fromEncoded("http://www.example.com/nbib#CodeOfLaw", QUrl::StrictMode)) {
    }

    CodeOfLaw& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.example.com/nbib#CodeOfLaw", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.example.com/nbib#codeNumber. 
     * The code number of an CodeOfLaw 
     */
    QString codeNumber() const {
        QString value;
        if(contains(QUrl::fromEncoded("http://www.example.com/nbib#codeNumber", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/nbib#codeNumber", QUrl::StrictMode)).first().value<QString>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#codeNumber. 
     * The code number of an CodeOfLaw 
     */
    void setCodeNumber(const QString& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#codeNumber", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#codeNumber. 
     * The code number of an CodeOfLaw 
     */
    void addCodeNumber(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#codeNumber", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/nbib#legislation. 
     * links to a legislation (Bill, Statute) in a Code of Law 
     */
    QList<QUrl> legislations() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.example.com/nbib#legislation", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#legislation. 
     * links to a legislation (Bill, Statute) in a Code of Law 
     */
    void setLegislations(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#legislation", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#legislation. 
     * links to a legislation (Bill, Statute) in a Code of Law 
     */
    void addLegislation(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#legislation", QUrl::StrictMode), value);
    }

protected:
    CodeOfLaw(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NIE::InformationElement(uri, type), NBIB::Publication(uri, type), NBIB::Collection(uri, type) {
    }
    CodeOfLaw(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NIE::InformationElement(res, type), NBIB::Publication(res, type), NBIB::Collection(res, type) {
    }
};
}
}

#endif
