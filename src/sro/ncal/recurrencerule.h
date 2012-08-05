#ifndef _NCAL_RECURRENCERULE_H_
#define _NCAL_RECURRENCERULE_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include <nepomuk2/simpleresource.h>

namespace Nepomuk2 {
namespace NCAL {
/**
 * 
 */
class RecurrenceRule : public virtual Nepomuk2::SimpleResource
{
public:
    RecurrenceRule(const QUrl& uri = QUrl())
      : SimpleResource(uri) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#RecurrenceRule", QUrl::StrictMode));
    }

    RecurrenceRule(const SimpleResource& res)
      : SimpleResource(res) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#RecurrenceRule", QUrl::StrictMode));
    }

    RecurrenceRule& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#RecurrenceRule", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.w3.org/2003/01/geo/wgs84_pos#long. 
     */
    double long() const {
        double value;
        if(contains(QUrl::fromEncoded("http://www.w3.org/2003/01/geo/wgs84_pos#long", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.w3.org/2003/01/geo/wgs84_pos#long", QUrl::StrictMode)).first().value<double>();
        return value;
    }

    /**
     * Set property http://www.w3.org/2003/01/geo/wgs84_pos#long. 
     */
    void setLong(const double& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.w3.org/2003/01/geo/wgs84_pos#long", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.w3.org/2003/01/geo/wgs84_pos#long. 
     */
    void addLong(const double& value) {
        addProperty(QUrl::fromEncoded("http://www.w3.org/2003/01/geo/wgs84_pos#long", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#byday. 
     * Weekdays the recurrence should occur. Defined in RFC 2445 sec. 
     * 4.3.10 
     */
    QList<QUrl> bydays() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#byday", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#byday. 
     * Weekdays the recurrence should occur. Defined in RFC 2445 sec. 
     * 4.3.10 
     */
    void setBydays(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#byday", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#byday. 
     * Weekdays the recurrence should occur. Defined in RFC 2445 sec. 
     * 4.3.10 
     */
    void addByday(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#byday", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.w3.org/2003/01/geo/wgs84_pos#alt. 
     */
    double alt() const {
        double value;
        if(contains(QUrl::fromEncoded("http://www.w3.org/2003/01/geo/wgs84_pos#alt", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.w3.org/2003/01/geo/wgs84_pos#alt", QUrl::StrictMode)).first().value<double>();
        return value;
    }

    /**
     * Set property http://www.w3.org/2003/01/geo/wgs84_pos#alt. 
     */
    void setAlt(const double& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.w3.org/2003/01/geo/wgs84_pos#alt", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.w3.org/2003/01/geo/wgs84_pos#alt. 
     */
    void addAlt(const double& value) {
        addProperty(QUrl::fromEncoded("http://www.w3.org/2003/01/geo/wgs84_pos#alt", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#sideChannels. 
     * Number of side channels 
     */
    qint64 sideChannels() const {
        qint64 value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#sideChannels", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#sideChannels", QUrl::StrictMode)).first().value<qint64>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#sideChannels. 
     * Number of side channels 
     */
    void setSideChannels(const qint64& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#sideChannels", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#sideChannels. 
     * Number of side channels 
     */
    void addSideChannels(const qint64& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#sideChannels", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#lfeChannels. 
     * Number of Low Frequency Expansion (subwoofer) channels. 
     */
    qint64 lfeChannels() const {
        qint64 value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#lfeChannels", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#lfeChannels", QUrl::StrictMode)).first().value<qint64>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#lfeChannels. 
     * Number of Low Frequency Expansion (subwoofer) channels. 
     */
    void setLfeChannels(const qint64& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#lfeChannels", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#lfeChannels. 
     * Number of Low Frequency Expansion (subwoofer) channels. 
     */
    void addLfeChannels(const qint64& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#lfeChannels", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#wkst. 
     * The day that's counted as the start of the week. It is used to disambiguate 
     * the byweekno rule. Defined in RFC 2445 sec. 4.3.10 
     */
    QUrl wkst() const {
        QUrl value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#wkst", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#wkst", QUrl::StrictMode)).first().value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#wkst. 
     * The day that's counted as the start of the week. It is used to disambiguate 
     * the byweekno rule. Defined in RFC 2445 sec. 4.3.10 
     */
    void setWkst(const QUrl& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#wkst", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#wkst. 
     * The day that's counted as the start of the week. It is used to disambiguate 
     * the byweekno rule. Defined in RFC 2445 sec. 4.3.10 
     */
    void addWkst(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#wkst", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#count. 
     * How many times should an event be repeated. Defined in RFC 2445 
     * sec. 4.3.10 
     */
    qint64 count() const {
        qint64 value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#count", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#count", QUrl::StrictMode)).first().value<qint64>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#count. 
     * How many times should an event be repeated. Defined in RFC 2445 
     * sec. 4.3.10 
     */
    void setCount(const qint64& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#count", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#count. 
     * How many times should an event be repeated. Defined in RFC 2445 
     * sec. 4.3.10 
     */
    void addCount(const qint64& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#count", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#bysecond. 
     * Second of a recurrence. Defined in RFC 2445 sec. 4.3.10 
     */
    QList<qint64> byseconds() const {
        QList<qint64> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#bysecond", QUrl::StrictMode)))
            value << v.value<qint64>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#bysecond. 
     * Second of a recurrence. Defined in RFC 2445 sec. 4.3.10 
     */
    void setByseconds(const QList<qint64>& value) {
        QVariantList values;
        foreach(const qint64& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#bysecond", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#bysecond. 
     * Second of a recurrence. Defined in RFC 2445 sec. 4.3.10 
     */
    void addBysecond(const qint64& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#bysecond", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#groundingForDeletedThing. 
     * This NIE Information Element was used as a grounding occurrence 
     * for the object Thing. The Thing was then deleted by the user manually, 
     * indicating that this Information Element should not cause 
     * an automatic creation of another Thing in the future. The object 
     * resource has no range to indicate that it was completely removed 
     * from the user's PIMO, including the rdf:type statement. Relevant 
     * for data alignment and enrichment algorithms. 
     */
    QList<QUrl> groundingForDeletedThings() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#groundingForDeletedThing", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#groundingForDeletedThing. 
     * This NIE Information Element was used as a grounding occurrence 
     * for the object Thing. The Thing was then deleted by the user manually, 
     * indicating that this Information Element should not cause 
     * an automatic creation of another Thing in the future. The object 
     * resource has no range to indicate that it was completely removed 
     * from the user's PIMO, including the rdf:type statement. Relevant 
     * for data alignment and enrichment algorithms. 
     */
    void setGroundingForDeletedThings(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#groundingForDeletedThing", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#groundingForDeletedThing. 
     * This NIE Information Element was used as a grounding occurrence 
     * for the object Thing. The Thing was then deleted by the user manually, 
     * indicating that this Information Element should not cause 
     * an automatic creation of another Thing in the future. The object 
     * resource has no range to indicate that it was completely removed 
     * from the user's PIMO, including the rdf:type statement. Relevant 
     * for data alignment and enrichment algorithms. 
     */
    void addGroundingForDeletedThing(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#groundingForDeletedThing", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/01/19/nie#copyright. 
     * Content copyright 
     */
    QStringList copyrights() const {
        QStringList value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#copyright", QUrl::StrictMode)))
            value << v.value<QString>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/01/19/nie#copyright. 
     * Content copyright 
     */
    void setCopyrights(const QStringList& value) {
        QVariantList values;
        foreach(const QString& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#copyright", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/01/19/nie#copyright. 
     * Content copyright 
     */
    void addCopyright(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#copyright", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#byhour. 
     * Hour of recurrence. Defined in RFC 2445 sec. 4.3.10 
     */
    QList<qint64> byhours() const {
        QList<qint64> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#byhour", QUrl::StrictMode)))
            value << v.value<qint64>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#byhour. 
     * Hour of recurrence. Defined in RFC 2445 sec. 4.3.10 
     */
    void setByhours(const QList<qint64>& value) {
        QVariantList values;
        foreach(const qint64& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#byhour", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#byhour. 
     * Hour of recurrence. Defined in RFC 2445 sec. 4.3.10 
     */
    void addByhour(const qint64& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#byhour", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/01/19/nie#depends. 
     * Dependency relation. A piece of content depends on another 
     * piece of data in order to be properly understood/used/interpreted. 
     */
    QList<QUrl> dependses() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#depends", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/01/19/nie#depends. 
     * Dependency relation. A piece of content depends on another 
     * piece of data in order to be properly understood/used/interpreted. 
     */
    void setDependses(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#depends", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/01/19/nie#depends. 
     * Dependency relation. A piece of content depends on another 
     * piece of data in order to be properly understood/used/interpreted. 
     */
    void addDepends(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#depends", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#byminute. 
     * Minute of recurrence. Defined in RFC 2445 sec. 4.3.10 
     */
    QList<qint64> byminutes() const {
        QList<qint64> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#byminute", QUrl::StrictMode)))
            value << v.value<qint64>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#byminute. 
     * Minute of recurrence. Defined in RFC 2445 sec. 4.3.10 
     */
    void setByminutes(const QList<qint64>& value) {
        QVariantList values;
        foreach(const qint64& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#byminute", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#byminute. 
     * Minute of recurrence. Defined in RFC 2445 sec. 4.3.10 
     */
    void addByminute(const qint64& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#byminute", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/01/19/nie#disclaimer. 
     * A disclaimer 
     */
    QStringList disclaimers() const {
        QStringList value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#disclaimer", QUrl::StrictMode)))
            value << v.value<QString>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/01/19/nie#disclaimer. 
     * A disclaimer 
     */
    void setDisclaimers(const QStringList& value) {
        QVariantList values;
        foreach(const QString& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#disclaimer", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/01/19/nie#disclaimer. 
     * A disclaimer 
     */
    void addDisclaimer(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#disclaimer", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/01/19/nie#licenseType. 
     * The type of the license. Possible values for this field may include 
     * "GPL", "BSD", "Creative Commons" etc. 
     */
    QStringList licenseTypes() const {
        QStringList value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#licenseType", QUrl::StrictMode)))
            value << v.value<QString>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/01/19/nie#licenseType. 
     * The type of the license. Possible values for this field may include 
     * "GPL", "BSD", "Creative Commons" etc. 
     */
    void setLicenseTypes(const QStringList& value) {
        QVariantList values;
        foreach(const QString& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#licenseType", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/01/19/nie#licenseType. 
     * The type of the license. Possible values for this field may include 
     * "GPL", "BSD", "Creative Commons" etc. 
     */
    void addLicenseType(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#licenseType", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#freq. 
     * Frequency of a recurrence rule. Defined in RFC 2445 sec. 4.3.10 
     */
    QUrl freq() const {
        QUrl value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#freq", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#freq", QUrl::StrictMode)).first().value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#freq. 
     * Frequency of a recurrence rule. Defined in RFC 2445 sec. 4.3.10 
     */
    void setFreq(const QUrl& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#freq", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#freq. 
     * Frequency of a recurrence rule. Defined in RFC 2445 sec. 4.3.10 
     */
    void addFreq(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#freq", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#rearChannels. 
     * Number of rear channels. 
     */
    qint64 rearChannels() const {
        qint64 value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#rearChannels", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#rearChannels", QUrl::StrictMode)).first().value<qint64>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#rearChannels. 
     * Number of rear channels. 
     */
    void setRearChannels(const qint64& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#rearChannels", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#rearChannels. 
     * Number of rear channels. 
     */
    void addRearChannels(const qint64& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#rearChannels", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#interval. 
     * The INTERVAL rule part contains a positive integer representing 
     * how often the recurrence rule repeats. The default value is 
     * "1", meaning every second for a SECONDLY rule, or every minute 
     * for a MINUTELY rule, every hour for an HOURLY rule, every day 
     * for a DAILY rule, every week for a WEEKLY rule, every month for 
     * a MONTHLY rule andevery year for a YEARLY rule. Defined in RFC 
     * 2445 sec. 4.3.10 
     */
    qint64 interval() const {
        qint64 value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#interval", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#interval", QUrl::StrictMode)).first().value<qint64>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#interval. 
     * The INTERVAL rule part contains a positive integer representing 
     * how often the recurrence rule repeats. The default value is 
     * "1", meaning every second for a SECONDLY rule, or every minute 
     * for a MINUTELY rule, every hour for an HOURLY rule, every day 
     * for a DAILY rule, every week for a WEEKLY rule, every month for 
     * a MONTHLY rule andevery year for a YEARLY rule. Defined in RFC 
     * 2445 sec. 4.3.10 
     */
    void setInterval(const qint64& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#interval", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#interval. 
     * The INTERVAL rule part contains a positive integer representing 
     * how often the recurrence rule repeats. The default value is 
     * "1", meaning every second for a SECONDLY rule, or every minute 
     * for a MINUTELY rule, every hour for an HOURLY rule, every day 
     * for a DAILY rule, every week for a WEEKLY rule, every month for 
     * a MONTHLY rule andevery year for a YEARLY rule. Defined in RFC 
     * 2445 sec. 4.3.10 
     */
    void addInterval(const qint64& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#interval", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#byyearday. 
     * Day of the year the event should occur. Defined in RFC 2445 sec. 
     * 4.3.10 
     */
    QList<qint64> byyeardays() const {
        QList<qint64> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#byyearday", QUrl::StrictMode)))
            value << v.value<qint64>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#byyearday. 
     * Day of the year the event should occur. Defined in RFC 2445 sec. 
     * 4.3.10 
     */
    void setByyeardays(const QList<qint64>& value) {
        QVariantList values;
        foreach(const qint64& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#byyearday", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#byyearday. 
     * Day of the year the event should occur. Defined in RFC 2445 sec. 
     * 4.3.10 
     */
    void addByyearday(const qint64& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#byyearday", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#until. 
     * The UNTIL rule part defines a date-time value which bounds the 
     * recurrence rule in an inclusive manner. If the value specified 
     * by UNTIL is synchronized with the specified recurrence, this 
     * date or date-time becomes the last instance of the recurrence. 
     * If specified as a date-time value, then it MUST be specified 
     * in an UTC time format. If not present, and the COUNT rule part 
     * is also not present, the RRULE is considered to repeat forever. 
     */
    QList<QDateTime> untils() const {
        QList<QDateTime> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#until", QUrl::StrictMode)))
            value << v.value<QDateTime>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#until. 
     * The UNTIL rule part defines a date-time value which bounds the 
     * recurrence rule in an inclusive manner. If the value specified 
     * by UNTIL is synchronized with the specified recurrence, this 
     * date or date-time becomes the last instance of the recurrence. 
     * If specified as a date-time value, then it MUST be specified 
     * in an UTC time format. If not present, and the COUNT rule part 
     * is also not present, the RRULE is considered to repeat forever. 
     */
    void setUntils(const QList<QDateTime>& value) {
        QVariantList values;
        foreach(const QDateTime& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#until", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#until. 
     * The UNTIL rule part defines a date-time value which bounds the 
     * recurrence rule in an inclusive manner. If the value specified 
     * by UNTIL is synchronized with the specified recurrence, this 
     * date or date-time becomes the last instance of the recurrence. 
     * If specified as a date-time value, then it MUST be specified 
     * in an UTC time format. If not present, and the COUNT rule part 
     * is also not present, the RRULE is considered to repeat forever. 
     */
    void addUntil(const QDateTime& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#until", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#byweekno. 
     * The number of the week an event should recur. Defined in RFC 2445 
     * sec. 4.3.10 
     */
    QList<qint64> byweeknos() const {
        QList<qint64> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#byweekno", QUrl::StrictMode)))
            value << v.value<qint64>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#byweekno. 
     * The number of the week an event should recur. Defined in RFC 2445 
     * sec. 4.3.10 
     */
    void setByweeknos(const QList<qint64>& value) {
        QVariantList values;
        foreach(const qint64& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#byweekno", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#byweekno. 
     * The number of the week an event should recur. Defined in RFC 2445 
     * sec. 4.3.10 
     */
    void addByweekno(const qint64& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#byweekno", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#depiction. 
     * Relates an information element to an image which depicts said 
     * element. 
     */
    QList<QUrl> depictions() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#depiction", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#depiction. 
     * Relates an information element to an image which depicts said 
     * element. 
     */
    void setDepictions(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#depiction", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#depiction. 
     * Relates an information element to an image which depicts said 
     * element. 
     */
    void addDepiction(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#depiction", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#isWriteable. 
     * Defines if this information model can be modified by the user 
     * of the system. This is usually false for imported ontologies 
     * and true for the user's own PersonalInformationModel. 
     */
    QStringList isWriteables() const {
        QStringList value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#isWriteable", QUrl::StrictMode)))
            value << v.value<QString>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#isWriteable. 
     * Defines if this information model can be modified by the user 
     * of the system. This is usually false for imported ontologies 
     * and true for the user's own PersonalInformationModel. 
     */
    void setIsWriteables(const QStringList& value) {
        QVariantList values;
        foreach(const QString& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#isWriteable", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#isWriteable. 
     * Defines if this information model can be modified by the user 
     * of the system. This is usually false for imported ontologies 
     * and true for the user's own PersonalInformationModel. 
     */
    void addIsWriteable(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#isWriteable", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/01/19/nie#license. 
     * Terms and intellectual property rights licensing conditions. 
     */
    QStringList licenses() const {
        QStringList value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#license", QUrl::StrictMode)))
            value << v.value<QString>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/01/19/nie#license. 
     * Terms and intellectual property rights licensing conditions. 
     */
    void setLicenses(const QStringList& value) {
        QVariantList values;
        foreach(const QString& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#license", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/01/19/nie#license. 
     * Terms and intellectual property rights licensing conditions. 
     */
    void addLicense(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#license", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#frontChannels. 
     * Number of front channels. 
     */
    qint64 frontChannels() const {
        qint64 value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#frontChannels", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#frontChannels", QUrl::StrictMode)).first().value<qint64>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#frontChannels. 
     * Number of front channels. 
     */
    void setFrontChannels(const qint64& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#frontChannels", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#frontChannels. 
     * Number of front channels. 
     */
    void addFrontChannels(const qint64& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#frontChannels", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#bysetpos. 
     * The BYSETPOS rule part specify values which correspond to the 
     * nth occurrence within the set of events specified by the rule. 
     * Valid values are 1 to 366 or -366 to -1. It MUST only be used in conjunction 
     * with another BYxxx rule part. For example "the last work day 
     * of the month" could be represented as: RRULE: FREQ=MONTHLY; 
     * BYDAY=MO, TU, WE, TH, FR; BYSETPOS=-1. Each BYSETPOS value 
     * can include a positive (+n) or negative (-n) integer. If present, 
     * this indicates the nth occurrence of the specific occurrence 
     * within the set of events specified by the rule. Defined in RFC 
     * 2445 sec. 4.3.10 
     */
    QList<qint64> bysetposes() const {
        QList<qint64> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#bysetpos", QUrl::StrictMode)))
            value << v.value<qint64>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#bysetpos. 
     * The BYSETPOS rule part specify values which correspond to the 
     * nth occurrence within the set of events specified by the rule. 
     * Valid values are 1 to 366 or -366 to -1. It MUST only be used in conjunction 
     * with another BYxxx rule part. For example "the last work day 
     * of the month" could be represented as: RRULE: FREQ=MONTHLY; 
     * BYDAY=MO, TU, WE, TH, FR; BYSETPOS=-1. Each BYSETPOS value 
     * can include a positive (+n) or negative (-n) integer. If present, 
     * this indicates the nth occurrence of the specific occurrence 
     * within the set of events specified by the rule. Defined in RFC 
     * 2445 sec. 4.3.10 
     */
    void setBysetposes(const QList<qint64>& value) {
        QVariantList values;
        foreach(const qint64& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#bysetpos", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#bysetpos. 
     * The BYSETPOS rule part specify values which correspond to the 
     * nth occurrence within the set of events specified by the rule. 
     * Valid values are 1 to 366 or -366 to -1. It MUST only be used in conjunction 
     * with another BYxxx rule part. For example "the last work day 
     * of the month" could be represented as: RRULE: FREQ=MONTHLY; 
     * BYDAY=MO, TU, WE, TH, FR; BYSETPOS=-1. Each BYSETPOS value 
     * can include a positive (+n) or negative (-n) integer. If present, 
     * this indicates the nth occurrence of the specific occurrence 
     * within the set of events specified by the rule. Defined in RFC 
     * 2445 sec. 4.3.10 
     */
    void addBysetpos(const qint64& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#bysetpos", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#bymonth. 
     * Number of the month of the recurrence. Valid values are integers 
     * from 1 (January) to 12 (December). Defined in RFC 2445 sec. 4.3.10 
     */
    QList<qint64> bymonths() const {
        QList<qint64> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#bymonth", QUrl::StrictMode)))
            value << v.value<qint64>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#bymonth. 
     * Number of the month of the recurrence. Valid values are integers 
     * from 1 (January) to 12 (December). Defined in RFC 2445 sec. 4.3.10 
     */
    void setBymonths(const QList<qint64>& value) {
        QVariantList values;
        foreach(const qint64& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#bymonth", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#bymonth. 
     * Number of the month of the recurrence. Valid values are integers 
     * from 1 (January) to 12 (December). Defined in RFC 2445 sec. 4.3.10 
     */
    void addBymonth(const qint64& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#bymonth", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.w3.org/2003/01/geo/wgs84_pos#lat. 
     */
    double lat() const {
        double value;
        if(contains(QUrl::fromEncoded("http://www.w3.org/2003/01/geo/wgs84_pos#lat", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.w3.org/2003/01/geo/wgs84_pos#lat", QUrl::StrictMode)).first().value<double>();
        return value;
    }

    /**
     * Set property http://www.w3.org/2003/01/geo/wgs84_pos#lat. 
     */
    void setLat(const double& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.w3.org/2003/01/geo/wgs84_pos#lat", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.w3.org/2003/01/geo/wgs84_pos#lat. 
     */
    void addLat(const double& value) {
        addProperty(QUrl::fromEncoded("http://www.w3.org/2003/01/geo/wgs84_pos#lat", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#classRole. 
     * Annotating abstract and concrete classes. Implementations 
     * may offer the feature to hide abstract classes. By default, 
     * classes are concrete. Classes can be declared abstract by setting 
     * their classRole to abstract. Instances should not have an abstract 
     * class as type (if not inferred). 
     */
    QUrl classRole() const {
        QUrl value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#classRole", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#classRole", QUrl::StrictMode)).first().value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#classRole. 
     * Annotating abstract and concrete classes. Implementations 
     * may offer the feature to hide abstract classes. By default, 
     * classes are concrete. Classes can be declared abstract by setting 
     * their classRole to abstract. Instances should not have an abstract 
     * class as type (if not inferred). 
     */
    void setClassRole(const QUrl& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#classRole", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#classRole. 
     * Annotating abstract and concrete classes. Implementations 
     * may offer the feature to hide abstract classes. By default, 
     * classes are concrete. Classes can be declared abstract by setting 
     * their classRole to abstract. Instances should not have an abstract 
     * class as type (if not inferred). 
     */
    void addClassRole(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#classRole", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#bymonthday. 
     * Day of the month when the event should recur. Defined in RFC 2445 
     * sec. 4.3.10 
     */
    QList<qint64> bymonthdays() const {
        QList<qint64> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#bymonthday", QUrl::StrictMode)))
            value << v.value<qint64>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#bymonthday. 
     * Day of the month when the event should recur. Defined in RFC 2445 
     * sec. 4.3.10 
     */
    void setBymonthdays(const QList<qint64>& value) {
        QVariantList values;
        foreach(const qint64& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#bymonthday", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#bymonthday. 
     * Day of the month when the event should recur. Defined in RFC 2445 
     * sec. 4.3.10 
     */
    void addBymonthday(const qint64& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#bymonthday", QUrl::StrictMode), value);
    }

protected:
    RecurrenceRule(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri) {
        addType(type);
    }
    RecurrenceRule(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res) {
        addType(type);
    }
};
}
}

#endif
