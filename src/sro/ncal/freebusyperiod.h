#ifndef _NCAL_FREEBUSYPERIOD_H_
#define _NCAL_FREEBUSYPERIOD_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include <nepomuk2/simpleresource.h>

#include "ncal/ncalperiod.h"

namespace Nepomuk2 {
namespace NCAL {
/**
 * An aggregate of a period and a freebusy type. This class has been 
 * introduced to serve as a range of the ncal:freebusy property. 
 * See documentation for ncal:freebusy for details. Note that 
 * the specification of freebusy property states that the period 
 * is to be expressed using UTC time, so the timezone properties 
 * should NOT be used for instances of this class. 
 */
class FreebusyPeriod : public virtual NCAL::NcalPeriod
{
public:
    FreebusyPeriod(const QUrl& uri = QUrl())
      : SimpleResource(uri), NCAL::NcalTimeEntity(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#FreebusyPeriod", QUrl::StrictMode)), NCAL::NcalPeriod(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#FreebusyPeriod", QUrl::StrictMode)) {
    }

    FreebusyPeriod(const SimpleResource& res)
      : SimpleResource(res), NCAL::NcalTimeEntity(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#FreebusyPeriod", QUrl::StrictMode)), NCAL::NcalPeriod(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#FreebusyPeriod", QUrl::StrictMode)) {
    }

    FreebusyPeriod& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#FreebusyPeriod", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#fbtype. 
     * To specify the free or busy time type. Inspired by RFC 2445 sec. 
     * 4.2.9. The RFC specified a limited vocabulary for the values 
     * of this property. The terms of this vocabulary have been expressed 
     * as instances of the FreebusyType class. The user can use instances 
     * provided with this ontology or create his own. 
     */
    QUrl fbtype() const {
        QUrl value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#fbtype", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#fbtype", QUrl::StrictMode)).first().value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#fbtype. 
     * To specify the free or busy time type. Inspired by RFC 2445 sec. 
     * 4.2.9. The RFC specified a limited vocabulary for the values 
     * of this property. The terms of this vocabulary have been expressed 
     * as instances of the FreebusyType class. The user can use instances 
     * provided with this ontology or create his own. 
     */
    void setFbtype(const QUrl& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#fbtype", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#fbtype. 
     * To specify the free or busy time type. Inspired by RFC 2445 sec. 
     * 4.2.9. The RFC specified a limited vocabulary for the values 
     * of this property. The terms of this vocabulary have been expressed 
     * as instances of the FreebusyType class. The user can use instances 
     * provided with this ontology or create his own. 
     */
    void addFbtype(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#fbtype", QUrl::StrictMode), value);
    }

protected:
    FreebusyPeriod(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NCAL::NcalTimeEntity(uri, type), NCAL::NcalPeriod(uri, type) {
    }
    FreebusyPeriod(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NCAL::NcalTimeEntity(res, type), NCAL::NcalPeriod(res, type) {
    }
};
}
}

#endif
