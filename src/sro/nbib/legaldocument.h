#ifndef _NBIB_LEGALDOCUMENT_H_
#define _NBIB_LEGALDOCUMENT_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include "dms-copy/simpleresource.h"

#include "nbib/publication.h"

namespace Nepomuk {
namespace NBIB {
/**
 * A legal document; for example, a court decision, a brief, and 
 * so forth. 
 */
class LegalDocument : public virtual NBIB::Publication
{
public:
    LegalDocument(const QUrl& uri = QUrl())
      : SimpleResource(uri), NBIB::Publication(uri, QUrl::fromEncoded("http://www.example.com/nbib#LegalDocument", QUrl::StrictMode)) {
    }

    LegalDocument(const SimpleResource& res)
      : SimpleResource(res), NBIB::Publication(res, QUrl::fromEncoded("http://www.example.com/nbib#LegalDocument", QUrl::StrictMode)) {
    }

    LegalDocument& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.example.com/nbib#LegalDocument", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.example.com/nbib#history. The 
     * history of a legal document 
     */
    QString history() const {
        QString value;
        if(contains(QUrl::fromEncoded("http://www.example.com/nbib#history", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/nbib#history", QUrl::StrictMode)).first().value<QString>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#history. The 
     * history of a legal document 
     */
    void setHistory(const QString& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#history", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#history. 
     * The history of a legal document 
     */
    void addHistory(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#history", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/nbib#counsel. The 
     * counsel related to a Legal document 
     */
    QList<QUrl> counsels() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.example.com/nbib#counsel", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#counsel. The 
     * counsel related to a Legal document 
     */
    void setCounsels(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#counsel", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#counsel. 
     * The counsel related to a Legal document 
     */
    void addCounsel(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#counsel", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/nbib#arguedDate. 
     * The date on which a legal case is argued before a court. 
     */
    QDateTime arguedDate() const {
        QDateTime value;
        if(contains(QUrl::fromEncoded("http://www.example.com/nbib#arguedDate", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/nbib#arguedDate", QUrl::StrictMode)).first().value<QDateTime>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#arguedDate. 
     * The date on which a legal case is argued before a court. 
     */
    void setArguedDate(const QDateTime& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#arguedDate", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#arguedDate. 
     * The date on which a legal case is argued before a court. 
     */
    void addArguedDate(const QDateTime& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#arguedDate", QUrl::StrictMode), value);
    }

protected:
    LegalDocument(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NBIB::Publication(uri, type) {
    }
    LegalDocument(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NBIB::Publication(res, type) {
    }
};
}
}

#endif
