#ifndef _NCAL_UNIONOFEVENTTODO_H_
#define _NCAL_UNIONOFEVENTTODO_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include <nepomuk2/simpleresource.h>

#include "ncal/unionparentclass.h"

namespace Nepomuk2 {
namespace NCAL {
/**
 * 
 */
class UnionOfEventTodo : public virtual NCAL::UnionParentClass
{
public:
    UnionOfEventTodo(const QUrl& uri = QUrl())
      : SimpleResource(uri), NCAL::UnionParentClass(uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#UnionOfEventTodo", QUrl::StrictMode)) {
    }

    UnionOfEventTodo(const SimpleResource& res)
      : SimpleResource(res), NCAL::UnionParentClass(res, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#UnionOfEventTodo", QUrl::StrictMode)) {
    }

    UnionOfEventTodo& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#UnionOfEventTodo", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#priority. 
     * The property defines the relative priority for a calendar component. 
     * Inspired by RFC 2445 sec. 4.8.1.9 
     */
    qint64 priority() const {
        qint64 value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#priority", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#priority", QUrl::StrictMode)).first().value<qint64>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#priority. 
     * The property defines the relative priority for a calendar component. 
     * Inspired by RFC 2445 sec. 4.8.1.9 
     */
    void setPriority(const qint64& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#priority", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#priority. 
     * The property defines the relative priority for a calendar component. 
     * Inspired by RFC 2445 sec. 4.8.1.9 
     */
    void addPriority(const qint64& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#priority", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#resources. 
     * Defines the equipment or resources anticipated for an activity 
     * specified by a calendar entity. Inspired by RFC 2445 sec. 4.8.1.10 
     * with the following reservations: the LANGUAGE parameter has 
     * been discarded. Please use xml:lang literals to express language. 
     * For the ALTREP parameter use the resourcesAltRep property. 
     * This property specifies multiple resources. The order is not 
     * important. it is recommended to introduce a separate triple 
     * for each resource. 
     */
    QStringList resourceses() const {
        QStringList value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#resources", QUrl::StrictMode)))
            value << v.value<QString>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#resources. 
     * Defines the equipment or resources anticipated for an activity 
     * specified by a calendar entity. Inspired by RFC 2445 sec. 4.8.1.10 
     * with the following reservations: the LANGUAGE parameter has 
     * been discarded. Please use xml:lang literals to express language. 
     * For the ALTREP parameter use the resourcesAltRep property. 
     * This property specifies multiple resources. The order is not 
     * important. it is recommended to introduce a separate triple 
     * for each resource. 
     */
    void setResourceses(const QStringList& value) {
        QVariantList values;
        foreach(const QString& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#resources", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#resources. 
     * Defines the equipment or resources anticipated for an activity 
     * specified by a calendar entity. Inspired by RFC 2445 sec. 4.8.1.10 
     * with the following reservations: the LANGUAGE parameter has 
     * been discarded. Please use xml:lang literals to express language. 
     * For the ALTREP parameter use the resourcesAltRep property. 
     * This property specifies multiple resources. The order is not 
     * important. it is recommended to introduce a separate triple 
     * for each resource. 
     */
    void addResources(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#resources", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#resourcesAltRep. 
     * Alternate representation of the resources needed for an event 
     * or todo. Introduced to cover the ALTREP parameter of the resources 
     * property. See documentation for ncal:resources for details. 
     */
    QList<QUrl> resourcesAltReps() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#resourcesAltRep", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#resourcesAltRep. 
     * Alternate representation of the resources needed for an event 
     * or todo. Introduced to cover the ALTREP parameter of the resources 
     * property. See documentation for ncal:resources for details. 
     */
    void setResourcesAltReps(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#resourcesAltRep", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#resourcesAltRep. 
     * Alternate representation of the resources needed for an event 
     * or todo. Introduced to cover the ALTREP parameter of the resources 
     * property. See documentation for ncal:resources for details. 
     */
    void addResourcesAltRep(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#resourcesAltRep", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#hasAlarm. 
     * Links an event or a todo with a DataObject that can be interpreted 
     * as an alarm. This property has no direct equivalent in the RFC 
     * 2445. It has been provided to express this relation. 
     */
    QList<QUrl> alarms() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#hasAlarm", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#hasAlarm. 
     * Links an event or a todo with a DataObject that can be interpreted 
     * as an alarm. This property has no direct equivalent in the RFC 
     * 2445. It has been provided to express this relation. 
     */
    void setAlarms(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#hasAlarm", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#hasAlarm. 
     * Links an event or a todo with a DataObject that can be interpreted 
     * as an alarm. This property has no direct equivalent in the RFC 
     * 2445. It has been provided to express this relation. 
     */
    void addAlarm(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#hasAlarm", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#location. 
     * Defines the intended venue for the activity defined by a calendar 
     * component. Inspired by RFC 2445 sec 4.8.1.7 with the following 
     * reservations: the LANGUAGE parameter has been discarded. 
     * Please use xml:lang literals to express language. For the ALTREP 
     * parameter use the locationAltRep property. 
     */
    QString location() const {
        QString value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#location", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#location", QUrl::StrictMode)).first().value<QString>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#location. 
     * Defines the intended venue for the activity defined by a calendar 
     * component. Inspired by RFC 2445 sec 4.8.1.7 with the following 
     * reservations: the LANGUAGE parameter has been discarded. 
     * Please use xml:lang literals to express language. For the ALTREP 
     * parameter use the locationAltRep property. 
     */
    void setLocation(const QString& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#location", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#location. 
     * Defines the intended venue for the activity defined by a calendar 
     * component. Inspired by RFC 2445 sec 4.8.1.7 with the following 
     * reservations: the LANGUAGE parameter has been discarded. 
     * Please use xml:lang literals to express language. For the ALTREP 
     * parameter use the locationAltRep property. 
     */
    void addLocation(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#location", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#locationAltRep. 
     * Alternate representation of the event or todo location. Introduced 
     * to cover the ALTREP parameter of the LOCATION property. See 
     * documentation of ncal:location for details. 
     */
    QList<QUrl> locationAltReps() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#locationAltRep", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#locationAltRep. 
     * Alternate representation of the event or todo location. Introduced 
     * to cover the ALTREP parameter of the LOCATION property. See 
     * documentation of ncal:location for details. 
     */
    void setLocationAltReps(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#locationAltRep", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#locationAltRep. 
     * Alternate representation of the event or todo location. Introduced 
     * to cover the ALTREP parameter of the LOCATION property. See 
     * documentation of ncal:location for details. 
     */
    void addLocationAltRep(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#locationAltRep", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#geo. 
     * This property specifies information related to the global 
     * position for the activity specified by a calendar component. 
     * Inspired by RFC 2445 sec. 4.8.1.6 
     */
    QUrl geo() const {
        QUrl value;
        if(contains(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#geo", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#geo", QUrl::StrictMode)).first().value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#geo. 
     * This property specifies information related to the global 
     * position for the activity specified by a calendar component. 
     * Inspired by RFC 2445 sec. 4.8.1.6 
     */
    void setGeo(const QUrl& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#geo", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#geo. 
     * This property specifies information related to the global 
     * position for the activity specified by a calendar component. 
     * Inspired by RFC 2445 sec. 4.8.1.6 
     */
    void addGeo(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#geo", QUrl::StrictMode), value);
    }

protected:
    UnionOfEventTodo(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NCAL::UnionParentClass(uri, type) {
    }
    UnionOfEventTodo(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NCAL::UnionParentClass(res, type) {
    }
};
}
}

#endif
