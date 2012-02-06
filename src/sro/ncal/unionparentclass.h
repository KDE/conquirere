#ifndef _NCAL_UNIONPARENTCLASS_H_
#define _NCAL_UNIONPARENTCLASS_H_

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
 * 
 */
class UnionParentClass : public virtual Nepomuk::SimpleResource
{
public:
    UnionParentClass(const QUrl& uri = QUrl())
      : SimpleResource(uri) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#UnionParentClass", QUrl::StrictMode));
    }

    UnionParentClass(const SimpleResource& res)
      : SimpleResource(res) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#UnionParentClass", QUrl::StrictMode));
    }

    UnionParentClass& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#UnionParentClass", QUrl::StrictMode));
        return *this;
    }

protected:
    UnionParentClass(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri) {
        addType(type);
    }
    UnionParentClass(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res) {
        addType(type);
    }
};
}
}

#endif
