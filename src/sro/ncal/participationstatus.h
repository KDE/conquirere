#ifndef _NCAL_PARTICIPATIONSTATUS_H_
#define _NCAL_PARTICIPATIONSTATUS_H_

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
 * Participation Status. This class has been introduced to express 
 * the limited vocabulary of values for the ncal:partstat property. 
 * See the documentation of ncal:partstat for details. 
 */
class ParticipationStatus : public virtual Nepomuk::SimpleResource
{
public:
    ParticipationStatus(const QUrl& uri = QUrl())
      : SimpleResource(uri) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#ParticipationStatus", QUrl::StrictMode));
    }

    ParticipationStatus(const SimpleResource& res)
      : SimpleResource(res) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#ParticipationStatus", QUrl::StrictMode));
    }

    ParticipationStatus& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#ParticipationStatus", QUrl::StrictMode));
        return *this;
    }

protected:
    ParticipationStatus(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri) {
        addType(type);
    }
    ParticipationStatus(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res) {
        addType(type);
    }
};
}
}

#endif
