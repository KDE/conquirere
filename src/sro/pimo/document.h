#ifndef _PIMO_DOCUMENT_H_
#define _PIMO_DOCUMENT_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include <nepomuk2/simpleresource.h>

#include "pimo/logicalmediatype.h"

namespace Nepomuk2 {
namespace PIMO {
/**
 * A generic document. This is a placeholder class for document-management 
 * domain ontologies to subclass. Create more and specified subclasses 
 * of pimo:Document for the document types in your domain. Documents 
 * are typically instances of both NFO:Document (modeling the 
 * information element used to store the document) and a LogicalMediaType 
 * subclass. Two examples are given for what to model here: a contract 
 * for a business domain, a BlogPost for an informal domain. 
 */
class Document : public virtual PIMO::LogicalMediaType
{
public:
    Document(const QUrl& uri = QUrl())
      : SimpleResource(uri), PIMO::ClassOrThingOrPropertyOrAssociation(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Document", QUrl::StrictMode)), PIMO::ClassOrThing(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Document", QUrl::StrictMode)), PIMO::Thing(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Document", QUrl::StrictMode)), PIMO::LogicalMediaType(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Document", QUrl::StrictMode)) {
    }

    Document(const SimpleResource& res)
      : SimpleResource(res), PIMO::ClassOrThingOrPropertyOrAssociation(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Document", QUrl::StrictMode)), PIMO::ClassOrThing(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Document", QUrl::StrictMode)), PIMO::Thing(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Document", QUrl::StrictMode)), PIMO::LogicalMediaType(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Document", QUrl::StrictMode)) {
    }

    Document& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Document", QUrl::StrictMode));
        return *this;
    }

protected:
    Document(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), PIMO::ClassOrThingOrPropertyOrAssociation(uri, type), PIMO::ClassOrThing(uri, type), PIMO::Thing(uri, type), PIMO::LogicalMediaType(uri, type) {
    }
    Document(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), PIMO::ClassOrThingOrPropertyOrAssociation(res, type), PIMO::ClassOrThing(res, type), PIMO::Thing(res, type), PIMO::LogicalMediaType(res, type) {
    }
};
}
}

#endif
