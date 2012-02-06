#ifndef _PIMO_TAG_H_
#define _PIMO_TAG_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include "dms-copy/simpleresource.h"

#include "pimo/thing.h"

namespace Nepomuk {
namespace PIMO {
/**
 * Tags in the context of PIMO. A marker class for Things that are 
 * used to categorize documents (or other things). Tags must be 
 * a kind of Thing and must have a unique label. Documents should 
 * not be Tags by default. 
 */
class Tag : public virtual PIMO::Thing
{
public:
    Tag(const QUrl& uri = QUrl())
      : SimpleResource(uri), PIMO::ClassOrThingOrPropertyOrAssociation(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Tag", QUrl::StrictMode)), PIMO::ClassOrThing(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Tag", QUrl::StrictMode)), PIMO::Thing(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Tag", QUrl::StrictMode)) {
    }

    Tag(const SimpleResource& res)
      : SimpleResource(res), PIMO::ClassOrThingOrPropertyOrAssociation(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Tag", QUrl::StrictMode)), PIMO::ClassOrThing(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Tag", QUrl::StrictMode)), PIMO::Thing(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Tag", QUrl::StrictMode)) {
    }

    Tag& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Tag", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#tagLabel. 
     * The unique label of the tag. The label must be unique within the 
     * scope of one PersonalInformationModel. It is required and 
     * a subproperty of nao:prefLabel. It clarifies the use of nao:personalIdentifier 
     * by restricting the scope to tags. Semantically equivalent 
     * to skos:prefLabel, where uniqueness of labels is also recommended. 
     */
    QString tagLabel() const {
        QString value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#tagLabel", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#tagLabel", QUrl::StrictMode)).first().value<QString>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#tagLabel. 
     * The unique label of the tag. The label must be unique within the 
     * scope of one PersonalInformationModel. It is required and 
     * a subproperty of nao:prefLabel. It clarifies the use of nao:personalIdentifier 
     * by restricting the scope to tags. Semantically equivalent 
     * to skos:prefLabel, where uniqueness of labels is also recommended. 
     */
    void setTagLabel(const QString& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#tagLabel", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#tagLabel. 
     * The unique label of the tag. The label must be unique within the 
     * scope of one PersonalInformationModel. It is required and 
     * a subproperty of nao:prefLabel. It clarifies the use of nao:personalIdentifier 
     * by restricting the scope to tags. Semantically equivalent 
     * to skos:prefLabel, where uniqueness of labels is also recommended. 
     */
    void addTagLabel(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#tagLabel", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#isTagFor. 
     * This thing is described further in the object thing. Similar 
     * semantics as skos:isSubjectOf. 
     */
    QList<QUrl> isTagFors() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#isTagFor", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#isTagFor. 
     * This thing is described further in the object thing. Similar 
     * semantics as skos:isSubjectOf. 
     */
    void setIsTagFors(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#isTagFor", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#isTagFor. 
     * This thing is described further in the object thing. Similar 
     * semantics as skos:isSubjectOf. 
     */
    void addIsTagFor(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#isTagFor", QUrl::StrictMode), value);
    }

protected:
    Tag(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), PIMO::ClassOrThingOrPropertyOrAssociation(uri, type), PIMO::ClassOrThing(uri, type), PIMO::Thing(uri, type) {
    }
    Tag(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), PIMO::ClassOrThingOrPropertyOrAssociation(res, type), PIMO::ClassOrThing(res, type), PIMO::Thing(res, type) {
    }
};
}
}

#endif
