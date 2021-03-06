#ifndef _NBIB_MAP_H_
#define _NBIB_MAP_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include <nepomuk2/simpleresource.h>

#include "nbib/publication.h"

namespace Nepomuk2 {
namespace NBIB {
/**
 * A map with geographical data 
 */
class Map : public virtual NBIB::Publication
{
public:
    Map(const QUrl& uri = QUrl())
      : SimpleResource(uri), NIE::InformationElement(uri, QUrl::fromEncoded("http://www.example.com/nbib#Map", QUrl::StrictMode)), NBIB::Publication(uri, QUrl::fromEncoded("http://www.example.com/nbib#Map", QUrl::StrictMode)) {
    }

    Map(const SimpleResource& res)
      : SimpleResource(res), NIE::InformationElement(res, QUrl::fromEncoded("http://www.example.com/nbib#Map", QUrl::StrictMode)), NBIB::Publication(res, QUrl::fromEncoded("http://www.example.com/nbib#Map", QUrl::StrictMode)) {
    }

    Map& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.example.com/nbib#Map", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.example.com/nbib#mapScale. The 
     * scale of a map 
     */
    QString mapScale() const {
        QString value;
        if(contains(QUrl::fromEncoded("http://www.example.com/nbib#mapScale", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/nbib#mapScale", QUrl::StrictMode)).first().value<QString>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#mapScale. The 
     * scale of a map 
     */
    void setMapScale(const QString& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#mapScale", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#mapScale. 
     * The scale of a map 
     */
    void addMapScale(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#mapScale", QUrl::StrictMode), value);
    }

protected:
    Map(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NIE::InformationElement(uri, type), NBIB::Publication(uri, type) {
    }
    Map(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NIE::InformationElement(res, type), NBIB::Publication(res, type) {
    }
};
}
}

#endif
