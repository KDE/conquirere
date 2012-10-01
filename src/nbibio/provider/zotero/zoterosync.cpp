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

#include "zoterosync.h"

#include <KDE/KConfig>
#include <KDE/KConfigGroup>

#include <qjson/serializer.h>
#include <qjson/parser.h>

#include <QtNetwork/QNetworkReply>
#include <QtCore/QXmlStreamReader>
#include <QtCore/QFileInfo>
#include <QtCore/QCryptographicHash>
#include <QSslConfiguration>

#include <KDE/KUrl>
#include <KDE/KMimeType>
#include <KDE/KIO/CopyJob>
#include <KDE/KLocalizedString>
#include <KDE/KDebug>

const QString BASE_URL      = QLatin1String("https://api.zotero.org/");
const int MAX_ITEM_REQUEST  = 50;
const int MAX_ITEMS_TO_PUSH = 50;

ZoteroSync::ZoteroSync(QObject *parent)
    : OnlineStorage(parent)
    , m_cancel(false)
    , m_reply(0)
{
    qRegisterMetaType<CollectionInfo>("CollectionInfo");
    qRegisterMetaType<QList<CollectionInfo> >("QList<CollectionInfo>");

    //FIXME: relative path for provider.ini
    KConfig keyMapping("/home/joerg/Development/KDE/conquirere/src/nbibio/provider/zotero/zoteromapping.ini", KConfig::SimpleConfig);

    QStringList groups = keyMapping.groupList();

    foreach(const QString &groupName, groups) {
        KConfigGroup zoteroGroup( &keyMapping, groupName );

        QVariantMap entryMap;
        QVariantMap keyMapping;
        QStringList defaultKeyList;

        QStringList keyList = zoteroGroup.keyList();
        foreach(const QString &key, keyList) {
            QString localkey  = zoteroGroup.readEntry(key).trimmed();
            entryMap.insert(key.trimmed(), localkey);

            if(key == QLatin1String("creators")) {
                QStringList creatorlist = localkey.split(QLatin1String(";"));
                foreach(const QString &creator, creatorlist) {
                    QStringList presonMapping = creator.split(QLatin1String(":"));
                    defaultKeyList << presonMapping.last();
                    keyMapping.insert(presonMapping.last(), presonMapping.first());
                }
            }
            else {
                defaultKeyList << localkey;
                keyMapping.insert(localkey, key.trimmed());
            }
        }
        m_fromZoteroMapping.insert(groupName, entryMap);
        m_defaultKeys.insert(groupName, defaultKeyList);
        m_toZoteroMapping.insert(groupName, keyMapping);
    }
}

QString ZoteroSync::providerId() const
{
    return QLatin1String("zotero");
}

QString ZoteroSync::providerName() const
{
    return QLatin1String("Zotero");
}

KIcon ZoteroSync::providerIcon() const
{
    return KIcon(QLatin1String("storage-zotero"));
}

bool ZoteroSync::supportCollections() const
{
    return true;
}

QString ZoteroSync::helpText() const
{
    return i18n("Provider to sync your data with http://www.Zotero.org\n\n"
                "Name : your userID number\n"
                "Password : your created API key\n"
                "url : either 'users' or 'group'\n"
                "collection : the collection to sync with\n");
}

QString ZoteroSync::defaultUrl() const
{
    return QLatin1String("users");
}

bool ZoteroSync::useUrlSelector() const
{
    return false;
}

QVariantList ZoteroSync::data() const
{
    return m_returnedData;
}

QList<CollectionInfo> ZoteroSync::collectionInfo() const
{
    return m_returnedCollectionInfo;
}

void ZoteroSync::cancel()
{
    m_reply->abort();
    resetState();
    emit finished();
}

void ZoteroSync::resetState()
{
    m_currentCollection.clear();
    m_idsForCollectionAdd.clear();
    m_idsForCollectionRemove.clear();
    m_cacheNewItems.clear();
    m_cacheNewChildItems.clear();
    m_cacheUpdateItems.clear();
    m_cacheDeleteItem.clear();
    m_cacheFileUpload.clear();
    m_curUploadKey.clear();
    m_returnedData.clear();
    m_returnedCollectionInfo.clear();
}

//---------------------------------------------------------------
//
// All request functions are below
//
//---------------------------------------------------------------
void ZoteroSync::fetchItem(const QString &itemId )
{
    resetState();

    QString apiCommand = BASE_URL + providerSettings().url + QLatin1String("/") + providerSettings().userName +
                         QLatin1String("/items/") + itemId + QLatin1String("?format=atom&content=json");

    if(!providerSettings().pwd.isEmpty()) {
        apiCommand.append( QLatin1String("&key=") + providerSettings().pwd);
    }

    if( !providerSettings().importAttachments ) {
        apiCommand.append( QLatin1String("&itemType=-attachment") ); // ignore file download
    }

    m_reply = m_qnam.get( QNetworkRequest(QUrl( apiCommand )) );
    connect(m_reply, SIGNAL(finished()),this, SLOT(itemRequestFinished()) );
}

void ZoteroSync::fetchItems(const QString &collectionId)
{
    resetState();
    m_currentCollection = collectionId;

    fetchItems(MAX_ITEM_REQUEST,0);
}

void ZoteroSync::fetchItems(int limit, int start)
{
    QString apiCommand;
    if(m_currentCollection.isEmpty()) {
        apiCommand = BASE_URL + providerSettings().url + QLatin1String("/") + providerSettings().userName + QLatin1String("/items?format=atom&content=json");
    }
    else {
        apiCommand = BASE_URL + providerSettings().url + QLatin1String("/") + providerSettings().userName + QLatin1String("/collections/") + m_currentCollection + QLatin1String("/items?format=atom&content=json");
    }

    apiCommand.append( QLatin1String("&limit=") + QString::number(limit) + QLatin1String("&start=") + QString::number(start) );

    if(!providerSettings().pwd.isEmpty()) {
        apiCommand.append( QLatin1String("&key=") + providerSettings().pwd);
    }

    if( !providerSettings().importAttachments ) {
        apiCommand.append( QLatin1String("&itemType=-attachment") ); // ignore file download
    }

    m_reply = m_qnam.get( QNetworkRequest(QUrl( apiCommand )) );
    connect(m_reply, SIGNAL(finished()),this, SLOT(itemsRequestFinished()) );
}

