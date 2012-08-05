#ifndef _NAO_TAG_H_
#define _NAO_TAG_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include <nepomuk2/simpleresource.h>

namespace Nepomuk2 {
namespace NAO {
/**
 * Represents a generic tag 
 */
class Tag : public virtual Nepomuk2::SimpleResource
{
public:
    Tag(const QUrl& uri = QUrl())
      : SimpleResource(uri) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#Tag", QUrl::StrictMode));
    }

    Tag(const SimpleResource& res)
      : SimpleResource(res) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#Tag", QUrl::StrictMode));
    }

    Tag& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#Tag", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#userVisible. 
     * Mark a property, class, or even resource as user visible or not. 
     * Non-user-visible entities should never be presented to the 
     * user. By default everything is user-visible. 
     */
    bool userVisible() const {
        bool value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#userVisible", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#userVisible", QUrl::StrictMode)).first().value<bool>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#userVisible. 
     * Mark a property, class, or even resource as user visible or not. 
     * Non-user-visible entities should never be presented to the 
     * user. By default everything is user-visible. 
     */
    void setUserVisible(const bool& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#userVisible", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#userVisible. 
     * Mark a property, class, or even resource as user visible or not. 
     * Non-user-visible entities should never be presented to the 
     * user. By default everything is user-visible. 
     */
    void addUserVisible(const bool& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#userVisible", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#prefLabel. 
     * A preferred label for a resource 
     */
    QString prefLabel() const {
        QString value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#prefLabel", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#prefLabel", QUrl::StrictMode)).first().value<QString>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#prefLabel. 
     * A preferred label for a resource 
     */
    void setPrefLabel(const QString& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#prefLabel", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#prefLabel. 
     * A preferred label for a resource 
     */
    void addPrefLabel(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#prefLabel", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.w3.org/2003/01/geo/wgs84_pos#long. 
     */
    double FAILlong() const {
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
     * Get property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#prefSymbol. 
     * A unique preferred symbol representation for a resource 
     */
    QUrl prefSymbol() const {
        QUrl value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#prefSymbol", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#prefSymbol", QUrl::StrictMode)).first().value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#prefSymbol. 
     * A unique preferred symbol representation for a resource 
     */
    void setPrefSymbol(const QUrl& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#prefSymbol", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#prefSymbol. 
     * A unique preferred symbol representation for a resource 
     */
    void addPrefSymbol(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#prefSymbol", QUrl::StrictMode), value);
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
     * Get property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#creator. 
     * Refers to the single or group of individuals that created the 
     * resource 
     */
    QUrl creator() const {
        QUrl value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#creator", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#creator", QUrl::StrictMode)).first().value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#creator. 
     * Refers to the single or group of individuals that created the 
     * resource 
     */
    void setCreator(const QUrl& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#creator", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#creator. 
     * Refers to the single or group of individuals that created the 
     * resource 
     */
    void addCreator(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#creator", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#numericRating. 
     * Annotation for a resource in the form of a numeric rating (float 
     * value), allowed values are between 1 and 10 whereas 0 is interpreted 
     * as not set 
     */
    qint64 numericRating() const {
        qint64 value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#numericRating", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#numericRating", QUrl::StrictMode)).first().value<qint64>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#numericRating. 
     * Annotation for a resource in the form of a numeric rating (float 
     * value), allowed values are between 1 and 10 whereas 0 is interpreted 
     * as not set 
     */
    void setNumericRating(const qint64& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#numericRating", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#numericRating. 
     * Annotation for a resource in the form of a numeric rating (float 
     * value), allowed values are between 1 and 10 whereas 0 is interpreted 
     * as not set 
     */
    void addNumericRating(const qint64& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#numericRating", QUrl::StrictMode), value);
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
     * Get property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#deprecated. 
     * If this property is assigned, the subject class, property, 
     * or resource, is deprecated and should not be used in production 
     * systems any longer. It may be removed without further notice. 
     */
    QUrl deprecated() const {
        QUrl value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#deprecated", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#deprecated", QUrl::StrictMode)).first().value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#deprecated. 
     * If this property is assigned, the subject class, property, 
     * or resource, is deprecated and should not be used in production 
     * systems any longer. It may be removed without further notice. 
     */
    void setDeprecated(const QUrl& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#deprecated", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#deprecated. 
     * If this property is assigned, the subject class, property, 
     * or resource, is deprecated and should not be used in production 
     * systems any longer. It may be removed without further notice. 
     */
    void addDeprecated(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#deprecated", QUrl::StrictMode), value);
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
     * Get property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#description. 
     * A non-technical textual annotation for a resource 
     */
    QStringList descriptions() const {
        QStringList value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#description", QUrl::StrictMode)))
            value << v.value<QString>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#description. 
     * A non-technical textual annotation for a resource 
     */
    void setDescriptions(const QStringList& value) {
        QVariantList values;
        foreach(const QString& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#description", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#description. 
     * A non-technical textual annotation for a resource 
     */
    void addDescription(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#description", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#lastModified. 
     * States the last modification time for a resource 
     */
    QDateTime lastModified() const {
        QDateTime value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#lastModified", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#lastModified", QUrl::StrictMode)).first().value<QDateTime>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#lastModified. 
     * States the last modification time for a resource 
     */
    void setLastModified(const QDateTime& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#lastModified", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#lastModified. 
     * States the last modification time for a resource 
     */
    void addLastModified(const QDateTime& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#lastModified", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#hasSuperResource. 
     * Defines a relationship between a resource and one or more super 
     * resources 
     */
    QList<QUrl> superResources() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#hasSuperResource", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#hasSuperResource. 
     * Defines a relationship between a resource and one or more super 
     * resources 
     */
    void setSuperResources(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#hasSuperResource", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#hasSuperResource. 
     * Defines a relationship between a resource and one or more super 
     * resources 
     */
    void addSuperResource(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#hasSuperResource", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#altLabel. 
     * An alternative label alongside the preferred label for a resource 
     */
    QStringList altLabels() const {
        QStringList value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#altLabel", QUrl::StrictMode)))
            value << v.value<QString>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#altLabel. 
     * An alternative label alongside the preferred label for a resource 
     */
    void setAltLabels(const QStringList& value) {
        QVariantList values;
        foreach(const QString& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#altLabel", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#altLabel. 
     * An alternative label alongside the preferred label for a resource 
     */
    void addAltLabel(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#altLabel", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#hasTopic. 
     * Defines a relationship between two resources, where the object 
     * is a topic of the subject 
     */
    QList<QUrl> topics() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#hasTopic", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#hasTopic. 
     * Defines a relationship between two resources, where the object 
     * is a topic of the subject 
     */
    void setTopics(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#hasTopic", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#hasTopic. 
     * Defines a relationship between two resources, where the object 
     * is a topic of the subject 
     */
    void addTopic(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#hasTopic", QUrl::StrictMode), value);
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
     * Get property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#personalIdentifier. 
     * Defines a personal string identifier for a resource 
     */
    QStringList personalIdentifiers() const {
        QStringList value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#personalIdentifier", QUrl::StrictMode)))
            value << v.value<QString>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#personalIdentifier. 
     * Defines a personal string identifier for a resource 
     */
    void setPersonalIdentifiers(const QStringList& value) {
        QVariantList values;
        foreach(const QString& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#personalIdentifier", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#personalIdentifier. 
     * Defines a personal string identifier for a resource 
     */
    void addPersonalIdentifier(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#personalIdentifier", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#hasSubResource. 
     * Defines a relationship between a resource and one or more sub 
     * resources. Descriptions of sub-resources are only interpretable 
     * when the super-resource exists. Deleting a super-resource 
     * should then also delete all sub-resources, and transferring 
     * a super-resource (for example, sending it to another user) 
     * must also include the sub-resource. 
     */
    QList<QUrl> subResources() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#hasSubResource", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#hasSubResource. 
     * Defines a relationship between a resource and one or more sub 
     * resources. Descriptions of sub-resources are only interpretable 
     * when the super-resource exists. Deleting a super-resource 
     * should then also delete all sub-resources, and transferring 
     * a super-resource (for example, sending it to another user) 
     * must also include the sub-resource. 
     */
    void setSubResources(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#hasSubResource", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#hasSubResource. 
     * Defines a relationship between a resource and one or more sub 
     * resources. Descriptions of sub-resources are only interpretable 
     * when the super-resource exists. Deleting a super-resource 
     * should then also delete all sub-resources, and transferring 
     * a super-resource (for example, sending it to another user) 
     * must also include the sub-resource. 
     */
    void addSubResource(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#hasSubResource", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#isRelated. 
     * Defines an annotation for a resource in the form of a relationship 
     * between the subject resource and another resource 
     */
    QList<QUrl> isRelateds() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#isRelated", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#isRelated. 
     * Defines an annotation for a resource in the form of a relationship 
     * between the subject resource and another resource 
     */
    void setIsRelateds(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#isRelated", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#isRelated. 
     * Defines an annotation for a resource in the form of a relationship 
     * between the subject resource and another resource 
     */
    void addIsRelated(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#isRelated", QUrl::StrictMode), value);
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
     * Get property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#contributor. 
     * Refers to a single or a group of individuals that contributed 
     * to a resource 
     */
    QList<QUrl> contributors() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#contributor", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#contributor. 
     * Refers to a single or a group of individuals that contributed 
     * to a resource 
     */
    void setContributors(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#contributor", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#contributor. 
     * Refers to a single or a group of individuals that contributed 
     * to a resource 
     */
    void addContributor(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#contributor", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#altSymbol. 
     * An alternative symbol representation for a resource 
     */
    QList<QUrl> altSymbols() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#altSymbol", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#altSymbol. 
     * An alternative symbol representation for a resource 
     */
    void setAltSymbols(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#altSymbol", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#altSymbol. 
     * An alternative symbol representation for a resource 
     */
    void addAltSymbol(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#altSymbol", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#isTopicOf. 
     * Defines a relationship between two resources, where the subject 
     * is a topic of the object 
     */
    QList<QUrl> isTopicOfs() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#isTopicOf", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#isTopicOf. 
     * Defines a relationship between two resources, where the subject 
     * is a topic of the object 
     */
    void setIsTopicOfs(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#isTopicOf", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#isTopicOf. 
     * Defines a relationship between two resources, where the subject 
     * is a topic of the object 
     */
    void addIsTopicOf(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#isTopicOf", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#modified. 
     * States the modification time for a resource 
     */
    QList<QDateTime> modifieds() const {
        QList<QDateTime> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#modified", QUrl::StrictMode)))
            value << v.value<QDateTime>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#modified. 
     * States the modification time for a resource 
     */
    void setModifieds(const QList<QDateTime>& value) {
        QVariantList values;
        foreach(const QDateTime& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#modified", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#modified. 
     * States the modification time for a resource 
     */
    void addModified(const QDateTime& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#modified", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#created. 
     * States the creation, or first modification time for a resource 
     */
    QDateTime created() const {
        QDateTime value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#created", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#created", QUrl::StrictMode)).first().value<QDateTime>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#created. 
     * States the creation, or first modification time for a resource 
     */
    void setCreated(const QDateTime& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#created", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#created. 
     * States the creation, or first modification time for a resource 
     */
    void addCreated(const QDateTime& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#created", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#hasTag. 
     * Defines an existing tag for a resource 
     */
    QList<QUrl> tags() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#hasTag", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#hasTag. 
     * Defines an existing tag for a resource 
     */
    void setTags(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#hasTag", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#hasTag. 
     * Defines an existing tag for a resource 
     */
    void addTag(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#hasTag", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#scoreParameter. 
     * A marker property to mark selected properties which are input 
     * to a mathematical algorithm to generate scores for resources. 
     * Properties are marked by being defined as subproperties of 
     * this property 
     */
    QList<double> scoreParameters() const {
        QList<double> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#scoreParameter", QUrl::StrictMode)))
            value << v.value<double>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#scoreParameter. 
     * A marker property to mark selected properties which are input 
     * to a mathematical algorithm to generate scores for resources. 
     * Properties are marked by being defined as subproperties of 
     * this property 
     */
    void setScoreParameters(const QList<double>& value) {
        QVariantList values;
        foreach(const double& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#scoreParameter", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#scoreParameter. 
     * A marker property to mark selected properties which are input 
     * to a mathematical algorithm to generate scores for resources. 
     * Properties are marked by being defined as subproperties of 
     * this property 
     */
    void addScoreParameter(const double& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#scoreParameter", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#maintainedBy. 
     * The agent that maintains this resource, ie. created it and knows 
     * what to do with it. 
     */
    QList<QUrl> maintainedBys() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#maintainedBy", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#maintainedBy. 
     * The agent that maintains this resource, ie. created it and knows 
     * what to do with it. 
     */
    void setMaintainedBys(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#maintainedBy", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#maintainedBy. 
     * The agent that maintains this resource, ie. created it and knows 
     * what to do with it. 
     */
    void addMaintainedBy(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#maintainedBy", QUrl::StrictMode), value);
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
     * Get property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#pluralPrefLabel. 
     * The plural form of the preferred label for a resource 
     */
    QString pluralPrefLabel() const {
        QString value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#pluralPrefLabel", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#pluralPrefLabel", QUrl::StrictMode)).first().value<QString>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#pluralPrefLabel. 
     * The plural form of the preferred label for a resource 
     */
    void setPluralPrefLabel(const QString& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#pluralPrefLabel", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#pluralPrefLabel. 
     * The plural form of the preferred label for a resource 
     */
    void addPluralPrefLabel(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#pluralPrefLabel", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#score. 
     * An authoritative score for an item valued between 0 and 1 
     */
    double score() const {
        double value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#score", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#score", QUrl::StrictMode)).first().value<double>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#score. 
     * An authoritative score for an item valued between 0 and 1 
     */
    void setScore(const double& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#score", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#score. 
     * An authoritative score for an item valued between 0 and 1 
     */
    void addScore(const double& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#score", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#hasSymbol. 
     * Annotation for a resource in the form of a visual representation. 
     * Typically the symbol is a double-typed image file or a nao:FreeDesktopIcon. 
     */
    QList<QUrl> symbols() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#hasSymbol", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#hasSymbol. 
     * Annotation for a resource in the form of a visual representation. 
     * Typically the symbol is a double-typed image file or a nao:FreeDesktopIcon. 
     */
    void setSymbols(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#hasSymbol", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#hasSymbol. 
     * Annotation for a resource in the form of a visual representation. 
     * Typically the symbol is a double-typed image file or a nao:FreeDesktopIcon. 
     */
    void addSymbol(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#hasSymbol", QUrl::StrictMode), value);
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
     * Get property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#isTagFor. 
     * States which resources a tag is associated with 
     */
    QList<QUrl> isTagFors() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#isTagFor", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#isTagFor. 
     * States which resources a tag is associated with 
     */
    void setIsTagFors(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#isTagFor", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/08/15/nao#isTagFor. 
     * States which resources a tag is associated with 
     */
    void addIsTagFor(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#isTagFor", QUrl::StrictMode), value);
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

protected:
    Tag(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri) {
        addType(type);
    }
    Tag(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res) {
        addType(type);
    }
};
}
}

#endif
