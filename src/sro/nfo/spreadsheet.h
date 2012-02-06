#ifndef _NFO_SPREADSHEET_H_
#define _NFO_SPREADSHEET_H_

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
 * A spreadsheet, created by a spreadsheet application. Examples 
 * might include Gnumeric, OpenOffice Calc or MS Excel. 
 */
class Spreadsheet : public virtual NFO::Document
{
public:
    Spreadsheet(const QUrl& uri = QUrl())
      : SimpleResource(uri), NFO::Document(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Spreadsheet", QUrl::StrictMode)) {
    }

    Spreadsheet(const SimpleResource& res)
      : SimpleResource(res), NFO::Document(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Spreadsheet", QUrl::StrictMode)) {
    }

    Spreadsheet& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Spreadsheet", QUrl::StrictMode));
        return *this;
    }

protected:
    Spreadsheet(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NFO::Document(uri, type) {
    }
    Spreadsheet(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NFO::Document(res, type) {
    }
};
}
}

#endif
