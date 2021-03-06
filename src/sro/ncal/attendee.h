#ifndef _NCAL_ATTENDEE_H_
#define _NCAL_ATTENDEE_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include <nepomuk2/simpleresource.h>

#include "ncal/attendeeororganizer.h"

namespace Nepomuk2 {
namespace NCAL {
/**
 * An attendee of an event. This class has been introduced to serve 
 * as the range for ncal:attendee property. See documentation 
 * of ncal:attendee for details. 
 */
class Attendee : public virtual NCAL::AttendeeOrOrganizer
{
public:
    Attendee(const QUrl& uri = QUrl())
      : SimpleResource(uri), NCAL::AttendeeOrOrganizer(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Attendee", QUrl::StrictMode)) {
    }

    Attendee(const SimpleResource& res)
      : SimpleResource(res), NCAL::AttendeeOrOrganizer(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Attendee", QUrl::StrictMode)) {
    }

    Attendee& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Attendee", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#partstat. 
     * To specify the participation status for the calendar user specified 
     * by the property. Inspired by RFC 2445 sec. 4.2.12. Originally 
     * this parameter had three sets of allowed values. Which set applied 
     * to a particular case - depended on the type of calendar entity 
     * this parameter occurred in. (event, todo, journal entry). This 
     * would be awkward to model in RDF so a single ParticipationStatus 
     * class has been introduced. Terms of the values vocabulary are 
     * expressed as instances of this class. Users are advised to pay 
     * attention which instances they use. 
     */
    QList<QUrl> partstats() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#partstat", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#partstat. 
     * To specify the participation status for the calendar user specified 
     * by the property. Inspired by RFC 2445 sec. 4.2.12. Originally 
     * this parameter had three sets of allowed values. Which set applied 
     * to a particular case - depended on the type of calendar entity 
     * this parameter occurred in. (event, todo, journal entry). This 
     * would be awkward to model in RDF so a single ParticipationStatus 
     * class has been introduced. Terms of the values vocabulary are 
     * expressed as instances of this class. Users are advised to pay 
     * attention which instances they use. 
     */
    void setPartstats(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#partstat", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#partstat. 
     * To specify the participation status for the calendar user specified 
     * by the property. Inspired by RFC 2445 sec. 4.2.12. Originally 
     * this parameter had three sets of allowed values. Which set applied 
     * to a particular case - depended on the type of calendar entity 
     * this parameter occurred in. (event, todo, journal entry). This 
     * would be awkward to model in RDF so a single ParticipationStatus 
     * class has been introduced. Terms of the values vocabulary are 
     * expressed as instances of this class. Users are advised to pay 
     * attention which instances they use. 
     */
    void addPartstat(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#partstat", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#member. 
     * To specify the group or list membership of the calendar user 
     * specified by the property. Inspired by RFC 2445 sec. 4.2.11. 
     * Originally this parameter had a value type of CAL-ADDRESS. 
     * This has been expressed as nco:Contact to promote integration 
     * between NCAL and NCO 
     */
    QList<QUrl> members() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#member", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#member. 
     * To specify the group or list membership of the calendar user 
     * specified by the property. Inspired by RFC 2445 sec. 4.2.11. 
     * Originally this parameter had a value type of CAL-ADDRESS. 
     * This has been expressed as nco:Contact to promote integration 
     * between NCAL and NCO 
     */
    void setMembers(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#member", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#member. 
     * To specify the group or list membership of the calendar user 
     * specified by the property. Inspired by RFC 2445 sec. 4.2.11. 
     * Originally this parameter had a value type of CAL-ADDRESS. 
     * This has been expressed as nco:Contact to promote integration 
     * between NCAL and NCO 
     */
    void addMember(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#member", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#delegatedFrom. 
     * To specify the calendar users that have delegated their participation 
     * to the calendar user specified by the property. Inspired by 
     * RFC 2445 sec. 4.2.4. Originally the value type for this property 
     * was CAL-ADDRESS. This has been expressed as nco:Contact to 
     * promote integration between NCAL and NCO. 
     */
    QList<QUrl> delegatedFroms() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#delegatedFrom", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#delegatedFrom. 
     * To specify the calendar users that have delegated their participation 
     * to the calendar user specified by the property. Inspired by 
     * RFC 2445 sec. 4.2.4. Originally the value type for this property 
     * was CAL-ADDRESS. This has been expressed as nco:Contact to 
     * promote integration between NCAL and NCO. 
     */
    void setDelegatedFroms(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#delegatedFrom", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#delegatedFrom. 
     * To specify the calendar users that have delegated their participation 
     * to the calendar user specified by the property. Inspired by 
     * RFC 2445 sec. 4.2.4. Originally the value type for this property 
     * was CAL-ADDRESS. This has been expressed as nco:Contact to 
     * promote integration between NCAL and NCO. 
     */
    void addDelegatedFrom(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#delegatedFrom", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#rsvp. 
     * To specify whether there is an expectation of a favor of a reply 
     * from the calendar user specified by the property value. Inspired 
     * by RFC 2445 sec. 4.2.17 
     */
    bool rsvp() const {
        bool value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#rsvp", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#rsvp", QUrl::StrictMode)).first().value<bool>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#rsvp. 
     * To specify whether there is an expectation of a favor of a reply 
     * from the calendar user specified by the property value. Inspired 
     * by RFC 2445 sec. 4.2.17 
     */
    void setRsvp(const bool& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#rsvp", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#rsvp. 
     * To specify whether there is an expectation of a favor of a reply 
     * from the calendar user specified by the property value. Inspired 
     * by RFC 2445 sec. 4.2.17 
     */
    void addRsvp(const bool& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#rsvp", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#delegatedTo. 
     * To specify the calendar users to whom the calendar user specified 
     * by the property has delegated participation. Inspired by RFC 
     * 2445 sec. 4.2.5. Originally the value type for this parameter 
     * was CAL-ADDRESS. This has been expressed as nco:Contact to 
     * promote integration between NCAL and NCO. 
     */
    QList<QUrl> delegatedTos() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#delegatedTo", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#delegatedTo. 
     * To specify the calendar users to whom the calendar user specified 
     * by the property has delegated participation. Inspired by RFC 
     * 2445 sec. 4.2.5. Originally the value type for this parameter 
     * was CAL-ADDRESS. This has been expressed as nco:Contact to 
     * promote integration between NCAL and NCO. 
     */
    void setDelegatedTos(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#delegatedTo", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#delegatedTo. 
     * To specify the calendar users to whom the calendar user specified 
     * by the property has delegated participation. Inspired by RFC 
     * 2445 sec. 4.2.5. Originally the value type for this parameter 
     * was CAL-ADDRESS. This has been expressed as nco:Contact to 
     * promote integration between NCAL and NCO. 
     */
    void addDelegatedTo(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#delegatedTo", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#cutype. 
     * To specify the type of calendar user specified by the property. 
     * Inspired by RFC 2445 sec. 4.2.3. This parameter has a limited 
     * vocabulary. The terms that may serve as values for this property 
     * have been expressed as instances of CalendarUserType class. 
     * The user may use instances provided with this ontology or create 
     * his own. 
     */
    QList<QUrl> cutypes() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#cutype", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#cutype. 
     * To specify the type of calendar user specified by the property. 
     * Inspired by RFC 2445 sec. 4.2.3. This parameter has a limited 
     * vocabulary. The terms that may serve as values for this property 
     * have been expressed as instances of CalendarUserType class. 
     * The user may use instances provided with this ontology or create 
     * his own. 
     */
    void setCutypes(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#cutype", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#cutype. 
     * To specify the type of calendar user specified by the property. 
     * Inspired by RFC 2445 sec. 4.2.3. This parameter has a limited 
     * vocabulary. The terms that may serve as values for this property 
     * have been expressed as instances of CalendarUserType class. 
     * The user may use instances provided with this ontology or create 
     * his own. 
     */
    void addCutype(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#cutype", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#role. 
     * To specify the participation role for the calendar user specified 
     * by the property. Inspired by the RFC 2445 sec. 4.2.16. Originally 
     * this property had a limited vocabulary for values. The terms 
     * of that vocabulary have been expressed as instances of the AttendeeRole 
     * class. 
     */
    QList<QUrl> roles() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#role", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#role. 
     * To specify the participation role for the calendar user specified 
     * by the property. Inspired by the RFC 2445 sec. 4.2.16. Originally 
     * this property had a limited vocabulary for values. The terms 
     * of that vocabulary have been expressed as instances of the AttendeeRole 
     * class. 
     */
    void setRoles(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#role", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#role. 
     * To specify the participation role for the calendar user specified 
     * by the property. Inspired by the RFC 2445 sec. 4.2.16. Originally 
     * this property had a limited vocabulary for values. The terms 
     * of that vocabulary have been expressed as instances of the AttendeeRole 
     * class. 
     */
    void addRole(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#role", QUrl::StrictMode), value);
    }

protected:
    Attendee(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NCAL::AttendeeOrOrganizer(uri, type) {
    }
    Attendee(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NCAL::AttendeeOrOrganizer(res, type) {
    }
};
}
}

#endif
