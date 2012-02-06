#ifndef _NCAL_ATTACHMENTENCODING_H_
#define _NCAL_ATTACHMENTENCODING_H_

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
 * Attachment encoding. This class has been introduced to express 
 * the limited vocabulary of values for the ncal:encoding property. 
 * See the documentation of ncal:encoding for details. 
 */
class AttachmentEncoding : public virtual Nepomuk::SimpleResource
{
public:
    AttachmentEncoding(const QUrl& uri = QUrl())
      : SimpleResource(uri) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#AttachmentEncoding", QUrl::StrictMode));
    }

    AttachmentEncoding(const SimpleResource& res)
      : SimpleResource(res) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#AttachmentEncoding", QUrl::StrictMode));
    }

    AttachmentEncoding& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#AttachmentEncoding", QUrl::StrictMode));
        return *this;
    }

protected:
    AttachmentEncoding(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri) {
        addType(type);
    }
    AttachmentEncoding(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res) {
        addType(type);
    }
};
}
}

#endif
