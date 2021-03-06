#ifndef _NCAL_UNIONOFEVENTFREEBUSYJOURNALTODO_H_
#define _NCAL_UNIONOFEVENTFREEBUSYJOURNALTODO_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include <nepomuk2/simpleresource.h>

#include "ncal/unionparentclass.h"

namespace Nepomuk2 {
namespace NCAL {
/**
 * 
 */
class UnionOfEventFreebusyJournalTodo : public virtual NCAL::UnionParentClass
{
public:
    UnionOfEventFreebusyJournalTodo(const QUrl& uri = QUrl())
      : SimpleResource(uri), NCAL::UnionParentClass(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#UnionOfEventFreebusyJournalTodo", QUrl::StrictMode)) {
    }

    UnionOfEventFreebusyJournalTodo(const SimpleResource& res)
      : SimpleResource(res), NCAL::UnionParentClass(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#UnionOfEventFreebusyJournalTodo", QUrl::StrictMode)) {
    }

    UnionOfEventFreebusyJournalTodo& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#UnionOfEventFreebusyJournalTodo", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#url. 
     * This property defines a Uniform Resource Locator (URL) associated 
     * with the iCalendar object. Inspired by the RFC 2445 sec. 4.8.4.6. 
     * Original range had been specified as URI. 
     */
    QUrl url() const {
        QUrl value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#url", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#url", QUrl::StrictMode)).first().value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#url. 
     * This property defines a Uniform Resource Locator (URL) associated 
     * with the iCalendar object. Inspired by the RFC 2445 sec. 4.8.4.6. 
     * Original range had been specified as URI. 
     */
    void setUrl(const QUrl& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#url", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#url. 
     * This property defines a Uniform Resource Locator (URL) associated 
     * with the iCalendar object. Inspired by the RFC 2445 sec. 4.8.4.6. 
     * Original range had been specified as URI. 
     */
    void addUrl(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#url", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#uid. 
     * This property defines the persistent, globally unique identifier 
     * for the calendar component. Inspired by the RFC 2445 sec 4.8.4.7 
     */
    QString uid() const {
        QString value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#uid", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#uid", QUrl::StrictMode)).first().value<QString>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#uid. 
     * This property defines the persistent, globally unique identifier 
     * for the calendar component. Inspired by the RFC 2445 sec 4.8.4.7 
     */
    void setUid(const QString& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#uid", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#uid. 
     * This property defines the persistent, globally unique identifier 
     * for the calendar component. Inspired by the RFC 2445 sec 4.8.4.7 
     */
    void addUid(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#uid", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#contact. 
     * The property is used to represent contact information or alternately 
     * a reference to contact information associated with the calendar 
     * component. Inspired by RFC 2445 sec. 4.8.4.2 with the following 
     * reservations: the LANGUAGE parameter has been discarded. 
     * Please use xml:lang literals to express language. For the ALTREP 
     * parameter use the contactAltRep property.RFC doesn't define 
     * any format for the string. 
     */
    QStringList contacts() const {
        QStringList value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#contact", QUrl::StrictMode)))
            value << v.value<QString>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#contact. 
     * The property is used to represent contact information or alternately 
     * a reference to contact information associated with the calendar 
     * component. Inspired by RFC 2445 sec. 4.8.4.2 with the following 
     * reservations: the LANGUAGE parameter has been discarded. 
     * Please use xml:lang literals to express language. For the ALTREP 
     * parameter use the contactAltRep property.RFC doesn't define 
     * any format for the string. 
     */
    void setContacts(const QStringList& value) {
        QVariantList values;
        foreach(const QString& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#contact", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#contact. 
     * The property is used to represent contact information or alternately 
     * a reference to contact information associated with the calendar 
     * component. Inspired by RFC 2445 sec. 4.8.4.2 with the following 
     * reservations: the LANGUAGE parameter has been discarded. 
     * Please use xml:lang literals to express language. For the ALTREP 
     * parameter use the contactAltRep property.RFC doesn't define 
     * any format for the string. 
     */
    void addContact(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#contact", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#contactAltRep. 
     * Alternate representation of the contact property. Introduced 
     * to cover the ALTREP parameter of the CONTACT property. See documentation 
     * of ncal:contact for details. 
     */
    QList<QUrl> contactAltReps() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#contactAltRep", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#contactAltRep. 
     * Alternate representation of the contact property. Introduced 
     * to cover the ALTREP parameter of the CONTACT property. See documentation 
     * of ncal:contact for details. 
     */
    void setContactAltReps(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#contactAltRep", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#contactAltRep. 
     * Alternate representation of the contact property. Introduced 
     * to cover the ALTREP parameter of the CONTACT property. See documentation 
     * of ncal:contact for details. 
     */
    void addContactAltRep(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#contactAltRep", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#requestStatus. 
     * This property defines the status code returned for a scheduling 
     * request. Inspired by RFC 2445 sec. 4.8.8.2. Original value 
     * of this property was a four-element structure. The RequestStatus 
     * class has been introduced to express it. In RFC 2445 this property 
     * could have the LANGUAGE parameter. This has been discarded 
     * in this ontology. Use xml:lang literals to express it if necessary. 
     */
    QUrl requestStatus() const {
        QUrl value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#requestStatus", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#requestStatus", QUrl::StrictMode)).first().value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#requestStatus. 
     * This property defines the status code returned for a scheduling 
     * request. Inspired by RFC 2445 sec. 4.8.8.2. Original value 
     * of this property was a four-element structure. The RequestStatus 
     * class has been introduced to express it. In RFC 2445 this property 
     * could have the LANGUAGE parameter. This has been discarded 
     * in this ontology. Use xml:lang literals to express it if necessary. 
     */
    void setRequestStatus(const QUrl& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#requestStatus", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#requestStatus. 
     * This property defines the status code returned for a scheduling 
     * request. Inspired by RFC 2445 sec. 4.8.8.2. Original value 
     * of this property was a four-element structure. The RequestStatus 
     * class has been introduced to express it. In RFC 2445 this property 
     * could have the LANGUAGE parameter. This has been discarded 
     * in this ontology. Use xml:lang literals to express it if necessary. 
     */
    void addRequestStatus(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#requestStatus", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#dtstamp. 
     * The property indicates the date/time that the instance of the 
     * iCalendar object was created. Inspired by RFC 2445 sec. 4.8.7.1. 
     * Note that the RFC allows ONLY UTC values for this property. 
     */
    QDateTime dtstamp() const {
        QDateTime value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#dtstamp", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#dtstamp", QUrl::StrictMode)).first().value<QDateTime>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#dtstamp. 
     * The property indicates the date/time that the instance of the 
     * iCalendar object was created. Inspired by RFC 2445 sec. 4.8.7.1. 
     * Note that the RFC allows ONLY UTC values for this property. 
     */
    void setDtstamp(const QDateTime& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#dtstamp", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#dtstamp. 
     * The property indicates the date/time that the instance of the 
     * iCalendar object was created. Inspired by RFC 2445 sec. 4.8.7.1. 
     * Note that the RFC allows ONLY UTC values for this property. 
     */
    void addDtstamp(const QDateTime& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#dtstamp", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#organizer. 
     * The property defines the organizer for a calendar component. 
     * Inspired by RFC 2445 sec. 4.8.4.3. Originally this property 
     * accepted many parameters. The Organizer class has been introduced 
     * to express them all. Note that NCAL is aligned with NCO. The actual 
     * value (of the CAL-ADDRESS type) is expressed as an instance 
     * of nco:Contact. Remember that the CN parameter has been removed 
     * from NCAL. Instead that value should be expressed using nco:fullname 
     * property of the above mentioned nco:Contact instance. 
     */
    QList<QUrl> organizers() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#organizer", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#organizer. 
     * The property defines the organizer for a calendar component. 
     * Inspired by RFC 2445 sec. 4.8.4.3. Originally this property 
     * accepted many parameters. The Organizer class has been introduced 
     * to express them all. Note that NCAL is aligned with NCO. The actual 
     * value (of the CAL-ADDRESS type) is expressed as an instance 
     * of nco:Contact. Remember that the CN parameter has been removed 
     * from NCAL. Instead that value should be expressed using nco:fullname 
     * property of the above mentioned nco:Contact instance. 
     */
    void setOrganizers(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#organizer", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#organizer. 
     * The property defines the organizer for a calendar component. 
     * Inspired by RFC 2445 sec. 4.8.4.3. Originally this property 
     * accepted many parameters. The Organizer class has been introduced 
     * to express them all. Note that NCAL is aligned with NCO. The actual 
     * value (of the CAL-ADDRESS type) is expressed as an instance 
     * of nco:Contact. Remember that the CN parameter has been removed 
     * from NCAL. Instead that value should be expressed using nco:fullname 
     * property of the above mentioned nco:Contact instance. 
     */
    void addOrganizer(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#organizer", QUrl::StrictMode), value);
    }

protected:
    UnionOfEventFreebusyJournalTodo(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NCAL::UnionParentClass(uri, type) {
    }
    UnionOfEventFreebusyJournalTodo(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NCAL::UnionParentClass(res, type) {
    }
};
}
}

#endif
