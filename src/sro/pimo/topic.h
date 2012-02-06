#ifndef _PIMO_TOPIC_H_
#define _PIMO_TOPIC_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include "dms-copy/simpleresource.h"

#include "pimo/tag.h"
#include "pimo/thing.h"

namespace Nepomuk {
namespace PIMO {
/**
 * A topic is the subject of a discussion or document. Topics are 
 * distinguished from Things in their taxonomic nature, examples 
 * are scientific areas such as "Information Science", "Biology", 
 * or categories used in content syndication such as "Sports", 
 * "Politics". They are specific to the user's domain. 
 */
class Topic : public virtual PIMO::Tag, public virtual PIMO::Thing
{
public:
    Topic(const QUrl& uri = QUrl())
      : SimpleResource(uri), PIMO::ClassOrThingOrPropertyOrAssociation(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Topic", QUrl::StrictMode)), PIMO::ClassOrThing(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Topic", QUrl::StrictMode)), PIMO::Thing(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Topic", QUrl::StrictMode)), PIMO::Tag(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Topic", QUrl::StrictMode)) {
    }

    Topic(const SimpleResource& res)
      : SimpleResource(res), PIMO::ClassOrThingOrPropertyOrAssociation(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Topic", QUrl::StrictMode)), PIMO::ClassOrThing(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Topic", QUrl::StrictMode)), PIMO::Thing(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Topic", QUrl::StrictMode)), PIMO::Tag(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Topic", QUrl::StrictMode)) {
    }

    Topic& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Topic", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#superTopic. 
     * The object topic is more general in meaning than the subject 
     * topic. Transitive. Similar to skos:broader. 
     */
    QList<QUrl> superTopics() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#superTopic", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#superTopic. 
     * The object topic is more general in meaning than the subject 
     * topic. Transitive. Similar to skos:broader. 
     */
    void setSuperTopics(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#superTopic", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#superTopic. 
     * The object topic is more general in meaning than the subject 
     * topic. Transitive. Similar to skos:broader. 
     */
    void addSuperTopic(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#superTopic", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#subTopic. 
     * The object topic is more specific in meaning than the subject 
     * topic. Transitive. Similar in meaning to skos:narrower 
     */
    QList<QUrl> subTopics() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#subTopic", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#subTopic. 
     * The object topic is more specific in meaning than the subject 
     * topic. Transitive. Similar in meaning to skos:narrower 
     */
    void setSubTopics(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#subTopic", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#subTopic. 
     * The object topic is more specific in meaning than the subject 
     * topic. Transitive. Similar in meaning to skos:narrower 
     */
    void addSubTopic(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#subTopic", QUrl::StrictMode), value);
    }

protected:
    Topic(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), PIMO::ClassOrThingOrPropertyOrAssociation(uri, type), PIMO::ClassOrThing(uri, type), PIMO::Thing(uri, type), PIMO::Tag(uri, type) {
    }
    Topic(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), PIMO::ClassOrThingOrPropertyOrAssociation(res, type), PIMO::ClassOrThing(res, type), PIMO::Thing(res, type), PIMO::Tag(res, type) {
    }
};
}
}

#endif
