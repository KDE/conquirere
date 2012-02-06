#ifndef _PIMO_MEETING_H_
#define _PIMO_MEETING_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include "dms-copy/simpleresource.h"

#include "pimo/socialevent.h"

namespace Nepomuk {
namespace PIMO {
/**
 * The social act of assembling for some common purpose; "his meeting 
 * with the salesman was the high point of his day". (Definition 
 * from SUMO) 
 */
class Meeting : public virtual PIMO::SocialEvent
{
public:
    Meeting(const QUrl& uri = QUrl())
      : SimpleResource(uri), PIMO::ClassOrThingOrPropertyOrAssociation(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Meeting", QUrl::StrictMode)), PIMO::ClassOrThing(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Meeting", QUrl::StrictMode)), PIMO::Thing(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Meeting", QUrl::StrictMode)), PIMO::ProcessConcept(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Meeting", QUrl::StrictMode)), PIMO::Event(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Meeting", QUrl::StrictMode)), PIMO::Locatable(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Meeting", QUrl::StrictMode)), PIMO::SocialEvent(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Meeting", QUrl::StrictMode)) {
    }

    Meeting(const SimpleResource& res)
      : SimpleResource(res), PIMO::ClassOrThingOrPropertyOrAssociation(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Meeting", QUrl::StrictMode)), PIMO::ClassOrThing(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Meeting", QUrl::StrictMode)), PIMO::Thing(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Meeting", QUrl::StrictMode)), PIMO::ProcessConcept(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Meeting", QUrl::StrictMode)), PIMO::Event(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Meeting", QUrl::StrictMode)), PIMO::Locatable(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Meeting", QUrl::StrictMode)), PIMO::SocialEvent(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Meeting", QUrl::StrictMode)) {
    }

    Meeting& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Meeting", QUrl::StrictMode));
        return *this;
    }

protected:
    Meeting(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), PIMO::ClassOrThingOrPropertyOrAssociation(uri, type), PIMO::ClassOrThing(uri, type), PIMO::Thing(uri, type), PIMO::ProcessConcept(uri, type), PIMO::Event(uri, type), PIMO::Locatable(uri, type), PIMO::SocialEvent(uri, type) {
    }
    Meeting(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), PIMO::ClassOrThingOrPropertyOrAssociation(res, type), PIMO::ClassOrThing(res, type), PIMO::Thing(res, type), PIMO::ProcessConcept(res, type), PIMO::Event(res, type), PIMO::Locatable(res, type), PIMO::SocialEvent(res, type) {
    }
};
}
}

#endif
