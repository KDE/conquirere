#ifndef _NCAL_CALENDARSCALE_H_
#define _NCAL_CALENDARSCALE_H_

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
 * A calendar scale. This class has been introduced to provide 
 * the limited vocabulary for the ncal:calscale property. 
 */
class CalendarScale : public virtual Nepomuk::SimpleResource
{
public:
    CalendarScale(const QUrl& uri = QUrl())
      : SimpleResource(uri) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#CalendarScale", QUrl::StrictMode));
    }

    CalendarScale(const SimpleResource& res)
      : SimpleResource(res) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#CalendarScale", QUrl::StrictMode));
    }

    CalendarScale& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#CalendarScale", QUrl::StrictMode));
        return *this;
    }

protected:
    CalendarScale(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri) {
        addType(type);
    }
    CalendarScale(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res) {
        addType(type);
    }
};
}
}

#endif
