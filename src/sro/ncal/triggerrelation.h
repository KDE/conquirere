#ifndef _NCAL_TRIGGERRELATION_H_
#define _NCAL_TRIGGERRELATION_H_

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
 * The relation between the trigger and its parent calendar component. 
 * This class has been introduced to express the limited vocabulary 
 * for the ncal:related property. See the documentation for ncal:related 
 * for more details. 
 */
class TriggerRelation : public virtual Nepomuk::SimpleResource
{
public:
    TriggerRelation(const QUrl& uri = QUrl())
      : SimpleResource(uri) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#TriggerRelation", QUrl::StrictMode));
    }

    TriggerRelation(const SimpleResource& res)
      : SimpleResource(res) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#TriggerRelation", QUrl::StrictMode));
    }

    TriggerRelation& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#TriggerRelation", QUrl::StrictMode));
        return *this;
    }

protected:
    TriggerRelation(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri) {
        addType(type);
    }
    TriggerRelation(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res) {
        addType(type);
    }
};
}
}

#endif