void ZoteroSync::fetchCollection(const QString &collectionId )
{
    QString apiCommand = BASE_URL + providerSettings().url + QLatin1String("/") + providerSettings().userName + QLatin1String("/collections/") + collectionId + QLatin1String("?format=atom");
    if(!providerSettings().pwd.isEmpty()) {
        apiCommand.append( QLatin1String("&key=") + providerSettings().pwd);
    }

    m_reply = m_qnam.get( QNetworkRequest(QUrl( apiCommand )) );
    connect(m_reply, SIGNAL(finished()),this, SLOT(collectionRequestFinished()) );
}

void ZoteroSync::fetchCollections(const QString &parent )
{
    resetState();
    m_currentCollection = parent;

    fetchCollections(MAX_ITEM_REQUEST,0);
}

void ZoteroSync::fetchCollections(int limit, int start)
{
    QString subCollection;
    if(!m_currentCollection.isEmpty()) {
        subCollection = QLatin1String("/") + m_currentCollection + QLatin1String("/collections");
    }

    QString apiCommand = BASE_URL + providerSettings().url + QLatin1String("/") + providerSettings().userName + QLatin1String("/collections") + subCollection + QLatin1String("?format=atom");
    if(!providerSettings().pwd.isEmpty()) {
        apiCommand.append( QLatin1String("&key=") + providerSettings().pwd);
    }

    apiCommand.append( QLatin1String("&limit=") + QString::number(limit) + QLatin1String("&start=") + QString::number(start) );

    m_reply = m_qnam.get( QNetworkRequest(QUrl( apiCommand )) );
    connect(m_reply, SIGNAL(finished()),this, SLOT(collectionsRequestFinished()) );
}

KUrl ZoteroSync::downloadFile(const QString &file, const QString &filename)
{
    KUrl destination( providerSettings().localStoragePath + filename );
    if(!destination.isValid()) {
        kDebug() << "local folder storage is not valid :: " << destination;
        return KUrl();
    }

    QString fileUrl = file + QLatin1String("?key=") + providerSettings().pwd;
    KUrl source( fileUrl );

    //download the file attachment into the user specified folder
    KIO::CopyJob *downloadJob = KIO::copy(source, destination);

    //TODO: emit download status in percent 0-100% so it can be shown whats going on
    // maybe also speed? see what KIO offers
    // QLatin1String("sync-attachment-filesize") full size of the file

    // blocking download, we need to have the file on the disk before we can proceed any further
    if( !downloadJob->exec() ) {
        kDebug() << downloadJob->errorText();
        kDebug() << downloadJob->errorString();

        return KUrl();
    }

    kDebug() << "downloaded the file into ::" << downloadJob->destUrl();

    return downloadJob->destUrl();
}

void ZoteroSync::pushItems(const QVariantList &items, const QString &collection)
{
    resetState();
    m_currentCollection = collection;

    // first spilt new items from existing ones that need an update
    foreach(const QVariant &item, items) {
        QVariantMap entryMap = item.toMap();

        if(entryMap.contains(QLatin1String("sync-key"))) {
            m_cacheUpdateItems.append(entryMap);
        }
        else if(entryMap.contains(QLatin1String("sync-parent"))) {
            m_cacheNewChildItems.append(entryMap);
        }
        else {
            m_cacheNewItems.append(entryMap);
        }
    }

    kDebug() << "#############################################################################################################################################";
    kDebug() << "push items :: New:" << m_cacheNewItems.size() << " New Children::" << m_cacheNewChildItems.size() << " Update: " << m_cacheUpdateItems.size();

    pushItemCache();
}

void ZoteroSync::pushItemCache()
{
    kDebug() << "push next item cache New :: " << m_cacheNewItems.size() << " NewChild :: " << m_cacheNewChildItems.size() << "edit : " << m_cacheUpdateItems.size();

    // push new items if cache is not empty
    if( !m_cacheNewItems.isEmpty() ) {
        QVariantList tmpList;
        if(m_cacheNewItems.size() >= MAX_ITEMS_TO_PUSH) {
            for(int i=0;i<=MAX_ITEMS_TO_PUSH;i++) { tmpList.append(m_cacheNewItems.takeFirst()); }
        }
        else {
            tmpList.append(m_cacheNewItems);
            m_cacheNewItems.clear();
        }

        //POST /users/1/items
        //Content-Type: application/json
        //Optional
        //X-Zotero-Write-Token: 19a4f01ad623aa7214f82347e3711f56
        QString pushString = BASE_URL + providerSettings().url + QLatin1String("/") + providerSettings().userName + QLatin1String("/items");

        if(!providerSettings().pwd.isEmpty()) {
            pushString.append(QLatin1String("?key=") + providerSettings().pwd);
        }

        QUrl pushUrl(pushString);

        kDebug() << pushString;

        QNetworkRequest request(pushUrl);
        request.setHeader(QNetworkRequest::ContentTypeHeader,QLatin1String("application/json"));

        m_reply = m_qnam.post(request, writeJsonContent(tmpList));
        connect(m_reply, SIGNAL(finished()),this, SLOT(itemPushFinished()));
    }
    else if( !m_cacheNewChildItems.isEmpty() ) {
        // take a single child and push it to zotero
        QVariantMap nextChild = m_cacheNewChildItems.takeFirst().toMap();

        //POST /users/1/items
        //Content-Type: application/json
        //Optional
        //X-Zotero-Write-Token: 19a4f01ad623aa7214f82347e3711f56
        QString pushString = BASE_URL + providerSettings().url + QLatin1String("/") + providerSettings().userName + QLatin1String("/items/") +
                             nextChild.value(QLatin1String("sync-parent")).toString() + QLatin1String("/children");

        if(!providerSettings().pwd.isEmpty()) {
            pushString.append(QLatin1String("?key=") + providerSettings().pwd);
        }

        QUrl pushUrl(pushString);

        QNetworkRequest request(pushUrl);
        request.setHeader(QNetworkRequest::ContentTypeHeader,QLatin1String("application/json"));

        m_reply = m_qnam.post(request, writeJsonContent(QVariantList() << nextChild));
        connect(m_reply, SIGNAL(finished()),this, SLOT(itemPushFinished()));
    }
    else if( !m_cacheUpdateItems.isEmpty() ) {
        //PUT /users/1/items/ABCD2345
        //If-Match: "8e984e9b2a8fb560b0085b40f6c2c2b7"
        m_cacheItemEditUpload = m_cacheUpdateItems.takeFirst().toMap();

        QString zoteroKey = m_cacheItemEditUpload.value(QLatin1String("sync-key")).toString();
        QString etag = m_cacheItemEditUpload.value(QLatin1String("sync-etag")).toString();

        QString pushString = BASE_URL + providerSettings().url + QLatin1String("/") + providerSettings().userName + QLatin1String("/items/") + zoteroKey;

        if(!providerSettings().pwd.isEmpty()) {
            pushString.append(QLatin1String("?key=") + providerSettings().pwd);
        }

        QUrl pushUrl(pushString);

        QNetworkRequest request(pushUrl);
        request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
        request.setRawHeader("If-Match", etag.toAscii());

        m_reply = m_qnam.put(request, writeJsonContent(QVariantList() << m_cacheItemEditUpload, true));
        connect(m_reply, SIGNAL(finished()),this, SLOT(itemPushFinished()));
    }
    else if( m_idsForCollectionAdd.isEmpty() ) {
        addItemsToCollection(m_idsForCollectionAdd, m_currentCollection);
    }
    else {
        emit finished();
    }
}

