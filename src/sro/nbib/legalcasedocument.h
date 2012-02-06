#ifndef _NBIB_LEGALCASEDOCUMENT_H_
#define _NBIB_LEGALCASEDOCUMENT_H_

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
 * A document accompanying a legal case. 
 */
class LegalCaseDocument : public virtual NBIB::LegalDocument
{
public:
    LegalCaseDocument(const QUrl& uri = QUrl())
      : SimpleResource(uri), NBIB::Publication(uri, QUrl::fromEncoded("http://www.example.com/nbib#LegalCaseDocument", QUrl::StrictMode)), NBIB::LegalDocument(uri, QUrl::fromEncoded("http://www.example.com/nbib#LegalCaseDocument", QUrl::StrictMode)) {
    }

    LegalCaseDocument(const SimpleResource& res)
      : SimpleResource(res), NBIB::Publication(res, QUrl::fromEncoded("http://www.example.com/nbib#LegalCaseDocument", QUrl::StrictMode)), NBIB::LegalDocument(res, QUrl::fromEncoded("http://www.example.com/nbib#LegalCaseDocument", QUrl::StrictMode)) {
    }

    LegalCaseDocument& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.example.com/nbib#LegalCaseDocument", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.example.com/nbib#courtReporter. 
     * Links case document to the collection of legal cases it belongs 
     * to. 
     */
    QUrl courtReporter() const {
        QUrl value;
        if(contains(QUrl::fromEncoded("http://www.example.com/nbib#courtReporter", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/nbib#courtReporter", QUrl::StrictMode)).first().value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#courtReporter. 
     * Links case document to the collection of legal cases it belongs 
     * to. 
     */
    void setCourtReporter(const QUrl& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#courtReporter", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#courtReporter. 
     * Links case document to the collection of legal cases it belongs 
     * to. 
     */
    void addCourtReporter(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#courtReporter", QUrl::StrictMode), value);
    }

protected:
    LegalCaseDocument(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NBIB::Publication(uri, type), NBIB::LegalDocument(uri, type) {
    }
    LegalCaseDocument(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NBIB::Publication(res, type), NBIB::LegalDocument(res, type) {
    }
};
}
}

#endif
