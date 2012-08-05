#ifndef _NCAL_TIMEZONEOBSERVANCE_H_
#define _NCAL_TIMEZONEOBSERVANCE_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include <nepomuk2/simpleresource.h>

#include "ncal/unionoftimezoneobservanceeventjournaltimezonetodo.h"
#include "ncal/unionoftimezoneobservanceeventfreebusytimezonetodo.h"
#include "ncal/unionoftimezoneobservanceeventfreebusyjournaltimezonetodo.h"

namespace Nepomuk2 {
namespace NCAL {
/**
 * 
 */
class TimezoneObservance : public virtual NCAL::UnionOfTimezoneObservanceEventJournalTimezoneTodo, public virtual NCAL::UnionOfTimezoneObservanceEventFreebusyTimezoneTodo, public virtual NCAL::UnionOfTimezoneObservanceEventFreebusyJournalTimezoneTodo
{
public:
    TimezoneObservance(const QUrl& uri = QUrl())
      : SimpleResource(uri), NCAL::UnionParentClass(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#TimezoneObservance", QUrl::StrictMode)), NCAL::UnionOfTimezoneObservanceEventJournalTimezoneTodo(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#TimezoneObservance", QUrl::StrictMode)), NCAL::UnionOfTimezoneObservanceEventFreebusyTimezoneTodo(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#TimezoneObservance", QUrl::StrictMode)), NCAL::UnionOfTimezoneObservanceEventFreebusyJournalTimezoneTodo(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#TimezoneObservance", QUrl::StrictMode)) {
    }

    TimezoneObservance(const SimpleResource& res)
      : SimpleResource(res), NCAL::UnionParentClass(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#TimezoneObservance", QUrl::StrictMode)), NCAL::UnionOfTimezoneObservanceEventJournalTimezoneTodo(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#TimezoneObservance", QUrl::StrictMode)), NCAL::UnionOfTimezoneObservanceEventFreebusyTimezoneTodo(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#TimezoneObservance", QUrl::StrictMode)), NCAL::UnionOfTimezoneObservanceEventFreebusyJournalTimezoneTodo(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#TimezoneObservance", QUrl::StrictMode)) {
    }

