#ifndef _NCAL_RECURRENCEIDENTIFIERRANGE_H_
#define _NCAL_RECURRENCEIDENTIFIERRANGE_H_

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
 * Recurrence Identifier Range. This class has been created to 
 * provide means to express the limited set of values for the ncal:range 
 * property. See documentation for ncal:range for details. 
 */
class RecurrenceIdentifierRange : public virtual Nepomuk::SimpleResource
{
public:
    RecurrenceIdentifierRange(const QUrl& uri = QUrl())
      : SimpleResource(uri) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#RecurrenceIdentifierRange", QUrl::StrictMode));
    }

    RecurrenceIdentifierRange(const SimpleResource& res)
      : SimpleResource(res) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#RecurrenceIdentifierRange", QUrl::StrictMode));
    }

    RecurrenceIdentifierRange& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#RecurrenceIdentifierRange", QUrl::StrictMode));
        return *this;
    }

protected:
    RecurrenceIdentifierRange(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri) {
        addType(type);
    }
    RecurrenceIdentifierRange(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res) {
        addType(type);
    }
};
}
}

#endif
