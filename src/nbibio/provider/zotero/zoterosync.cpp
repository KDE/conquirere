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

#include <KDE/KDebug>

const QString BASE_URL      = QLatin1String("https://api.zotero.org/");
const int MAX_ITEM_REQUEST  = 50;
const int MAX_ITEMS_TO_PUSH = 50;

ZoteroSync::ZoteroSync(QObject *parent)
    : QObject(parent)
    , m_cancel(false)
{
    qRegisterMetaType<CollectionInfo>("CollectionInfo");
    qRegisterMetaType<QList<CollectionInfo> >("QList<CollectionInfo>");

    KConfig keyMapping("/home/joerg/Development/KDE/publicationdata-sync/lib/provider/zotero/zoteromapping.ini", KConfig::SimpleConfig);

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

void ZoteroSync::setProviderSettings(const ProviderSyncDetails &psd)
{
    m_psd = psd;
}

ProviderSyncDetails ZoteroSync::providerSettings() const
{
    return m_psd;
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
    m_returnedData.clear();
    m_returnedCollectionInfo.clear();
    m_currentCollection.clear();
    m_cacheDeleteItem.clear();
    m_idsForCollectionAdd.clear();
    m_idsForCollectionRemove.clear();
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

    apiCommand.append( QLatin1String("&itemType=-attachment") ); // ignore file download

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

    apiCommand.append( QLatin1String("&itemType=-attachment") ); // ignore file download

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
        else {
            m_cacheNewItems.append(entryMap);
        }
    }

    pushItemCache();
}

void ZoteroSync::pushItemCache()
{
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
            pushString.append(QLatin1String("?&key=") + providerSettings().pwd);
        }

        QUrl pushUrl(pushString);

        QNetworkRequest request(pushUrl);
        request.setHeader(QNetworkRequest::ContentTypeHeader,QLatin1String("application/json"));

        m_reply = m_qnam.post(request, writeJsonContent(tmpList));
        connect(m_reply, SIGNAL(finished()),this, SLOT(itemPushFinished()));
    }
    else if( !m_cacheUpdateItems.isEmpty() ) {
        //PUT /users/1/items/ABCD2345
        //If-Match: "8e984e9b2a8fb560b0085b40f6c2c2b7"
        QVariantMap currentItem = m_cacheUpdateItems.takeFirst().toMap();

        QString zoteroKey = currentItem.value(QLatin1String("sync-key")).toString();
        QString etag =currentItem.value(QLatin1String("sync-etag")).toString();

        QString pushString = BASE_URL+ providerSettings().url + QLatin1String("/") + providerSettings().userName + QLatin1String("/items/") + zoteroKey;

        if(!providerSettings().pwd.isEmpty()) {
            pushString.append(QLatin1String("?&key=") + providerSettings().pwd);
        }

        QUrl pushUrl(pushString);

        QNetworkRequest request(pushUrl);
        request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
        request.setRawHeader("If-Match", etag.toAscii());

        m_reply = m_qnam.put(request, writeJsonContent(QVariantList() << currentItem, true));
        connect(m_reply, SIGNAL(finished()),this, SLOT(itemPushFinished()));
    }
    else {
        emit finished();
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
        }
        else {
            entryType = entry.value(QLatin1String("publicationtype")).toString();
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

    return json;
}

