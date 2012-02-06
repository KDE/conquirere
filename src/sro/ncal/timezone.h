#ifndef _NCAL_TIMEZONE_H_
#define _NCAL_TIMEZONE_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include "dms-copy/simpleresource.h"

#include "ncal/unionoftimezoneobservanceeventjournaltimezonetodo.h"
#include "ncal/unionoftimezoneobservanceeventfreebusyjournaltimezonetodo.h"
#include "ncal/unionoftimezoneobservanceeventfreebusytimezonetodo.h"
#include "ncal/unionofeventjournaltimezonetodo.h"

namespace Nepomuk {
namespace NCAL {
/**
 * Provide a grouping of component properties that defines a time 
 * zone. 
 */
class Timezone : public virtual NCAL::UnionOfTimezoneObservanceEventJournalTimezoneTodo, public virtual NCAL::UnionOfTimezoneObservanceEventFreebusyJournalTimezoneTodo, public virtual NCAL::UnionOfTimezoneObservanceEventFreebusyTimezoneTodo, public virtual NCAL::UnionOfEventJournalTimezoneTodo
{
public:
    Timezone(const QUrl& uri = QUrl())
      : SimpleResource(uri), NCAL::UnionParentClass(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Timezone", QUrl::StrictMode)), NCAL::UnionOfTimezoneObservanceEventJournalTimezoneTodo(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Timezone", QUrl::StrictMode)), NCAL::UnionOfTimezoneObservanceEventFreebusyJournalTimezoneTodo(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Timezone", QUrl::StrictMode)), NCAL::UnionOfTimezoneObservanceEventFreebusyTimezoneTodo(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Timezone", QUrl::StrictMode)), NCAL::UnionOfEventJournalTimezoneTodo(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Timezone", QUrl::StrictMode)) {
    }

    Timezone(const SimpleResource& res)
      : SimpleResource(res), NCAL::UnionParentClass(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Timezone", QUrl::StrictMode)), NCAL::UnionOfTimezoneObservanceEventJournalTimezoneTodo(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Timezone", QUrl::StrictMode)), NCAL::UnionOfTimezoneObservanceEventFreebusyJournalTimezoneTodo(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Timezone", QUrl::StrictMode)), NCAL::UnionOfTimezoneObservanceEventFreebusyTimezoneTodo(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Timezone", QUrl::StrictMode)), NCAL::UnionOfEventJournalTimezoneTodo(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Timezone", QUrl::StrictMode)) {
    }

    Timezone& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Timezone", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#daylight. 
     * Links a timezone with it's daylight observance. This property 
     * has no direct equivalent in the RFC 2445. It has been inspired 
     * by the structure of the Vtimezone component defined in sec.4.6.5 
     */
    QUrl daylight() const {
        QUrl value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#daylight", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#daylight", QUrl::StrictMode)).first().value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#daylight. 
     * Links a timezone with it's daylight observance. This property 
     * has no direct equivalent in the RFC 2445. It has been inspired 
     * by the structure of the Vtimezone component defined in sec.4.6.5 
     */
    void setDaylight(const QUrl& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#daylight", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#daylight. 
     * Links a timezone with it's daylight observance. This property 
     * has no direct equivalent in the RFC 2445. It has been inspired 
     * by the structure of the Vtimezone component defined in sec.4.6.5 
     */
    void addDaylight(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#daylight", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#tzurl. 
     * The TZURL provides a means for a VTIMEZONE component to point 
     * to a network location that can be used to retrieve an up-to- date 
     * version of itself. Inspired by RFC 2445 sec. 4.8.3.5. Originally 
     * the range of this property had been specified as URI. 
     */
    QList<QUrl> tzurls() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#tzurl", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#tzurl. 
     * The TZURL provides a means for a VTIMEZONE component to point 
     * to a network location that can be used to retrieve an up-to- date 
     * version of itself. Inspired by RFC 2445 sec. 4.8.3.5. Originally 
     * the range of this property had been specified as URI. 
     */
    void setTzurls(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#tzurl", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#tzurl. 
     * The TZURL provides a means for a VTIMEZONE component to point 
     * to a network location that can be used to retrieve an up-to- date 
     * version of itself. Inspired by RFC 2445 sec. 4.8.3.5. Originally 
     * the range of this property had been specified as URI. 
     */
    void addTzurl(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#tzurl", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#standard. 
     * Links the timezone with the standard timezone observance. 
     * This property has no direct equivalent in the RFC 2445. It has 
     * been inspired by the structure of the Vtimezone component defined 
     * in sec.4.6.5 
     */
    QUrl standard() const {
        QUrl value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#standard", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#standard", QUrl::StrictMode)).first().value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#standard. 
     * Links the timezone with the standard timezone observance. 
     * This property has no direct equivalent in the RFC 2445. It has 
     * been inspired by the structure of the Vtimezone component defined 
     * in sec.4.6.5 
     */
    void setStandard(const QUrl& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#standard", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#standard. 
     * Links the timezone with the standard timezone observance. 
     * This property has no direct equivalent in the RFC 2445. It has 
     * been inspired by the structure of the Vtimezone component defined 
     * in sec.4.6.5 
     */
    void addStandard(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#standard", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#tzid. 
     * This property specifies the text value that uniquely identifies 
     * the "VTIMEZONE" calendar component. Inspired by RFC 2445 sec 
     * 4.8.3.1 
     */
    QString tzid() const {
        QString value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#tzid", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#tzid", QUrl::StrictMode)).first().value<QString>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#tzid. 
     * This property specifies the text value that uniquely identifies 
     * the "VTIMEZONE" calendar component. Inspired by RFC 2445 sec 
     * 4.8.3.1 
     */
    void setTzid(const QString& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#tzid", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#tzid. 
     * This property specifies the text value that uniquely identifies 
     * the "VTIMEZONE" calendar component. Inspired by RFC 2445 sec 
     * 4.8.3.1 
     */
    void addTzid(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#tzid", QUrl::StrictMode), value);
    }

protected:
    Timezone(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NCAL::UnionParentClass(uri, type), NCAL::UnionOfTimezoneObservanceEventJournalTimezoneTodo(uri, type), NCAL::UnionOfTimezoneObservanceEventFreebusyJournalTimezoneTodo(uri, type), NCAL::UnionOfTimezoneObservanceEventFreebusyTimezoneTodo(uri, type), NCAL::UnionOfEventJournalTimezoneTodo(uri, type) {
    }
    Timezone(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NCAL::UnionParentClass(res, type), NCAL::UnionOfTimezoneObservanceEventJournalTimezoneTodo(res, type), NCAL::UnionOfTimezoneObservanceEventFreebusyJournalTimezoneTodo(res, type), NCAL::UnionOfTimezoneObservanceEventFreebusyTimezoneTodo(res, type), NCAL::UnionOfEventJournalTimezoneTodo(res, type) {
    }
};
}
}

#endif
