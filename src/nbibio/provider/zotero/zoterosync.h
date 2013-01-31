/*
 * Copyright 2012 Jörg Ehrichs <joerg.ehrichs@gmx.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ZOTEROSYNC_H
#define ZOTEROSYNC_H

#include "nbibio/provider/onlinestorage.h"

#include <KDE/KUrl>

#include <QtCore/QObject>
#include <QtCore/QVariantMap>
#include <QtCore/QVariantList>
#include <QtCore/QStringList>
#include <QtCore/QXmlStreamReader>

#include <QtNetwork/QNetworkAccessManager>

class QNetworkReply;

class ZoteroSync : public OnlineStorage
{
    Q_OBJECT
public:
    explicit ZoteroSync(QObject *parent = 0);

    QString providerId() const;
    QString providerName() const;
    KIcon providerIcon() const;

    bool supportCollections() const;
    QString helpText() const;
    QString defaultUrl() const;
    bool useUrlSelector() const;

    QVariantList data() const;
    QList<CollectionInfo> collectionInfo() const;

signals:
    void status(const QString &curentStatus);
    void progress(int currentProgress);
    void finished();

    void error(const QString &errorMessage);
    void itemNeedMerge(const QVariantMap & item);

public slots:
    void cancel();

    //-------------------------------------------------
    // read request
    //-------------------------------------------------
    void fetchItem(const QString &itemId );
    void fetchItems(const QString &collectionId = QString());
    void fetchCollections(const QString &parent = QString() );
    void fetchCollection(const QString &collectionId );
    KUrl downloadFile(const QString &file, const QString &filename);

    //-------------------------------------------------
    // write request
    //-------------------------------------------------
    void pushItems(const QVariantList &items, const QString &collection = QString());
    void pushFile(const QVariantMap &fileData);
    void addItemsToCollection(const QStringList &ids, const QString &collection);
    void removeItemsFromCollection(const QStringList&ids, const QString &collection);
    void deleteItems(const QVariantList &items);

    void createCollection(const CollectionInfo &ci);
    void editCollection(const CollectionInfo &ci);
    void deleteCollection(const CollectionInfo &ci);

private slots:
    void itemRequestFinished();
    void itemsRequestFinished();
    void collectionRequestFinished();
    void collectionsRequestFinished();
    void itemCollectionFinished();
    void itemCollectionRemoveFinished();

    void itemPushFinished();
    void itemDeleteFinished();

    void fileItemPushFinished();
    void fileAuthorizationFinished();
    void fileUploadFinished();
    void fileRegisterFinished();

private:
    void resetState();
    void fetchItems(int limit, int start);
    void fetchCollections(int limit, int start);

    CollectionInfo readCollectionEntry(QXmlStreamReader &xmlReader);
    QVariantMap readItemEntry(QXmlStreamReader &xmlReader);
    void readJsonContent(const QString &json, QVariantMap &entry);

    QByteArray writeJsonContent(const CollectionInfo &collection);
    QByteArray writeJsonContent(const QVariantList &items, bool updateItem = false);
    void pushItemCache();
    void deleteItemCache();
    void removeItemFromCollectionCache();

    QVariantMap transformToJsonMap(const QString &entryType, const QVariantMap &item);
    QVariantList transformCreators(const QString &zoteroType,const QString &personList);
    bool networkError(QNetworkReply *reply);

private:
    bool m_cancel;

    QVariantMap m_fromZoteroMapping;
    QMap<QString, QStringList> m_defaultKeys;
    QVariantMap m_toZoteroMapping;

    QNetworkAccessManager m_qnam;
    QNetworkReply *m_reply;

    QString m_currentCollection;
    QStringList m_idsForCollectionAdd;
    QStringList m_idsForCollectionRemove;
    QVariantList m_cacheNewItems;
    QVariantList m_cacheNewChildItems;
    QVariantList m_cacheUpdateItems;
    QList<QPair<QString,QString> > m_cacheDeleteItem;
    QVariantMap m_cacheItemEditUpload;
    QVariantMap m_cacheFileUpload;
    QString m_curUploadKey;

    QVariantList m_returnedData;
    QList<CollectionInfo> m_returnedCollectionInfo;
};

#endif // ZOTEROSYNC_H