void ZoteroSync::pushFile(const QVariantMap &fileData)
{
    resetState();

    m_cacheFileUpload = fileData;

    // update existing file
    if( fileData.contains(QLatin1String("sync-key")) ) {

        QString zoteroKey = fileData.value(QLatin1String("sync-key")).toString();
        QString etag = fileData.value(QLatin1String("sync-etag")).toString();

        QString pushString = BASE_URL + providerSettings().url + QLatin1String("/") + providerSettings().userName + QLatin1String("/items/") + zoteroKey;

        if(!providerSettings().pwd.isEmpty()) {
            pushString.append(QLatin1String("?key=") + providerSettings().pwd);
        }

        QUrl pushUrl(pushString);

        kDebug() << pushString;

        QNetworkRequest request(pushUrl);
        request.setHeader(QNetworkRequest::ContentTypeHeader,QLatin1String("application/json"));
        request.setRawHeader("If-Match", etag.toAscii());

        m_reply = m_qnam.post(request, writeJsonContent(QVariantList() << fileData));
        connect(m_reply, SIGNAL(finished()),this, SLOT(fileItemPushFinished()));
    }
    // push new file
    else {
        //POST /users/<userID>/items/<parentItemKey>/children
        //Content-Type: application/json
        //X-Zotero-Write-Token: <token>

        QString pushString = BASE_URL + providerSettings().url + QLatin1String("/") + providerSettings().userName + QLatin1String("/items");

        if( fileData.contains(QLatin1String("sync-parent")) ) {
            pushString.append(QLatin1String("/"));
            pushString.append(fileData.value(QLatin1String("sync-parent")).toString());
            pushString.append(QLatin1String("/children"));
        }

        if(!providerSettings().pwd.isEmpty()) {
            pushString.append(QLatin1String("?key=") + providerSettings().pwd);
        }

        QUrl pushUrl(pushString);

        QNetworkRequest request(pushUrl);
        request.setHeader(QNetworkRequest::ContentTypeHeader,QLatin1String("application/json"));

        m_reply = m_qnam.post(request, writeJsonContent(QVariantList() << fileData));
        connect(m_reply, SIGNAL(finished()),this, SLOT(fileItemPushFinished()));
    }
}

void ZoteroSync::deleteItemCache()
{
    if( !m_cacheDeleteItem.isEmpty() ) {
        //DELETE /users/1/items/ABCD2345
        //If-Match: "8e984e9b2a8fb560b0085b40f6c2c2b7"

        QPair<QString,QString> currentItem = m_cacheDeleteItem.takeFirst();

        QString zoteroKey = currentItem.first;
        QString etag =currentItem.second;

        QString pushString = BASE_URL + providerSettings().url + QLatin1String("/") + providerSettings().userName + QLatin1String("/items/") + zoteroKey;

        if(!providerSettings().pwd.isEmpty()) {
            pushString.append(QLatin1String("?key=") + providerSettings().pwd);
        }
        QUrl pushUrl(pushString);

        QNetworkRequest request(pushUrl);
        request.setHeader(QNetworkRequest::ContentTypeHeader,QLatin1String("application/json"));
        request.setRawHeader("If-Match", etag.toLatin1());

        m_reply = m_qnam.deleteResource(request);
        connect(m_reply, SIGNAL(finished()),this, SLOT(itemDeleteFinished()));
    }
    else {
        emit finished();
    }
}

void ZoteroSync::addItemsToCollection(const QStringList &ids, const QString &collection)
{
    resetState();
    m_idsForCollectionAdd.empty();

    //POST /users/1/collections/QRST9876/items
    //ABCD2345 BCDE3456 CDEF4567 DEFG5678

    QString pushString = BASE_URL + providerSettings().url + QLatin1String("/") + providerSettings().userName +
                         QLatin1String("/collections/") + collection + QLatin1String("/items");

    if(!providerSettings().pwd.isEmpty()) {
        pushString.append(QLatin1String("?key=") + providerSettings().pwd);
    }

    QUrl pushUrl(pushString);
    QNetworkRequest request(pushUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader,QLatin1String("application/octet-stream"));

    m_reply = m_qnam.post(request, ids.join(QLatin1String(" ")).toAscii());
    connect(m_reply, SIGNAL(finished()),this, SLOT(itemCollectionFinished()));
}

void ZoteroSync::removeItemsFromCollection(const QStringList &ids, const QString &collection)
{
    resetState();

    m_idsForCollectionRemove = ids;
    m_currentCollection = collection;

    removeItemFromCollectionCache();
}

void ZoteroSync::removeItemFromCollectionCache()
{
    if( !m_idsForCollectionRemove.isEmpty() ) {
        //DELETE /users/1/collections/QRST9876/items/ABCD2345
        QString id = m_idsForCollectionRemove.takeFirst();
        QString pushString = BASE_URL + providerSettings().url + QLatin1String("/") + providerSettings().userName +
                             QLatin1String("/collections/") + m_currentCollection + QLatin1String("/items/") + id;

        if(!providerSettings().pwd.isEmpty()) {
            pushString.append(QLatin1String("?key=") + providerSettings().pwd);
        }

        QUrl pushUrl(pushString);
        QNetworkRequest request(pushUrl);
        request.setHeader(QNetworkRequest::ContentTypeHeader,QLatin1String("application/json"));

        m_reply = m_qnam.deleteResource(request);
        connect(m_reply, SIGNAL(finished()),this, SLOT(itemCollectionRemoveFinished()));
    }
    else {
        emit finished();
    }
}

