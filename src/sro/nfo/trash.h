#ifndef _NFO_TRASH_H_
#define _NFO_TRASH_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include "dms-copy/simpleresource.h"

#include "nfo/datacontainer.h"

namespace Nepomuk {
namespace NFO {
/**
 * Represents a container for deleted files, a feature common 
 * in modern operating systems. 
 */
class Trash : public virtual NFO::DataContainer
{
public:
    Trash(const QUrl& uri = QUrl())
      : SimpleResource(uri), NFO::DataContainer(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Trash", QUrl::StrictMode)) {
    }

    Trash(const SimpleResource& res)
      : SimpleResource(res), NFO::DataContainer(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Trash", QUrl::StrictMode)) {
    }

    Trash& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Trash", QUrl::StrictMode));
        return *this;
    }

protected:
    Trash(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NFO::DataContainer(uri, type) {
    }
    Trash(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NFO::DataContainer(res, type) {
    }
};
}
}

#endif
