#ifndef _NCAL_CALENDAR_H_
#define _NCAL_CALENDAR_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include <nepomuk2/simpleresource.h>

#include "nie/informationelement.h"

namespace Nepomuk2 {
namespace NCAL {
/**
 * A calendar. Inspirations for this class can be traced to the 
 * VCALENDAR component defined in RFC 2445 sec. 4.4, but it may 
 * just as well be used to represent any kind of Calendar. 
 */
class Calendar : public virtual NIE::InformationElement
{
public:
    Calendar(const QUrl& uri = QUrl())
      : SimpleResource(uri), NIE::InformationElement(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Calendar", QUrl::StrictMode)) {
    }

    Calendar(const SimpleResource& res)
      : SimpleResource(res), NIE::InformationElement(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Calendar", QUrl::StrictMode)) {
    }

    Calendar& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Calendar", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#component. 
     * Links the Vcalendar instance with the calendar components. 
     * This property has no direct equivalent in the RFC specification. 
     * It has been introduced to express the containmnent relations. 
     */
    QList<QUrl> components() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#component", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#component. 
     * Links the Vcalendar instance with the calendar components. 
     * This property has no direct equivalent in the RFC specification. 
     * It has been introduced to express the containmnent relations. 
     */
    void setComponents(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#component", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#component. 
     * Links the Vcalendar instance with the calendar components. 
     * This property has no direct equivalent in the RFC specification. 
     * It has been introduced to express the containmnent relations. 
     */
    void addComponent(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#component", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#prodid. 
     * This property specifies the identifier for the product that 
     * created the iCalendar object. Defined in RFC 2445 sec. 4.7.2 
     */
    QStringList prodids() const {
        QStringList value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#prodid", QUrl::StrictMode)))
            value << v.value<QString>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#prodid. 
     * This property specifies the identifier for the product that 
     * created the iCalendar object. Defined in RFC 2445 sec. 4.7.2 
     */
    void setProdids(const QStringList& value) {
        QVariantList values;
        foreach(const QString& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#prodid", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#prodid. 
     * This property specifies the identifier for the product that 
     * created the iCalendar object. Defined in RFC 2445 sec. 4.7.2 
     */
    void addProdid(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#prodid", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#method. 
     * This property defines the iCalendar object method associated 
     * with the calendar object. Defined in RFC 2445 sec. 4.7.2 
     */
    QString method() const {
        QString value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#method", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#method", QUrl::StrictMode)).first().value<QString>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#method. 
     * This property defines the iCalendar object method associated 
     * with the calendar object. Defined in RFC 2445 sec. 4.7.2 
     */
    void setMethod(const QString& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#method", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#method. 
     * This property defines the iCalendar object method associated 
     * with the calendar object. Defined in RFC 2445 sec. 4.7.2 
     */
    void addMethod(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#method", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#version. 
     * This property specifies the identifier corresponding to the 
     * highest version number or the minimum and maximum range of the 
     * iCalendar specification that is required in order to interpret 
     * the iCalendar object. Defined in RFC 2445 sec. 4.7.4 
     */
    QStringList versions() const {
        QStringList value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#version", QUrl::StrictMode)))
            value << v.value<QString>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#version. 
     * This property specifies the identifier corresponding to the 
     * highest version number or the minimum and maximum range of the 
     * iCalendar specification that is required in order to interpret 
     * the iCalendar object. Defined in RFC 2445 sec. 4.7.4 
     */
    void setVersions(const QStringList& value) {
        QVariantList values;
        foreach(const QString& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#version", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#version. 
     * This property specifies the identifier corresponding to the 
     * highest version number or the minimum and maximum range of the 
     * iCalendar specification that is required in order to interpret 
     * the iCalendar object. Defined in RFC 2445 sec. 4.7.4 
     */
    void addVersion(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#version", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#calscale. 
     * This property defines the calendar scale used for the calendar 
     * information specified in the iCalendar object. Defined in 
     * RFC 2445 sec. 4.7.1 
     */
    QUrl calscale() const {
        QUrl value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#calscale", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#calscale", QUrl::StrictMode)).first().value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#calscale. 
     * This property defines the calendar scale used for the calendar 
     * information specified in the iCalendar object. Defined in 
     * RFC 2445 sec. 4.7.1 
     */
    void setCalscale(const QUrl& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#calscale", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#calscale. 
     * This property defines the calendar scale used for the calendar 
     * information specified in the iCalendar object. Defined in 
     * RFC 2445 sec. 4.7.1 
     */
    void addCalscale(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#calscale", QUrl::StrictMode), value);
    }

protected:
    Calendar(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NIE::InformationElement(uri, type) {
    }
    Calendar(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NIE::InformationElement(res, type) {
    }
};
}
}

#endif
