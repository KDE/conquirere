#ifndef _NBIB_SERIES_H_
#define _NBIB_SERIES_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include "dms-copy/simpleresource.h"

namespace Nepomuk {
namespace NBIB {
/**
 * Superclass to represent a series of some published data. (Book 
 * Series, Journal with several issues etc) 
 */
class Series : public virtual Nepomuk::SimpleResource
{
public:
    Series(const QUrl& uri = QUrl())
      : SimpleResource(uri) {
        addType(QUrl::fromEncoded("http://www.example.com/nbib#Series", QUrl::StrictMode));
    }

    Series(const SimpleResource& res)
      : SimpleResource(res) {
        addType(QUrl::fromEncoded("http://www.example.com/nbib#Series", QUrl::StrictMode));
    }

    Series& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.example.com/nbib#Series", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.example.com/nbib#issn. The International 
     * Standard Serial Number. Used to identify a series of publications. 
     */
    QString issn() const {
        QString value;
        if(contains(QUrl::fromEncoded("http://www.example.com/nbib#issn", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/nbib#issn", QUrl::StrictMode)).first().value<QString>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#issn. The International 
     * Standard Serial Number. Used to identify a series of publications. 
     */
    void setIssn(const QString& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#issn", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#issn. 
     * The International Standard Serial Number. Used to identify 
     * a series of publications. 
     */
    void addIssn(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#issn", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/nbib#seriesOf. The 
     * publication this series belongs to 
     */
    QList<QUrl> seriesOfs() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.example.com/nbib#seriesOf", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#seriesOf. The 
     * publication this series belongs to 
     */
    void setSeriesOfs(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#seriesOf", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#seriesOf. 
     * The publication this series belongs to 
     */
    void addSeriesOf(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#seriesOf", QUrl::StrictMode), value);
    }

protected:
    Series(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri) {
        addType(type);
    }
    Series(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res) {
        addType(type);
    }
};
}
}

#endif