void ZoteroSync::deleteItems(const QVariantList &items)
{
    resetState();

    foreach(const QVariant &item, items) {
        QVariantMap entry = item.toMap();

        QPair<QString,QString> tobeDeleted;
        tobeDeleted.first = entry.value(QLatin1String("sync-key")).toString();
        tobeDeleted.second = entry.value(QLatin1String("sync-etag")).toString();

        m_cacheDeleteItem.append(tobeDeleted);
    }

    deleteItemCache();
}

void ZoteroSync::createCollection(const CollectionInfo &ci)
{
    resetState();

    //POST /users/1/collections
    //X-Zotero-Write-Token: 19a4f01ad623aa7214f82347e3711f56

    QString pushString = BASE_URL + providerSettings().url + QLatin1String("/") + providerSettings().userName + QLatin1String("/collections");

    if(!providerSettings().pwd.isEmpty()) {
        pushString.append(QLatin1String("?key=") + providerSettings().pwd);
    }
    QUrl pushUrl(pushString);

    QNetworkRequest request(pushUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader,QLatin1String("application/json"));

    m_reply = m_qnam.post(request, writeJsonContent(ci));
    connect(m_reply, SIGNAL(finished()),this, SLOT(collectionRequestFinished()));
}

void ZoteroSync::editCollection(const CollectionInfo &ci)
{
    resetState();

    //PUT /users/1/collections/RSTU8765
    //If-Match: "f0ebb2240a57f4115b3ce841d5218fa2"

    QString pushString = BASE_URL + providerSettings().url + QLatin1String("/") + providerSettings().userName + QLatin1String("/collections/") + ci.id;

    if(!providerSettings().pwd.isEmpty()) {
        pushString.append(QLatin1String("?key=") + providerSettings().pwd);
    }

    QUrl pushUrl(pushString);

    QNetworkRequest request(pushUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader,QLatin1String("application/json"));
    request.setRawHeader("If-Match", ci.eTag.toLatin1());

    m_reply = m_qnam.put(request, writeJsonContent(ci));
    connect(m_reply, SIGNAL(finished()),this, SLOT(collectionRequestFinished()));
}

void ZoteroSync::deleteCollection(const CollectionInfo &ci)
{
    resetState();

    //DELETE /users/1/collections/RSTU8765
    //If-Match: "f0ebb2240a57f4115b3ce841d5218fa2"

    QString pushString = BASE_URL + providerSettings().url + QLatin1String("/") + providerSettings().userName + QLatin1String("/collections/") + ci.id;

    if(!providerSettings().pwd.isEmpty()) {
        pushString.append(QLatin1String("?key=") + providerSettings().pwd);
    }
    QUrl pushUrl(pushString);

    QNetworkRequest request(pushUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader,QLatin1String("application/json"));
    request.setRawHeader("If-Match", ci.eTag.toLatin1());

    m_reply = m_qnam.deleteResource( request );
    connect(m_reply, SIGNAL(finished()),this, SLOT(collectionRequestFinished()));
}

//---------------------------------------------------------------
//
// All helper function for the above requests
//
//---------------------------------------------------------------
QByteArray ZoteroSync::writeJsonContent(const CollectionInfo &collection)
{
    QVariantMap jsonMap;

    jsonMap.insert( QLatin1String("name"), collection.name );
    jsonMap.insert( QLatin1String("parent"), collection.parentId );

    QJson::Serializer serializer;
    QByteArray json = serializer.serialize(jsonMap);

    return json;
}


QByteArray ZoteroSync::writeJsonContent(const QVariantList &items, bool updateItem)
{
    QVariantList jsonList;

    foreach(const QVariant &item, items) {
        QVariantMap entry = item.toMap();

        QString entryType;
        if(entry.contains(QLatin1String("articletype"))) {
            entryType = entry.value(QLatin1String("articletype")).toString();

            entryType = entryType.replace(QLatin1String("journal"), QLatin1String("journalArticle"));
            entryType = entryType.replace(QLatin1String("magazine"), QLatin1String("magazineArticle"));
            entryType = entryType.replace(QLatin1String("newspaper"), QLatin1String("newspaperArticle"));
            entryType = entryType.replace(QLatin1String("encyclopedia"), QLatin1String("encyclopediaArticle"));
        }
        else {
            entryType = entry.value(QLatin1String("bibtexentrytype")).toString();
        }

        if(entryType.isEmpty()) {
            kWarning() << "could not find publication type for the item";
            kWarning() << item;
            continue;
        }

        jsonList.append( transformToJsonMap(entryType, entry) );
    }

    QJson::Serializer serializer;
    QByteArray json;

    if(updateItem) {
        json = serializer.serialize(jsonList.first());
    }
    else {
        QVariantMap jsonMap;
        jsonMap.insert(QLatin1String("items"), jsonList);

        json = serializer.serialize(jsonMap);
    }

    kDebug() << json;

    return json;
}

QVariantMap ZoteroSync::transformToJsonMap(const QString &entryType, const QVariantMap &item)
{
    QVariantMap jsonMap;

    // first get the right mappingentry
    QVariantMap mapping = m_fromZoteroMapping.value(entryType).toMap();

    // now foreach entry in the mapping find the right entry in the item map
    QMapIterator<QString, QVariant> i(mapping);
    while (i.hasNext()) {
        i.next();

        if(i.key() == QLatin1String("itemType")) {
            jsonMap.insert(i.key(), entryType);
        }
        else if(i.key() == QLatin1String("tags")) {
            // split keyswords from tag1;tag2;tag3 etc int oa list
            QString itemKey = i.value().toString();
            QString itemValue = item.value(itemKey, QString()).toString();
            QStringList tags = itemValue.split(QLatin1String(";"));

            QVariantList tagList;
            foreach(const QString &tag, tags) {
                if(tag.isEmpty())
                    continue;

                QVariantMap tagMap;
                tagMap.insert(QLatin1String("tag"), tag);
                tagList.append(tagMap);
            }
            jsonMap.insert(i.key(), tagList);
        }
        else if(i.key() == QLatin1String("creators")) {
            // we read in a list like this
            //artist:author;contributor:contributor
            QStringList creators = i.value().toString().split(QLatin1String(";"));

            QVariantList creatorsList;
            foreach(const QString &creator,creators) {
                // if we get artist:author fetch all author entries from the publication item and add them as artist person
                QStringList keys = creator.split(QLatin1String(":"));

                QString personName = item.value(keys.last(), QString()).toString();
                if(personName.isEmpty())
                    continue;

                creatorsList.append( transformCreators(keys.first(),personName) );
            }
            jsonMap.insert(i.key(), creatorsList);

        }
        else if(i.key() == QLatin1String("notes") ) {
            QVariantList noteList;

//            QVariantMap note;
//            note.insert(QLatin1String("itemType"), QLatin1String("note"));
//            note.insert(QLatin1String("note"), QLatin1String("text of the note"));
//            noteList.append(note);

            //jsonMap.insert(QLatin1String("notes"), noteList);
            //TODO: handle note updates without child in zotero
            //kWarning() << "notes are ignored at the moment";
        }
        // otherwise insert the direct entry
        else {
            QString itemKey = i.value().toString();
            QString itemValue = item.value(itemKey, QString()).toString();
            jsonMap.insert(i.key(), itemValue);

        }
    }

    return jsonMap;
}

