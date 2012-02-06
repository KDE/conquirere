#ifndef _PIMO_ATTENDEE_H_
#define _PIMO_ATTENDEE_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include "dms-copy/simpleresource.h"

#include "pimo/personrole.h"

namespace Nepomuk {
namespace PIMO {
/**
 * The role of someone attending a social event. 
 */
class Attendee : public virtual PIMO::PersonRole
{
public:
    Attendee(const QUrl& uri = QUrl())
      : SimpleResource(uri), PIMO::ClassOrThingOrPropertyOrAssociation(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Attendee", QUrl::StrictMode)), PIMO::Association(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Attendee", QUrl::StrictMode)), PIMO::PersonRole(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Attendee", QUrl::StrictMode)) {
    }

    Attendee(const SimpleResource& res)
      : SimpleResource(res), PIMO::ClassOrThingOrPropertyOrAssociation(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Attendee", QUrl::StrictMode)), PIMO::Association(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Attendee", QUrl::StrictMode)), PIMO::PersonRole(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Attendee", QUrl::StrictMode)) {
    }

    Attendee& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#Attendee", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#attendingMeeting. 
     * the attended meeting 
     */
    QList<QUrl> attendingMeetings() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#attendingMeeting", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#attendingMeeting. 
     * the attended meeting 
     */
    void setAttendingMeetings(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#attendingMeeting", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#attendingMeeting. 
     * the attended meeting 
     */
    void addAttendingMeeting(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#attendingMeeting", QUrl::StrictMode), value);
    }

protected:
    Attendee(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), PIMO::ClassOrThingOrPropertyOrAssociation(uri, type), PIMO::Association(uri, type), PIMO::PersonRole(uri, type) {
    }
    Attendee(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), PIMO::ClassOrThingOrPropertyOrAssociation(res, type), PIMO::Association(res, type), PIMO::PersonRole(res, type) {
    }
};
}
}

#endif
