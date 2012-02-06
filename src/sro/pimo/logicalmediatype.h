#ifndef _PIMO_LOGICALMEDIATYPE_H_
#define _PIMO_LOGICALMEDIATYPE_H_

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
 * Logical media types represent the content aspect of information 
 * elements e.g. a flyer, a contract, a promotional video, a todo 
 * list. The user can create new logical media types dependend 
 * on their domain: a salesman will need MarketingFlyer, Offer, 
 * Invoice while a student might create Report, Thesis and Homework. 
 * This is independent from the information element and data object 
 * (NIE/NFO) in which the media type will be stored. The same contract 
 * can be stored in a PDF file, a text file, or an HTML website. The 
 * groundingOccurrence of a LogicalMediaType is the Document 
 * that stores the content. 
 */
class LogicalMediaType : public virtual PIMO::Thing
{
public:
    LogicalMediaType(const QUrl& uri = QUrl())
      : SimpleResource(uri), PIMO::ClassOrThingOrPropertyOrAssociation(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#LogicalMediaType", QUrl::StrictMode)), PIMO::ClassOrThing(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#LogicalMediaType", QUrl::StrictMode)), PIMO::Thing(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#LogicalMediaType", QUrl::StrictMode)) {
    }

    LogicalMediaType(const SimpleResource& res)
      : SimpleResource(res), PIMO::ClassOrThingOrPropertyOrAssociation(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#LogicalMediaType", QUrl::StrictMode)), PIMO::ClassOrThing(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#LogicalMediaType", QUrl::StrictMode)), PIMO::Thing(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#LogicalMediaType", QUrl::StrictMode)) {
    }

    LogicalMediaType& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#LogicalMediaType", QUrl::StrictMode));
        return *this;
    }

protected:
    LogicalMediaType(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), PIMO::ClassOrThingOrPropertyOrAssociation(uri, type), PIMO::ClassOrThing(uri, type), PIMO::Thing(uri, type) {
    }
    LogicalMediaType(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), PIMO::ClassOrThingOrPropertyOrAssociation(res, type), PIMO::ClassOrThing(res, type), PIMO::Thing(res, type) {
    }
};
}
}

#endif
