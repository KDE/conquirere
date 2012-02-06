#ifndef _NCAL_ALARMACTION_H_
#define _NCAL_ALARMACTION_H_

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
 * Action to be performed on alarm. This class has been introduced 
 * to express the limited set of values of the ncal:action property. 
 * Please refer to the documentation of ncal:action for details. 
 */
class AlarmAction : public virtual Nepomuk::SimpleResource
{
public:
    AlarmAction(const QUrl& uri = QUrl())
      : SimpleResource(uri) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#AlarmAction", QUrl::StrictMode));
    }

    AlarmAction(const SimpleResource& res)
      : SimpleResource(res) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#AlarmAction", QUrl::StrictMode));
    }

    AlarmAction& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#AlarmAction", QUrl::StrictMode));
        return *this;
    }

protected:
    AlarmAction(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri) {
        addType(type);
    }
    AlarmAction(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res) {
        addType(type);
    }
};
}
}

#endif
