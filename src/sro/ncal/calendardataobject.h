#ifndef _NCAL_CALENDARDATAOBJECT_H_
#define _NCAL_CALENDARDATAOBJECT_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include <nepomuk2/simpleresource.h>

#include "nie/dataobject.h"

namespace Nepomuk2 {
namespace NCAL {
/**
 * A DataObject found in a calendar. It is usually interpreted 
 * as one of the calendar entity types (e.g. Event, Journal, Todo 
 * etc.) 
 */
class CalendarDataObject : public virtual NIE::DataObject
{
public:
    CalendarDataObject(const QUrl& uri = QUrl())
      : SimpleResource(uri), NIE::DataObject(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#CalendarDataObject", QUrl::StrictMode)) {
    }

    CalendarDataObject(const SimpleResource& res)
      : SimpleResource(res), NIE::DataObject(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#CalendarDataObject", QUrl::StrictMode)) {
    }

    CalendarDataObject& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#CalendarDataObject", QUrl::StrictMode));
        return *this;
    }

protected:
    CalendarDataObject(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NIE::DataObject(uri, type) {
    }
    CalendarDataObject(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NIE::DataObject(res, type) {
    }
};
}
}

#endif
