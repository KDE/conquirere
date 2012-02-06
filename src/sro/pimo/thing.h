#ifndef _PIMO_THING_H_
#define _PIMO_THING_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include "dms-copy/simpleresource.h"

#include "pimo/classorthing.h"

namespace Nepomuk {
namespace PIMO {
/**
 * Entities that are in the direct attention of the user when doing 
 * knowledge work. 
 */
class Thing : public virtual PIMO::ClassOrThing
{
public:
    Thing(const QUrl& uri = QUrl())
      : SimpleResource(uri), PIMO::ClassOrThingOrPropertyOrAssociation(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Thing", QUrl::StrictMode)), PIMO::ClassOrThing(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Thing", QUrl::StrictMode)) {
    }

    Thing(const SimpleResource& res)
      : SimpleResource(res), PIMO::ClassOrThingOrPropertyOrAssociation(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Thing", QUrl::StrictMode)), PIMO::ClassOrThing(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Thing", QUrl::StrictMode)) {
    }

    Thing& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Thing", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#occurrence. 
     * The subject Thing is represented also in the object resource. 
     * All facts added to the object resource are valid for the subject 
     * thing. The subject is the canonical represtation of the object. 
     * In particual, this implies when (?object ?p ?v) -> (?subject 
     * ?p ?v) and (?s ?p ?object) -> (?s ?p ?subject). The class of the 
     * object is not defined, but should be compatible with the class 
     * of the subject. Occurrence relations can be inferred through 
     * same identifiers or referencingOccurrence relations. 
     */
    QList<QUrl> occurrences() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#occurrence", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#occurrence. 
     * The subject Thing is represented also in the object resource. 
     * All facts added to the object resource are valid for the subject 
     * thing. The subject is the canonical represtation of the object. 
     * In particual, this implies when (?object ?p ?v) -> (?subject 
     * ?p ?v) and (?s ?p ?object) -> (?s ?p ?subject). The class of the 
     * object is not defined, but should be compatible with the class 
     * of the subject. Occurrence relations can be inferred through 
     * same identifiers or referencingOccurrence relations. 
     */
    void setOccurrences(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#occurrence", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#occurrence. 
     * The subject Thing is represented also in the object resource. 
     * All facts added to the object resource are valid for the subject 
     * thing. The subject is the canonical represtation of the object. 
     * In particual, this implies when (?object ?p ?v) -> (?subject 
     * ?p ?v) and (?s ?p ?object) -> (?s ?p ?subject). The class of the 
     * object is not defined, but should be compatible with the class 
     * of the subject. Occurrence relations can be inferred through 
     * same identifiers or referencingOccurrence relations. 
     */
    void addOccurrence(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#occurrence", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#hasTag. 
     * The subject's contents describes the object. Or the subject 
     * can be seen as belonging to the thing described by the object. 
     * Similar semantics as skos:subject. 
     */
    QList<QUrl> tags() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#hasTag", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#hasTag. 
     * The subject's contents describes the object. Or the subject 
     * can be seen as belonging to the thing described by the object. 
     * Similar semantics as skos:subject. 
     */
    void setTags(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#hasTag", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#hasTag. 
     * The subject's contents describes the object. Or the subject 
     * can be seen as belonging to the thing described by the object. 
     * Similar semantics as skos:subject. 
     */
    void addTag(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#hasTag", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#hasDeprecatedRepresentation. 
     * The subject Thing was represented previously using the object 
     * resource. This indicates that the object resource was a duplicate 
     * representation of the subject and merged with the subject. 
     * Implementations can use this property to resolve dangling 
     * links in distributed system. When encountering resources 
     * that are deprecated representations of a Thing, they should 
     * be replaced with the Thing. The range is not declared as we assume 
     * all knowledge about the object is gone, including its rdf:type. 
     */
    QList<QUrl> deprecatedRepresentations() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#hasDeprecatedRepresentation", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#hasDeprecatedRepresentation. 
     * The subject Thing was represented previously using the object 
     * resource. This indicates that the object resource was a duplicate 
     * representation of the subject and merged with the subject. 
     * Implementations can use this property to resolve dangling 
     * links in distributed system. When encountering resources 
     * that are deprecated representations of a Thing, they should 
     * be replaced with the Thing. The range is not declared as we assume 
     * all knowledge about the object is gone, including its rdf:type. 
     */
    void setDeprecatedRepresentations(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#hasDeprecatedRepresentation", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#hasDeprecatedRepresentation. 
     * The subject Thing was represented previously using the object 
     * resource. This indicates that the object resource was a duplicate 
     * representation of the subject and merged with the subject. 
     * Implementations can use this property to resolve dangling 
     * links in distributed system. When encountering resources 
     * that are deprecated representations of a Thing, they should 
     * be replaced with the Thing. The range is not declared as we assume 
     * all knowledge about the object is gone, including its rdf:type. 
     */
    void addDeprecatedRepresentation(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#hasDeprecatedRepresentation", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#partOf. 
     * This is part of the object. Like a page is part of a book or an engine 
     * is part of a car. You can make sub-properties of this to reflect 
     * more detailed relations. 
     */
    QList<QUrl> partOfs() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#partOf", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#partOf. 
     * This is part of the object. Like a page is part of a book or an engine 
     * is part of a car. You can make sub-properties of this to reflect 
     * more detailed relations. 
     */
    void setPartOfs(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#partOf", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#partOf. 
     * This is part of the object. Like a page is part of a book or an engine 
     * is part of a car. You can make sub-properties of this to reflect 
     * more detailed relations. 
     */
    void addPartOf(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#partOf", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#hasPart. 
     * The object is part of the subject. Like a page is part of a book 
     * or an engine is part of a car. You can make sub-properties of this 
     * to reflect more detailed relations. The semantics of this relations 
     * is the same as skos:narrowerPartitive 
     */
    QList<QUrl> parts() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#hasPart", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#hasPart. 
     * The object is part of the subject. Like a page is part of a book 
     * or an engine is part of a car. You can make sub-properties of this 
     * to reflect more detailed relations. The semantics of this relations 
     * is the same as skos:narrowerPartitive 
     */
    void setParts(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#hasPart", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#hasPart. 
     * The object is part of the subject. Like a page is part of a book 
     * or an engine is part of a car. You can make sub-properties of this 
     * to reflect more detailed relations. The semantics of this relations 
     * is the same as skos:narrowerPartitive 
     */
    void addPart(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#hasPart", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#hasOtherRepresentation. 
     * hasOtherRepresentation points from a Thing in your PIMO to 
     * a thing in an ontology that represents the same real world thing. 
     * This means that the real world object O represented by an instance 
     * I1 has additional representations (as instances I2-In of different 
     * conceptualizations). This means: IF (I_i represents O_j in 
     * Ontology_k) AND (I_m represents O_n in Ontology_o) THEN (O_n 
     * and O_j are the same object). hasOtherRepresentation is a transitive 
     * relation, but not equivalent (not symmetric nor reflexive). 
     * For example, the URI of a foaf:Person representation published 
     * on the web is a hasOtherRepresentation for the person. This 
     * property is inverse functional, two Things from two information 
     * models having the same hasOtherRepresentation are considered 
     * to be representations of the same entity from the real world. 
     * TODO: rename this to subjectIndicatorRef to resemble topic 
     * maps ideas? 
     */
    QList<QUrl> otherRepresentations() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#hasOtherRepresentation", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#hasOtherRepresentation. 
     * hasOtherRepresentation points from a Thing in your PIMO to 
     * a thing in an ontology that represents the same real world thing. 
     * This means that the real world object O represented by an instance 
     * I1 has additional representations (as instances I2-In of different 
     * conceptualizations). This means: IF (I_i represents O_j in 
     * Ontology_k) AND (I_m represents O_n in Ontology_o) THEN (O_n 
     * and O_j are the same object). hasOtherRepresentation is a transitive 
     * relation, but not equivalent (not symmetric nor reflexive). 
     * For example, the URI of a foaf:Person representation published 
     * on the web is a hasOtherRepresentation for the person. This 
     * property is inverse functional, two Things from two information 
     * models having the same hasOtherRepresentation are considered 
     * to be representations of the same entity from the real world. 
     * TODO: rename this to subjectIndicatorRef to resemble topic 
     * maps ideas? 
     */
    void setOtherRepresentations(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#hasOtherRepresentation", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#hasOtherRepresentation. 
     * hasOtherRepresentation points from a Thing in your PIMO to 
     * a thing in an ontology that represents the same real world thing. 
     * This means that the real world object O represented by an instance 
     * I1 has additional representations (as instances I2-In of different 
     * conceptualizations). This means: IF (I_i represents O_j in 
     * Ontology_k) AND (I_m represents O_n in Ontology_o) THEN (O_n 
     * and O_j are the same object). hasOtherRepresentation is a transitive 
     * relation, but not equivalent (not symmetric nor reflexive). 
     * For example, the URI of a foaf:Person representation published 
     * on the web is a hasOtherRepresentation for the person. This 
     * property is inverse functional, two Things from two information 
     * models having the same hasOtherRepresentation are considered 
     * to be representations of the same entity from the real world. 
     * TODO: rename this to subjectIndicatorRef to resemble topic 
     * maps ideas? 
     */
    void addOtherRepresentation(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#hasOtherRepresentation", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#isRelated. 
     * The thing is related to the other thing. Similar in meaning to 
     * skos:related. Symmetric but not transitive. 
     */
    QList<QUrl> isRelateds() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#isRelated", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#isRelated. 
     * The thing is related to the other thing. Similar in meaning to 
     * skos:related. Symmetric but not transitive. 
     */
    void setIsRelateds(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#isRelated", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#isRelated. 
     * The thing is related to the other thing. Similar in meaning to 
     * skos:related. Symmetric but not transitive. 
     */
    void addIsRelated(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#isRelated", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#objectProperty. 
     * The object of statements is another Thing. Users should be able 
     * to edit statements defined with this property. Abstract super-property. 
     */
    QList<QUrl> objectPropertys() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#objectProperty", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#objectProperty. 
     * The object of statements is another Thing. Users should be able 
     * to edit statements defined with this property. Abstract super-property. 
     */
    void setObjectPropertys(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#objectProperty", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#objectProperty. 
     * The object of statements is another Thing. Users should be able 
     * to edit statements defined with this property. Abstract super-property. 
     */
    void addObjectProperty(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#objectProperty", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#referencingOccurrence. 
     * The subject thing is described in the object document. Ideally, 
     * the document is public and its primary topic is the thing. Although 
     * this property is not inverse-functional (because the Occurrences 
     * are not canonical elements of a formal ontology) this property 
     * allows to use public documents, such as wikipedia pages, as 
     * indicators identity. The more formal hasOtherRepresentation 
     * property can be used when an ontology about the subject exists. 
     */
    QList<QUrl> referencingOccurrences() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#referencingOccurrence", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#referencingOccurrence. 
     * The subject thing is described in the object document. Ideally, 
     * the document is public and its primary topic is the thing. Although 
     * this property is not inverse-functional (because the Occurrences 
     * are not canonical elements of a formal ontology) this property 
     * allows to use public documents, such as wikipedia pages, as 
     * indicators identity. The more formal hasOtherRepresentation 
     * property can be used when an ontology about the subject exists. 
     */
    void setReferencingOccurrences(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#referencingOccurrence", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#referencingOccurrence. 
     * The subject thing is described in the object document. Ideally, 
     * the document is public and its primary topic is the thing. Although 
     * this property is not inverse-functional (because the Occurrences 
     * are not canonical elements of a formal ontology) this property 
     * allows to use public documents, such as wikipedia pages, as 
     * indicators identity. The more formal hasOtherRepresentation 
     * property can be used when an ontology about the subject exists. 
     */
    void addReferencingOccurrence(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#referencingOccurrence", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#groundingOccurrence. 
     * The subject Thing represents the entity that is described in 
     * the object InformationElement. The subject Thing is the canonical, 
     * unique representation in the personal information model for 
     * the entity described in the object. Multiple InformationElements 
     * can be the grounding occurrence of the same Thing, one InformationElement 
     * can be the groundingOccurrence of only one Thing. 
     */
    QList<QUrl> groundingOccurrences() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#groundingOccurrence", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#groundingOccurrence. 
     * The subject Thing represents the entity that is described in 
     * the object InformationElement. The subject Thing is the canonical, 
     * unique representation in the personal information model for 
     * the entity described in the object. Multiple InformationElements 
     * can be the grounding occurrence of the same Thing, one InformationElement 
     * can be the groundingOccurrence of only one Thing. 
     */
    void setGroundingOccurrences(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#groundingOccurrence", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#groundingOccurrence. 
     * The subject Thing represents the entity that is described in 
     * the object InformationElement. The subject Thing is the canonical, 
     * unique representation in the personal information model for 
     * the entity described in the object. Multiple InformationElements 
     * can be the grounding occurrence of the same Thing, one InformationElement 
     * can be the groundingOccurrence of only one Thing. 
     */
    void addGroundingOccurrence(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#groundingOccurrence", QUrl::StrictMode), value);
    }

protected:
    Thing(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), PIMO::ClassOrThingOrPropertyOrAssociation(uri, type), PIMO::ClassOrThing(uri, type) {
    }
    Thing(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), PIMO::ClassOrThingOrPropertyOrAssociation(res, type), PIMO::ClassOrThing(res, type) {
    }
};
}
}

#endif
