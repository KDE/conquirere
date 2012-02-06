#ifndef _NCO_IMSTATUSTYPE_H_
#define _NCO_IMSTATUSTYPE_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include "dms-copy/simpleresource.h"

namespace Nepomuk {
namespace NCO {
/**
 * The status type of an IMAccount. Based on the Connection_Presence_Type 
 * enumeration of the Telepathy project: http://telepathy.freedesktop.org/spec/Connection_Interface_Simple_Presence.html#Enum:Connection_Presence_Type 
 */
class IMStatusType : public virtual Nepomuk::SimpleResource
{
public:
    IMStatusType(const QUrl& uri = QUrl())
      : SimpleResource(uri) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#IMStatusType", QUrl::StrictMode));
    }

    IMStatusType(const SimpleResource& res)
      : SimpleResource(res) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#IMStatusType", QUrl::StrictMode));
    }

    IMStatusType& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#IMStatusType", QUrl::StrictMode));
        return *this;
    }

protected:
    IMStatusType(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri) {
        addType(type);
    }
    IMStatusType(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res) {
        addType(type);
    }
};
}
}

#endif
