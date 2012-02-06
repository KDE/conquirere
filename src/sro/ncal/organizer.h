#ifndef _NCAL_ORGANIZER_H_
#define _NCAL_ORGANIZER_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include "dms-copy/simpleresource.h"

#include "ncal/attendeeororganizer.h"

namespace Nepomuk {
namespace NCAL {
/**
 * An organizer of an event. This class has been introduced to serve 
 * as a range of ncal:organizer property. See documentation of 
 * ncal:organizer for details. 
 */
class Organizer : public virtual NCAL::AttendeeOrOrganizer
{
public:
    Organizer(const QUrl& uri = QUrl())
      : SimpleResource(uri), NCAL::AttendeeOrOrganizer(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Organizer", QUrl::StrictMode)) {
    }

    Organizer(const SimpleResource& res)
      : SimpleResource(res), NCAL::AttendeeOrOrganizer(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Organizer", QUrl::StrictMode)) {
    }

    Organizer& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Organizer", QUrl::StrictMode));
        return *this;
    }

protected:
    Organizer(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NCAL::AttendeeOrOrganizer(uri, type) {
    }
    Organizer(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NCAL::AttendeeOrOrganizer(res, type) {
    }
};
}
}

#endif
