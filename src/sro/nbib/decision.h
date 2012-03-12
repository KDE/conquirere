#ifndef _NBIB_DECISION_H_
#define _NBIB_DECISION_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include "dms-copy/simpleresource.h"

#include "nbib/legalcasedocument.h"

namespace Nepomuk {
namespace NBIB {
/**
 * A document containing an authoritative determination (as 
 * a decree or judgment) made after consideration of facts or law. 
 */
class Decision : public virtual NBIB::LegalCaseDocument
{
public:
    Decision(const QUrl& uri = QUrl())
      : SimpleResource(uri), NIE::InformationElement(uri, QUrl::fromEncoded("http://www.example.com/nbib#Decision", QUrl::StrictMode)), NBIB::Publication(uri, QUrl::fromEncoded("http://www.example.com/nbib#Decision", QUrl::StrictMode)), NBIB::LegalDocument(uri, QUrl::fromEncoded("http://www.example.com/nbib#Decision", QUrl::StrictMode)), NBIB::LegalCaseDocument(uri, QUrl::fromEncoded("http://www.example.com/nbib#Decision", QUrl::StrictMode)) {
    }

    Decision(const SimpleResource& res)
      : SimpleResource(res), NIE::InformationElement(res, QUrl::fromEncoded("http://www.example.com/nbib#Decision", QUrl::StrictMode)), NBIB::Publication(res, QUrl::fromEncoded("http://www.example.com/nbib#Decision", QUrl::StrictMode)), NBIB::LegalDocument(res, QUrl::fromEncoded("http://www.example.com/nbib#Decision", QUrl::StrictMode)), NBIB::LegalCaseDocument(res, QUrl::fromEncoded("http://www.example.com/nbib#Decision", QUrl::StrictMode)) {
    }

    Decision& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.example.com/nbib#Decision", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.example.com/nbib#affirmedBy. 
     * A legal decision that affirms a ruling. 
     */
    QList<QUrl> affirmedBys() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.example.com/nbib#affirmedBy", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#affirmedBy. 
     * A legal decision that affirms a ruling. 
     */
    void setAffirmedBys(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#affirmedBy", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#affirmedBy. 
     * A legal decision that affirms a ruling. 
     */
    void addAffirmedBy(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#affirmedBy", QUrl::StrictMode), value);
    }

protected:
    Decision(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NIE::InformationElement(uri, type), NBIB::Publication(uri, type), NBIB::LegalDocument(uri, type), NBIB::LegalCaseDocument(uri, type) {
    }
    Decision(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NIE::InformationElement(res, type), NBIB::Publication(res, type), NBIB::LegalDocument(res, type), NBIB::LegalCaseDocument(res, type) {
    }
};
}
}

#endif
