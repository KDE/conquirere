#ifndef _NCAL_BYDAYRULEPART_H_
#define _NCAL_BYDAYRULEPART_H_

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
 * Expresses the compound value of a byday part of a recurrence 
 * rule. It stores the weekday and the integer modifier. Inspired 
 * by RFC 2445 sec. 4.3.10 
 */
class BydayRulePart : public virtual Nepomuk::SimpleResource
{
public:
    BydayRulePart(const QUrl& uri = QUrl())
      : SimpleResource(uri) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#BydayRulePart", QUrl::StrictMode));
    }

    BydayRulePart(const SimpleResource& res)
      : SimpleResource(res) {
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#BydayRulePart", QUrl::StrictMode));
    }

    BydayRulePart& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#BydayRulePart", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#bydayWeekday. 
     * Connects a BydayRulePath with a weekday. 
     */
    QList<QUrl> bydayWeekdays() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#bydayWeekday", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#bydayWeekday. 
     * Connects a BydayRulePath with a weekday. 
     */
    void setBydayWeekdays(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#bydayWeekday", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#bydayWeekday. 
     * Connects a BydayRulePath with a weekday. 
     */
    void addBydayWeekday(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#bydayWeekday", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#bydayModifier. 
     * An integer modifier for the BYDAY rule part. Each BYDAY value 
     * can also be preceded by a positive (+n) or negative (-n) integer. 
     * If present, this indicates the nth occurrence of the specific 
     * day within the MONTHLY or YEARLY RRULE. For example, within 
     * a MONTHLY rule, +1MO (or simply 1MO) represents the first Monday 
     * within the month, whereas -1MO represents the last Monday of 
     * the month. If an integer modifier is not present, it means all 
     * days of this type within the specified frequency. For example, 
     * within a MONTHLY rule, MO represents all Mondays within the 
     * month. Inspired by RFC 2445 sec. 4.3.10 
     */
    qint64 bydayModifier() const {
        qint64 value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#bydayModifier", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#bydayModifier", QUrl::StrictMode)).first().value<qint64>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#bydayModifier. 
     * An integer modifier for the BYDAY rule part. Each BYDAY value 
     * can also be preceded by a positive (+n) or negative (-n) integer. 
     * If present, this indicates the nth occurrence of the specific 
     * day within the MONTHLY or YEARLY RRULE. For example, within 
     * a MONTHLY rule, +1MO (or simply 1MO) represents the first Monday 
     * within the month, whereas -1MO represents the last Monday of 
     * the month. If an integer modifier is not present, it means all 
     * days of this type within the specified frequency. For example, 
     * within a MONTHLY rule, MO represents all Mondays within the 
     * month. Inspired by RFC 2445 sec. 4.3.10 
     */
    void setBydayModifier(const qint64& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#bydayModifier", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#bydayModifier. 
     * An integer modifier for the BYDAY rule part. Each BYDAY value 
     * can also be preceded by a positive (+n) or negative (-n) integer. 
     * If present, this indicates the nth occurrence of the specific 
     * day within the MONTHLY or YEARLY RRULE. For example, within 
     * a MONTHLY rule, +1MO (or simply 1MO) represents the first Monday 
     * within the month, whereas -1MO represents the last Monday of 
     * the month. If an integer modifier is not present, it means all 
     * days of this type within the specified frequency. For example, 
     * within a MONTHLY rule, MO represents all Mondays within the 
     * month. Inspired by RFC 2445 sec. 4.3.10 
     */
    void addBydayModifier(const qint64& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#bydayModifier", QUrl::StrictMode), value);
    }

protected:
    BydayRulePart(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri) {
        addType(type);
    }
    BydayRulePart(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res) {
        addType(type);
    }
};
}
}

#endif
