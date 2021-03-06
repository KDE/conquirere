#ifndef _ANEO_AKONADIDATAOBJECT_H_
#define _ANEO_AKONADIDATAOBJECT_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include <nepomuk2/simpleresource.h>

#include "nie/dataobject.h"

namespace Nepomuk2 {
namespace ANEO {
/**
 * used to identify akonadi entities (items and collections) 
 * created by the akonadi-nepomuk feeders 
 */
class AkonadiDataObject : public virtual NIE::DataObject
{
public:
    AkonadiDataObject(const QUrl& uri = QUrl())
      : SimpleResource(uri), NIE::DataObject(uri, QUrl::fromEncoded("http://akonadi-project.org/ontologies/aneo#AkonadiDataObject", QUrl::StrictMode)) {
    }

    AkonadiDataObject(const SimpleResource& res)
      : SimpleResource(res), NIE::DataObject(res, QUrl::fromEncoded("http://akonadi-project.org/ontologies/aneo#AkonadiDataObject", QUrl::StrictMode)) {
    }

    AkonadiDataObject& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://akonadi-project.org/ontologies/aneo#AkonadiDataObject", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://akonadi-project.org/ontologies/aneo#akonadiItemId. 
     * used to identify items created by the akonadi-nepomuk feeders 
     * (depreceated usage, use the AkonadiDataObject type instead) 
     */
    QString akonadiItemId() const {
        QString value;
        if(contains(QUrl::fromEncoded("http://akonadi-project.org/ontologies/aneo#akonadiItemId", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://akonadi-project.org/ontologies/aneo#akonadiItemId", QUrl::StrictMode)).first().value<QString>();
        return value;
    }

    /**
     * Set property http://akonadi-project.org/ontologies/aneo#akonadiItemId. 
     * used to identify items created by the akonadi-nepomuk feeders 
     * (depreceated usage, use the AkonadiDataObject type instead) 
     */
    void setAkonadiItemId(const QString& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://akonadi-project.org/ontologies/aneo#akonadiItemId", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://akonadi-project.org/ontologies/aneo#akonadiItemId. 
     * used to identify items created by the akonadi-nepomuk feeders 
     * (depreceated usage, use the AkonadiDataObject type instead) 
     */
    void addAkonadiItemId(const QString& value) {
        addProperty(QUrl::fromEncoded("http://akonadi-project.org/ontologies/aneo#akonadiItemId", QUrl::StrictMode), value);
    }

    /**
     * Get property http://akonadi-project.org/ontologies/aneo#akonadiIndexCompatLevel. 
     * Saves the version of the indexer used to write this item. 
     */
    qint32 akonadiIndexCompatLevel() const {
        qint32 value;
        if(contains(QUrl::fromEncoded("http://akonadi-project.org/ontologies/aneo#akonadiIndexCompatLevel", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://akonadi-project.org/ontologies/aneo#akonadiIndexCompatLevel", QUrl::StrictMode)).first().value<qint32>();
        return value;
    }

    /**
     * Set property http://akonadi-project.org/ontologies/aneo#akonadiIndexCompatLevel. 
     * Saves the version of the indexer used to write this item. 
     */
    void setAkonadiIndexCompatLevel(const qint32& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://akonadi-project.org/ontologies/aneo#akonadiIndexCompatLevel", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://akonadi-project.org/ontologies/aneo#akonadiIndexCompatLevel. 
     * Saves the version of the indexer used to write this item. 
     */
    void addAkonadiIndexCompatLevel(const qint32& value) {
        addProperty(QUrl::fromEncoded("http://akonadi-project.org/ontologies/aneo#akonadiIndexCompatLevel", QUrl::StrictMode), value);
    }

protected:
    AkonadiDataObject(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NIE::DataObject(uri, type) {
    }
    AkonadiDataObject(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NIE::DataObject(res, type) {
    }
};
}
}

#endif