    TimezoneObservance& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#TimezoneObservance", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#tzname. 
     * Specifies the customary designation for a timezone description. 
     * Inspired by RFC 2445 sec. 4.8.3.2 The LANGUAGE parameter has 
     * been discarded. Please xml:lang literals to express languages. 
     * Original specification for the domain of this property stated 
     * that it must appear within the timezone component. In this ontology 
     * the TimezoneObservance class has been itroduced to clarify 
     * this specification. 
     */
    QStringList tznames() const {
        QStringList value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#tzname", QUrl::StrictMode)))
            value << v.value<QString>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#tzname. 
     * Specifies the customary designation for a timezone description. 
     * Inspired by RFC 2445 sec. 4.8.3.2 The LANGUAGE parameter has 
     * been discarded. Please xml:lang literals to express languages. 
     * Original specification for the domain of this property stated 
     * that it must appear within the timezone component. In this ontology 
     * the TimezoneObservance class has been itroduced to clarify 
     * this specification. 
     */
    void setTznames(const QStringList& value) {
        QVariantList values;
        foreach(const QString& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#tzname", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#tzname. 
     * Specifies the customary designation for a timezone description. 
     * Inspired by RFC 2445 sec. 4.8.3.2 The LANGUAGE parameter has 
     * been discarded. Please xml:lang literals to express languages. 
     * Original specification for the domain of this property stated 
     * that it must appear within the timezone component. In this ontology 
     * the TimezoneObservance class has been itroduced to clarify 
     * this specification. 
     */
    void addTzname(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#tzname", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#tzoffsetto. 
     * This property specifies the offset which is in use in this time 
     * zone observance. nspired by RFC 2445 sec. 4.8.3.4. The original 
     * domain was underspecified. It said that this property must 
     * appear within a Timezone component. In this ontology a TimezoneObservance 
     * class has been introduced to clarify this specification. The 
     * original range was UTC-OFFSET. There is no equivalent among 
     * the XSD datatypes so plain string was chosen. 
     */
    QString tzoffsetto() const {
        QString value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#tzoffsetto", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#tzoffsetto", QUrl::StrictMode)).first().value<QString>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#tzoffsetto. 
     * This property specifies the offset which is in use in this time 
     * zone observance. nspired by RFC 2445 sec. 4.8.3.4. The original 
     * domain was underspecified. It said that this property must 
     * appear within a Timezone component. In this ontology a TimezoneObservance 
     * class has been introduced to clarify this specification. The 
     * original range was UTC-OFFSET. There is no equivalent among 
     * the XSD datatypes so plain string was chosen. 
     */
    void setTzoffsetto(const QString& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#tzoffsetto", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#tzoffsetto. 
     * This property specifies the offset which is in use in this time 
     * zone observance. nspired by RFC 2445 sec. 4.8.3.4. The original 
     * domain was underspecified. It said that this property must 
     * appear within a Timezone component. In this ontology a TimezoneObservance 
     * class has been introduced to clarify this specification. The 
     * original range was UTC-OFFSET. There is no equivalent among 
     * the XSD datatypes so plain string was chosen. 
     */
    void addTzoffsetto(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#tzoffsetto", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#tzoffsetfrom. 
     * This property specifies the offset which is in use prior to this 
     * time zone observance. Inspired by RFC 2445 sec. 4.8.3.3. The 
     * original domain was underspecified. It said that this property 
     * must appear within a Timezone component. In this ontology a 
     * TimezoneObservance class has been introduced to clarify this 
     * specification. The original range was UTC-OFFSET. There is 
     * no equivalent among the XSD datatypes so plain string was chosen. 
     */
    QString tzoffsetfrom() const {
        QString value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#tzoffsetfrom", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#tzoffsetfrom", QUrl::StrictMode)).first().value<QString>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#tzoffsetfrom. 
     * This property specifies the offset which is in use prior to this 
     * time zone observance. Inspired by RFC 2445 sec. 4.8.3.3. The 
     * original domain was underspecified. It said that this property 
     * must appear within a Timezone component. In this ontology a 
     * TimezoneObservance class has been introduced to clarify this 
     * specification. The original range was UTC-OFFSET. There is 
     * no equivalent among the XSD datatypes so plain string was chosen. 
     */
    void setTzoffsetfrom(const QString& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#tzoffsetfrom", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#tzoffsetfrom. 
     * This property specifies the offset which is in use prior to this 
     * time zone observance. Inspired by RFC 2445 sec. 4.8.3.3. The 
     * original domain was underspecified. It said that this property 
     * must appear within a Timezone component. In this ontology a 
     * TimezoneObservance class has been introduced to clarify this 
     * specification. The original range was UTC-OFFSET. There is 
     * no equivalent among the XSD datatypes so plain string was chosen. 
     */
    void addTzoffsetfrom(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#tzoffsetfrom", QUrl::StrictMode), value);
    }

protected:
    TimezoneObservance(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NCAL::UnionParentClass(uri, type), NCAL::UnionOfTimezoneObservanceEventJournalTimezoneTodo(uri, type), NCAL::UnionOfTimezoneObservanceEventFreebusyTimezoneTodo(uri, type), NCAL::UnionOfTimezoneObservanceEventFreebusyJournalTimezoneTodo(uri, type) {
    }
    TimezoneObservance(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NCAL::UnionParentClass(res, type), NCAL::UnionOfTimezoneObservanceEventJournalTimezoneTodo(res, type), NCAL::UnionOfTimezoneObservanceEventFreebusyTimezoneTodo(res, type), NCAL::UnionOfTimezoneObservanceEventFreebusyJournalTimezoneTodo(res, type) {
    }
};
}
}

#endif
