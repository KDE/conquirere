#ifndef _NCAL_ACCESSCLASSIFICATION_H_
#define _NCAL_ACCESSCLASSIFICATION_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include "dms-copy/simpleresource.h"

namespace Nepomuk {
namespace NCAL {
/**
 * Access classification of a calendar component. Introduced 
 * to express the set of values for the ncal:class property. The 
 * user may use instances provided with this ontology or create 
 * his/her own with desired semantics. See the documentation 
 * of ncal:class for details. 
 */
class AccessClassification : public virtual Nepomuk::SimpleResource
{
public:
    AccessClassification(const QUrl& uri = QUrl())
      : SimpleResource(uri) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#AccessClassification", QUrl::StrictMode));
    }

    AccessClassification(const SimpleResource& res)
      : SimpleResource(res) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#AccessClassification", QUrl::StrictMode));
    }

    AccessClassification& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#AccessClassification", QUrl::StrictMode));
        return *this;
    }

protected:
    AccessClassification(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri) {
        addType(type);
    }
    AccessClassification(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res) {
        addType(type);
    }
};
}
}

#endif