QVariantMap ZoteroSync::transformToJsonMap(const QString &entryType, const QVariantMap &item)
{
    QVariantMap jsonMap;

    // first get the right mappingentry
    QVariantMap mapping = m_fromZoteroMapping.value(entryType).toMap();

    // now foreach entry in the mapping find the right entyr in the item map
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
                // if we get artist:author fetch all author entries from the publication item and add the as artist person
                QStringList keys = creator.split(QLatin1String(":"));
                creatorsList.append( transformCreators(keys.first(),
                                                       item.value(keys.last(), QString()).toString()) );
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
            //TODO: handle note updates in zotero
            kWarning() << "notes are ignored at the moment";
        }
        else if(i.key() == QLatin1String("attachments") ) {
            //TODO: handle attachment updates in zotero
            kWarning() << "attachments are ignored at the moment";
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
void ZoteroSync::itemRequestFinished()
{
    if(m_reply->error()) {
        kDebug() << m_reply->error() << m_reply->errorString();

        QString errorString = QString("%1\n%2").arg(m_reply->error()).arg(m_reply->errorString());
        emit error(errorString);
        m_reply->deleteLater();
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

    emit finished();
    m_reply->deleteLater();
}

void ZoteroSync::itemsRequestFinished()
{
    if(m_reply->error()) {
        kDebug() << m_reply->error() << m_reply->errorString();

        QString errorString = QString("%1\n%2").arg(m_reply->error()).arg(m_reply->errorString());
        emit error(errorString);
        m_reply->deleteLater();
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

            // if there does exist more items we need to fetch get the right values for the next start
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
        m_reply->deleteLater();
        fetchItems(nextFetchLimit, nextFetchStart);
    }
    else {
        emit finished();
        m_reply->deleteLater();
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
                    qWarning() << "could not parse parent item for the note/attachment!";
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
                entry.insert(QLatin1String("publicationtype"), QLatin1String("incollection"));
            }
            else if(zoteroType == QLatin1String("conferencepaper")) {
                entry.insert(QLatin1String("publicationtype"), QLatin1String("inproceedings"));
            }
            else if(zoteroType == QLatin1String("document")) {
                entry.insert(QLatin1String("publicationtype"), QLatin1String("misc"));
            }
            else if(zoteroType == QLatin1String("manuscript")) {
                entry.insert(QLatin1String("publicationtype"), QLatin1String("script"));
            }
            else if(zoteroType == QLatin1String("journalarticle")) {
                entry.insert(QLatin1String("publicationtype"), QLatin1String("article"));
                entry.insert(QLatin1String("articletype"), QLatin1String("journal"));
            }
            else if(zoteroType == QLatin1String("magazinearticle")) {
                entry.insert(QLatin1String("publicationtype"), QLatin1String("article"));
                entry.insert(QLatin1String("articletype"), QLatin1String("magazine"));
            }
            else if(zoteroType == QLatin1String("newspaperarticle")) {
                entry.insert(QLatin1String("publicationtype"), QLatin1String("article"));
                entry.insert(QLatin1String("articletype"), QLatin1String("newspaper"));
            }
            else if(zoteroType == QLatin1String("encyclopediaarticle")) {
                entry.insert(QLatin1String("publicationtype"), QLatin1String("article"));
                entry.insert(QLatin1String("articletype"), QLatin1String("encyclopedia"));
            }
            else if(zoteroType == QLatin1String("blogpost")) {
                entry.insert(QLatin1String("publicationtype"), QLatin1String("article"));
                entry.insert(QLatin1String("articletype"), QLatin1String("blog"));
            }
            else if(zoteroType == QLatin1String("forumpost")) {
                entry.insert(QLatin1String("publicationtype"), QLatin1String("article"));
                entry.insert(QLatin1String("articletype"), QLatin1String("forum"));
            }
            else if(zoteroType == QLatin1String("webpage")) {
                entry.insert(QLatin1String("publicationtype"), QLatin1String("article"));
                entry.insert(QLatin1String("articletype"), QLatin1String("webpage"));
            }
            else {
                entry.insert(QLatin1String("publicationtype"), zoteroType);
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
    if(m_reply->error()) {
        kDebug() << m_reply->error() << m_reply->errorString();

        QString errorString = QString("%1\n%2").arg(m_reply->error()).arg(m_reply->errorString());
        emit error(errorString);
        m_reply->deleteLater();
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

    emit finished();
    m_reply->deleteLater();
}

void ZoteroSync::collectionsRequestFinished()
{
    if(m_reply->error()) {
        kDebug() << m_reply->error() << m_reply->errorString();

        QString errorString = QString("%1\n%2").arg(m_reply->error()).arg(m_reply->errorString());
        emit error(errorString);
        m_reply->deleteLater();
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
        m_reply->deleteLater();
        fetchCollections(nextFetchLimit, nextFetchStart);
    }
    else {
        emit finished();
        m_reply->deleteLater();
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
        kDebug() << m_reply->error() << m_reply->errorString();

        QString errorString = QString("%1\n%2").arg(m_reply->error()).arg(m_reply->errorString());
        emit error(errorString);
        m_reply->deleteLater();
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

    pushItemCache();
    m_reply->deleteLater();
}

void ZoteroSync::itemDeleteFinished()
{
    if(m_reply->error()) {
        kDebug() << m_reply->error() << m_reply->errorString();

        QString errorString = QString("%1\n%2").arg(m_reply->error()).arg(m_reply->errorString());
        emit error(errorString);
        m_reply->deleteLater();
    }

    deleteItemCache();
    m_reply->deleteLater();
}

void ZoteroSync::itemCollectionFinished()
{
    if(m_reply->error()) {
        kDebug() << m_reply->error() << m_reply->errorString();

        QString errorString = QString("%1\n%2").arg(m_reply->error()).arg(m_reply->errorString());
        emit error(errorString);
        m_reply->deleteLater();
    }

    m_reply->deleteLater();

    emit finished();
}

void ZoteroSync::itemCollectionRemoveFinished()
{
    if(m_reply->error()) {
        kDebug() << m_reply->error() << m_reply->errorString();

        QString errorString = QString("%1\n%2").arg(m_reply->error()).arg(m_reply->errorString());
        emit error(errorString);
        m_reply->deleteLater();
    }

    m_reply->deleteLater();

    removeItemFromCollectionCache();
}