QVariantList ZoteroSync::transformCreators(const QString &zoteroType,const QString &personList)
{
    QVariantList list;

    QStringList persons = personList.split(QLatin1String(";"));

    foreach(const QString &person, persons) {
        QVariantMap personMap;
        personMap.insert(QLatin1String("creatorType"), zoteroType);

        QStringList personNames = person.split(QLatin1String(" "));

        if(personNames.size() < 2) {
            personMap.insert(QLatin1String("firstName"), QString(" "));
            personMap.insert(QLatin1String("lastName"), person);
        }
        if(personNames.size() == 2) {
            personMap.insert(QLatin1String("firstName"), personNames.first());
            personMap.insert(QLatin1String("lastName"), personNames.last());
        }
        if(personNames.size() > 2) {
            personMap.insert(QLatin1String("firstName"), personNames.takeFirst());
            personMap.insert(QLatin1String("lastName"), personNames.join(QLatin1String(" ")));
        }

        list.append(personMap);
    }

    return list;
}

//---------------------------------------------------------------
//
// Below here we deal with the server response evaluation
//
//---------------------------------------------------------------
void ZoteroSync::itemRequestFinished() // response from a single item request
{
    if( networkError(m_reply) ) {
        return;
    }

    // read the data retrieved from the server
    QXmlStreamReader xmlReader;
    xmlReader.setDevice(m_reply);

    while(!xmlReader.atEnd()) {
        if(!xmlReader.readNextStartElement()) { continue; }

        if(xmlReader.name() == QLatin1String("entry")) {
            m_returnedData.append( readItemEntry(xmlReader) );
        }
    }

    disconnect(m_reply);
    m_reply->deleteLater();
    emit finished();
}

void ZoteroSync::itemsRequestFinished() // response from several item requests
{
    if( networkError(m_reply) ) {
        return;
    }

    int nextFetchLimit = MAX_ITEM_REQUEST;
    int nextFetchStart = -1; // if it stays -1 we know we caught all follow up request and can stop now

    QXmlStreamReader xmlReader;
    xmlReader.setDevice( m_reply );

    while(!xmlReader.atEnd()) {
        if(!xmlReader.readNextStartElement()) {
            continue;
        }

        // parse link suggestions for the case that we have to fetch more items
        if(xmlReader.name() == QLatin1String("link")) {
            QXmlStreamAttributes linkAttributes = xmlReader.attributes();

            // if there does exist more items we need to fetch the right values for the next start
            if(QLatin1String("next") == linkAttributes.value(QLatin1String("rel")) ) {
                QString href = linkAttributes.value(QLatin1String("href")).toString();

                // fetch the next start suggesstion
                QRegExp rxStart(QLatin1String("start=(\\d+)"));
                int pos = rxStart.indexIn(href);
                if (pos > -1) {
                    nextFetchStart = rxStart.cap(1).toInt();
                }

                // returns a suggested limit value. Always the last used or nothing if no limit was specified
                // as zotero does not support more than 50 items in one go, we default to this
                QRegExp rxLimit(QLatin1String("limit=(\\d+)"));
                pos = rxLimit.indexIn(href);
                if (pos > -1) {
                    nextFetchLimit = rxLimit.cap(1).toInt();
                }
                else {
                    nextFetchLimit = MAX_ITEM_REQUEST;
                }
            }
        }

        // parse the entry content (all item information we actually wanted)
        else if(xmlReader.name() == QLatin1String("entry")) {
            m_returnedData.append( readItemEntry(xmlReader) );
        }
    }

    if(nextFetchStart != -1) {
        disconnect(m_reply);
        m_reply->deleteLater();
        fetchItems(nextFetchLimit, nextFetchStart);
    }
    else {
        disconnect(m_reply);
        m_reply->deleteLater();
        emit finished();
    }
}

QVariantMap ZoteroSync::readItemEntry(QXmlStreamReader &xmlReader)
{
    QVariantMap entry;

    bool finishEntry = false;
    while(!finishEntry) {
        bool startelement = xmlReader.readNextStartElement();

        if(xmlReader.name() == QLatin1String("link")) {
            QXmlStreamAttributes linkAttributes = xmlReader.attributes();

            // only parse the link entry that gives us the parent item
            if(QLatin1String("up") == linkAttributes.value(QLatin1String("rel")) ) {
                QString href = linkAttributes.value(QLatin1String("href")).toString();

                // fetch the next start suggesstion
                QRegExp rxStart(QLatin1String("items/(\\w+)"));
                int pos = rxStart.indexIn(href);
                if (pos > -1) {
                    QString itemParent = rxStart.cap(1);
                    entry.insert( QLatin1String("sync-parent"), itemParent);
                }
                else {
                    kWarning() << "could not parse parent item for the note/attachment!";
                }
            }
            else if(QLatin1String("enclosure") == linkAttributes.value(QLatin1String("rel")) ) {
                QString itemFile = linkAttributes.value(QLatin1String("href")).toString();
                entry.insert( QLatin1String("sync-attachment-file"), itemFile);

                QString itemFileSize = linkAttributes.value(QLatin1String("length")).toString();
                entry.insert( QLatin1String("sync-attachment-filesize"), itemFileSize);
            }
        }
        else if(startelement && xmlReader.name() == QLatin1String("itemType")) {
            QString itemType = xmlReader.readElementText();
            entry.insert( QLatin1String("sync-type"), itemType);
        }
        else if(startelement && xmlReader.name() == QLatin1String("title")) {
            QString zoteroTitle = xmlReader.readElementText();
            entry.insert( QLatin1String("sync-title"), zoteroTitle);
        }
        else if(startelement && xmlReader.name() == QLatin1String("key")) {
            QString key = xmlReader.readElementText();
            entry.insert( QLatin1String("sync-key"), key);
            m_idsForCollectionAdd << key;
        }
        else if(startelement && xmlReader.name() == QLatin1String("numChildren")) {
            QString zoteroChildren = xmlReader.readElementText();
            entry.insert( QLatin1String("sync-children"), zoteroChildren);
        }
        else if(startelement && xmlReader.name() == QLatin1String("updated")) {
            QString zoteroUpdated = xmlReader.readElementText();
            entry.insert( QLatin1String("sync-updated"), zoteroUpdated);
        }
        else if(xmlReader.name() == "content") {
            QString etag = xmlReader.attributes().value(QLatin1String("zapi:etag")).toString();
            entry.insert( QLatin1String("sync-etag"), etag);

            readJsonContent(xmlReader.readElementText(), entry);
        }
        else if(xmlReader.name() == QLatin1String("entry")) {
            finishEntry = true;
        }
    }

    return entry;
}

