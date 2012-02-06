#ifndef _NFO_PRESENTATION_H_
#define _NFO_PRESENTATION_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include "dms-copy/simpleresource.h"

#include "nfo/document.h"

namespace Nepomuk {
namespace NFO {
/**
 * A Presentation made by some presentation software (Corel Presentations, 
 * OpenOffice Impress, MS Powerpoint etc.) 
 */
class Presentation : public virtual NFO::Document
{
public:
    Presentation(const QUrl& uri = QUrl())
      : SimpleResource(uri), NFO::Document(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Presentation", QUrl::StrictMode)) {
    }

    Presentation(const SimpleResource& res)
      : SimpleResource(res), NFO::Document(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Presentation", QUrl::StrictMode)) {
    }

    Presentation& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Presentation", QUrl::StrictMode));
        return *this;
    }

protected:
    Presentation(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NFO::Document(uri, type) {
    }
    Presentation(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NFO::Document(res, type) {
    }
};
}
}

#endif
