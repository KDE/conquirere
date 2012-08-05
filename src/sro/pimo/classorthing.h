#ifndef _PIMO_CLASSORTHING_H_
#define _PIMO_CLASSORTHING_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include <nepomuk2/simpleresource.h>

#include "pimo/classorthingorpropertyorassociation.h"

namespace Nepomuk2 {
namespace PIMO {
/**
 * Superclass of class and thing. To add properties to both class 
 * and thing. 
 */
class ClassOrThing : public virtual PIMO::ClassOrThingOrPropertyOrAssociation
{
public:
    ClassOrThing(const QUrl& uri = QUrl())
      : SimpleResource(uri), PIMO::ClassOrThingOrPropertyOrAssociation(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#ClassOrThing", QUrl::StrictMode)) {
    }

    ClassOrThing(const SimpleResource& res)
      : SimpleResource(res), PIMO::ClassOrThingOrPropertyOrAssociation(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#ClassOrThing", QUrl::StrictMode)) {
    }

    ClassOrThing& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#ClassOrThing", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#wikiText. 
     * A wiki-like free-text description of a Thing or a Class. The 
     * text can be formatted using a limited set of HTML elements and 
     * can contain links to other Things. The format is described in 
     * detail in the WIF specification (http://semanticweb.org/wiki/Wiki_Interchange_Format). 
     */
    QString wikiText() const {
        QString value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#wikiText", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#wikiText", QUrl::StrictMode)).first().value<QString>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#wikiText. 
     * A wiki-like free-text description of a Thing or a Class. The 
     * text can be formatted using a limited set of HTML elements and 
     * can contain links to other Things. The format is described in 
     * detail in the WIF specification (http://semanticweb.org/wiki/Wiki_Interchange_Format). 
     */
    void setWikiText(const QString& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#wikiText", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#wikiText. 
     * A wiki-like free-text description of a Thing or a Class. The 
     * text can be formatted using a limited set of HTML elements and 
     * can contain links to other Things. The format is described in 
     * detail in the WIF specification (http://semanticweb.org/wiki/Wiki_Interchange_Format). 
     */
    void addWikiText(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#wikiText", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#hasFolder. 
     * Folders can be used to store information elements related to 
     * a Thing or Class. This property can be used to connect a Class 
     * or Thing to existing Folders. Implementations can suggest 
     * annotations for documents stored inside these folders or suggest 
     * the folder for new documents related to the Thing or Class. 
     */
    QList<QUrl> folders() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#hasFolder", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#hasFolder. 
     * Folders can be used to store information elements related to 
     * a Thing or Class. This property can be used to connect a Class 
     * or Thing to existing Folders. Implementations can suggest 
     * annotations for documents stored inside these folders or suggest 
     * the folder for new documents related to the Thing or Class. 
     */
    void setFolders(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#hasFolder", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#hasFolder. 
     * Folders can be used to store information elements related to 
     * a Thing or Class. This property can be used to connect a Class 
     * or Thing to existing Folders. Implementations can suggest 
     * annotations for documents stored inside these folders or suggest 
     * the folder for new documents related to the Thing or Class. 
     */
    void addFolder(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#hasFolder", QUrl::StrictMode), value);
    }

protected:
    ClassOrThing(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), PIMO::ClassOrThingOrPropertyOrAssociation(uri, type) {
    }
    ClassOrThing(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), PIMO::ClassOrThingOrPropertyOrAssociation(res, type) {
    }
};
}
}

#endif