void ZoteroSync::readJsonContent(const QString &json, QVariantMap &entry)
{
    QJson::Parser parser;
    bool ok;

    QVariantMap result = parser.parse(json.toLatin1(), &ok).toMap();
    if (!ok) {
        kWarning() << "An error occurred during json parsing";
        return;
    }

    QString citeKey = entry.value( QLatin1String("sync-key") ).toString();
    entry.insert(QLatin1String("bibtexcitekey"), citeKey);

    QString zoteroType;
    QVariantMap keyTranslation = m_fromZoteroMapping.value( result.value( QLatin1String("itemType") ).toString()).toMap();
    QStringList creators = keyTranslation.value(QLatin1String("creators")).toString().split(QLatin1String(";"));
    QVariantMap creatorTranslation;

    foreach(const QString &creator, creators) {
        QStringList ct = creator.split(QLatin1String(":"));
        creatorTranslation.insert(ct.first(), ct.last());
    }

    QMapIterator<QString, QVariant> i(result);
    while (i.hasNext()) {
        i.next();

        //##########################################################################
        //# special tag handling
        if(i.key() == QLatin1String("tags")) {
            QString tagList;
            foreach (const QVariant &tag, i.value().toList()) {
                QVariantMap tagMap = tag.toMap();
                QString tag = tagMap.value(QLatin1String("tag")).toString();
                tagList.append( tag );
                tagList.append( QLatin1String(";") );
            }

            tagList.chop(1);
            entry.insert(QLatin1String("keywords"), tagList);
        }

        //##########################################################################
        //# change the entry type to something bibtex offers if possible

        else if(i.key() == QLatin1String("itemType")) {
            zoteroType = i.value().toString().toLower();

            if(zoteroType == QLatin1String("booksection")) {
                // as inbook/incollection is part of a book with its own name
                entry.insert(QLatin1String("bibtexentrytype"), QLatin1String("incollection"));
            }
            else if(zoteroType == QLatin1String("conferencepaper")) {
                entry.insert(QLatin1String("bibtexentrytype"), QLatin1String("inproceedings"));
            }
            else if(zoteroType == QLatin1String("document")) {
                entry.insert(QLatin1String("bibtexentrytype"), QLatin1String("misc"));
            }
            else if(zoteroType == QLatin1String("manuscript")) {
                entry.insert(QLatin1String("bibtexentrytype"), QLatin1String("script"));
            }
            else if(zoteroType == QLatin1String("journalarticle")) {
                entry.insert(QLatin1String("bibtexentrytype"), QLatin1String("article"));
                entry.insert(QLatin1String("articletype"), QLatin1String("journal"));
            }
            else if(zoteroType == QLatin1String("magazinearticle")) {
                entry.insert(QLatin1String("bibtexentrytype"), QLatin1String("article"));
                entry.insert(QLatin1String("articletype"), QLatin1String("magazine"));
            }
            else if(zoteroType == QLatin1String("newspaperarticle")) {
                entry.insert(QLatin1String("bibtexentrytype"), QLatin1String("article"));
                entry.insert(QLatin1String("articletype"), QLatin1String("newspaper"));
            }
            else if(zoteroType == QLatin1String("encyclopediaarticle")) {
                entry.insert(QLatin1String("bibtexentrytype"), QLatin1String("article"));
                entry.insert(QLatin1String("articletype"), QLatin1String("encyclopedia"));
            }
            else if(zoteroType == QLatin1String("blogpost")) {
                entry.insert(QLatin1String("bibtexentrytype"), QLatin1String("article"));
                entry.insert(QLatin1String("articletype"), QLatin1String("blog"));
            }
            else if(zoteroType == QLatin1String("forumpost")) {
                entry.insert(QLatin1String("bibtexentrytype"), QLatin1String("article"));
                entry.insert(QLatin1String("articletype"), QLatin1String("forum"));
            }
            else if(zoteroType == QLatin1String("webpage")) {
                entry.insert(QLatin1String("bibtexentrytype"), QLatin1String("article"));
                entry.insert(QLatin1String("articletype"), QLatin1String("webpage"));
            }
            else {
                entry.insert(QLatin1String("bibtexentrytype"), zoteroType);
            }
        }

        //##########################################################################
        //# parse creators, change some fields to author/editor if possible
        //# all other fields are left alone and inserted with their normal name (like creator type translator)

        else if(i.key() == QLatin1String("creators")) {
            foreach (const QVariant &author, i.value().toList()) {
                QVariantMap authorMap = author.toMap();

                QString person = authorMap.value(QLatin1String("firstName")).toString() + QLatin1String(" ") + authorMap.value(QLatin1String("lastName")).toString();

                // get the Valuelist for the current creator type
                // either the translated one if availabe or the zotero one, if none exist an empty new Value
                QString creatorType = authorMap.value(QLatin1String("creatorType")).toString();

                QString translatedCreatorType = creatorTranslation.value(creatorType.toLower(), creatorType.toLower()).toString();

                QString oldPersonList = entry.value( translatedCreatorType ).toString();

                if( !oldPersonList.isEmpty() ) {
                    oldPersonList.append(QLatin1String(";"));
                }
                oldPersonList.append(person);

                entry.insert(translatedCreatorType.trimmed(), oldPersonList);
            }
        }
        else {
            QString text = i.value().toString();

            // here either the transformed key name from the lookup table is used
            // or if nothing is found the key from zotero is used
            QString key = keyTranslation.value(i.key(), i.key()).toString();
            entry.insert(key, text);
        }
    }

    // now fill the entry with empty values for any key that was not downloaded but is supported by the server
    // these entries must be removed from a local storage then if we merge it with an existing entry
    QStringList defaultkeys = m_defaultKeys.value( zoteroType );

    foreach(const QString &key, defaultkeys) {
        if(!entry.contains(key.toLower())) {
            entry.insert(key.toLower().trimmed(), QLatin1String(""));
        }
    }
}

