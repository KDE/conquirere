#ifndef _NCAL_JOURNALSTATUS_H_
#define _NCAL_JOURNALSTATUS_H_

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
 * A status of a journal entry. This class has been introduced to 
 * express the limited set of values for the ncal:status property. 
 * The user may use the instances provided with this ontology or 
 * create his/her own. See the documentation for ncal:journalStatus 
 * for details. 
 */
class JournalStatus : public virtual Nepomuk::SimpleResource
{
public:
    JournalStatus(const QUrl& uri = QUrl())
      : SimpleResource(uri) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#JournalStatus", QUrl::StrictMode));
    }

    JournalStatus(const SimpleResource& res)
      : SimpleResource(res) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#JournalStatus", QUrl::StrictMode));
    }

    JournalStatus& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#JournalStatus", QUrl::StrictMode));
        return *this;
    }

protected:
    JournalStatus(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri) {
        addType(type);
    }
    JournalStatus(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res) {
        addType(type);
    }
};
}
}

#endif
