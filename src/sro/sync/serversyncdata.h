#ifndef _SYNC_SERVERSYNCDATA_H_
#define _SYNC_SERVERSYNCDATA_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include "dms-copy/simpleresource.h"

namespace Nepomuk {
namespace SYNC {
/**
 * Class to hold additional information about a synced publication 
 */
class ServerSyncData : public virtual Nepomuk::SimpleResource
{
public:
    ServerSyncData(const QUrl& uri = QUrl())
      : SimpleResource(uri) {
        addType(QUrl::fromEncoded("http://www.example.com/sync#ServerSyncData", QUrl::StrictMode));
    }

    ServerSyncData(const SimpleResource& res)
      : SimpleResource(res) {
        addType(QUrl::fromEncoded("http://www.example.com/sync#ServerSyncData", QUrl::StrictMode));
    }

    ServerSyncData& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.example.com/sync#ServerSyncData", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.example.com/sync#userId. The user 
     * name or user id the sync is done 
     */
    QString userId() const {
        QString value;
        if(contains(QUrl::fromEncoded("http://www.example.com/sync#userId", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/sync#userId", QUrl::StrictMode)).first().value<QString>();
        return value;
    }

    /**
     * Set property http://www.example.com/sync#userId. The user 
     * name or user id the sync is done 
     */
    void setUserId(const QString& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/sync#userId", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/sync#userId. 
     * The user name or user id the sync is done 
     */
    void addUserId(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/sync#userId", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/sync#syncDataType. 
     * Saves which SyncDataType enum value this resource represents 
     * (Note, Attachment, BibResource) 
     */
    QUrl syncDataType() const {
        QUrl value;
        if(contains(QUrl::fromEncoded("http://www.example.com/sync#syncDataType", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/sync#syncDataType", QUrl::StrictMode)).first().value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.example.com/sync#syncDataType. 
     * Saves which SyncDataType enum value this resource represents 
     * (Note, Attachment, BibResource) 
     */
    void setSyncDataType(const QUrl& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/sync#syncDataType", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/sync#syncDataType. 
     * Saves which SyncDataType enum value this resource represents 
     * (Note, Attachment, BibResource) 
     */
    void addSyncDataType(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/sync#syncDataType", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/sync#note. Links to 
     * the note that is ment to be an attachment for another sync entry. 
     * In zotero they have a unique etag/id too to identify changes 
     */
    QUrl note() const {
        QUrl value;
        if(contains(QUrl::fromEncoded("http://www.example.com/sync#note", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/sync#note", QUrl::StrictMode)).first().value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.example.com/sync#note. Links to 
     * the note that is ment to be an attachment for another sync entry. 
     * In zotero they have a unique etag/id too to identify changes 
     */
    void setNote(const QUrl& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/sync#note", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/sync#note. 
     * Links to the note that is ment to be an attachment for another 
     * sync entry. In zotero they have a unique etag/id too to identify 
     * changes 
     */
    void addNote(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/sync#note", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/sync#provider. The 
     * provider name the sync info comes from. for example zotero 
     */
    QString provider() const {
        QString value;
        if(contains(QUrl::fromEncoded("http://www.example.com/sync#provider", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/sync#provider", QUrl::StrictMode)).first().value<QString>();
        return value;
    }

    /**
     * Set property http://www.example.com/sync#provider. The 
     * provider name the sync info comes from. for example zotero 
     */
    void setProvider(const QString& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/sync#provider", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/sync#provider. 
     * The provider name the sync info comes from. for example zotero 
     */
    void addProvider(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/sync#provider", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/sync#id. The unique 
     * ID of the publication for the specific provider at the specific 
     * url with the given userid 
     */
    QString id() const {
        QString value;
        if(contains(QUrl::fromEncoded("http://www.example.com/sync#id", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/sync#id", QUrl::StrictMode)).first().value<QString>();
        return value;
    }

    /**
     * Set property http://www.example.com/sync#id. The unique 
     * ID of the publication for the specific provider at the specific 
     * url with the given userid 
     */
    void setId(const QString& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/sync#id", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/sync#id. The 
     * unique ID of the publication for the specific provider at the 
     * specific url with the given userid 
     */
    void addId(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/sync#id", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/sync#reference. Links 
     * to the reference that this sync data represents 
     */
    QUrl reference() const {
        QUrl value;
        if(contains(QUrl::fromEncoded("http://www.example.com/sync#reference", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/sync#reference", QUrl::StrictMode)).first().value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.example.com/sync#reference. Links 
     * to the reference that this sync data represents 
     */
    void setReference(const QUrl& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/sync#reference", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/sync#reference. 
     * Links to the reference that this sync data represents 
     */
    void addReference(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/sync#reference", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/sync#etag. Special 
     * hash to denote if teh item was manipulated on the server since 
     * last sync 
     */
    QString etag() const {
        QString value;
        if(contains(QUrl::fromEncoded("http://www.example.com/sync#etag", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/sync#etag", QUrl::StrictMode)).first().value<QString>();
        return value;
    }

    /**
     * Set property http://www.example.com/sync#etag. Special 
     * hash to denote if teh item was manipulated on the server since 
     * last sync 
     */
    void setEtag(const QString& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/sync#etag", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/sync#etag. 
     * Special hash to denote if teh item was manipulated on the server 
     * since last sync 
     */
    void addEtag(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/sync#etag", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/sync#url. The provider 
     * url addition. for zotero this is users or groups could be full 
     * url in other cases 
     */
    QString url() const {
        QString value;
        if(contains(QUrl::fromEncoded("http://www.example.com/sync#url", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/sync#url", QUrl::StrictMode)).first().value<QString>();
        return value;
    }

    /**
     * Set property http://www.example.com/sync#url. The provider 
     * url addition. for zotero this is users or groups could be full 
     * url in other cases 
     */
    void setUrl(const QString& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/sync#url", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/sync#url. 
     * The provider url addition. for zotero this is users or groups 
     * could be full url in other cases 
     */
    void addUrl(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/sync#url", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/sync#publication. 
     * Links to the Publication that this sync data represents 
     */
    QUrl publication() const {
        QUrl value;
        if(contains(QUrl::fromEncoded("http://www.example.com/sync#publication", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/sync#publication", QUrl::StrictMode)).first().value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.example.com/sync#publication. 
     * Links to the Publication that this sync data represents 
     */
    void setPublication(const QUrl& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/sync#publication", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/sync#publication. 
     * Links to the Publication that this sync data represents 
     */
    void addPublication(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/sync#publication", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/sync#attachment. 
     * Links to the document that is ment to be an attachment for another 
     * sync entry. In zotero they have a unique etag/id too to identify 
     * changes 
     */
    QUrl attachment() const {
        QUrl value;
        if(contains(QUrl::fromEncoded("http://www.example.com/sync#attachment", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/sync#attachment", QUrl::StrictMode)).first().value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.example.com/sync#attachment. 
     * Links to the document that is ment to be an attachment for another 
     * sync entry. In zotero they have a unique etag/id too to identify 
     * changes 
     */
    void setAttachment(const QUrl& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/sync#attachment", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/sync#attachment. 
     * Links to the document that is ment to be an attachment for another 
     * sync entry. In zotero they have a unique etag/id too to identify 
     * changes 
     */
    void addAttachment(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/sync#attachment", QUrl::StrictMode), value);
    }

protected:
    ServerSyncData(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri) {
        addType(type);
    }
    ServerSyncData(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res) {
        addType(type);
    }
};
}
}

#endif