void ZoteroSync::collectionRequestFinished()
{
    if( networkError(m_reply) ) {
        return;
    }

    // we parse the response
    QXmlStreamReader xmlReader;
    xmlReader.setDevice(m_reply);

    while(!xmlReader.atEnd()) {
        if(!xmlReader.readNextStartElement()) { continue; }

        if(xmlReader.name() == QLatin1String("entry")) {
            m_returnedCollectionInfo.append( readCollectionEntry( xmlReader ) );

            break;
        }
    }

    disconnect(m_reply);
    m_reply->deleteLater();
    emit finished();
}

void ZoteroSync::collectionsRequestFinished()
{
    if( networkError(m_reply) ) {
        return;
    }

    int nextFetchLimit = MAX_ITEM_REQUEST;
    int nextFetchStart = -1; // if it stays -1 we know we caught all follow up request and can stop now

    QXmlStreamReader xmlReader;
    xmlReader.setDevice( m_reply );

    while(!xmlReader.atEnd()) {
        if(!xmlReader.readNextStartElement()) {
            continue;
        }

        // parse link suggestions for the case that we have to fetch more items
        if(xmlReader.name() == QLatin1String("link")) {
            QXmlStreamAttributes linkAttributes = xmlReader.attributes();

            // if there do exist more items we need to fetch get the right values for the next start
            if(QLatin1String("next") == linkAttributes.value(QLatin1String("rel")) ) {
                QString href = linkAttributes.value(QLatin1String("href")).toString();

                // fetch the next start suggesstion
                QRegExp rxStart(QLatin1String("start=(\\d+)"));
                int pos = rxStart.indexIn(href);
                if (pos > -1) {
                    nextFetchStart = rxStart.cap(1).toInt();
                }

                // returns a suggested limit value. Always the last used or nothing if no limit was specified
                // as zotero does not support more than 50 items in one go, we default to this
                QRegExp rxLimit(QLatin1String("limit=(\\d+)"));
                pos = rxLimit.indexIn(href);
                if (pos > -1) {
                    nextFetchLimit = rxLimit.cap(1).toInt();
                }
                else {
                    nextFetchLimit = MAX_ITEM_REQUEST;
                }
            }
        }

        // parse the entry content (all item information we actually wanted)
        else if(xmlReader.name() == QLatin1String("entry")) {
            m_returnedCollectionInfo.append( readCollectionEntry(xmlReader) );
        }
    }

    if(nextFetchStart != -1) {
        disconnect(m_reply);
        m_reply->deleteLater();
        fetchCollections(nextFetchLimit, nextFetchStart);
    }
    else {
        disconnect(m_reply);
        m_reply->deleteLater();
        emit finished();
    }
}

CollectionInfo ZoteroSync::readCollectionEntry(QXmlStreamReader &xmlReader)
{
    CollectionInfo ci;
    bool finishEntry = false;
    while(!finishEntry) {
        bool startelement = xmlReader.readNextStartElement();

        if(startelement && xmlReader.name() == QLatin1String("title")) {
            ci.name = xmlReader.readElementText();
        }
        else if( startelement && xmlReader.name() == QLatin1String("key") ) {
            ci.id = xmlReader.readElementText();
        }
        else if(startelement && xmlReader.name() == QLatin1String("numCollections")) {
            ci.subCollections = xmlReader.readElementText().toInt();
        }
        else if(startelement && xmlReader.name() == QLatin1String("numItems")) {
            ci.items = xmlReader.readElementText().toInt();
        }
        else if(startelement && xmlReader.name() == QLatin1String("content")) {
            ci.eTag = xmlReader.attributes().value(QLatin1String("zapi:etag")).toString();
        }
        else if(!startelement && xmlReader.name() == QLatin1String("entry")) {
            finishEntry = true;
        }
    }

    return ci;
}

void ZoteroSync::itemPushFinished()
{
    if(m_reply->error()) {
        if(QString(m_reply->readAll()) == QLatin1String("ETag does not match current version of item")) {
            kDebug() << "Etag error, item changed on the server, needs manual merging";
            emit itemNeedMerge(m_cacheItemEditUpload);
        }
        else {
            kDebug() << m_reply->readAll();
            kDebug() << m_reply->error() << m_reply->errorString();

            QString errorString = QString("%1\n%2").arg(m_reply->error()).arg(m_reply->errorString());
            emit error(errorString);
        }
        disconnect(m_reply);
        m_reply->deleteLater();
        pushItemCache();
        return;
    }

    // we parse the response
    QXmlStreamReader xmlReader;
    xmlReader.setDevice(m_reply);

    while(!xmlReader.atEnd()) {
        if(!xmlReader.readNextStartElement()) { continue; }

        if(xmlReader.name() == QLatin1String("entry")) {
            m_returnedData.append( readItemEntry(xmlReader) );
        }
    }

    if( !m_currentCollection.isEmpty() ) {
        foreach(const QVariant &v, m_returnedData) {
            m_idsForCollectionAdd << v.toMap().value(QLatin1String("sync-key")).toString();
        }
    }

    disconnect(m_reply);
    m_reply->deleteLater();
    pushItemCache();
}

void ZoteroSync::itemDeleteFinished()
{
    if( networkError(m_reply) ) {
        return;
    }

    disconnect(m_reply);
    m_reply->deleteLater();

    deleteItemCache();
}

void ZoteroSync::itemCollectionFinished()
{
    if( networkError(m_reply) ) {
        return;
    }

    disconnect(m_reply);
    m_reply->deleteLater();

    emit finished();
}

void ZoteroSync::itemCollectionRemoveFinished()
{
    if( networkError(m_reply) ) {
        return;
    }

    disconnect(m_reply);
    m_reply->deleteLater();

    removeItemFromCollectionCache();
}

