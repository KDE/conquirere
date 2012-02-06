#ifndef _PIMO_COLLECTION_H_
#define _PIMO_COLLECTION_H_

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
 * A collection of Things, independent of their class. The items 
 * in the collection share a common property. Which property may 
 * be modelled explicitly or mentioned in the description of the 
 * Collection. The requirement of explicit modelling the semantic 
 * meaning of the collection is not mandatory, as collections 
 * can be created ad-hoc. Implizit modelling can be applied by 
 * the system by learning the properties. For example, a Collection 
 * of "Coworkers" could be defined as that all elements must be 
 * of class "Person" and have an attribute "work for the same Organization 
 * as the user". Further standards can be used to model these attributes. 
 */
class Collection : public virtual PIMO::Thing
{
public:
    Collection(const QUrl& uri = QUrl())
      : SimpleResource(uri), PIMO::ClassOrThingOrPropertyOrAssociation(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Collection", QUrl::StrictMode)), PIMO::ClassOrThing(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Collection", QUrl::StrictMode)), PIMO::Thing(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Collection", QUrl::StrictMode)) {
    }

    Collection(const SimpleResource& res)
      : SimpleResource(res), PIMO::ClassOrThingOrPropertyOrAssociation(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Collection", QUrl::StrictMode)), PIMO::ClassOrThing(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Collection", QUrl::StrictMode)), PIMO::Thing(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Collection", QUrl::StrictMode)) {
    }

    Collection& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Collection", QUrl::StrictMode));
        return *this;
    }

protected:
    Collection(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), PIMO::ClassOrThingOrPropertyOrAssociation(uri, type), PIMO::ClassOrThing(uri, type), PIMO::Thing(uri, type) {
    }
    Collection(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), PIMO::ClassOrThingOrPropertyOrAssociation(res, type), PIMO::ClassOrThing(res, type), PIMO::Thing(res, type) {
    }
};
}
}

#endif
