#ifndef _PIMO_PROJECT_H_
#define _PIMO_PROJECT_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include <nepomuk2/simpleresource.h>

#include "pimo/processconcept.h"

namespace Nepomuk2 {
namespace PIMO {
/**
 * Any piece of work that is undertaken or attempted (Wordnet). 
 * An enterprise carefully planned to achieve a particular aim 
 * (Oxford Dictionary). 
 */
class Project : public virtual PIMO::ProcessConcept
{
public:
    Project(const QUrl& uri = QUrl())
      : SimpleResource(uri), PIMO::ClassOrThingOrPropertyOrAssociation(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Project", QUrl::StrictMode)), PIMO::ClassOrThing(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Project", QUrl::StrictMode)), PIMO::Thing(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Project", QUrl::StrictMode)), PIMO::ProcessConcept(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Project", QUrl::StrictMode)) {
    }

    Project(const SimpleResource& res)
      : SimpleResource(res), PIMO::ClassOrThingOrPropertyOrAssociation(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Project", QUrl::StrictMode)), PIMO::ClassOrThing(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Project", QUrl::StrictMode)), PIMO::Thing(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Project", QUrl::StrictMode)), PIMO::ProcessConcept(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Project", QUrl::StrictMode)) {
    }

    Project& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Project", QUrl::StrictMode));
        return *this;
    }

protected:
    Project(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), PIMO::ClassOrThingOrPropertyOrAssociation(uri, type), PIMO::ClassOrThing(uri, type), PIMO::Thing(uri, type), PIMO::ProcessConcept(uri, type) {
    }
    Project(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), PIMO::ClassOrThingOrPropertyOrAssociation(res, type), PIMO::ClassOrThing(res, type), PIMO::Thing(res, type), PIMO::ProcessConcept(res, type) {
    }
};
}
}

#endif