void ZoteroSync::fileItemPushFinished()
{
    if( networkError(m_reply) ) {
        return;
    }

    // we parse the response
    QXmlStreamReader xmlReader;
    xmlReader.setDevice(m_reply);

    while(!xmlReader.atEnd()) {
        if(!xmlReader.readNextStartElement()) { continue; }

        if(xmlReader.name() == QLatin1String("entry")) {
            m_returnedData.append( readItemEntry(xmlReader) );

            break;
        }
    }

    disconnect(m_reply);
    m_reply->deleteLater();

    if( m_returnedData.isEmpty()) {
        kDebug() << "file creation failed could not read response entry";
        //emit error(i18n("file creation failed could not read response entry"));
        emit finished();
        return;
    }

    QString syncKey = m_returnedData.first().toMap().value(QLatin1String("sync-key")).toString();

    // ok we got a response from the server with the created item (child or normal)
    // to upload the actual file, we need to get upload authorization

    // @see https://www.zotero.org/support/dev/server_api/file_upload
    //POST /users/<userID>/items/<itemKey>/file
    //Content-Type: application/x-www-form-urlencoded
    //If-None-Match: *
    //md5=<hash>&filename=<filename>&filesize=<bytes>&mtime=<milliseconds>[&contentType=<type>&charset=<charset>]
    QString pushString = BASE_URL + providerSettings().url + QLatin1String("/") + providerSettings().userName + QLatin1String("/items/") +
                         syncKey + QLatin1String("/file?params=0");

    if(!providerSettings().pwd.isEmpty()) {
        pushString.append(QLatin1String("&key=") + providerSettings().pwd);
    }

    // add necessary file info data
    QFileInfo file( m_cacheFileUpload.value(QLatin1String("url")).toString() );
    QFile file2( m_cacheFileUpload.value(QLatin1String("url")).toString() );
    if (!file2.open(QIODevice::ReadOnly)) {
        kError() << "could not open file for upload";
        emit error(i18n("Error opening the file for the upload"));
        return;
    }

    QCryptographicHash md5(QCryptographicHash::Md5);
    md5.addData( file2.readAll() );
    KMimeType::Ptr mimeTypePtr = KMimeType::findByUrl( m_cacheFileUpload.value(QLatin1String("url")).toString() );

    QString fileFormData = QString("md5=%1&filename=%2&filesize=%3&mtime=%4&contentType=%5&charset=None")
                           .arg( QString(md5.result().toHex()) )
                           .arg( QString(QUrl::toPercentEncoding(file.fileName())) )
                           .arg( file.size() )
                           .arg( file.lastModified().toMSecsSinceEpoch())
                           .arg( QString(QUrl::toPercentEncoding(mimeTypePtr->name())) );

    QUrl pushUrl(pushString);

    QNetworkRequest request(pushUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader,QLatin1String("application/x-www-form-urlencoded"));

    if(m_cacheFileUpload.contains(QLatin1String("sync-etag"))) {
        // seems we already added a file, so we want to update the current
        //QString etag = m_cacheFileUpload.value(QLatin1String("sync-etag"));
        //request.setRawHeader("If-Match", etag.toAscii());
    }
    else {
        request.setRawHeader("If-None-Match", "*");
    }

    m_reply = m_qnam.post(request, fileFormData.toAscii() );
    connect(m_reply, SIGNAL(finished()),this, SLOT(fileAuthorizationFinished()));
}

void ZoteroSync::fileAuthorizationFinished()
{
    if( networkError(m_reply) ) {
        return;
    }

    QJson::Parser parser;
    bool ok;

    QVariantMap result = parser.parse(m_reply->readAll(), &ok).toMap();
    if (!ok) {
        kWarning() << "An error occurred during json parsing";
        emit error(i18n("An error occurred during json parsing for the file authorization request"));
        return;
    }

    disconnect(m_reply);
    m_reply->deleteLater();

    if( result.contains(QLatin1String("exists"))) {
        kDebug() << "file already exist";
        emit finished();
        return;
    }

    m_curUploadKey = result.value(QLatin1String("uploadKey")).toString();

    //the multipart data
    QByteArray data;
    data.append(result.value(QLatin1String("prefix")).toByteArray());

    QFile file( m_cacheFileUpload.value(QLatin1String("url")).toString() );
    if (!file.open(QIODevice::ReadOnly)) {
        kError() << "could not open file for upload";
        emit error(i18n("Error opening the file for the upload"));
        return;
    }
    data.append(file.readAll());

    data.append(result.value(QLatin1String("suffix")).toByteArray());

    QNetworkRequest request(result.value(QLatin1String("url")).toUrl());
    request.setHeader(QNetworkRequest::ContentTypeHeader,result.value(QLatin1String("contentType")).toString());
    request.setRawHeader(QString("Content-Length").toAscii(), QString::number(data.length()).toAscii());

//    QSslConfiguration config; //after this, above error pops up
//    config.setProtocol(QSsl::SecureProtocols);
//    request.setSslConfiguration(config);

    m_reply = m_qnam.post(request, data);
    connect(m_reply, SIGNAL(finished()),this, SLOT(fileUploadFinished()));
}

void ZoteroSync::fileUploadFinished()
{
    if( networkError(m_reply) ) {
        return;
    }

    QString syncKey = m_returnedData.first().toMap().value(QLatin1String("sync-key")).toString();

    // great, file upload worked, now register the file

    //POST /users/<userID>/items/<itemKey>/file
    //Content-Type: application/x-www-form-urlencoded
    //If-None-Match: *
    QString pushString = BASE_URL + providerSettings().url + QLatin1String("/") + providerSettings().userName + QLatin1String("/items/") +
                         syncKey + QLatin1String("/file");

    if(!providerSettings().pwd.isEmpty()) {
        pushString.append(QLatin1String("?key=") + providerSettings().pwd);
    }
    QUrl pushUrl(pushString);

    QString fileFormData = QString("upload=%1").arg( m_curUploadKey );

    QNetworkRequest request(pushUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader,QLatin1String("application/x-www-form-urlencoded"));
    request.setRawHeader("If-None-Match", "*");

    m_reply = m_qnam.post(request, fileFormData.toAscii());
    connect(m_reply, SIGNAL(finished()),this, SLOT(fileRegisterFinished()));
}

void ZoteroSync::fileRegisterFinished()
{
    if( networkError(m_reply) ) {
        return;
    }

    disconnect(m_reply);
    m_reply->deleteLater();

    emit finished();
}

bool ZoteroSync::networkError(QNetworkReply *reply)
{
    if(reply->error()) {
        kDebug() << reply->readAll();
        kDebug() << reply->error() << reply->errorString();

        QString errorString = QString("%1\n%2").arg(reply->error()).arg(reply->errorString());
        emit error(errorString);
        reply->deleteLater();

        return true;
    }
    else {
        return false;
    }
}
