#ifndef _NCAL_REQUESTSTATUS_H_
#define _NCAL_REQUESTSTATUS_H_

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
 * Request Status. A class that was introduced to provide a structure 
 * for the value of ncal:requestStatus property. See documentation 
 * for ncal:requestStatus for details. 
 */
class RequestStatus : public virtual Nepomuk2::SimpleResource
{
public:
    RequestStatus(const QUrl& uri = QUrl())
      : SimpleResource(uri) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#RequestStatus", QUrl::StrictMode));
    }

    RequestStatus(const SimpleResource& res)
      : SimpleResource(res) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#RequestStatus", QUrl::StrictMode));
    }

    RequestStatus& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#RequestStatus", QUrl::StrictMode));
        return *this;
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
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#requestStatusData. 
     * Additional data associated with a request status. Inspired 
     * by the third part of the structured value for the REQUEST-STATUS 
     * property defined in RFC 2445 sec. 4.8.8.2 ("Textual exception 
     * data. For example, the offending property name and value or 
     * complete property line") 
     */
    QStringList requestStatusDatas() const {
        QStringList value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#requestStatusData", QUrl::StrictMode)))
            value << v.value<QString>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#requestStatusData. 
     * Additional data associated with a request status. Inspired 
     * by the third part of the structured value for the REQUEST-STATUS 
     * property defined in RFC 2445 sec. 4.8.8.2 ("Textual exception 
     * data. For example, the offending property name and value or 
     * complete property line") 
     */
    void setRequestStatusDatas(const QStringList& value) {
        QVariantList values;
        foreach(const QString& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#requestStatusData", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#requestStatusData. 
     * Additional data associated with a request status. Inspired 
     * by the third part of the structured value for the REQUEST-STATUS 
     * property defined in RFC 2445 sec. 4.8.8.2 ("Textual exception 
     * data. For example, the offending property name and value or 
     * complete property line") 
     */
    void addRequestStatusData(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#requestStatusData", QUrl::StrictMode), value);
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
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#returnStatus. 
     * Short return status. Inspired by the first element of the structured 
     * value of the REQUEST-STATUS property described in RFC 2445 
     * sec. 4.8.8.2. The short return status is a PERIOD character 
     * (US-ASCII decimal 46) separated 3-tuple of integers. For example, 
     * "3.1.1". The successive levels of integers provide for a successive 
     * level of status code granularity. The following are initial 
     * classes for the return status code. Individual iCalendar object 
     * methods will define specific return status codes for these 
     * classes. In addition, other classes for the return status code 
     * may be defined using the registration process defined later 
     * in this memo. 1.xx - Preliminary success. This class of status 
     * of status code indicates that the request has request has been 
     * initially processed but that completion is pending. 2.xx -Successful. 
     * This class of status code indicates that the request was completed 
     * successfuly. However, the exact status code can indicate that 
     * a fallback has been taken. 3.xx - Client Error. This class of 
     * status code indicates that the request was not successful. 
     * The error is the result of either a syntax or a semantic error 
     * in the client formatted request. Request should not be retried 
     * until the condition in the request is corrected. 4.xx - Scheduling 
     * Error. This class of status code indicates that the request 
     * was not successful. Some sort of error occurred within the calendaring 
     * and scheduling service, not directly related to the request 
     * itself. 
     */
    QString returnStatus() const {
        QString value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#returnStatus", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#returnStatus", QUrl::StrictMode)).first().value<QString>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#returnStatus. 
     * Short return status. Inspired by the first element of the structured 
     * value of the REQUEST-STATUS property described in RFC 2445 
     * sec. 4.8.8.2. The short return status is a PERIOD character 
     * (US-ASCII decimal 46) separated 3-tuple of integers. For example, 
     * "3.1.1". The successive levels of integers provide for a successive 
     * level of status code granularity. The following are initial 
     * classes for the return status code. Individual iCalendar object 
     * methods will define specific return status codes for these 
     * classes. In addition, other classes for the return status code 
     * may be defined using the registration process defined later 
     * in this memo. 1.xx - Preliminary success. This class of status 
     * of status code indicates that the request has request has been 
     * initially processed but that completion is pending. 2.xx -Successful. 
     * This class of status code indicates that the request was completed 
     * successfuly. However, the exact status code can indicate that 
     * a fallback has been taken. 3.xx - Client Error. This class of 
     * status code indicates that the request was not successful. 
     * The error is the result of either a syntax or a semantic error 
     * in the client formatted request. Request should not be retried 
     * until the condition in the request is corrected. 4.xx - Scheduling 
     * Error. This class of status code indicates that the request 
     * was not successful. Some sort of error occurred within the calendaring 
     * and scheduling service, not directly related to the request 
     * itself. 
     */
    void setReturnStatus(const QString& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#returnStatus", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#returnStatus. 
     * Short return status. Inspired by the first element of the structured 
     * value of the REQUEST-STATUS property described in RFC 2445 
     * sec. 4.8.8.2. The short return status is a PERIOD character 
     * (US-ASCII decimal 46) separated 3-tuple of integers. For example, 
     * "3.1.1". The successive levels of integers provide for a successive 
     * level of status code granularity. The following are initial 
     * classes for the return status code. Individual iCalendar object 
     * methods will define specific return status codes for these 
     * classes. In addition, other classes for the return status code 
     * may be defined using the registration process defined later 
     * in this memo. 1.xx - Preliminary success. This class of status 
     * of status code indicates that the request has request has been 
     * initially processed but that completion is pending. 2.xx -Successful. 
     * This class of status code indicates that the request was completed 
     * successfuly. However, the exact status code can indicate that 
     * a fallback has been taken. 3.xx - Client Error. This class of 
     * status code indicates that the request was not successful. 
     * The error is the result of either a syntax or a semantic error 
     * in the client formatted request. Request should not be retried 
     * until the condition in the request is corrected. 4.xx - Scheduling 
     * Error. This class of status code indicates that the request 
     * was not successful. Some sort of error occurred within the calendaring 
     * and scheduling service, not directly related to the request 
     * itself. 
     */
    void addReturnStatus(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#returnStatus", QUrl::StrictMode), value);
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
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#statusDescription. 
     * Longer return status description. Inspired by the second part 
     * of the structured value of the REQUEST-STATUS property defined 
     * in RFC 2445 sec. 4.8.8.2 
     */
    QString statusDescription() const {
        QString value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#statusDescription", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#statusDescription", QUrl::StrictMode)).first().value<QString>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#statusDescription. 
     * Longer return status description. Inspired by the second part 
     * of the structured value of the REQUEST-STATUS property defined 
     * in RFC 2445 sec. 4.8.8.2 
     */
    void setStatusDescription(const QString& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#statusDescription", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#statusDescription. 
     * Longer return status description. Inspired by the second part 
     * of the structured value of the REQUEST-STATUS property defined 
     * in RFC 2445 sec. 4.8.8.2 
     */
    void addStatusDescription(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#statusDescription", QUrl::StrictMode), value);
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

protected:
    RequestStatus(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri) {
        addType(type);
    }
    RequestStatus(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res) {
        addType(type);
    }
};
}
}

#endif
