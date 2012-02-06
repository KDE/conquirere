#ifndef _NCAL_RECURRENCEFREQUENCY_H_
#define _NCAL_RECURRENCEFREQUENCY_H_

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
 * Frequency of a recurrence rule. This class has been introduced 
 * to express a limited set of allowed values for the ncal:freq 
 * property. See the documentation of ncal:freq for details. 
 */
class RecurrenceFrequency : public virtual Nepomuk::SimpleResource
{
public:
    RecurrenceFrequency(const QUrl& uri = QUrl())
      : SimpleResource(uri) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#RecurrenceFrequency", QUrl::StrictMode));
    }

    RecurrenceFrequency(const SimpleResource& res)
      : SimpleResource(res) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#RecurrenceFrequency", QUrl::StrictMode));
    }

    RecurrenceFrequency& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#RecurrenceFrequency", QUrl::StrictMode));
        return *this;
    }

protected:
    RecurrenceFrequency(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri) {
        addType(type);
    }
    RecurrenceFrequency(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res) {
        addType(type);
    }
};
}
}

#endif
