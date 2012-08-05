#ifndef _PIMO_BLOGPOST_H_
#define _PIMO_BLOGPOST_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include <nepomuk2/simpleresource.h>

#include "pimo/document.h"

namespace Nepomuk2 {
namespace PIMO {
/**
 * A blog note. You just want to write something down right now and 
 * need a place to do that. Add a blog-note! This is an example class 
 * for a document type, there are more detailled ontologies to 
 * model Blog-Posts (like SIOC). 
 */
class BlogPost : public virtual PIMO::Document
{
public:
    BlogPost(const QUrl& uri = QUrl())
      : SimpleResource(uri), PIMO::ClassOrThingOrPropertyOrAssociation(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#BlogPost", QUrl::StrictMode)), PIMO::ClassOrThing(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#BlogPost", QUrl::StrictMode)), PIMO::Thing(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#BlogPost", QUrl::StrictMode)), PIMO::LogicalMediaType(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#BlogPost", QUrl::StrictMode)), PIMO::Document(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#BlogPost", QUrl::StrictMode)) {
    }

    BlogPost(const SimpleResource& res)
      : SimpleResource(res), PIMO::ClassOrThingOrPropertyOrAssociation(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#BlogPost", QUrl::StrictMode)), PIMO::ClassOrThing(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#BlogPost", QUrl::StrictMode)), PIMO::Thing(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#BlogPost", QUrl::StrictMode)), PIMO::LogicalMediaType(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#BlogPost", QUrl::StrictMode)), PIMO::Document(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#BlogPost", QUrl::StrictMode)) {
    }

    BlogPost& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#BlogPost", QUrl::StrictMode));
        return *this;
    }

protected:
    BlogPost(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), PIMO::ClassOrThingOrPropertyOrAssociation(uri, type), PIMO::ClassOrThing(uri, type), PIMO::Thing(uri, type), PIMO::LogicalMediaType(uri, type), PIMO::Document(uri, type) {
    }
    BlogPost(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), PIMO::ClassOrThingOrPropertyOrAssociation(res, type), PIMO::ClassOrThing(res, type), PIMO::Thing(res, type), PIMO::LogicalMediaType(res, type), PIMO::Document(res, type) {
    }
};
}
}

#endif
