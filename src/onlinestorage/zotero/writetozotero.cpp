/*
 * Copyright 2011 Jörg Ehrichs <joerg.ehrichs@gmx.de>
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

#include "writetozotero.h"
#include "readfromzotero.h"

#include <kbibtex/entry.h>
#include <kbibtex/fileimporterbibtex.h>

#include <qjson/serializer.h>
#include <QtNetwork/QNetworkReply>
#include <QtCore/QXmlStreamReader>

#include <KDE/KDebug>

const int MAX_ITEMS_TO_PUSH = 45;

WriteToZotero::WriteToZotero(QObject *parent)
    : WriteToStorage(parent)
    , m_allRequestsSend(false)
    , m_entriesAfterSync(new File)
{
}

WriteToZotero::~WriteToZotero()
{
}

File *WriteToZotero::getFile()
{
    return m_entriesAfterSync;
}

File *WriteToZotero::getFailedPushRequestItems()
{
    File *corruptedEntries = new File;

    foreach(File *f,  m_failedItemPush) {
        corruptedEntries->append(*f);
    }

    return corruptedEntries;
}

void WriteToZotero::pushItems(File *items, const QString &collection)
{
    if(items->isEmpty()) {
        emit finished();
        return;
    }

    m_allRequestsSend = false;
    m_entriesAfterSync->clear();
    m_addToCollection = collection;
    m_progressPerFile = (qreal)items->size() / 200.0;
    m_progress = 0;

    // separate new items from the ones that send updates
    File *newItems = new File;
    File *updatingItems = new File;

    foreach(const QSharedPointer<Element> &element, *items) {
        Entry *entry = dynamic_cast<Entry *>(element.data());
        if(!entry) {
            continue;
        }
        QString zoteroKey = PlainTextValue::text(entry->value(QLatin1String("zoteroKey")));

        if(zoteroKey.isEmpty()) {
            newItems->append(element);
        }
        else {
            updatingItems->append(element);
            updateItem(element);
        }

        m_progress = m_progress + m_progressPerFile;
        emit progress(m_progress);
    }

    kDebug() << QLatin1String("send new items:") <<  newItems->size() << QLatin1String(" send updated items:") <<  updatingItems->size();

    if(!newItems->isEmpty()) {
        pushNewItems(newItems, m_addToCollection);
    }
    else {
        m_allRequestsSend = true;
    }
}

void WriteToZotero::pushNewItems(File *items, const QString &collection)
{
    if(items->isEmpty()) {
        if(openReplies() == 0) { emit finished(); }
        return;
    }

    // we can upload a maximum of 50 items per request
    if(items->size() > MAX_ITEMS_TO_PUSH) {
        m_allRequestsSend = false;
        // split the QList into smaller pieces
        File *smallList = new File;
        foreach(QSharedPointer<Element> e, *items) {
            if(smallList->size() >= MAX_ITEMS_TO_PUSH) {
                m_itemsToPushCache.append(e);
            }
            else {
                smallList->append(e);
            }
        }
        m_failedItemPush.append(smallList); // will be removed from this list when the zotero server did not return "Internal server error"
        pushNewItems(smallList, collection);
    }
    else {
        //POST /users/1/items
        //Content-Type: application/json
        //X-Zotero-Write-Token: 19a4f01ad623aa7214f82347e3711f56

        QString pushString = QLatin1String("https://api.zotero.org/") + m_psd.url + QLatin1String("/") + m_psd.userName + QLatin1String("/items");

        // This is a special occasion. When we add child notes (attachments) to an item. We add all items in "items"
        // to the same parent, so if we specified a parent, we now that all following items are an attachment
        QSharedPointer<Element> attachmentItem = items->first();
        Entry *entry = dynamic_cast<Entry *>(attachmentItem.data());
        if(!entry) { return; }
        QString attachmentparent = PlainTextValue::text( entry->value(QLatin1String("zoteroParent")) );

        kDebug() << "push new item with parent" << attachmentparent;

        if(!attachmentparent.isEmpty()) {
            pushString.append( QLatin1String("/") + attachmentparent + QLatin1String("/children"));
        }

        if(!m_psd.pwd.isEmpty()) {
            pushString.append(QLatin1String("?key=") + m_psd.pwd);
        }
        QUrl pushUrl(pushString);

        kDebug() << pushString;

        QNetworkRequest request(pushUrl);
        request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
        //request.setRawHeader("X-Zotero-Write-Token", etag);

        startRequest(request, writeJsonContent(items), QNetworkAccessManager::PostOperation);
        m_allRequestsSend = true;
    }
}

void WriteToZotero::updateItem(QSharedPointer<Element> item)
{
    Entry *entry = dynamic_cast<Entry *>(item.data());
    //PUT /users/1/items/ABCD2345
    //If-Match: "8e984e9b2a8fb560b0085b40f6c2c2b7"

    QString zoteroKey = PlainTextValue::text(entry->value(QLatin1String("zoteroKey")));
    QString etag = PlainTextValue::text(entry->value(QLatin1String("zoteroEtag")));

    QString pushString = QLatin1String("https://api.zotero.org/") + m_psd.url + QLatin1String("/") + m_psd.userName + QLatin1String("/items/") + zoteroKey;

    if(!m_psd.pwd.isEmpty()) {
        pushString.append(QLatin1String("?key=") + m_psd.pwd);
    }

    QUrl pushUrl(pushString);

    QNetworkRequest request(pushUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    request.setRawHeader("If-Match", etag.toLatin1());

    File *itemFile = new File;
    itemFile->append(item);
    QSharedPointer<Entry> entryPointer(entry);

    startRequest(request, writeJsonContent(itemFile, true), QNetworkAccessManager::PutOperation, entryPointer);
}

void WriteToZotero::addItemsToCollection(const QList<QString> &ids, const QString &collection )
{
    //POST /users/1/collections/QRST9876/items
    //ABCD2345 BCDE3456 CDEF4567 DEFG5678

    QString pushString = QLatin1String("https://api.zotero.org/") + m_psd.url + QLatin1String("/") + m_psd.userName + QLatin1String("/collections/") + collection + QLatin1String("/items");

    if(!m_psd.pwd.isEmpty()) {
        pushString.append(QLatin1String("?key=") + m_psd.pwd);
    }
    QUrl pushUrl(pushString);


    QNetworkRequest request(pushUrl);

    QString payload;
    foreach(const QString &id, ids) {
        payload.append(id);
        payload.append(QLatin1String(" "));
    }
    payload.chop(1);

    startRequest(request, payload.toLatin1(), QNetworkAccessManager::PostOperation);
}

void WriteToZotero::removeItemsFromCollection(const QList<QString> &ids, const QString &collection )
{
    //DELETE /users/1/collections/QRST9876/items/ABCD2345

    m_allRequestsSend = false;
    foreach(const QString &id, ids) {
        QString pushString = QLatin1String("https://api.zotero.org/") + m_psd.url + QLatin1String("/") + m_psd.userName + QLatin1String("/collections/") + collection + QLatin1String("/items/") + id;

        if(!m_psd.pwd.isEmpty()) {
            pushString.append(QLatin1String("?key=") + m_psd.pwd);
        }
        QUrl pushUrl(pushString);

        QNetworkRequest request(pushUrl);
        request.setHeader(QNetworkRequest::ContentTypeHeader,QLatin1String("application/json"));

        startRequest(request, 0, QNetworkAccessManager::DeleteOperation);
    }
    m_allRequestsSend = true;
}

void WriteToZotero::deleteItems(File *items)
{
    //DELETE /users/1/items/ABCD2345
    //If-Match: "8e984e9b2a8fb560b0085b40f6c2c2b7"

    m_allRequestsSend = false;
    foreach(QSharedPointer<Element> element, *items) {
        Entry *entry = dynamic_cast<Entry *>(element.data());
        if(!entry) {
            continue;
        }
        QString zoteroKey = PlainTextValue::text(entry->value(QLatin1String("zoteroKey")));
        QString pushString = QLatin1String("https://api.zotero.org/") + m_psd.url + QLatin1String("/") + m_psd.userName + QLatin1String("/items/") + zoteroKey;

        if(!m_psd.pwd.isEmpty()) {
            pushString.append(QLatin1String("?key=") + m_psd.pwd);
        }
        QUrl pushUrl(pushString);

        QNetworkRequest request(pushUrl);
        request.setHeader(QNetworkRequest::ContentTypeHeader,QLatin1String("application/json"));
        QString etag = PlainTextValue::text(entry->value(QLatin1String("zoteroEtag")));
        request.setRawHeader("If-Match", etag.toLatin1());

        startRequest(request, 0, QNetworkAccessManager::DeleteOperation);
    }
    m_allRequestsSend = true;
}

void WriteToZotero::deleteItems(QList<QPair<QString, QString> > items)
{
    //DELETE /users/1/items/ABCD2345
    //If-Match: "8e984e9b2a8fb560b0085b40f6c2c2b7"

    m_allRequestsSend = false;
    typedef QPair<QString,QString> Item;
    foreach(const Item &pair, items) {
        QString zoteroKey = pair.first;
        QString pushString = QLatin1String("https://api.zotero.org/") + m_psd.url + QLatin1String("/") + m_psd.userName + QLatin1String("/items/") + zoteroKey;

        if(!m_psd.pwd.isEmpty()) {
            pushString.append(QLatin1String("?key=") + m_psd.pwd);
        }
        QUrl pushUrl(pushString);

        QNetworkRequest request(pushUrl);
        request.setHeader(QNetworkRequest::ContentTypeHeader,QLatin1String("application/json"));
        QString etag = pair.second;
        request.setRawHeader("If-Match", etag.toLatin1());

        startRequest(request, 0, QNetworkAccessManager::DeleteOperation);
    }
    m_allRequestsSend = true;
}

void WriteToZotero::createCollection(const CollectionInfo &ci)
{
    //POST /users/1/collections
    //X-Zotero-Write-Token: 19a4f01ad623aa7214f82347e3711f56

    QString pushString = QLatin1String("https://api.zotero.org/") + m_psd.url + QLatin1String("/") + m_psd.userName + QLatin1String("/collections");

    if(!m_psd.pwd.isEmpty()) {
        pushString.append(QLatin1String("?key=") + m_psd.pwd);
    }
    QUrl pushUrl(pushString);

    QNetworkRequest request(pushUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader,QLatin1String("application/json"));
    //request.setRawHeader("X-Zotero-Write-Token", etag);

    startRequest(request, writeJsonContent(ci), QNetworkAccessManager::PostOperation);
}

void WriteToZotero::editCollection(const CollectionInfo &ci)
{
    //PUT /users/1/collections/RSTU8765
    //If-Match: "f0ebb2240a57f4115b3ce841d5218fa2"

    QString pushString = QLatin1String("https://api.zotero.org/") + m_psd.url + QLatin1String("/") + m_psd.userName + QLatin1String("/collections/") + ci.id;

    if(!m_psd.pwd.isEmpty()) {
        pushString.append(QLatin1String("?key=") + m_psd.pwd);
    }

    QUrl pushUrl(pushString);

    QNetworkRequest request(pushUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader,QLatin1String("application/json"));
    request.setRawHeader("If-Match", ci.eTag.toLatin1());

    startRequest(request, writeJsonContent(ci), QNetworkAccessManager::PutOperation);
}

void WriteToZotero::deleteCollection(const CollectionInfo &ci)
{
    //DELETE /users/1/collections/RSTU8765
    //If-Match: "f0ebb2240a57f4115b3ce841d5218fa2"

    QString pushString = QLatin1String("https://api.zotero.org/") + m_psd.url + QLatin1String("/") + m_psd.userName + QLatin1String("/collections/") + ci.id;

    if(!m_psd.pwd.isEmpty()) {
        pushString.append(QLatin1String("?key=") + m_psd.pwd);
    }
    QUrl pushUrl(pushString);

    QNetworkRequest request(pushUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader,QLatin1String("application/json"));
    request.setRawHeader("If-Match", ci.eTag.toLatin1());

    startRequest(request, 0, QNetworkAccessManager::DeleteOperation);
}

void WriteToZotero::requestFinished()
{
    // we get a reply from the server
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());

    QSharedPointer<Entry> updateEntry = serverReplyEntry(reply);
    serverReplyFinished(reply);

    // if the updateEntry is 0 we pushed new items to the server
    // otherwise we updated the item

    // no check if we got an error responce
    // sadly I can't seem to retrieve the error codes mentioned in the api documentation .. :/
    if(reply->error() != QNetworkReply::NoError) {
        kDebug() <<  reply->error() << reply->errorString();

        // well was an error but if this came from an itemCreation request it was very likely the
        // 299 "Error downloading https://.... - server replied: Internal Server Error"
        // means .. the items are stored successfully on the server but we did not get the server details
        // back. Thats bad, as we can't add teh sync information to these files
        // so on the next sync we will create duplications ... >_<

        // none the less if we have more items to send, send them and hope for the best
        if(m_allRequestsSend && openReplies() == 0) {
            if(!m_itemsToPushCache.isEmpty()) {
                File *smallList = new File;
                int nextItemCount = m_itemsToPushCache.size() >= MAX_ITEMS_TO_PUSH ? MAX_ITEMS_TO_PUSH : m_itemsToPushCache.size();
                while (nextItemCount != 0) {
                    smallList->append( m_itemsToPushCache.takeFirst() );
                    nextItemCount--;
                }
                m_failedItemPush.append(smallList); // will be removed from this list when the zotero server did not return "Internal server error"
                pushNewItems(smallList, m_addToCollection);
            }
            else {
                // if we have no open replies and no items in the queue we concider the
                emit finished();
            }
        }

        return;
    }

    // so we do have a responce from an item creation request which did not return the "internal server error"
    // remove the items from this list
    if(updateEntry.isNull() && !m_failedItemPush.isEmpty()) {
        m_failedItemPush.takeLast();
    }


    // we parse the response
    QXmlStreamReader xmlReader;
    xmlReader.setDevice(reply);

    int newFilesAdded = 0;
    QList<QString> ids;
    while(!xmlReader.atEnd()) {
        if(!xmlReader.readNextStartElement()) { continue; }

        if(xmlReader.name() == QLatin1String("entry")) {
            ReadFromZotero rfz;
            rfz.setAdoptBibtexTypes(adoptBibtexTypes());
            QSharedPointer<Element> newElement(rfz.readItemEntry(xmlReader));
            Entry *newElementEntry = dynamic_cast<Entry *>(newElement.data());

            m_progress = m_progress + m_progressPerFile;

            emit progress(m_progress);

            // if we got an earlier Entry in the server reply we know we updated the item
            // update the tag and updated date
            if(!updateEntry.isNull()) {
                updateEntry->remove(QLatin1String("zoteroupdated"));
                updateEntry->insert(QLatin1String("zoteroupdated"), newElementEntry->value(QLatin1String("zoteroupdated")));
                updateEntry->remove(QLatin1String("zoteroetag"));
                updateEntry->insert(QLatin1String("zoteroetag"), newElementEntry->value(QLatin1String("zoteroetag")));

                QString etag =  PlainTextValue::text(newElementEntry->value(QLatin1String("zoteroetag")));
                QString udated =  PlainTextValue::text(newElementEntry->value(QLatin1String("zoteroupdated")));
                QString newId = newElementEntry->id();
                ids << newId;
                emit entryItemUpdated(newId,etag, udated);
                break;
            }
            // otherwise, if we have no updateEntry we got a responce from an item creation request
            else {
                m_entriesAfterSync->append( newElement );
                newFilesAdded++;

                if(!m_addToCollection.isEmpty()) {
                    Entry *newElementEntry = dynamic_cast<Entry *>(newElement.data());
                    ids << PlainTextValue::text(newElementEntry->value(QLatin1String("zoterokey")));
                }
            }
        }
    }

    // push items to collection
    if(!m_addToCollection.isEmpty()) {
        addItemsToCollection(ids, m_addToCollection);
        m_progress += m_progressPerFile;
    }

    if(newFilesAdded != 0) {
        m_progress = m_progress + m_progressPerFile * newFilesAdded;
        emit progress(m_progress);
    }

    if(m_allRequestsSend && openReplies() == 0) {

        // we still have to send next 50 new items
        if(!m_itemsToPushCache.isEmpty()) {
            File *smallList = new File;
            int nextItemCount = m_itemsToPushCache.size() >= MAX_ITEMS_TO_PUSH ? MAX_ITEMS_TO_PUSH : m_itemsToPushCache.size();
            while (nextItemCount != 0) {
                smallList->append( m_itemsToPushCache.takeFirst() );
                nextItemCount--;
            }
            m_failedItemPush.append(smallList); // will be removed from this list when the zotero server did not return "Internal server error"
            pushNewItems(smallList, m_addToCollection);
        }
        else {
            // now we emit all the new entries we retrived from the server as response to our sent action
            emit finished();
        }
    }
}

QByteArray WriteToZotero::writeJsonContent(File *items, bool onlyUpdate)
{
    QVariantMap jsonMap;
    QVariantList itemList;

    foreach(QSharedPointer<Element> element, *items) {
        Entry *entry = dynamic_cast<Entry *>(element.data());
        if(!entry) {
            continue;
        }

        // attachment types note and file attachment
        if(entry->type().toLower() == QLatin1String("note")) {
            itemList.append( createNoteJson( entry ) );
            continue;
        }
        else if(entry->type().toLower() == QLatin1String("attachment")) {
            itemList.append( createAttachmentJson( entry ) );
            continue;
        }

        // none Zotero types which I try to squeeze into zotero
        if(adoptBibtexTypes()) {
            if(entry->type().toLower() == QLatin1String("article")) {
                QString articleType = PlainTextValue::text(entry->value(QLatin1String("articletype")));

                if(articleType == QLatin1String("newspaper")) {
                    itemList.append( createNewspaperArticleJson( entry) );
                }
                else if(articleType == QLatin1String("magazine")) {
                    itemList.append( createMagazineArticleJson( entry) );
                }
                else if(articleType == QLatin1String("encyclopedia")) {
                    itemList.append( createEncyclopediaArticleJson( entry) );
                }
                else if(articleType == QLatin1String("forum")) {
                    itemList.append( createForumPostJson( entry ) );
                }
                else if(articleType == QLatin1String("blog")) {
                    itemList.append( createBlogPostJson( entry ) );
                }
                else if(articleType == QLatin1String("webpage")) {
                    itemList.append( createWebpageJson( entry ) );
                }
                else {
                    itemList.append( createJournalArticleJson( entry) );
                }
            }
            else if(entry->type().toLower() == QLatin1String("inbook")) {
                itemList.append( createBookSectionJson( entry) );
            }
            else if(entry->type().toLower() == QLatin1String("inproceedings")) {
                itemList.append( createConferencePaperJson( entry) );
            }
            else if(entry->type().toLower() == QLatin1String("booklet")) {
                itemList.append( createBookJson( entry) );
            }
            else if(entry->type().toLower() == QLatin1String("conference")) {
                itemList.append( createConferencePaperJson( entry) );
            }
            else if(entry->type().toLower() == QLatin1String("proceedings")) {
                itemList.append( createConferencePaperJson( entry) );
            }
            else if(entry->type().toLower() == QLatin1String("incollection")) {
                itemList.append( createBookSectionJson( entry) );
            }
            else if(entry->type().toLower() == QLatin1String("dictionary")) {
                itemList.append( createEncyclopediaArticleJson(entry) );
            }
            else if(entry->type().toLower() == QLatin1String("encyclopedia")) {
                itemList.append( createEncyclopediaArticleJson(entry) );
            }
            else if(entry->type().toLower() == QLatin1String("manual")) {
                itemList.append( createDocumentJson( entry) );
            }
            else if(entry->type().toLower() == QLatin1String("mastersthesis")) {
                PlainText *ptValue = new PlainText(QLatin1String("master"));
                Value valueList;
                valueList.append(QSharedPointer<ValueItem>(ptValue));
                entry->insert(QLatin1String("thesisType"), valueList);

                itemList.append( createThesisJson( entry ) );
            }
            else if(entry->type().toLower() == QLatin1String("misc")) {
                itemList.append( createDocumentJson( entry) );
            }
            else if(entry->type().toLower() == QLatin1String("phdthesis")) {
                PlainText *ptValue = new PlainText(QLatin1String("phd"));
                Value valueList;
                valueList.append(QSharedPointer<ValueItem>(ptValue));
                entry->insert(QLatin1String("thesisType"), valueList);

                itemList.append( createThesisJson( entry) );
            }
            else if(entry->type().toLower() == QLatin1String("bachelor")) {
                PlainText *ptValue = new PlainText(QLatin1String("master"));
                Value valueList;
                valueList.append(QSharedPointer<ValueItem>(ptValue));
                entry->insert(QLatin1String("thesisType"), valueList);

                itemList.append( createThesisJson( entry) );
            }
            else if(entry->type().toLower() == QLatin1String("techreport")) {
                itemList.append( createReportJson( entry) );
            }
            else if(entry->type().toLower() == QLatin1String("script")) {
                itemList.append( createManuscriptJson(entry) );
            }
            else if(entry->type().toLower() == QLatin1String("unpublished")) {
                itemList.append( createDocumentJson( entry) );
            }
            else if(entry->type().toLower() == QLatin1String("legalcasedocument")) {
                itemList.append( createCaseJson( entry) );
            }
        }


        if(entry->type().toLower() == QLatin1String("artwork")) {
            itemList.append( createArtworkJson(entry) );
        }
        else if(entry->type().toLower() == QLatin1String("audiorecording")) {
            itemList.append( createAudioRecordingJson(entry) );
        }
        else if(entry->type().toLower() == QLatin1String("bill")) {
            itemList.append( createBillJson(entry) );
        }
        else if(entry->type().toLower() == QLatin1String("blogpost")) {
            itemList.append( createBlogPostJson(entry) );
        }
        else if(entry->type().toLower() == QLatin1String("book")) {
            itemList.append( createBookJson(entry) );
        }
        else if(entry->type().toLower() == QLatin1String("booksection")) {
            itemList.append( createBookSectionJson(entry) );
        }
        else if(entry->type().toLower() == QLatin1String("case")) {
            itemList.append( createCaseJson(entry) );
        }
        else if(entry->type().toLower() == QLatin1String("computerprogram")) {
            itemList.append( createComputerProgramJson(entry) );
        }
        else if(entry->type().toLower() == QLatin1String("conferencepaper")) {
            itemList.append( createConferencePaperJson(entry) );
        }
        else if(entry->type().toLower() == QLatin1String("dictionaryentry")) {
            itemList.append( createDictionaryEntryJson(entry) );
        }
        else if(entry->type().toLower() == QLatin1String("document")) {
            itemList.append( createDocumentJson(entry) );
        }
        else if(entry->type().toLower() == QLatin1String("email")) {
            itemList.append( createEmailJson(entry) );
        }
        else if(entry->type().toLower() == QLatin1String("encyclopediaarticle")) {
            itemList.append( createEncyclopediaArticleJson(entry) );
        }
        else if(entry->type().toLower() == QLatin1String("film")) {
            itemList.append( createFilmJson(entry) );
        }
        else if(entry->type().toLower() == QLatin1String("forumpost")) {
            itemList.append( createForumPostJson(entry) );
        }
        else if(entry->type().toLower() == QLatin1String("hearing")) {
            itemList.append( createHearingJson(entry) );
        }
        else if(entry->type().toLower() == QLatin1String("instantmessage")) {
            itemList.append( createInstantMessageJson(entry) );
        }
        else if(entry->type().toLower() == QLatin1String("interview")) {
            itemList.append( createInterviewJson(entry) );
        }
        else if(entry->type().toLower() == QLatin1String("journalarticle")) {
            itemList.append( createJournalArticleJson(entry) );
        }
        else if(entry->type().toLower() == QLatin1String("letter")) {
            itemList.append( createLetterJson(entry) );
        }
        else if(entry->type().toLower() == QLatin1String("magazinearticle")) {
            itemList.append( createMagazineArticleJson(entry) );
        }
        else if(entry->type().toLower() == QLatin1String("manuscript")) {
            itemList.append( createManuscriptJson(entry) );
        }
        else if(entry->type().toLower() == QLatin1String("map")) {
            itemList.append( createMapJson(entry) );
        }
        else if(entry->type().toLower() == QLatin1String("newspaperarticle")) {
            itemList.append( createNewspaperArticleJson(entry) );
        }
        else if(entry->type().toLower() == QLatin1String("note")) {
            itemList.append( createNoteJson(entry) );
        }
        else if(entry->type().toLower() == QLatin1String("patent")) {
            itemList.append( createPatentJson(entry) );
        }
        else if(entry->type().toLower() == QLatin1String("podcast")) {
            itemList.append( createPodcastJson(entry) );
        }
        else if(entry->type().toLower() == QLatin1String("presentation")) {
            itemList.append( createPresentationJson(entry) );
        }
        else if(entry->type().toLower() == QLatin1String("radiobroadcast")) {
            itemList.append( createRadioBroadcastJson(entry) );
        }
        else if(entry->type().toLower() == QLatin1String("report")) {
            itemList.append( createReportJson(entry) );
        }
        else if(entry->type().toLower() == QLatin1String("statute")) {
            itemList.append( createStatuteJson(entry) );
        }
        else if(entry->type().toLower() == QLatin1String("tvbroadcast")) {
            itemList.append( createTvBroadcastJson(entry) );
        }
        else if(entry->type().toLower() == QLatin1String("thesis")) {
            itemList.append( createThesisJson(entry) );
        }
        else if(entry->type().toLower() == QLatin1String("videorecording")) {
            itemList.append( createVideoRecordingJson(entry) );
        }
        else if(entry->type().toLower() == QLatin1String("webpage")) {
            itemList.append( createWebpageJson(entry) );
        }
        else {
             // this else is thrown when adoptToBibtex is on
            // if we filled the itemList in this case, we ignore the return here
            if(itemList.isEmpty()) {
                kDebug() << "unknwon bibtex entry type" <<  entry->type() << "can't create zotero json from it";

                QByteArray tmp;
                return tmp;
            }
        }
    }

    jsonMap.insert(QLatin1String("items"), itemList);

    QJson::Serializer serializer;
    QByteArray json;
    if(onlyUpdate) {
        json = serializer.serialize(itemList.first());
    }
    else {
        json = serializer.serialize(jsonMap);
    }

    return json;
}

QByteArray WriteToZotero::writeJsonContent(const CollectionInfo &collection)
{
    QVariantMap jsonMap;

    jsonMap.insert(QLatin1String("name"),collection.name);
    jsonMap.insert(QLatin1String("parent"),collection.parentId);

    QJson::Serializer serializer;
    QByteArray json = serializer.serialize(jsonMap);

    return json;
}

QVariantList WriteToZotero::createCreatorsJson(Entry *e, const QString &type)
{
    QVariantList jsonMap;

    QStringList creatorList;
    creatorList = creatorTypeForZoteroMapping(type);

    bool handledAuthor = false;
    bool handledEditor = false;

    foreach(const QString &creatorType, creatorList) {
        foreach(QSharedPointer<ValueItem> vi, e->value(creatorType)) {

            Person *p = dynamic_cast<Person *>(vi.data());

            if(p) {
                QVariantMap personMap;
                personMap.insert(QLatin1String("creatorType"),creatorType);

                personMap.insert(QLatin1String("firstName"),p->firstName());
                personMap.insert(QLatin1String("lastName"),p->lastName());

                jsonMap.append(personMap);
            }
            // no valid person name during import
            // parse person again
            // necessary for fields representing a person which are not author= or editor= as kbibtex
            // dosn't support this out of the box
            else {
                QString personString = PlainTextValue::text(*vi);
                QStringList personList;
                splitPersonList(personString, personList);

                foreach(const QString &pit, personList) {
                    QString first;
                    QString last;
                    splitName(pit, first, last);

                    QVariantMap personMap;
                    personMap.insert(QLatin1String("creatorType"),creatorType);

                    personMap.insert(QLatin1String("firstName"),first);
                    personMap.insert(QLatin1String("lastName"),last);

                    jsonMap.append(personMap);
                }
            }
        }

        if(creatorType == QLatin1String("author")) {
            handledAuthor=true;
        }
        if(creatorType == QLatin1String("editor")) {
            handledEditor=true;
        }
    }

    // beside the zotero key, fetch the normal bibtex entries
    // these have to be mapped to something that fits into the zotero version
    if(adoptBibtexTypes()) {
        QStringList bibTexCreatorList;
        if(!handledAuthor)
            bibTexCreatorList << QLatin1String("author");
        if(!handledEditor)
            bibTexCreatorList << QLatin1String("editor");

        QStringList mappingList = bibtexCreatorZoteroMapping(type);

        int pos = 0;
        foreach(const QString &creatorType, bibTexCreatorList) {

            if (mappingList.size() < pos + 1)
                continue;

            foreach(QSharedPointer<ValueItem> vi, e->value(creatorType)) {

                Person *p = dynamic_cast<Person *>(vi.data());

                if(p) {
                    QVariantMap personMap;
                    personMap.insert(QLatin1String("creatorType"),mappingList.at(pos));

                    personMap.insert(QLatin1String("firstName"),p->firstName());
                    personMap.insert(QLatin1String("lastName"),p->lastName());

                    jsonMap.append(personMap);
                }
                // no valid person name during import
                // parse person again
                // necessary for fields representing a person which are not author= or editor= as kbibtex
                // dosn't support this out of the box
                else {
                    QString personString = PlainTextValue::text(*vi);
                    QStringList personList;
                    splitPersonList(personString, personList);

                    foreach(const QString &pit, personList) {
                        QString first;
                        QString last;
                        splitName(pit, first, last);

                        QVariantMap personMap;
                        personMap.insert(QLatin1String("creatorType"),creatorType);

                        personMap.insert(QLatin1String("firstName"),first);
                        personMap.insert(QLatin1String("lastName"),last);

                        jsonMap.append(personMap);
                    }
                }
            }
            pos++;
        }
    }

    return jsonMap;
}

QStringList WriteToZotero::creatorTypeForZoteroMapping(const QString &type)
{
    QStringList creatorTypes;

    // taken from https://api.zotero.org/itemTypeCreatorTypes?itemType=webpage etc

    if(type.toLower() == QLatin1String("artwork")) {
        creatorTypes << QLatin1String("artist") <<  QLatin1String("contributor");
    }
    else if(type.toLower() == QLatin1String("audiorecording")) {
        creatorTypes << QLatin1String("performer") <<  QLatin1String("composer") <<  QLatin1String("contributor") <<  QLatin1String("wordsBy");
    }
    else if(type.toLower() == QLatin1String("bill")) {
        creatorTypes << QLatin1String("sponsor") <<  QLatin1String("contributor") <<  QLatin1String("cosponsor");
    }
    else if(type.toLower() == QLatin1String("blogpost")) {
        creatorTypes << QLatin1String("author") <<  QLatin1String("commenter") <<  QLatin1String("contributor");
    }
    else if(type.toLower() == QLatin1String("book")) {
        creatorTypes << QLatin1String("author") <<  QLatin1String("editor") <<  QLatin1String("contributor") <<  QLatin1String("seriesEditor") <<  QLatin1String("translator");
    }
    else if(type.toLower() == QLatin1String("booksection")) {
        creatorTypes << QLatin1String("author") <<  QLatin1String("bookAuthor") <<  QLatin1String("editor") <<  QLatin1String("contributor") <<  QLatin1String("seriesEditor") <<  QLatin1String("translator");
    }
    else if(type.toLower() == QLatin1String("case")) {
        creatorTypes << QLatin1String("author") <<  QLatin1String("contributor") <<  QLatin1String("counsel");
    }
    else if(type.toLower() == QLatin1String("computerprogram")) {
        creatorTypes << QLatin1String("programmer") <<  QLatin1String("contributor");
    }
    else if(type.toLower() == QLatin1String("conferencepaper")) {
        creatorTypes << QLatin1String("author") <<  QLatin1String("contributor") <<  QLatin1String("editor") <<  QLatin1String("seriesEditor") <<  QLatin1String("translator");
    }
    else if(type.toLower() == QLatin1String("dictionaryentry")) {
        creatorTypes << QLatin1String("author") <<  QLatin1String("contributor") <<  QLatin1String("editor") <<  QLatin1String("seriesEditor") <<  QLatin1String("translator");
    }
    else if(type.toLower() == QLatin1String("document")) {
        creatorTypes << QLatin1String("author") <<  QLatin1String("contributor") <<  QLatin1String("editor") <<  QLatin1String("reviewedAuthor") <<  QLatin1String("translator");
    }
    else if(type.toLower() == QLatin1String("email")) {
        creatorTypes << QLatin1String("author") <<  QLatin1String("contributor") <<  QLatin1String("recipient");
    }
    else if(type.toLower() == QLatin1String("encyclopediaarticle")) {
        creatorTypes << QLatin1String("author") <<  QLatin1String("contributor") <<  QLatin1String("editor") <<  QLatin1String("seriesEditor") <<  QLatin1String("translator");
    }
    else if(type.toLower() == QLatin1String("film")) {
        creatorTypes << QLatin1String("director") <<  QLatin1String("contributor") <<  QLatin1String("producer") <<  QLatin1String("scriptwriter");
    }
    else if(type.toLower() == QLatin1String("forumpost")) {
        creatorTypes << QLatin1String("author") <<  QLatin1String("contributor");
    }
    else if(type.toLower() == QLatin1String("hearing")) {
        creatorTypes << QLatin1String("contributor");
    }
    else if(type.toLower() == QLatin1String("instantmessage")) {
        creatorTypes << QLatin1String("author") <<  QLatin1String("contributor") <<  QLatin1String("recipient");
    }
    else if(type.toLower() == QLatin1String("interview")) {
        creatorTypes << QLatin1String("interviewee") <<  QLatin1String("contributor") <<  QLatin1String("interviewer") <<  QLatin1String("translator");
    }
    else if(type.toLower() == QLatin1String("journalarticle")) {
        creatorTypes << QLatin1String("author") <<  QLatin1String("contributor") <<  QLatin1String("editor") <<  QLatin1String("reviewedAuthor") <<  QLatin1String("translator");
    }
    else if(type.toLower() == QLatin1String("letter")) {
        creatorTypes << QLatin1String("author") <<  QLatin1String("contributor") <<  QLatin1String("recipient");
    }
    else if(type.toLower() == QLatin1String("magazinearticle")) {
        creatorTypes << QLatin1String("author") <<  QLatin1String("contributor") <<  QLatin1String("reviewedAuthor") <<  QLatin1String("translator");
    }
    else if(type.toLower() == QLatin1String("manuscript")) {
        creatorTypes << QLatin1String("author") <<  QLatin1String("contributor") <<  QLatin1String("translator");
    }
    else if(type.toLower() == QLatin1String("map")) {
        creatorTypes << QLatin1String("cartographer") <<  QLatin1String("contributor") <<  QLatin1String("seriesEditor");
    }
    else if(type.toLower() == QLatin1String("newspaperarticle")) {
        creatorTypes << QLatin1String("author") <<  QLatin1String("contributor") <<  QLatin1String("reviewedAuthor") <<  QLatin1String("translator");
    }
    else if(type.toLower() == QLatin1String("note")) {
    }
    else if(type.toLower() == QLatin1String("patent")) {
        creatorTypes << QLatin1String("inventor") <<  QLatin1String("attorneyAgent") <<  QLatin1String("contributor");
    }
    else if(type.toLower() == QLatin1String("podcast")) {
        creatorTypes << QLatin1String("podcaster") <<  QLatin1String("contributor") <<  QLatin1String("guest");
    }
    else if(type.toLower() == QLatin1String("presentation")) {
        creatorTypes << QLatin1String("presenter") <<  QLatin1String("contributor");
    }
    else if(type.toLower() == QLatin1String("radiobroadcast")) {
        creatorTypes << QLatin1String("director") <<  QLatin1String("castMember") <<  QLatin1String("contributor") <<  QLatin1String("guest") <<  QLatin1String("producer") <<  QLatin1String("scriptwriter");
    }
    else if(type.toLower() == QLatin1String("report")) {
        creatorTypes << QLatin1String("author") <<  QLatin1String("contributor") <<  QLatin1String("seriesEditor") <<  QLatin1String("translator");
    }
    else if(type.toLower() == QLatin1String("statute")) {
        creatorTypes << QLatin1String("author") <<  QLatin1String("contributor");
    }
    else if(type.toLower() == QLatin1String("tvbroadcast")) {
        creatorTypes << QLatin1String("director") <<  QLatin1String("castMember") <<  QLatin1String("contributor") <<  QLatin1String("guest") <<  QLatin1String("producer") <<  QLatin1String("scriptwriter");
    }
    else if(type.toLower() == QLatin1String("thesis")) {
        creatorTypes << QLatin1String("author") <<  QLatin1String("contributor");
    }
    else if(type.toLower() == QLatin1String("videorecording")) {
        creatorTypes << QLatin1String("director") <<  QLatin1String("castMember") <<  QLatin1String("contributor") <<  QLatin1String("producer") <<  QLatin1String("scriptwriter");
    }
    else if(type.toLower() == QLatin1String("webpage")) {
        creatorTypes << QLatin1String("author") <<  QLatin1String("contributor") <<  QLatin1String("translator");
    }

    return creatorTypes;
}

QStringList WriteToZotero::bibtexCreatorZoteroMapping(const QString &type)
{
    // here we map what the author and editor field in bibtex means for the zotero fields
    // first entry is the author 2nd the editor

    QStringList creatorTypes;

    if(type.toLower() == QLatin1String("artwork")) {
        creatorTypes << QLatin1String("artist");
    }
    else if(type.toLower() == QLatin1String("audiorecording")) {
        creatorTypes << QLatin1String("performer") <<  QLatin1String("composer");
    }
    else if(type.toLower() == QLatin1String("bill")) {
        creatorTypes << QLatin1String("sponsor");
    }
    else if(type.toLower() == QLatin1String("blogpost")) {
        creatorTypes << QLatin1String("author");
    }
    else if(type.toLower() == QLatin1String("book")) {
        creatorTypes << QLatin1String("author") <<  QLatin1String("editor");
    }
    else if(type.toLower() == QLatin1String("booksection")) {
        creatorTypes << QLatin1String("author") <<  QLatin1String("editor");
    }
    else if(type.toLower() == QLatin1String("case")) {
        creatorTypes << QLatin1String("author");
    }
    else if(type.toLower() == QLatin1String("computerprogram")) {
        creatorTypes << QLatin1String("programmer");
    }
    else if(type.toLower() == QLatin1String("conferencepaper")) {
        creatorTypes << QLatin1String("author") <<  QLatin1String("editor");
    }
    else if(type.toLower() == QLatin1String("dictionaryentry")) {
        creatorTypes << QLatin1String("author") <<  QLatin1String("editor");
    }
    else if(type.toLower() == QLatin1String("document")) {
        creatorTypes << QLatin1String("author") <<  QLatin1String("editor");
    }
    else if(type.toLower() == QLatin1String("email")) {
        creatorTypes << QLatin1String("author");
    }
    else if(type.toLower() == QLatin1String("encyclopediaarticle")) {
        creatorTypes << QLatin1String("author") <<  QLatin1String("editor");
    }
    else if(type.toLower() == QLatin1String("film")) {
        creatorTypes << QLatin1String("director");
    }
    else if(type.toLower() == QLatin1String("forumpost")) {
        creatorTypes << QLatin1String("author");
    }
    else if(type.toLower() == QLatin1String("hearing")) {
        //creatorTypes << QLatin1String("contributor") <<  QLatin1String("contributor");
    }
    else if(type.toLower() == QLatin1String("instantmessage")) {
        creatorTypes << QLatin1String("author");
    }
    else if(type.toLower() == QLatin1String("interview")) {
        creatorTypes << QLatin1String("interviewee");
    }
    else if(type.toLower() == QLatin1String("journalarticle")) {
        creatorTypes << QLatin1String("author") <<  QLatin1String("editor");
    }
    else if(type.toLower() == QLatin1String("letter")) {
        creatorTypes << QLatin1String("author");
    }
    else if(type.toLower() == QLatin1String("magazinearticle")) {
        creatorTypes << QLatin1String("author");
    }
    else if(type.toLower() == QLatin1String("manuscript")) {
        creatorTypes << QLatin1String("author");
    }
    else if(type.toLower() == QLatin1String("map")) {
        creatorTypes << QLatin1String("cartographer");
    }
    else if(type.toLower() == QLatin1String("newspaperarticle")) {
        creatorTypes << QLatin1String("author");
    }
    else if(type.toLower() == QLatin1String("note")) {
    }
    else if(type.toLower() == QLatin1String("patent")) {
        creatorTypes << QLatin1String("inventor");
    }
    else if(type.toLower() == QLatin1String("podcast")) {
        creatorTypes << QLatin1String("podcaster");
    }
    else if(type.toLower() == QLatin1String("presentation")) {
        creatorTypes << QLatin1String("presenter");
    }
    else if(type.toLower() == QLatin1String("radiobroadcast")) {
        creatorTypes << QLatin1String("director");
    }
    else if(type.toLower() == QLatin1String("report")) {
        creatorTypes << QLatin1String("author");
    }
    else if(type.toLower() == QLatin1String("statute")) {
        creatorTypes << QLatin1String("author");
    }
    else if(type.toLower() == QLatin1String("tvbroadcast")) {
        creatorTypes << QLatin1String("director");
    }
    else if(type.toLower() == QLatin1String("thesis")) {
        creatorTypes << QLatin1String("author");
    }
    else if(type.toLower() == QLatin1String("videorecording")) {
        creatorTypes << QLatin1String("director");
    }
    else if(type.toLower() == QLatin1String("webpage")) {
        creatorTypes << QLatin1String("author");
    }

    return creatorTypes;
}

QVariantList WriteToZotero::createTagsJson(Entry *e)
{
    QVariantList jsonMap;
    QString checkValue;
    if(adoptBibtexTypes()) {
        checkValue = QLatin1String("keywords");
    }
    else {
        checkValue = QLatin1String("tag");
    }

    foreach(QSharedPointer<ValueItem> vi, e->value(checkValue)) {
        Keyword *k = dynamic_cast<Keyword *>(vi.data());

        if(k) {
            QVariantMap tagMap;
            tagMap.insert(QLatin1String("tag"),k->text());
            jsonMap.append(tagMap);
        }
    }

    return jsonMap;
}

QVariantMap WriteToZotero::createArtworkJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert(QLatin1String("itemType"),QLatin1String("artwork"));
    jsonMap.insert(QLatin1String("title"), PlainTextValue::text(e->value(QLatin1String("title"))));
    jsonMap.insert(QLatin1String("creators"),createCreatorsJson(e, QLatin1String("artwork")));
    jsonMap.insert(QLatin1String("artworkMedium"),PlainTextValue::text(e->value(QLatin1String("artworkMedium"))));
    jsonMap.insert(QLatin1String("artworkSize"),PlainTextValue::text(e->value(QLatin1String("artworkSize"))));
    jsonMap.insert(QLatin1String("date"),PlainTextValue::text(e->value(QLatin1String("date"))));
    jsonMap.insert(QLatin1String("language"),PlainTextValue::text(e->value(QLatin1String("language"))));
    jsonMap.insert(QLatin1String("shortTitle"),PlainTextValue::text(e->value(QLatin1String("shortTitle"))));
    jsonMap.insert(QLatin1String("archive"),PlainTextValue::text(e->value(QLatin1String("archive"))));
    jsonMap.insert(QLatin1String("archiveLocation"),PlainTextValue::text(e->value(QLatin1String("archiveLocation"))));
    jsonMap.insert(QLatin1String("libraryCatalog"),PlainTextValue::text(e->value(QLatin1String("libraryCatalog"))));
    jsonMap.insert(QLatin1String("url"),PlainTextValue::text(e->value(QLatin1String("url"))));
    jsonMap.insert(QLatin1String("accessDate"),PlainTextValue::text(e->value(QLatin1String("accessDate"))));
    jsonMap.insert(QLatin1String("tags"),createTagsJson(e));
    //jsonMap.insert(QLatin1String("notes","");

    if(adoptBibtexTypes()) {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstract"))));
        jsonMap.insert(QLatin1String("callNumber"),PlainTextValue::text(e->value(QLatin1String("lccn"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("copyright"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("note"))));
    }
    else {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstractNote"))));
        jsonMap.insert(QLatin1String("callNumber"),PlainTextValue::text(e->value(QLatin1String("callNumber"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("rights"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("extra"))));
    }

    return jsonMap;
}

QVariantMap WriteToZotero::createAudioRecordingJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert(QLatin1String("itemType"),QLatin1String("audioRecording"));
    jsonMap.insert(QLatin1String("title"), PlainTextValue::text(e->value(QLatin1String("title"))));
    jsonMap.insert(QLatin1String("creators"),createCreatorsJson(e, QLatin1String("audioRecording")));
    jsonMap.insert(QLatin1String("audioRecordingFormat"),PlainTextValue::text(e->value(QLatin1String("audioRecordingFormat"))));
    jsonMap.insert(QLatin1String("volume"),PlainTextValue::text(e->value(QLatin1String("volume"))));
    jsonMap.insert(QLatin1String("numberOfVolumes"),PlainTextValue::text(e->value(QLatin1String("numberOfVolumes"))));
    jsonMap.insert(QLatin1String("date"),PlainTextValue::text(e->value(QLatin1String("date"))));
    jsonMap.insert(QLatin1String("runningTime"),PlainTextValue::text(e->value(QLatin1String("runningTime"))));
    jsonMap.insert(QLatin1String("language"),PlainTextValue::text(e->value(QLatin1String("language"))));
    jsonMap.insert(QLatin1String("ISBN"),PlainTextValue::text(e->value(QLatin1String("isbn"))));
    jsonMap.insert(QLatin1String("shortTitle"),PlainTextValue::text(e->value(QLatin1String("shortTitle"))));
    jsonMap.insert(QLatin1String("archive"),PlainTextValue::text(e->value(QLatin1String("archive"))));
    jsonMap.insert(QLatin1String("archiveLocation"),PlainTextValue::text(e->value(QLatin1String("archiveLocation"))));
    jsonMap.insert(QLatin1String("libraryCatalog"),PlainTextValue::text(e->value(QLatin1String("libraryCatalog"))));
    jsonMap.insert(QLatin1String("url"),PlainTextValue::text(e->value(QLatin1String("url"))));
    jsonMap.insert(QLatin1String("accessDate"),PlainTextValue::text(e->value(QLatin1String("accessDate"))));
    jsonMap.insert(QLatin1String("tags"),createTagsJson(e));
    //jsonMap.insert(QLatin1String("notes","");

    if(adoptBibtexTypes()) {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstract"))));
        jsonMap.insert(QLatin1String("callNumber"),PlainTextValue::text(e->value(QLatin1String("lccn"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("copyright"))));
        jsonMap.insert(QLatin1String("seriesTitle"),PlainTextValue::text(e->value(QLatin1String("series"))));
        jsonMap.insert(QLatin1String("place"),PlainTextValue::text(e->value(QLatin1String("address"))));
        jsonMap.insert(QLatin1String("label"),PlainTextValue::text(e->value(QLatin1String("publisher"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("note"))));
    }
    else {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstractNote"))));
        jsonMap.insert(QLatin1String("callNumber"),PlainTextValue::text(e->value(QLatin1String("callNumber"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("rights"))));
        jsonMap.insert(QLatin1String("seriesTitle"),PlainTextValue::text(e->value(QLatin1String("seriesTitle"))));
        jsonMap.insert(QLatin1String("place"),PlainTextValue::text(e->value(QLatin1String("place"))));
        jsonMap.insert(QLatin1String("label"),PlainTextValue::text(e->value(QLatin1String("label"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("extra"))));
    }


    return jsonMap;
}

QVariantMap WriteToZotero::createBillJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert(QLatin1String("itemType"),QLatin1String("bill"));
    jsonMap.insert(QLatin1String("title"), PlainTextValue::text(e->value(QLatin1String("title"))));
    jsonMap.insert(QLatin1String("creators"),createCreatorsJson(e, QLatin1String("bill")));
    jsonMap.insert(QLatin1String("code"),PlainTextValue::text(e->value(QLatin1String("code"))));
    jsonMap.insert(QLatin1String("codeVolume"),PlainTextValue::text(e->value(QLatin1String("codeVolume"))));
    jsonMap.insert(QLatin1String("history"),PlainTextValue::text(e->value(QLatin1String("history"))));
    jsonMap.insert(QLatin1String("date"),PlainTextValue::text(e->value(QLatin1String("date"))));
    jsonMap.insert(QLatin1String("language"),PlainTextValue::text(e->value(QLatin1String("language"))));
    jsonMap.insert(QLatin1String("url"),PlainTextValue::text(e->value(QLatin1String("url"))));
    jsonMap.insert(QLatin1String("accessDate"),PlainTextValue::text(e->value(QLatin1String("accessDate"))));
    jsonMap.insert(QLatin1String("shortTitle"),PlainTextValue::text(e->value(QLatin1String("shortTitle"))));
    jsonMap.insert(QLatin1String("tags"),createTagsJson(e));
    //jsonMap.insert(QLatin1String("notes","");

    if(adoptBibtexTypes()) {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstract"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("copyright"))));
        jsonMap.insert(QLatin1String("billNumber"),PlainTextValue::text(e->value(QLatin1String("number"))));
        jsonMap.insert(QLatin1String("section"),PlainTextValue::text(e->value(QLatin1String("chapter"))));
        jsonMap.insert(QLatin1String("codePages"),PlainTextValue::text(e->value(QLatin1String("pages"))));

        QString LegislativeBody = PlainTextValue::text(e->value(QLatin1String("organization")));
        if(LegislativeBody.isEmpty())
            LegislativeBody = PlainTextValue::text(e->value(QLatin1String("institution")));
        jsonMap.insert(QLatin1String("legislativeBody"),LegislativeBody);

        jsonMap.insert(QLatin1String("session"),PlainTextValue::text(e->value(QLatin1String("event"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("note"))));
    }
    else {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstractNote"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("rights"))));
        jsonMap.insert(QLatin1String("billNumber"),PlainTextValue::text(e->value(QLatin1String("billNumber"))));
        jsonMap.insert(QLatin1String("section"),PlainTextValue::text(e->value(QLatin1String("section"))));
        jsonMap.insert(QLatin1String("codePages"),PlainTextValue::text(e->value(QLatin1String("codePages"))));
        jsonMap.insert(QLatin1String("legislativeBody"),PlainTextValue::text(e->value(QLatin1String("legislativeBody"))));
        jsonMap.insert(QLatin1String("session"),PlainTextValue::text(e->value(QLatin1String("session"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("extra"))));
    }

    return jsonMap;
}

QVariantMap WriteToZotero::createBlogPostJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert(QLatin1String("itemType"),QLatin1String("blogPost"));
    jsonMap.insert(QLatin1String("title"), PlainTextValue::text(e->value(QLatin1String("title"))));
    jsonMap.insert(QLatin1String("creators"),createCreatorsJson(e, QLatin1String("blogPost")));
    jsonMap.insert(QLatin1String("date"),PlainTextValue::text(e->value(QLatin1String("date"))));
    jsonMap.insert(QLatin1String("url"),PlainTextValue::text(e->value(QLatin1String("url"))));
    jsonMap.insert(QLatin1String("accessDate"),PlainTextValue::text(e->value(QLatin1String("accessDate"))));
    jsonMap.insert(QLatin1String("language"),PlainTextValue::text(e->value(QLatin1String("language"))));
    jsonMap.insert(QLatin1String("shortTitle"),PlainTextValue::text(e->value(QLatin1String("shortTitle"))));
    jsonMap.insert(QLatin1String("tags"),createTagsJson(e));
    //jsonMap.insert(QLatin1String("notes","");

    if(adoptBibtexTypes()) {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstract"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("copyright"))));
        jsonMap.insert(QLatin1String("blogTitle"),PlainTextValue::text(e->value(QLatin1String("booktitle"))));
        jsonMap.insert(QLatin1String("websiteType"),PlainTextValue::text(e->value(QLatin1String("type"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("note"))));
    }
    else {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstractNote"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("rights"))));
        jsonMap.insert(QLatin1String("blogTitle"),PlainTextValue::text(e->value(QLatin1String("blogTitle"))));
        jsonMap.insert(QLatin1String("websiteType"),PlainTextValue::text(e->value(QLatin1String("websiteType"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("extra"))));
    }

    return jsonMap;
}

QVariantMap WriteToZotero::createBookJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert(QLatin1String("itemType"),QLatin1String("book"));
    jsonMap.insert(QLatin1String("title"), PlainTextValue::text(e->value(QLatin1String("title"))));
    jsonMap.insert(QLatin1String("creators"),createCreatorsJson(e, QLatin1String("book")));
    jsonMap.insert(QLatin1String("series"),PlainTextValue::text(e->value(QLatin1String("series"))));
    jsonMap.insert(QLatin1String("volume"),PlainTextValue::text(e->value(QLatin1String("volume"))));
    jsonMap.insert(QLatin1String("numberOfVolumes"),PlainTextValue::text(e->value(QLatin1String("numberOfVolumes"))));
    jsonMap.insert(QLatin1String("edition"),PlainTextValue::text(e->value(QLatin1String("edition"))));
    jsonMap.insert(QLatin1String("publisher"),PlainTextValue::text(e->value(QLatin1String("publisher"))));
    jsonMap.insert(QLatin1String("date"),PlainTextValue::text(e->value(QLatin1String("date"))));
    jsonMap.insert(QLatin1String("numPages"),PlainTextValue::text(e->value(QLatin1String("numPages"))));
    jsonMap.insert(QLatin1String("language"),PlainTextValue::text(e->value(QLatin1String("language"))));
    jsonMap.insert(QLatin1String("ISBN"),PlainTextValue::text(e->value(QLatin1String("isbn"))));
    jsonMap.insert(QLatin1String("shortTitle"),PlainTextValue::text(e->value(QLatin1String("shortTitle"))));
    jsonMap.insert(QLatin1String("url"),PlainTextValue::text(e->value(QLatin1String("url"))));
    jsonMap.insert(QLatin1String("accessDate"),PlainTextValue::text(e->value(QLatin1String("accessDate"))));
    jsonMap.insert(QLatin1String("archive"),PlainTextValue::text(e->value(QLatin1String("archive"))));
    jsonMap.insert(QLatin1String("archiveLocation"),PlainTextValue::text(e->value(QLatin1String("archiveLocation"))));
    jsonMap.insert(QLatin1String("libraryCatalog"),PlainTextValue::text(e->value(QLatin1String("libraryCatalog"))));
    jsonMap.insert(QLatin1String("tags"),createTagsJson(e));
    //jsonMap.insert(QLatin1String("notes","");

    if(adoptBibtexTypes()) {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstract"))));
        jsonMap.insert(QLatin1String("callNumber"),PlainTextValue::text(e->value(QLatin1String("lccn"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("copyright"))));
        jsonMap.insert(QLatin1String("seriesNumber"),PlainTextValue::text(e->value(QLatin1String("number"))));
        jsonMap.insert(QLatin1String("place"),PlainTextValue::text(e->value(QLatin1String("address"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("note"))));
        jsonMap.insert(QLatin1String("place"),PlainTextValue::text(e->value(QLatin1String("address"))));
    }
    else {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstractNote"))));
        jsonMap.insert(QLatin1String("callNumber"),PlainTextValue::text(e->value(QLatin1String("callNumber"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("rights"))));
        jsonMap.insert(QLatin1String("place"),PlainTextValue::text(e->value(QLatin1String("place"))));
        jsonMap.insert(QLatin1String("seriesNumber"),PlainTextValue::text(e->value(QLatin1String("seriesNumber"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("extra"))));
        jsonMap.insert(QLatin1String("place"),PlainTextValue::text(e->value(QLatin1String("place"))));
    }

    return jsonMap;
}

QVariantMap WriteToZotero::createBookSectionJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert(QLatin1String("itemType"),QLatin1String("bookSection"));
    jsonMap.insert(QLatin1String("creators"),createCreatorsJson(e, QLatin1String("bookSection")));
    jsonMap.insert(QLatin1String("series"),PlainTextValue::text(e->value(QLatin1String("series"))));
    jsonMap.insert(QLatin1String("volume"),PlainTextValue::text(e->value(QLatin1String("volume"))));
    jsonMap.insert(QLatin1String("numberOfVolumes"),PlainTextValue::text(e->value(QLatin1String("numberOfVolumes"))));
    jsonMap.insert(QLatin1String("edition"),PlainTextValue::text(e->value(QLatin1String("edition"))));
    jsonMap.insert(QLatin1String("publisher"),PlainTextValue::text(e->value(QLatin1String("publisher"))));
    jsonMap.insert(QLatin1String("date"),PlainTextValue::text(e->value(QLatin1String("date"))));
    jsonMap.insert(QLatin1String("pages"),PlainTextValue::text(e->value(QLatin1String("pages"))));
    jsonMap.insert(QLatin1String("language"),PlainTextValue::text(e->value(QLatin1String("language"))));
    jsonMap.insert(QLatin1String("ISBN"),PlainTextValue::text(e->value(QLatin1String("isbn"))));
    jsonMap.insert(QLatin1String("shortTitle"),PlainTextValue::text(e->value(QLatin1String("shortTitle"))));
    jsonMap.insert(QLatin1String("url"),PlainTextValue::text(e->value(QLatin1String("url"))));
    jsonMap.insert(QLatin1String("accessDate"),PlainTextValue::text(e->value(QLatin1String("accessDate"))));
    jsonMap.insert(QLatin1String("archive"),PlainTextValue::text(e->value(QLatin1String("archive"))));
    jsonMap.insert(QLatin1String("archiveLocation"),PlainTextValue::text(e->value(QLatin1String("archiveLocation"))));
    jsonMap.insert(QLatin1String("libraryCatalog"),PlainTextValue::text(e->value(QLatin1String("libraryCatalog"))));
    jsonMap.insert(QLatin1String("title"), PlainTextValue::text(e->value(QLatin1String("title"))));
    jsonMap.insert(QLatin1String("tags"),createTagsJson(e));
    //jsonMap.insert(QLatin1String("notes","");

    if(adoptBibtexTypes()) {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstract"))));
        jsonMap.insert(QLatin1String("callNumber"),PlainTextValue::text(e->value(QLatin1String("lccn"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("copyright"))));
        jsonMap.insert(QLatin1String("seriesNumber"),PlainTextValue::text(e->value(QLatin1String("number"))));
        jsonMap.insert(QLatin1String("place"),PlainTextValue::text(e->value(QLatin1String("address"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("note"))));

        if(e->value(QLatin1String("booktitle")).isEmpty()) {
            jsonMap.insert(QLatin1String("bookTitle"),PlainTextValue::text(e->value(QLatin1String("title"))));
        }
        else {
            jsonMap.insert(QLatin1String("bookTitle"),PlainTextValue::text(e->value(QLatin1String("booktitle"))));
        }
    }
    else {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstractNote"))));
        jsonMap.insert(QLatin1String("callNumber"),PlainTextValue::text(e->value(QLatin1String("callNumber"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("rights"))));
        jsonMap.insert(QLatin1String("place"),PlainTextValue::text(e->value(QLatin1String("place"))));
        jsonMap.insert(QLatin1String("seriesNumber"),PlainTextValue::text(e->value(QLatin1String("seriesNumber"))));
        jsonMap.insert(QLatin1String("bookTitle"),PlainTextValue::text(e->value(QLatin1String("bookTitle"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("extra"))));
    }

    return jsonMap;
}

QVariantMap WriteToZotero::createCaseJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert(QLatin1String("itemType"),QLatin1String("case"));
    jsonMap.insert(QLatin1String("creators"),createCreatorsJson(e, QLatin1String("case")));
    jsonMap.insert(QLatin1String("reporter"),PlainTextValue::text(e->value(QLatin1String("reporter"))));
    jsonMap.insert(QLatin1String("history"),PlainTextValue::text(e->value(QLatin1String("history"))));
    jsonMap.insert(QLatin1String("language"),PlainTextValue::text(e->value(QLatin1String("language"))));
    jsonMap.insert(QLatin1String("shortTitle"),PlainTextValue::text(e->value(QLatin1String("shortTitle"))));
    jsonMap.insert(QLatin1String("url"),PlainTextValue::text(e->value(QLatin1String("url"))));
    jsonMap.insert(QLatin1String("accessDate"),PlainTextValue::text(e->value(QLatin1String("accessDate"))));
    jsonMap.insert(QLatin1String("reporterVolume"),PlainTextValue::text(e->value(QLatin1String("reportervolume"))));
    jsonMap.insert(QLatin1String("tags"),createTagsJson(e));
    //jsonMap.insert(QLatin1String("notes","");

    if(adoptBibtexTypes()) {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstract"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("copyright"))));
        jsonMap.insert(QLatin1String("place"),PlainTextValue::text(e->value(QLatin1String("address"))));
        jsonMap.insert(QLatin1String("docketNumber"),PlainTextValue::text(e->value(QLatin1String("number"))));
        jsonMap.insert(QLatin1String("caseName"),PlainTextValue::text(e->value(QLatin1String("title"))));
        jsonMap.insert(QLatin1String("dateDecided"),PlainTextValue::text(e->value(QLatin1String("date"))));
        jsonMap.insert(QLatin1String("firstPage"),PlainTextValue::text(e->value(QLatin1String("pages"))));
        jsonMap.insert(QLatin1String("court"),PlainTextValue::text(e->value(QLatin1String("organization"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("note"))));
    }
    else {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstractNote"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("rights"))));
        jsonMap.insert(QLatin1String("place"),PlainTextValue::text(e->value(QLatin1String("place"))));
        jsonMap.insert(QLatin1String("docketNumber"),PlainTextValue::text(e->value(QLatin1String("docketNumber"))));
        jsonMap.insert(QLatin1String("caseName"),PlainTextValue::text(e->value(QLatin1String("caseName"))));
        jsonMap.insert(QLatin1String("dateDecided"),PlainTextValue::text(e->value(QLatin1String("dateDecided"))));
        jsonMap.insert(QLatin1String("firstPage"),PlainTextValue::text(e->value(QLatin1String("firstPage"))));
        jsonMap.insert(QLatin1String("court"),PlainTextValue::text(e->value(QLatin1String("court"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("extra"))));
    }

    return jsonMap;
}

QVariantMap WriteToZotero::createComputerProgramJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert(QLatin1String("itemType"),QLatin1String("computerProgram"));
    jsonMap.insert(QLatin1String("title"), PlainTextValue::text(e->value(QLatin1String("title"))));
    jsonMap.insert(QLatin1String("creators"),createCreatorsJson(e, QLatin1String("computerProgram")));
    jsonMap.insert(QLatin1String("version"),PlainTextValue::text(e->value(QLatin1String("version"))));
    jsonMap.insert(QLatin1String("date"),PlainTextValue::text(e->value(QLatin1String("date"))));
    jsonMap.insert(QLatin1String("system"),PlainTextValue::text(e->value(QLatin1String("system"))));
    jsonMap.insert(QLatin1String("company"),PlainTextValue::text(e->value(QLatin1String("company"))));
    jsonMap.insert(QLatin1String("programmingLanguage"),PlainTextValue::text(e->value(QLatin1String("programmingLanguage"))));
    jsonMap.insert(QLatin1String("ISBN"),PlainTextValue::text(e->value(QLatin1String("isbn"))));
    jsonMap.insert(QLatin1String("shortTitle"),PlainTextValue::text(e->value(QLatin1String("shortTitle"))));
    jsonMap.insert(QLatin1String("url"),PlainTextValue::text(e->value(QLatin1String("url"))));
    jsonMap.insert(QLatin1String("archive"),PlainTextValue::text(e->value(QLatin1String("archive"))));
    jsonMap.insert(QLatin1String("archiveLocation"),PlainTextValue::text(e->value(QLatin1String("archiveLocation"))));
    jsonMap.insert(QLatin1String("libraryCatalog"),PlainTextValue::text(e->value(QLatin1String("libraryCatalog"))));
    jsonMap.insert(QLatin1String("accessDate"),PlainTextValue::text(e->value(QLatin1String("accessDate"))));
    jsonMap.insert(QLatin1String("tags"),createTagsJson(e));
    //jsonMap.insert(QLatin1String("notes","");

    if(adoptBibtexTypes()) {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstract"))));
        jsonMap.insert(QLatin1String("callNumber"),PlainTextValue::text(e->value(QLatin1String("lccn"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("copyright"))));
        jsonMap.insert(QLatin1String("place"),PlainTextValue::text(e->value(QLatin1String("address"))));
        jsonMap.insert(QLatin1String("seriesTitle"),PlainTextValue::text(e->value(QLatin1String("series"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("note"))));
    }
    else {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstractNote"))));
        jsonMap.insert(QLatin1String("callNumber"),PlainTextValue::text(e->value(QLatin1String("callNumber"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("rights"))));
        jsonMap.insert(QLatin1String("seriesTitle"),PlainTextValue::text(e->value(QLatin1String("seriesTitle"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("extra"))));
    }

    return jsonMap;
}

QVariantMap WriteToZotero::createConferencePaperJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert(QLatin1String("itemType"),QLatin1String("conferencePaper"));
    jsonMap.insert(QLatin1String("title"), PlainTextValue::text(e->value(QLatin1String("title"))));
    jsonMap.insert(QLatin1String("creators"),createCreatorsJson(e, QLatin1String("conferencePaper")));
    jsonMap.insert(QLatin1String("date"),PlainTextValue::text(e->value(QLatin1String("date"))));
    jsonMap.insert(QLatin1String("publisher"),PlainTextValue::text(e->value(QLatin1String("publisher"))));
    jsonMap.insert(QLatin1String("volume"),PlainTextValue::text(e->value(QLatin1String("volume"))));
    jsonMap.insert(QLatin1String("pages"),PlainTextValue::text(e->value(QLatin1String("pages"))));
    jsonMap.insert(QLatin1String("series"),PlainTextValue::text(e->value(QLatin1String("series"))));
    jsonMap.insert(QLatin1String("language"),PlainTextValue::text(e->value(QLatin1String("language"))));
    jsonMap.insert(QLatin1String("DOI"),PlainTextValue::text(e->value(QLatin1String("doi"))));
    jsonMap.insert(QLatin1String("ISBN"),PlainTextValue::text(e->value(QLatin1String("isbn"))));
    jsonMap.insert(QLatin1String("shortTitle"),PlainTextValue::text(e->value(QLatin1String("shortTitle"))));
    jsonMap.insert(QLatin1String("url"),PlainTextValue::text(e->value(QLatin1String("url"))));
    jsonMap.insert(QLatin1String("accessDate"),PlainTextValue::text(e->value(QLatin1String("accessDate"))));
    jsonMap.insert(QLatin1String("archive"),PlainTextValue::text(e->value(QLatin1String("archive"))));
    jsonMap.insert(QLatin1String("archiveLocation"),PlainTextValue::text(e->value(QLatin1String("archiveLocation"))));
    jsonMap.insert(QLatin1String("libraryCatalog"),PlainTextValue::text(e->value(QLatin1String("libraryCatalog"))));
    jsonMap.insert(QLatin1String("tags"),createTagsJson(e));
    //jsonMap.insert(QLatin1String("notes","");

    if(adoptBibtexTypes()) {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstract"))));
        jsonMap.insert(QLatin1String("callNumber"),PlainTextValue::text(e->value(QLatin1String("lccn"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("copyright"))));
        jsonMap.insert(QLatin1String("place"),PlainTextValue::text(e->value(QLatin1String("address"))));
        jsonMap.insert(QLatin1String("proceedingsTitle"),PlainTextValue::text(e->value(QLatin1String("booktitle"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("note"))));
        jsonMap.insert(QLatin1String("conferenceName"),PlainTextValue::text(e->value(QLatin1String("event"))));
    }
    else {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstractNote"))));
        jsonMap.insert(QLatin1String("callNumber"),PlainTextValue::text(e->value(QLatin1String("callNumber"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("rights"))));
        jsonMap.insert(QLatin1String("place"),PlainTextValue::text(e->value(QLatin1String("place"))));
        jsonMap.insert(QLatin1String("proceedingsTitle"),PlainTextValue::text(e->value(QLatin1String("proceedingsTitle"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("extra"))));
        jsonMap.insert(QLatin1String("conferenceName"),PlainTextValue::text(e->value(QLatin1String("conferenceName"))));
    }

    return jsonMap;
}

QVariantMap WriteToZotero::createDictionaryEntryJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert(QLatin1String("itemType"),QLatin1String("dictionaryEntry"));
    jsonMap.insert(QLatin1String("title"), PlainTextValue::text(e->value(QLatin1String("title"))));
    jsonMap.insert(QLatin1String("creators"),createCreatorsJson(e, QLatin1String("dictionaryEntry")));
    jsonMap.insert(QLatin1String("series"),PlainTextValue::text(e->value(QLatin1String("series"))));
    jsonMap.insert(QLatin1String("volume"),PlainTextValue::text(e->value(QLatin1String("volume"))));
    jsonMap.insert(QLatin1String("numberOfVolumes"),PlainTextValue::text(e->value(QLatin1String("numberOfVolumes"))));
    jsonMap.insert(QLatin1String("edition"),PlainTextValue::text(e->value(QLatin1String("edition"))));
    jsonMap.insert(QLatin1String("publisher"),PlainTextValue::text(e->value(QLatin1String("publisher"))));
    jsonMap.insert(QLatin1String("date"),PlainTextValue::text(e->value(QLatin1String("date"))));
    jsonMap.insert(QLatin1String("pages"),PlainTextValue::text(e->value(QLatin1String("pages"))));
    jsonMap.insert(QLatin1String("language"),PlainTextValue::text(e->value(QLatin1String("language"))));
    jsonMap.insert(QLatin1String("ISBN"),PlainTextValue::text(e->value(QLatin1String("isbn"))));
    jsonMap.insert(QLatin1String("shortTitle"),PlainTextValue::text(e->value(QLatin1String("shortTitle"))));
    jsonMap.insert(QLatin1String("url"),PlainTextValue::text(e->value(QLatin1String("url"))));
    jsonMap.insert(QLatin1String("accessDate"),PlainTextValue::text(e->value(QLatin1String("accessDate"))));
    jsonMap.insert(QLatin1String("archive"),PlainTextValue::text(e->value(QLatin1String("archive"))));
    jsonMap.insert(QLatin1String("archiveLocation"),PlainTextValue::text(e->value(QLatin1String("archiveLocation"))));
    jsonMap.insert(QLatin1String("libraryCatalog"),PlainTextValue::text(e->value(QLatin1String("libraryCatalog"))));
    jsonMap.insert(QLatin1String("tags"),createTagsJson(e));
    //jsonMap.insert(QLatin1String("notes","");

    if(adoptBibtexTypes()) {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstract"))));
        jsonMap.insert(QLatin1String("callNumber"),PlainTextValue::text(e->value(QLatin1String("lccn"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("copyright"))));
        jsonMap.insert(QLatin1String("place"),PlainTextValue::text(e->value(QLatin1String("address"))));
        jsonMap.insert(QLatin1String("seriesNumber"),PlainTextValue::text(e->value(QLatin1String("number"))));
        jsonMap.insert(QLatin1String("dictionaryTitle"),PlainTextValue::text(e->value(QLatin1String("booktitle"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("note"))));
    }
    else {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstractNote"))));
        jsonMap.insert(QLatin1String("callNumber"),PlainTextValue::text(e->value(QLatin1String("callNumber"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("rights"))));
        jsonMap.insert(QLatin1String("place"),PlainTextValue::text(e->value(QLatin1String("place"))));
        jsonMap.insert(QLatin1String("seriesNumber"),PlainTextValue::text(e->value(QLatin1String("seriesNumber"))));
        jsonMap.insert(QLatin1String("dictionaryTitle"),PlainTextValue::text(e->value(QLatin1String("dictionaryTitle"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("extra"))));
    }

    return jsonMap;
}

QVariantMap WriteToZotero::createDocumentJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert(QLatin1String("itemType"),QLatin1String("document"));
    jsonMap.insert(QLatin1String("title"), PlainTextValue::text(e->value(QLatin1String("title"))));
    jsonMap.insert(QLatin1String("creators"),createCreatorsJson(e, QLatin1String("document")));
    jsonMap.insert(QLatin1String("publisher"),PlainTextValue::text(e->value(QLatin1String("publisher"))));
    jsonMap.insert(QLatin1String("date"),PlainTextValue::text(e->value(QLatin1String("date"))));
    jsonMap.insert(QLatin1String("language"),PlainTextValue::text(e->value(QLatin1String("language"))));
    jsonMap.insert(QLatin1String("shortTitle"),PlainTextValue::text(e->value(QLatin1String("shortTitle"))));
    jsonMap.insert(QLatin1String("url"),PlainTextValue::text(e->value(QLatin1String("url"))));
    jsonMap.insert(QLatin1String("accessDate"),PlainTextValue::text(e->value(QLatin1String("accessDate"))));
    jsonMap.insert(QLatin1String("archive"),PlainTextValue::text(e->value(QLatin1String("archive"))));
    jsonMap.insert(QLatin1String("archiveLocation"),PlainTextValue::text(e->value(QLatin1String("archiveLocation"))));
    jsonMap.insert(QLatin1String("libraryCatalog"),PlainTextValue::text(e->value(QLatin1String("libraryCatalog"))));
    jsonMap.insert(QLatin1String("tags"),createTagsJson(e));
    //jsonMap.insert(QLatin1String("notes","");

    if(adoptBibtexTypes()) {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstract"))));
        jsonMap.insert(QLatin1String("callNumber"),PlainTextValue::text(e->value(QLatin1String("lccn"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("copyright"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("note"))));
    }
    else {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstractNote"))));
        jsonMap.insert(QLatin1String("callNumber"),PlainTextValue::text(e->value(QLatin1String("callNumber"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("rights"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("extra"))));
    }

    return jsonMap;
}

QVariantMap WriteToZotero::createEmailJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert(QLatin1String("itemType"),QLatin1String("email"));
    jsonMap.insert(QLatin1String("creators"),createCreatorsJson(e, QLatin1String("email")));

    jsonMap.insert(QLatin1String("subject"),PlainTextValue::text(e->value(QLatin1String("subject"))));
    jsonMap.insert(QLatin1String("date"),PlainTextValue::text(e->value(QLatin1String("date"))));
    jsonMap.insert(QLatin1String("shortTitle"),PlainTextValue::text(e->value(QLatin1String("shortTitle"))));
    jsonMap.insert(QLatin1String("url"),PlainTextValue::text(e->value(QLatin1String("url"))));
    jsonMap.insert(QLatin1String("accessDate"),PlainTextValue::text(e->value(QLatin1String("accessDate"))));
    jsonMap.insert(QLatin1String("language"),PlainTextValue::text(e->value(QLatin1String("language"))));
    jsonMap.insert(QLatin1String("tags"),createTagsJson(e));
    //jsonMap.insert(QLatin1String("notes","");

    if(adoptBibtexTypes()) {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstract"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("copyright"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("note"))));
    }
    else {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstractNote"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("rights"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("extra"))));
    }

    return jsonMap;
}

QVariantMap WriteToZotero::createEncyclopediaArticleJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert(QLatin1String("itemType"),QLatin1String("encyclopediaArticle"));
    jsonMap.insert(QLatin1String("title"), PlainTextValue::text(e->value(QLatin1String("title"))));
    jsonMap.insert(QLatin1String("creators"),createCreatorsJson(e, QLatin1String("encyclopediaArticle")));
    jsonMap.insert(QLatin1String("series"),PlainTextValue::text(e->value(QLatin1String("series"))));
    jsonMap.insert(QLatin1String("volume"),PlainTextValue::text(e->value(QLatin1String("volume"))));
    jsonMap.insert(QLatin1String("numberOfVolumes"),PlainTextValue::text(e->value(QLatin1String("numberOfVolumes"))));
    jsonMap.insert(QLatin1String("edition"),PlainTextValue::text(e->value(QLatin1String("edition"))));
    jsonMap.insert(QLatin1String("publisher"),PlainTextValue::text(e->value(QLatin1String("publisher"))));
    jsonMap.insert(QLatin1String("date"),PlainTextValue::text(e->value(QLatin1String("date"))));
    jsonMap.insert(QLatin1String("pages"),PlainTextValue::text(e->value(QLatin1String("pages"))));
    jsonMap.insert(QLatin1String("ISBN"),PlainTextValue::text(e->value(QLatin1String("isbn"))));
    jsonMap.insert(QLatin1String("shortTitle"),PlainTextValue::text(e->value(QLatin1String("shortTitle"))));
    jsonMap.insert(QLatin1String("url"),PlainTextValue::text(e->value(QLatin1String("url"))));
    jsonMap.insert(QLatin1String("accessDate"),PlainTextValue::text(e->value(QLatin1String("accessDate"))));
    jsonMap.insert(QLatin1String("language"),PlainTextValue::text(e->value(QLatin1String("language"))));
    jsonMap.insert(QLatin1String("archive"),PlainTextValue::text(e->value(QLatin1String("archive"))));
    jsonMap.insert(QLatin1String("archiveLocation"),PlainTextValue::text(e->value(QLatin1String("archiveLocation"))));
    jsonMap.insert(QLatin1String("libraryCatalog"),PlainTextValue::text(e->value(QLatin1String("libraryCatalog"))));
    jsonMap.insert(QLatin1String("tags"),createTagsJson(e));
    //jsonMap.insert(QLatin1String("notes","");

    if(adoptBibtexTypes()) {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstract"))));
        jsonMap.insert(QLatin1String("callNumber"),PlainTextValue::text(e->value(QLatin1String("lccn"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("copyright"))));
        jsonMap.insert(QLatin1String("place"),PlainTextValue::text(e->value(QLatin1String("address"))));
        jsonMap.insert(QLatin1String("seriesNumber"),PlainTextValue::text(e->value(QLatin1String("number"))));
        jsonMap.insert(QLatin1String("encyclopediaTitle"),PlainTextValue::text(e->value(QLatin1String("booktitle"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("note"))));
    }
    else {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstractNote"))));
        jsonMap.insert(QLatin1String("callNumber"),PlainTextValue::text(e->value(QLatin1String("callNumber"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("rights"))));
        jsonMap.insert(QLatin1String("place"),PlainTextValue::text(e->value(QLatin1String("place"))));
        jsonMap.insert(QLatin1String("seriesNumber"),PlainTextValue::text(e->value(QLatin1String("seriesNumber"))));
        jsonMap.insert(QLatin1String("encyclopediaTitle"),PlainTextValue::text(e->value(QLatin1String("encyclopediaTitle"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("extra"))));
    }

    return jsonMap;
}

QVariantMap WriteToZotero::createFilmJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert(QLatin1String("itemType"),QLatin1String("film"));
    jsonMap.insert(QLatin1String("title"), PlainTextValue::text(e->value(QLatin1String("title"))));
    jsonMap.insert(QLatin1String("creators"),createCreatorsJson(e, QLatin1String("film")));
    jsonMap.insert(QLatin1String("date"),PlainTextValue::text(e->value(QLatin1String("date"))));
    jsonMap.insert(QLatin1String("genre"),PlainTextValue::text(e->value(QLatin1String("genre"))));
    jsonMap.insert(QLatin1String("videoRecordingFormat"),PlainTextValue::text(e->value(QLatin1String("videoRecordingFormat"))));
    jsonMap.insert(QLatin1String("runningTime"),PlainTextValue::text(e->value(QLatin1String("runningTime"))));
    jsonMap.insert(QLatin1String("language"),PlainTextValue::text(e->value(QLatin1String("language"))));
    jsonMap.insert(QLatin1String("shortTitle"),PlainTextValue::text(e->value(QLatin1String("shortTitle"))));
    jsonMap.insert(QLatin1String("url"),PlainTextValue::text(e->value(QLatin1String("url"))));
    jsonMap.insert(QLatin1String("accessDate"),PlainTextValue::text(e->value(QLatin1String("accessDate"))));
    jsonMap.insert(QLatin1String("archive"),PlainTextValue::text(e->value(QLatin1String("archive"))));
    jsonMap.insert(QLatin1String("archiveLocation"),PlainTextValue::text(e->value(QLatin1String("archiveLocation"))));
    jsonMap.insert(QLatin1String("libraryCatalog"),PlainTextValue::text(e->value(QLatin1String("libraryCatalog"))));
    jsonMap.insert(QLatin1String("tags"),createTagsJson(e));
    //jsonMap.insert(QLatin1String("notes","");

    if(adoptBibtexTypes()) {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstract"))));
        jsonMap.insert(QLatin1String("callNumber"),PlainTextValue::text(e->value(QLatin1String("lccn"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("copyright"))));
        jsonMap.insert(QLatin1String("place"),PlainTextValue::text(e->value(QLatin1String("address"))));
        jsonMap.insert(QLatin1String("distributor"),PlainTextValue::text(e->value(QLatin1String("publisher"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("note"))));
    }
    else {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstractNote"))));
        jsonMap.insert(QLatin1String("callNumber"),PlainTextValue::text(e->value(QLatin1String("callNumber"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("rights"))));
        jsonMap.insert(QLatin1String("place"),PlainTextValue::text(e->value(QLatin1String("place"))));
        jsonMap.insert(QLatin1String("distributor"),PlainTextValue::text(e->value(QLatin1String("distributor"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("extra"))));
    }

    return jsonMap;
}

QVariantMap WriteToZotero::createForumPostJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert(QLatin1String("itemType"),QLatin1String("forumPost"));
    jsonMap.insert(QLatin1String("title"), PlainTextValue::text(e->value(QLatin1String("title"))));
    jsonMap.insert(QLatin1String("creators"),createCreatorsJson(e, QLatin1String("forumPost")));
    jsonMap.insert(QLatin1String("date"),PlainTextValue::text(e->value(QLatin1String("date"))));
    jsonMap.insert(QLatin1String("language"),PlainTextValue::text(e->value(QLatin1String("language"))));
    jsonMap.insert(QLatin1String("shortTitle"),PlainTextValue::text(e->value(QLatin1String("shortTitle"))));
    jsonMap.insert(QLatin1String("url"),PlainTextValue::text(e->value(QLatin1String("url"))));
    jsonMap.insert(QLatin1String("accessDate"),PlainTextValue::text(e->value(QLatin1String("accessDate"))));
    jsonMap.insert(QLatin1String("tags"),createTagsJson(e));
    //jsonMap.insert(QLatin1String("notes","");

    if(adoptBibtexTypes()) {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstract"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("copyright"))));
        jsonMap.insert(QLatin1String("forumTitle"),PlainTextValue::text(e->value(QLatin1String("booktitle"))));
        jsonMap.insert(QLatin1String("postType"),PlainTextValue::text(e->value(QLatin1String("type"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("note"))));
    }
    else {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstractNote"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("rights"))));
        jsonMap.insert(QLatin1String("forumTitle"),PlainTextValue::text(e->value(QLatin1String("forumTitle"))));
        jsonMap.insert(QLatin1String("postType"),PlainTextValue::text(e->value(QLatin1String("postType"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("extra"))));
    }

    return jsonMap;
}

QVariantMap WriteToZotero::createHearingJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert(QLatin1String("itemType"),QLatin1String("hearing"));
    jsonMap.insert(QLatin1String("title"), PlainTextValue::text(e->value(QLatin1String("title"))));
    jsonMap.insert(QLatin1String("creators"),createCreatorsJson(e, QLatin1String("hearing")));
    jsonMap.insert(QLatin1String("committee"),PlainTextValue::text(e->value(QLatin1String("committee"))));
    jsonMap.insert(QLatin1String("publisher"),PlainTextValue::text(e->value(QLatin1String("publisher"))));
    jsonMap.insert(QLatin1String("numberOfVolumes"),PlainTextValue::text(e->value(QLatin1String("numberOfVolumes"))));
    jsonMap.insert(QLatin1String("pages"),PlainTextValue::text(e->value(QLatin1String("pages"))));
    jsonMap.insert(QLatin1String("legislativeBody"),PlainTextValue::text(e->value(QLatin1String("legislativeBody"))));
    jsonMap.insert(QLatin1String("session"),PlainTextValue::text(e->value(QLatin1String("session"))));
    jsonMap.insert(QLatin1String("history"),PlainTextValue::text(e->value(QLatin1String("history"))));
    jsonMap.insert(QLatin1String("date"),PlainTextValue::text(e->value(QLatin1String("date"))));
    jsonMap.insert(QLatin1String("language"),PlainTextValue::text(e->value(QLatin1String("language"))));
    jsonMap.insert(QLatin1String("shortTitle"),PlainTextValue::text(e->value(QLatin1String("shortTitle"))));
    jsonMap.insert(QLatin1String("url"),PlainTextValue::text(e->value(QLatin1String("url"))));
    jsonMap.insert(QLatin1String("accessDate"),PlainTextValue::text(e->value(QLatin1String("accessDate"))));
    jsonMap.insert(QLatin1String("tags"),createTagsJson(e));
    //jsonMap.insert(QLatin1String("notes","");

    if(adoptBibtexTypes()) {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstract"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("copyright"))));
        jsonMap.insert(QLatin1String("place"),PlainTextValue::text(e->value(QLatin1String("address"))));
        jsonMap.insert(QLatin1String("documentNumber"),PlainTextValue::text(e->value(QLatin1String("number"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("note"))));
    }
    else {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstractNote"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("rights"))));
        jsonMap.insert(QLatin1String("place"),PlainTextValue::text(e->value(QLatin1String("place"))));
        jsonMap.insert(QLatin1String("documentNumber"),PlainTextValue::text(e->value(QLatin1String("documentNumber"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("extra"))));
    }

    return jsonMap;
}

QVariantMap WriteToZotero::createInstantMessageJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert(QLatin1String("itemType"),QLatin1String("instantMessage"));
    jsonMap.insert(QLatin1String("title"), PlainTextValue::text(e->value(QLatin1String("title"))));
    jsonMap.insert(QLatin1String("creators"),createCreatorsJson(e, QLatin1String("instantMessage")));
    jsonMap.insert(QLatin1String("date"),PlainTextValue::text(e->value(QLatin1String("date"))));
    jsonMap.insert(QLatin1String("language"),PlainTextValue::text(e->value(QLatin1String("language"))));
    jsonMap.insert(QLatin1String("shortTitle"),PlainTextValue::text(e->value(QLatin1String("shortTitle"))));
    jsonMap.insert(QLatin1String("url"),PlainTextValue::text(e->value(QLatin1String("url"))));
    jsonMap.insert(QLatin1String("accessDate"),PlainTextValue::text(e->value(QLatin1String("accessDate"))));
    jsonMap.insert(QLatin1String("tags"),createTagsJson(e));
    //jsonMap.insert(QLatin1String("notes","");

    if(adoptBibtexTypes()) {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstract"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("copyright"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("note"))));
    }
    else {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstractNote"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("rights"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("extra"))));
    }

    return jsonMap;
}

QVariantMap WriteToZotero::createInterviewJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert(QLatin1String("itemType"),QLatin1String("interview"));
    jsonMap.insert(QLatin1String("title"), PlainTextValue::text(e->value(QLatin1String("title"))));
    jsonMap.insert(QLatin1String("creators"),createCreatorsJson(e, QLatin1String("interview")));
    jsonMap.insert(QLatin1String("date"),PlainTextValue::text(e->value(QLatin1String("date"))));
    jsonMap.insert(QLatin1String("interviewMedium"),PlainTextValue::text(e->value(QLatin1String("interviewMedium"))));
    jsonMap.insert(QLatin1String("language"),PlainTextValue::text(e->value(QLatin1String("language"))));
    jsonMap.insert(QLatin1String("shortTitle"),PlainTextValue::text(e->value(QLatin1String("shortTitle"))));
    jsonMap.insert(QLatin1String("url"),PlainTextValue::text(e->value(QLatin1String("url"))));
    jsonMap.insert(QLatin1String("accessDate"),PlainTextValue::text(e->value(QLatin1String("accessDate"))));
    jsonMap.insert(QLatin1String("archive"),PlainTextValue::text(e->value(QLatin1String("archive"))));
    jsonMap.insert(QLatin1String("archiveLocation"),PlainTextValue::text(e->value(QLatin1String("archiveLocation"))));
    jsonMap.insert(QLatin1String("libraryCatalog"),PlainTextValue::text(e->value(QLatin1String("libraryCatalog"))));
    jsonMap.insert(QLatin1String("tags"),createTagsJson(e));
    //jsonMap.insert(QLatin1String("notes","");

    if(adoptBibtexTypes()) {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstract"))));
        jsonMap.insert(QLatin1String("callNumber"),PlainTextValue::text(e->value(QLatin1String("lccn"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("copyright"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("note"))));
    }
    else {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstractNote"))));
        jsonMap.insert(QLatin1String("callNumber"),PlainTextValue::text(e->value(QLatin1String("callNumber"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("rights"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("extra"))));
    }

    return jsonMap;
}

QVariantMap WriteToZotero::createJournalArticleJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert(QLatin1String("itemType"),QLatin1String("journalArticle"));
    jsonMap.insert(QLatin1String("title"), PlainTextValue::text(e->value(QLatin1String("title"))));
    jsonMap.insert(QLatin1String("creators"),createCreatorsJson(e, QLatin1String("journalArticle")));
    jsonMap.insert(QLatin1String("volume"),PlainTextValue::text(e->value(QLatin1String("volume"))));
    jsonMap.insert(QLatin1String("pages"),PlainTextValue::text(e->value(QLatin1String("pages"))));
    jsonMap.insert(QLatin1String("date"),PlainTextValue::text(e->value(QLatin1String("date"))));
    jsonMap.insert(QLatin1String("series"),PlainTextValue::text(e->value(QLatin1String("series"))));
    jsonMap.insert(QLatin1String("seriesText"),PlainTextValue::text(e->value(QLatin1String("seriesText"))));
    jsonMap.insert(QLatin1String("journalAbbreviation"),PlainTextValue::text(e->value(QLatin1String("journalAbbreviation"))));
    jsonMap.insert(QLatin1String("language"),PlainTextValue::text(e->value(QLatin1String("language"))));
    jsonMap.insert(QLatin1String("DOI"),PlainTextValue::text(e->value(QLatin1String("doi"))));
    jsonMap.insert(QLatin1String("ISSN"),PlainTextValue::text(e->value(QLatin1String("issn"))));
    jsonMap.insert(QLatin1String("shortTitle"),PlainTextValue::text(e->value(QLatin1String("shortTitle"))));
    jsonMap.insert(QLatin1String("url"),PlainTextValue::text(e->value(QLatin1String("url"))));
    jsonMap.insert(QLatin1String("accessDate"),PlainTextValue::text(e->value(QLatin1String("accessDate"))));
    jsonMap.insert(QLatin1String("archive"),PlainTextValue::text(e->value(QLatin1String("archive"))));
    jsonMap.insert(QLatin1String("archiveLocation"),PlainTextValue::text(e->value(QLatin1String("archiveLocation"))));
    jsonMap.insert(QLatin1String("libraryCatalog"),PlainTextValue::text(e->value(QLatin1String("libraryCatalog"))));
    jsonMap.insert(QLatin1String("tags"),createTagsJson(e));
    //jsonMap.insert(QLatin1String("notes","");

    if(adoptBibtexTypes()) {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstract"))));
        jsonMap.insert(QLatin1String("callNumber"),PlainTextValue::text(e->value(QLatin1String("lccn"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("copyright"))));
        jsonMap.insert(QLatin1String("publicationTitle"),PlainTextValue::text(e->value(QLatin1String("journal"))));
        jsonMap.insert(QLatin1String("seriesTitle"),PlainTextValue::text(e->value(QLatin1String("series"))));
        jsonMap.insert(QLatin1String("issue"),PlainTextValue::text(e->value(QLatin1String("number"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("note"))));
    }
    else {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstractNote"))));
        jsonMap.insert(QLatin1String("callNumber"),PlainTextValue::text(e->value(QLatin1String("callNumber"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("rights"))));
        jsonMap.insert(QLatin1String("publicationTitle"),PlainTextValue::text(e->value(QLatin1String("publicationTitle"))));
        jsonMap.insert(QLatin1String("seriesTitle"),PlainTextValue::text(e->value(QLatin1String("seriesTitle"))));
        jsonMap.insert(QLatin1String("issue"),PlainTextValue::text(e->value(QLatin1String("issue"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("extra"))));
    }

    return jsonMap;
}

QVariantMap WriteToZotero::createLetterJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert(QLatin1String("itemType"),QLatin1String("letter"));
    jsonMap.insert(QLatin1String("title"), PlainTextValue::text(e->value(QLatin1String("title"))));
    jsonMap.insert(QLatin1String("creators"),createCreatorsJson(e,QLatin1String("letter")));
    jsonMap.insert(QLatin1String("date"),PlainTextValue::text(e->value(QLatin1String("date"))));
    jsonMap.insert(QLatin1String("language"),PlainTextValue::text(e->value(QLatin1String("language"))));
    jsonMap.insert(QLatin1String("shortTitle"),PlainTextValue::text(e->value(QLatin1String("shortTitle"))));
    jsonMap.insert(QLatin1String("url"),PlainTextValue::text(e->value(QLatin1String("url"))));
    jsonMap.insert(QLatin1String("accessDate"),PlainTextValue::text(e->value(QLatin1String("accessDate"))));
    jsonMap.insert(QLatin1String("archive"),PlainTextValue::text(e->value(QLatin1String("archive"))));
    jsonMap.insert(QLatin1String("archiveLocation"),PlainTextValue::text(e->value(QLatin1String("archiveLocation"))));
    jsonMap.insert(QLatin1String("libraryCatalog"),PlainTextValue::text(e->value(QLatin1String("libraryCatalog"))));
    jsonMap.insert(QLatin1String("tags"),createTagsJson(e));
    //jsonMap.insert(QLatin1String("notes","");

    if(adoptBibtexTypes()) {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstract"))));
        jsonMap.insert(QLatin1String("callNumber"),PlainTextValue::text(e->value(QLatin1String("lccn"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("copyright"))));
        jsonMap.insert(QLatin1String("letterType"),PlainTextValue::text(e->value(QLatin1String("type"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("note"))));
    }
    else {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstractNote"))));
        jsonMap.insert(QLatin1String("callNumber"),PlainTextValue::text(e->value(QLatin1String("callNumber"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("rights"))));
        jsonMap.insert(QLatin1String("letterType"),PlainTextValue::text(e->value(QLatin1String("letterType"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("extra"))));
    }

    return jsonMap;
}

QVariantMap WriteToZotero::createMagazineArticleJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert(QLatin1String("itemType"),QLatin1String("magazineArticle"));
    jsonMap.insert(QLatin1String("title"), PlainTextValue::text(e->value(QLatin1String("title"))));
    jsonMap.insert(QLatin1String("creators"),createCreatorsJson(e, QLatin1String("magazineArticle")));
    jsonMap.insert(QLatin1String("volume"),PlainTextValue::text(e->value(QLatin1String("volume"))));
    jsonMap.insert(QLatin1String("pages"),PlainTextValue::text(e->value(QLatin1String("pages"))));
    jsonMap.insert(QLatin1String("date"),PlainTextValue::text(e->value(QLatin1String("date"))));
    jsonMap.insert(QLatin1String("series"),PlainTextValue::text(e->value(QLatin1String("series"))));
    jsonMap.insert(QLatin1String("seriesText"),PlainTextValue::text(e->value(QLatin1String("seriesText"))));
    jsonMap.insert(QLatin1String("journalAbbreviation"),PlainTextValue::text(e->value(QLatin1String("journalAbbreviation"))));
    jsonMap.insert(QLatin1String("language"),PlainTextValue::text(e->value(QLatin1String("language"))));
    jsonMap.insert(QLatin1String("DOI"),PlainTextValue::text(e->value(QLatin1String("doi"))));
    jsonMap.insert(QLatin1String("ISSN"),PlainTextValue::text(e->value(QLatin1String("issn"))));
    jsonMap.insert(QLatin1String("shortTitle"),PlainTextValue::text(e->value(QLatin1String("shortTitle"))));
    jsonMap.insert(QLatin1String("url"),PlainTextValue::text(e->value(QLatin1String("url"))));
    jsonMap.insert(QLatin1String("accessDate"),PlainTextValue::text(e->value(QLatin1String("accessDate"))));
    jsonMap.insert(QLatin1String("archive"),PlainTextValue::text(e->value(QLatin1String("archive"))));
    jsonMap.insert(QLatin1String("archiveLocation"),PlainTextValue::text(e->value(QLatin1String("archiveLocation"))));
    jsonMap.insert(QLatin1String("libraryCatalog"),PlainTextValue::text(e->value(QLatin1String("libraryCatalog"))));
    jsonMap.insert(QLatin1String("tags"),createTagsJson(e));
    //jsonMap.insert(QLatin1String("notes","");

    if(adoptBibtexTypes()) {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstract"))));
        jsonMap.insert(QLatin1String("callNumber"),PlainTextValue::text(e->value(QLatin1String("lccn"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("copyright"))));
        jsonMap.insert(QLatin1String("publicationTitle"),PlainTextValue::text(e->value(QLatin1String("journal"))));
        jsonMap.insert(QLatin1String("seriesTitle"),PlainTextValue::text(e->value(QLatin1String("series"))));
        jsonMap.insert(QLatin1String("issue"),PlainTextValue::text(e->value(QLatin1String("number"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("note"))));
    }
    else {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstractNote"))));
        jsonMap.insert(QLatin1String("callNumber"),PlainTextValue::text(e->value(QLatin1String("callNumber"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("rights"))));
        jsonMap.insert(QLatin1String("publicationTitle"),PlainTextValue::text(e->value(QLatin1String("publicationTitle"))));
        jsonMap.insert(QLatin1String("seriesTitle"),PlainTextValue::text(e->value(QLatin1String("seriesTitle"))));
        jsonMap.insert(QLatin1String("issue"),PlainTextValue::text(e->value(QLatin1String("issue"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("extra"))));
    }

    return jsonMap;
}

QVariantMap WriteToZotero::createManuscriptJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert(QLatin1String("itemType"),QLatin1String("manuscript"));
    jsonMap.insert(QLatin1String("title"), PlainTextValue::text(e->value(QLatin1String("title"))));
    jsonMap.insert(QLatin1String("creators"),createCreatorsJson(e, QLatin1String("manuscript")));
    jsonMap.insert(QLatin1String("date"),PlainTextValue::text(e->value(QLatin1String("date"))));
    jsonMap.insert(QLatin1String("numPages"),PlainTextValue::text(e->value(QLatin1String("numPages"))));
    jsonMap.insert(QLatin1String("language"),PlainTextValue::text(e->value(QLatin1String("language"))));
    jsonMap.insert(QLatin1String("shortTitle"),PlainTextValue::text(e->value(QLatin1String("shortTitle"))));
    jsonMap.insert(QLatin1String("url"),PlainTextValue::text(e->value(QLatin1String("url"))));
    jsonMap.insert(QLatin1String("accessDate"),PlainTextValue::text(e->value(QLatin1String("accessDate"))));
    jsonMap.insert(QLatin1String("archive"),PlainTextValue::text(e->value(QLatin1String("archive"))));
    jsonMap.insert(QLatin1String("archiveLocation"),PlainTextValue::text(e->value(QLatin1String("archiveLocation"))));
    jsonMap.insert(QLatin1String("libraryCatalog"),PlainTextValue::text(e->value(QLatin1String("libraryCatalog"))));
    jsonMap.insert(QLatin1String("tags"),createTagsJson(e));
    //jsonMap.insert(QLatin1String("notes","");

    if(adoptBibtexTypes()) {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstract"))));
        jsonMap.insert(QLatin1String("callNumber"),PlainTextValue::text(e->value(QLatin1String("lccn"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("copyright"))));
        jsonMap.insert(QLatin1String("place"),PlainTextValue::text(e->value(QLatin1String("address"))));
        jsonMap.insert(QLatin1String("manuscriptType"),PlainTextValue::text(e->value(QLatin1String("type"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("note"))));
    }
    else {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstractNote"))));
        jsonMap.insert(QLatin1String("callNumber"),PlainTextValue::text(e->value(QLatin1String("callNumber"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("rights"))));
        jsonMap.insert(QLatin1String("place"),PlainTextValue::text(e->value(QLatin1String("place"))));
        jsonMap.insert(QLatin1String("manuscriptType"),PlainTextValue::text(e->value(QLatin1String("manuscriptType"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("extra"))));
    }

    return jsonMap;
}

QVariantMap WriteToZotero::createMapJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert(QLatin1String("itemType"),QLatin1String("map"));
    jsonMap.insert(QLatin1String("title"), PlainTextValue::text(e->value(QLatin1String("title"))));
    jsonMap.insert(QLatin1String("creators"),createCreatorsJson(e, QLatin1String("map")));
    jsonMap.insert(QLatin1String("scale"),PlainTextValue::text(e->value(QLatin1String("scale"))));
    jsonMap.insert(QLatin1String("edition"),PlainTextValue::text(e->value(QLatin1String("edition"))));
    jsonMap.insert(QLatin1String("publisher"),PlainTextValue::text(e->value(QLatin1String("publisher"))));
    jsonMap.insert(QLatin1String("language"),PlainTextValue::text(e->value(QLatin1String("language"))));
    jsonMap.insert(QLatin1String("ISBN"),PlainTextValue::text(e->value(QLatin1String("isbn"))));
    jsonMap.insert(QLatin1String("shortTitle"),PlainTextValue::text(e->value(QLatin1String("shortTitle"))));
    jsonMap.insert(QLatin1String("url"),PlainTextValue::text(e->value(QLatin1String("url"))));
    jsonMap.insert(QLatin1String("accessDate"),PlainTextValue::text(e->value(QLatin1String("accessDate"))));
    jsonMap.insert(QLatin1String("date"),PlainTextValue::text(e->value(QLatin1String("date"))));
    jsonMap.insert(QLatin1String("archive"),PlainTextValue::text(e->value(QLatin1String("archive"))));
    jsonMap.insert(QLatin1String("archiveLocation"),PlainTextValue::text(e->value(QLatin1String("archiveLocation"))));
    jsonMap.insert(QLatin1String("libraryCatalog"),PlainTextValue::text(e->value(QLatin1String("libraryCatalog"))));
    jsonMap.insert(QLatin1String("tags"),createTagsJson(e));
    //jsonMap.insert(QLatin1String("notes","");

    if(adoptBibtexTypes()) {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstract"))));
        jsonMap.insert(QLatin1String("callNumber"),PlainTextValue::text(e->value(QLatin1String("lccn"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("copyright"))));
        jsonMap.insert(QLatin1String("place"),PlainTextValue::text(e->value(QLatin1String("address"))));
        jsonMap.insert(QLatin1String("seriesTitle"),PlainTextValue::text(e->value(QLatin1String("series"))));
        jsonMap.insert(QLatin1String("mapType"),PlainTextValue::text(e->value(QLatin1String("type"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("note"))));
    }
    else {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstractNote"))));
        jsonMap.insert(QLatin1String("callNumber"),PlainTextValue::text(e->value(QLatin1String("callNumber"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("rights"))));
        jsonMap.insert(QLatin1String("place"),PlainTextValue::text(e->value(QLatin1String("place"))));
        jsonMap.insert(QLatin1String("seriesTitle"),PlainTextValue::text(e->value(QLatin1String("seriesTitle"))));
        jsonMap.insert(QLatin1String("mapType"),PlainTextValue::text(e->value(QLatin1String("mapType"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("extra"))));
    }

    return jsonMap;
}

QVariantMap WriteToZotero::createNewspaperArticleJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert(QLatin1String("itemType"),QLatin1String("newspaperArticle"));
    jsonMap.insert(QLatin1String("title"), PlainTextValue::text(e->value(QLatin1String("title"))));
    jsonMap.insert(QLatin1String("creators"),createCreatorsJson(e, QLatin1String("newspaperArticle")));
    jsonMap.insert(QLatin1String("volume"),PlainTextValue::text(e->value(QLatin1String("volume"))));
    jsonMap.insert(QLatin1String("pages"),PlainTextValue::text(e->value(QLatin1String("pages"))));
    jsonMap.insert(QLatin1String("date"),PlainTextValue::text(e->value(QLatin1String("date"))));
    jsonMap.insert(QLatin1String("series"),PlainTextValue::text(e->value(QLatin1String("series"))));
    jsonMap.insert(QLatin1String("seriesText"),PlainTextValue::text(e->value(QLatin1String("seriesText"))));
    jsonMap.insert(QLatin1String("edition"),PlainTextValue::text(e->value(QLatin1String("edition"))));
    jsonMap.insert(QLatin1String("journalAbbreviation"),PlainTextValue::text(e->value(QLatin1String("journalAbbreviation"))));
    jsonMap.insert(QLatin1String("language"),PlainTextValue::text(e->value(QLatin1String("language"))));
    jsonMap.insert(QLatin1String("DOI"),PlainTextValue::text(e->value(QLatin1String("doi"))));
    jsonMap.insert(QLatin1String("ISSN"),PlainTextValue::text(e->value(QLatin1String("issn"))));
    jsonMap.insert(QLatin1String("shortTitle"),PlainTextValue::text(e->value(QLatin1String("shortTitle"))));
    jsonMap.insert(QLatin1String("url"),PlainTextValue::text(e->value(QLatin1String("url"))));
    jsonMap.insert(QLatin1String("accessDate"),PlainTextValue::text(e->value(QLatin1String("accessDate"))));
    jsonMap.insert(QLatin1String("archive"),PlainTextValue::text(e->value(QLatin1String("archive"))));
    jsonMap.insert(QLatin1String("archiveLocation"),PlainTextValue::text(e->value(QLatin1String("archiveLocation"))));
    jsonMap.insert(QLatin1String("libraryCatalog"),PlainTextValue::text(e->value(QLatin1String("libraryCatalog"))));
    jsonMap.insert(QLatin1String("tags"),createTagsJson(e));
    //jsonMap.insert(QLatin1String("notes","");

    if(adoptBibtexTypes()) {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstract"))));
        jsonMap.insert(QLatin1String("callNumber"),PlainTextValue::text(e->value(QLatin1String("lccn"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("copyright"))));
        jsonMap.insert(QLatin1String("publicationTitle"),PlainTextValue::text(e->value(QLatin1String("journal"))));
        jsonMap.insert(QLatin1String("seriesTitle"),PlainTextValue::text(e->value(QLatin1String("series"))));
        jsonMap.insert(QLatin1String("issue"),PlainTextValue::text(e->value(QLatin1String("number"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("note"))));
        jsonMap.insert(QLatin1String("section"),PlainTextValue::text(e->value(QLatin1String("chapter"))));
        jsonMap.insert(QLatin1String("place"),PlainTextValue::text(e->value(QLatin1String("address"))));
    }
    else {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstractNote"))));
        jsonMap.insert(QLatin1String("callNumber"),PlainTextValue::text(e->value(QLatin1String("callNumber"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("rights"))));
        jsonMap.insert(QLatin1String("publicationTitle"),PlainTextValue::text(e->value(QLatin1String("publicationTitle"))));
        jsonMap.insert(QLatin1String("seriesTitle"),PlainTextValue::text(e->value(QLatin1String("seriesTitle"))));
        jsonMap.insert(QLatin1String("issue"),PlainTextValue::text(e->value(QLatin1String("issue"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("extra"))));
        jsonMap.insert(QLatin1String("section"),PlainTextValue::text(e->value(QLatin1String("section"))));
        jsonMap.insert(QLatin1String("place"),PlainTextValue::text(e->value(QLatin1String("place"))));
    }

    return jsonMap;
}

QVariantMap WriteToZotero::createNoteJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert(QLatin1String("itemType"),QLatin1String("note"));

    jsonMap.insert(QLatin1String("note"), PlainTextValue::text(e->value(QLatin1String("note"))));
    jsonMap.insert(QLatin1String("creators"),createCreatorsJson(e, QLatin1String("newspaperArticle")));
    jsonMap.insert(QLatin1String("tags"),createTagsJson(e));

    return jsonMap;
}

QVariantMap WriteToZotero::createAttachmentJson(Entry *e)
{
    qWarning() << "attachment upload is currently not supported by the normal zotero write api :(";
    QVariantMap jsonMap;

    jsonMap.insert(QLatin1String("itemType"),QLatin1String("attachment"));
//    jsonMap.insert(QLatin1String("note"),QLatin1String("text"));
    jsonMap.insert(QLatin1String("tags"),createTagsJson(e));

    return jsonMap;
}

QVariantMap WriteToZotero::createPatentJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert(QLatin1String("itemType"),QLatin1String("patent"));
    jsonMap.insert(QLatin1String("title"), PlainTextValue::text(e->value(QLatin1String("title"))));
    jsonMap.insert(QLatin1String("creators"),createCreatorsJson(e, QLatin1String("patent")));
    jsonMap.insert(QLatin1String("country"),PlainTextValue::text(e->value(QLatin1String("country"))));
    jsonMap.insert(QLatin1String("assignee"),PlainTextValue::text(e->value(QLatin1String("assignee"))));
    jsonMap.insert(QLatin1String("filingDate"),PlainTextValue::text(e->value(QLatin1String("filingDate"))));
    jsonMap.insert(QLatin1String("pages"),PlainTextValue::text(e->value(QLatin1String("pages"))));
    jsonMap.insert(QLatin1String("applicationNumber"),PlainTextValue::text(e->value(QLatin1String("applicationNumber"))));
    jsonMap.insert(QLatin1String("priorityNumbers"),PlainTextValue::text(e->value(QLatin1String("priorityNumbers"))));
    jsonMap.insert(QLatin1String("references"),PlainTextValue::text(e->value(QLatin1String("references"))));
    jsonMap.insert(QLatin1String("legalStatus"),PlainTextValue::text(e->value(QLatin1String("legalStatus"))));
    jsonMap.insert(QLatin1String("language"),PlainTextValue::text(e->value(QLatin1String("language"))));
    jsonMap.insert(QLatin1String("shortTitle"),PlainTextValue::text(e->value(QLatin1String("shortTitle"))));
    jsonMap.insert(QLatin1String("url"),PlainTextValue::text(e->value(QLatin1String("url"))));
    jsonMap.insert(QLatin1String("accessDate"),PlainTextValue::text(e->value(QLatin1String("accessDate"))));
    jsonMap.insert(QLatin1String("tags"),createTagsJson(e));
    //jsonMap.insert(QLatin1String("notes","");

    if(adoptBibtexTypes()) {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstract"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("copyright"))));
        jsonMap.insert(QLatin1String("place"),PlainTextValue::text(e->value(QLatin1String("address"))));
        jsonMap.insert(QLatin1String("patentNumber"),PlainTextValue::text(e->value(QLatin1String("number"))));
        jsonMap.insert(QLatin1String("issueDate"),PlainTextValue::text(e->value(QLatin1String("date"))));
        jsonMap.insert(QLatin1String("issuingAuthority"),PlainTextValue::text(e->value(QLatin1String("organization"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("note"))));
    }
    else {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstractNote"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("rights"))));
        jsonMap.insert(QLatin1String("place"),PlainTextValue::text(e->value(QLatin1String("place"))));
        jsonMap.insert(QLatin1String("patentNumber"),PlainTextValue::text(e->value(QLatin1String("patentNumber"))));
        jsonMap.insert(QLatin1String("issueDate"),PlainTextValue::text(e->value(QLatin1String("issueDate"))));
        jsonMap.insert(QLatin1String("issuingAuthority"),PlainTextValue::text(e->value(QLatin1String("issuingAuthority"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("extra"))));
    }

    return jsonMap;
}

QVariantMap WriteToZotero::createPodcastJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert(QLatin1String("itemType"),QLatin1String("podcast"));
    jsonMap.insert(QLatin1String("title"), PlainTextValue::text(e->value(QLatin1String("title"))));
    jsonMap.insert(QLatin1String("creators"),createCreatorsJson(e, QLatin1String("podcast")));
    jsonMap.insert(QLatin1String("audioFileType"),PlainTextValue::text(e->value(QLatin1String("audioFileType"))));
    jsonMap.insert(QLatin1String("runningTime"),PlainTextValue::text(e->value(QLatin1String("runningTime"))));
    jsonMap.insert(QLatin1String("url"),PlainTextValue::text(e->value(QLatin1String("url"))));
    jsonMap.insert(QLatin1String("accessDate"),PlainTextValue::text(e->value(QLatin1String("accessDate"))));
    jsonMap.insert(QLatin1String("language"),PlainTextValue::text(e->value(QLatin1String("language"))));
    jsonMap.insert(QLatin1String("shortTitle"),PlainTextValue::text(e->value(QLatin1String("shortTitle"))));
    jsonMap.insert(QLatin1String("tags"),createTagsJson(e));
    //jsonMap.insert(QLatin1String("notes","");

    if(adoptBibtexTypes()) {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstract"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("copyright"))));
        jsonMap.insert(QLatin1String("seriesTitle"),PlainTextValue::text(e->value(QLatin1String("series"))));
        jsonMap.insert(QLatin1String("episodeNumber"),PlainTextValue::text(e->value(QLatin1String("number"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("note"))));
    }
    else {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstractNote"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("rights"))));
        jsonMap.insert(QLatin1String("seriesTitle"),PlainTextValue::text(e->value(QLatin1String("series"))));
        jsonMap.insert(QLatin1String("episodeNumber"),PlainTextValue::text(e->value(QLatin1String("episodeNumber"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("extra"))));
    }

    return jsonMap;
}

QVariantMap WriteToZotero::createPresentationJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert(QLatin1String("itemType"),QLatin1String("presentation"));
    jsonMap.insert(QLatin1String("title"), PlainTextValue::text(e->value(QLatin1String("title"))));
    jsonMap.insert(QLatin1String("creators"),createCreatorsJson(e, QLatin1String("presentation")));
    jsonMap.insert(QLatin1String("date"),PlainTextValue::text(e->value(QLatin1String("date"))));
    jsonMap.insert(QLatin1String("url"),PlainTextValue::text(e->value(QLatin1String("url"))));
    jsonMap.insert(QLatin1String("accessDate"),PlainTextValue::text(e->value(QLatin1String("accessDate"))));
    jsonMap.insert(QLatin1String("language"),PlainTextValue::text(e->value(QLatin1String("language"))));
    jsonMap.insert(QLatin1String("shortTitle"),PlainTextValue::text(e->value(QLatin1String("shortTitle"))));
    jsonMap.insert(QLatin1String("tags"),createTagsJson(e));
    //jsonMap.insert(QLatin1String("notes","");

    if(adoptBibtexTypes()) {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstract"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("copyright"))));
        jsonMap.insert(QLatin1String("place"),PlainTextValue::text(e->value(QLatin1String("address"))));
        jsonMap.insert(QLatin1String("presentationType"),PlainTextValue::text(e->value(QLatin1String("type"))));
        jsonMap.insert(QLatin1String("meetingName"),PlainTextValue::text(e->value(QLatin1String("event"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("note"))));
    }
    else {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstractNote"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("rights"))));
        jsonMap.insert(QLatin1String("place"),PlainTextValue::text(e->value(QLatin1String("place"))));
        jsonMap.insert(QLatin1String("presentationType"),PlainTextValue::text(e->value(QLatin1String("presentationType"))));
        jsonMap.insert(QLatin1String("meetingName"),PlainTextValue::text(e->value(QLatin1String("meetingName"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("extra"))));
    }

    return jsonMap;
}

QVariantMap WriteToZotero::createRadioBroadcastJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert(QLatin1String("itemType"),QLatin1String("radioBroadcast"));
    jsonMap.insert(QLatin1String("title"), PlainTextValue::text(e->value(QLatin1String("title"))));
    jsonMap.insert(QLatin1String("creators"),createCreatorsJson(e,QLatin1String("radioBroadcast")));
    jsonMap.insert(QLatin1String("audioRecordingFormat"),PlainTextValue::text(e->value(QLatin1String("audioRecordingFormat"))));
    jsonMap.insert(QLatin1String("network"),PlainTextValue::text(e->value(QLatin1String("network"))));
    jsonMap.insert(QLatin1String("date"),PlainTextValue::text(e->value(QLatin1String("date"))));
    jsonMap.insert(QLatin1String("runningTime"),PlainTextValue::text(e->value(QLatin1String("runningTime"))));
    jsonMap.insert(QLatin1String("language"),PlainTextValue::text(e->value(QLatin1String("language"))));
    jsonMap.insert(QLatin1String("shortTitle"),PlainTextValue::text(e->value(QLatin1String("shortTitle"))));
    jsonMap.insert(QLatin1String("url"),PlainTextValue::text(e->value(QLatin1String("url"))));
    jsonMap.insert(QLatin1String("accessDate"),PlainTextValue::text(e->value(QLatin1String("accessDate"))));
    jsonMap.insert(QLatin1String("archive"),PlainTextValue::text(e->value(QLatin1String("archive"))));
    jsonMap.insert(QLatin1String("archiveLocation"),PlainTextValue::text(e->value(QLatin1String("archiveLocation"))));
    jsonMap.insert(QLatin1String("libraryCatalog"),PlainTextValue::text(e->value(QLatin1String("libraryCatalog"))));
    jsonMap.insert(QLatin1String("tags"),createTagsJson(e));
    //jsonMap.insert(QLatin1String("notes","");

    if(adoptBibtexTypes()) {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstract"))));
        jsonMap.insert(QLatin1String("callNumber"),PlainTextValue::text(e->value(QLatin1String("lccn"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("copyright"))));
        jsonMap.insert(QLatin1String("place"),PlainTextValue::text(e->value(QLatin1String("address"))));
        jsonMap.insert(QLatin1String("programTitle"),PlainTextValue::text(e->value(QLatin1String("booktitle"))));
        jsonMap.insert(QLatin1String("episodeNumber"),PlainTextValue::text(e->value(QLatin1String("number"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("note"))));
    }
    else {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstractNote"))));
        jsonMap.insert(QLatin1String("callNumber"),PlainTextValue::text(e->value(QLatin1String("callNumber"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("rights"))));
        jsonMap.insert(QLatin1String("place"),PlainTextValue::text(e->value(QLatin1String("place"))));
        jsonMap.insert(QLatin1String("programTitle"),PlainTextValue::text(e->value(QLatin1String("programTitle"))));
        jsonMap.insert(QLatin1String("episodeNumber"),PlainTextValue::text(e->value(QLatin1String("episodeNumber"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("extra"))));
    }

    return jsonMap;
}

QVariantMap WriteToZotero::createReportJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert(QLatin1String("itemType"),QLatin1String("report"));
    jsonMap.insert(QLatin1String("title"), PlainTextValue::text(e->value(QLatin1String("title"))));
    jsonMap.insert(QLatin1String("creators"),createCreatorsJson(e,QLatin1String("report")));
    jsonMap.insert(QLatin1String("institution"),PlainTextValue::text(e->value(QLatin1String("institution"))));
    jsonMap.insert(QLatin1String("date"),PlainTextValue::text(e->value(QLatin1String("date"))));
    jsonMap.insert(QLatin1String("pages"),PlainTextValue::text(e->value(QLatin1String("pages"))));
    jsonMap.insert(QLatin1String("language"),PlainTextValue::text(e->value(QLatin1String("language"))));
    jsonMap.insert(QLatin1String("shortTitle"),PlainTextValue::text(e->value(QLatin1String("shortTitle"))));
    jsonMap.insert(QLatin1String("url"),PlainTextValue::text(e->value(QLatin1String("url"))));
    jsonMap.insert(QLatin1String("accessDate"),PlainTextValue::text(e->value(QLatin1String("accessDate"))));
    jsonMap.insert(QLatin1String("archive"),PlainTextValue::text(e->value(QLatin1String("archive"))));
    jsonMap.insert(QLatin1String("archiveLocation"),PlainTextValue::text(e->value(QLatin1String("archiveLocation"))));
    jsonMap.insert(QLatin1String("libraryCatalog"),PlainTextValue::text(e->value(QLatin1String("libraryCatalog"))));
    jsonMap.insert(QLatin1String("tags"),createTagsJson(e));
    //jsonMap.insert(QLatin1String("notes","");

    if(adoptBibtexTypes()) {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstract"))));
        jsonMap.insert(QLatin1String("callNumber"),PlainTextValue::text(e->value(QLatin1String("lccn"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("copyright"))));
        jsonMap.insert(QLatin1String("place"),PlainTextValue::text(e->value(QLatin1String("address"))));
        jsonMap.insert(QLatin1String("seriesTitle"),PlainTextValue::text(e->value(QLatin1String("series"))));
        jsonMap.insert(QLatin1String("reportType"),PlainTextValue::text(e->value(QLatin1String("type"))));
        jsonMap.insert(QLatin1String("reportNumber"),PlainTextValue::text(e->value(QLatin1String("number"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("note"))));
    }
    else {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstractNote"))));
        jsonMap.insert(QLatin1String("callNumber"),PlainTextValue::text(e->value(QLatin1String("callNumber"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("rights"))));
        jsonMap.insert(QLatin1String("place"),PlainTextValue::text(e->value(QLatin1String("place"))));
        jsonMap.insert(QLatin1String("seriesTitle"),PlainTextValue::text(e->value(QLatin1String("seriesTitle"))));
        jsonMap.insert(QLatin1String("reportType"),PlainTextValue::text(e->value(QLatin1String("reportType"))));
        jsonMap.insert(QLatin1String("reportNumber"),PlainTextValue::text(e->value(QLatin1String("reportNumber"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("extra"))));
    }

    return jsonMap;
}

QVariantMap WriteToZotero::createStatuteJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert(QLatin1String("itemType"),QLatin1String("statute"));
    jsonMap.insert(QLatin1String("creators"),createCreatorsJson(e, QLatin1String("statute")));
    jsonMap.insert(QLatin1String("code"),PlainTextValue::text(e->value(QLatin1String("code"))));
    jsonMap.insert(QLatin1String("codeNumber"),PlainTextValue::text(e->value(QLatin1String("codeNumber"))));
    jsonMap.insert(QLatin1String("pages"),PlainTextValue::text(e->value(QLatin1String("pages"))));
    jsonMap.insert(QLatin1String("history"),PlainTextValue::text(e->value(QLatin1String("history"))));
    jsonMap.insert(QLatin1String("language"),PlainTextValue::text(e->value(QLatin1String("language"))));
    jsonMap.insert(QLatin1String("shortTitle"),PlainTextValue::text(e->value(QLatin1String("shortTitle"))));
    jsonMap.insert(QLatin1String("url"),PlainTextValue::text(e->value(QLatin1String("url"))));
    jsonMap.insert(QLatin1String("accessDate"),PlainTextValue::text(e->value(QLatin1String("accessDate"))));
    jsonMap.insert(QLatin1String("tags"),createTagsJson(e));
    //jsonMap.insert(QLatin1String("notes","");

    if(adoptBibtexTypes()) {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstract"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("copyright"))));
        jsonMap.insert(QLatin1String("publicLawNumber"),PlainTextValue::text(e->value(QLatin1String("number"))));
        jsonMap.insert(QLatin1String("nameOfAct"),PlainTextValue::text(e->value(QLatin1String("title"))));
        jsonMap.insert(QLatin1String("section"),PlainTextValue::text(e->value(QLatin1String("chapter"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("note"))));
        jsonMap.insert(QLatin1String("session"),PlainTextValue::text(e->value(QLatin1String("event"))));
        jsonMap.insert(QLatin1String("dateEnacted"),PlainTextValue::text(e->value(QLatin1String("date"))));
    }
    else {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstractNote"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("rights"))));
        jsonMap.insert(QLatin1String("publicLawNumber"),PlainTextValue::text(e->value(QLatin1String("publicLawNumber"))));
        jsonMap.insert(QLatin1String("nameOfAct"),QLatin1String("nameOfAct"));
        jsonMap.insert(QLatin1String("section"),PlainTextValue::text(e->value(QLatin1String("section"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("extra"))));
        jsonMap.insert(QLatin1String("session"),PlainTextValue::text(e->value(QLatin1String("session"))));
        jsonMap.insert(QLatin1String("dateEnacted"),PlainTextValue::text(e->value(QLatin1String("dateEnacted"))));
    }

    return jsonMap;
}

QVariantMap WriteToZotero::createTvBroadcastJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert(QLatin1String("itemType"),QLatin1String("tvBroadcast"));
    jsonMap.insert(QLatin1String("title"), PlainTextValue::text(e->value(QLatin1String("title"))));
    jsonMap.insert(QLatin1String("creators"),createCreatorsJson(e, QLatin1String("tvBroadcast")));
    jsonMap.insert(QLatin1String("videoRecordingFormat"),PlainTextValue::text(e->value(QLatin1String("videoRecordingFormat"))));
    jsonMap.insert(QLatin1String("network"),PlainTextValue::text(e->value(QLatin1String("network"))));
    jsonMap.insert(QLatin1String("date"),PlainTextValue::text(e->value(QLatin1String("date"))));
    jsonMap.insert(QLatin1String("runningTime"),PlainTextValue::text(e->value(QLatin1String("runningTime"))));
    jsonMap.insert(QLatin1String("language"),PlainTextValue::text(e->value(QLatin1String("language"))));
    jsonMap.insert(QLatin1String("shortTitle"),PlainTextValue::text(e->value(QLatin1String("shortTitle"))));
    jsonMap.insert(QLatin1String("url"),PlainTextValue::text(e->value(QLatin1String("url"))));
    jsonMap.insert(QLatin1String("accessDate"),PlainTextValue::text(e->value(QLatin1String("accessDate"))));
    jsonMap.insert(QLatin1String("archive"),PlainTextValue::text(e->value(QLatin1String("archive"))));
    jsonMap.insert(QLatin1String("archiveLocation"),PlainTextValue::text(e->value(QLatin1String("archiveLocation"))));
    jsonMap.insert(QLatin1String("libraryCatalog"),PlainTextValue::text(e->value(QLatin1String("libraryCatalog"))));
    jsonMap.insert(QLatin1String("tags"),createTagsJson(e));
    //jsonMap.insert(QLatin1String("notes","");

    if(adoptBibtexTypes()) {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstract"))));
        jsonMap.insert(QLatin1String("callNumber"),PlainTextValue::text(e->value(QLatin1String("lccn"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("copyright"))));
        jsonMap.insert(QLatin1String("place"),PlainTextValue::text(e->value(QLatin1String("address"))));
        jsonMap.insert(QLatin1String("programTitle"),PlainTextValue::text(e->value(QLatin1String("booktitle"))));
        jsonMap.insert(QLatin1String("episodeNumber"),PlainTextValue::text(e->value(QLatin1String("number"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("note"))));
    }
    else {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstractNote"))));
        jsonMap.insert(QLatin1String("callNumber"),PlainTextValue::text(e->value(QLatin1String("callNumber"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("rights"))));
        jsonMap.insert(QLatin1String("place"),PlainTextValue::text(e->value(QLatin1String("place"))));
        jsonMap.insert(QLatin1String("programTitle"),PlainTextValue::text(e->value(QLatin1String("programTitle"))));
        jsonMap.insert(QLatin1String("episodeNumber"),PlainTextValue::text(e->value(QLatin1String("episodeNumber"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("extra"))));
    }

    return jsonMap;
}

QVariantMap WriteToZotero::createThesisJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert(QLatin1String("itemType"),QLatin1String("thesis"));
    jsonMap.insert(QLatin1String("title"), PlainTextValue::text(e->value(QLatin1String("title"))));
    jsonMap.insert(QLatin1String("creators"),createCreatorsJson(e, QLatin1String("thesis")));
    jsonMap.insert(QLatin1String("date"),PlainTextValue::text(e->value(QLatin1String("date"))));
    jsonMap.insert(QLatin1String("numPages"),PlainTextValue::text(e->value(QLatin1String("numPages"))));
    jsonMap.insert(QLatin1String("language"),PlainTextValue::text(e->value(QLatin1String("language"))));
    jsonMap.insert(QLatin1String("shortTitle"),PlainTextValue::text(e->value(QLatin1String("shortTitle"))));
    jsonMap.insert(QLatin1String("url"),PlainTextValue::text(e->value(QLatin1String("url"))));
    jsonMap.insert(QLatin1String("accessDate"),PlainTextValue::text(e->value(QLatin1String("accessDate"))));
    jsonMap.insert(QLatin1String("archive"),PlainTextValue::text(e->value(QLatin1String("archive"))));
    jsonMap.insert(QLatin1String("archiveLocation"),PlainTextValue::text(e->value(QLatin1String("archiveLocation"))));
    jsonMap.insert(QLatin1String("libraryCatalog"),PlainTextValue::text(e->value(QLatin1String("libraryCatalog"))));
    jsonMap.insert(QLatin1String("tags"),createTagsJson(e));
    //jsonMap.insert(QLatin1String("notes","");

    if(adoptBibtexTypes()) {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstract"))));
        jsonMap.insert(QLatin1String("callNumber"),PlainTextValue::text(e->value(QLatin1String("lccn"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("copyright"))));
        jsonMap.insert(QLatin1String("place"),PlainTextValue::text(e->value(QLatin1String("address"))));
        jsonMap.insert(QLatin1String("university"),PlainTextValue::text(e->value(QLatin1String("school"))));
        jsonMap.insert(QLatin1String("thesisType"),PlainTextValue::text(e->value(QLatin1String("type"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("note"))));
    }
    else {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstractNote"))));
        jsonMap.insert(QLatin1String("callNumber"),PlainTextValue::text(e->value(QLatin1String("callNumber"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("rights"))));
        jsonMap.insert(QLatin1String("place"),PlainTextValue::text(e->value(QLatin1String("place"))));
        jsonMap.insert(QLatin1String("university"),PlainTextValue::text(e->value(QLatin1String("university"))));
        jsonMap.insert(QLatin1String("thesisType"),PlainTextValue::text(e->value(QLatin1String("thesisType"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("extra"))));
    }

    return jsonMap;
}

QVariantMap WriteToZotero::createVideoRecordingJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert(QLatin1String("itemType"),QLatin1String("videoRecording"));
    jsonMap.insert(QLatin1String("title"), PlainTextValue::text(e->value(QLatin1String("title"))));
    jsonMap.insert(QLatin1String("creators"),createCreatorsJson(e, QLatin1String("videoRecording")));
    jsonMap.insert(QLatin1String("videoRecordingFormat"),PlainTextValue::text(e->value(QLatin1String("videoRecordingFormat"))));
    jsonMap.insert(QLatin1String("volume"),PlainTextValue::text(e->value(QLatin1String("volume"))));
    jsonMap.insert(QLatin1String("numberOfVolumes"),PlainTextValue::text(e->value(QLatin1String("numberOfVolumes"))));
    jsonMap.insert(QLatin1String("studio"),PlainTextValue::text(e->value(QLatin1String("studio"))));
    jsonMap.insert(QLatin1String("date"),PlainTextValue::text(e->value(QLatin1String("date"))));
    jsonMap.insert(QLatin1String("runningTime"),PlainTextValue::text(e->value(QLatin1String("runningTime"))));
    jsonMap.insert(QLatin1String("language"),PlainTextValue::text(e->value(QLatin1String("language"))));
    jsonMap.insert(QLatin1String("ISBN"),PlainTextValue::text(e->value(QLatin1String("isbn"))));
    jsonMap.insert(QLatin1String("shortTitle"),PlainTextValue::text(e->value(QLatin1String("shortTitle"))));
    jsonMap.insert(QLatin1String("url"),PlainTextValue::text(e->value(QLatin1String("url"))));
    jsonMap.insert(QLatin1String("accessDate"),PlainTextValue::text(e->value(QLatin1String("accessDate"))));
    jsonMap.insert(QLatin1String("archive"),PlainTextValue::text(e->value(QLatin1String("archive"))));
    jsonMap.insert(QLatin1String("archiveLocation"),PlainTextValue::text(e->value(QLatin1String("archiveLocation"))));
    jsonMap.insert(QLatin1String("libraryCatalog"),PlainTextValue::text(e->value(QLatin1String("libraryCatalog"))));
    jsonMap.insert(QLatin1String("tags"),createTagsJson(e));
    //jsonMap.insert(QLatin1String("notes","");

    if(adoptBibtexTypes()) {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstract"))));
        jsonMap.insert(QLatin1String("callNumber"),PlainTextValue::text(e->value(QLatin1String("lccn"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("copyright"))));
        jsonMap.insert(QLatin1String("place"),PlainTextValue::text(e->value(QLatin1String("address"))));
        jsonMap.insert(QLatin1String("seriesTitle"),PlainTextValue::text(e->value(QLatin1String("series"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("note"))));
    }
    else {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstractNote"))));
        jsonMap.insert(QLatin1String("callNumber"),PlainTextValue::text(e->value(QLatin1String("callNumber"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("rights"))));
        jsonMap.insert(QLatin1String("place"),PlainTextValue::text(e->value(QLatin1String("place"))));
        jsonMap.insert(QLatin1String("seriesTitle"),PlainTextValue::text(e->value(QLatin1String("seriesTitle"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("extra"))));
    }

    return jsonMap;
}

QVariantMap WriteToZotero::createWebpageJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert(QLatin1String("itemType"),QLatin1String("webpage"));
    jsonMap.insert(QLatin1String("title"), PlainTextValue::text(e->value(QLatin1String("title"))));
    jsonMap.insert(QLatin1String("creators"),createCreatorsJson(e, QLatin1String("webpage")));
    jsonMap.insert(QLatin1String("date"),PlainTextValue::text(e->value(QLatin1String("date"))));
    jsonMap.insert(QLatin1String("shortTitle"),PlainTextValue::text(e->value(QLatin1String("shortTitle"))));
    jsonMap.insert(QLatin1String("url"),PlainTextValue::text(e->value(QLatin1String("url"))));
    jsonMap.insert(QLatin1String("accessDate"),PlainTextValue::text(e->value(QLatin1String("accessDate"))));
    jsonMap.insert(QLatin1String("language"),PlainTextValue::text(e->value(QLatin1String("language"))));
    jsonMap.insert(QLatin1String("tags"),createTagsJson(e));
    //jsonMap.insert(QLatin1String("notes","");

    if(adoptBibtexTypes()) {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstract"))));
        jsonMap.insert(QLatin1String("callNumber"),PlainTextValue::text(e->value(QLatin1String("lccn"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("copyright"))));
        jsonMap.insert(QLatin1String("websiteTitle"),PlainTextValue::text(e->value(QLatin1String("booktitle"))));
        jsonMap.insert(QLatin1String("websiteType"),PlainTextValue::text(e->value(QLatin1String("type"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("note"))));
    }
    else {
        jsonMap.insert(QLatin1String("abstractNote"),PlainTextValue::text(e->value(QLatin1String("abstractNote"))));
        jsonMap.insert(QLatin1String("callNumber"),PlainTextValue::text(e->value(QLatin1String("callNumber"))));
        jsonMap.insert(QLatin1String("rights"),PlainTextValue::text(e->value(QLatin1String("rights"))));
        jsonMap.insert(QLatin1String("websiteTitle"),PlainTextValue::text(e->value(QLatin1String("websiteTitle"))));
        jsonMap.insert(QLatin1String("websiteType"),PlainTextValue::text(e->value(QLatin1String("websiteType"))));
        jsonMap.insert(QLatin1String("extra"),PlainTextValue::text(e->value(QLatin1String("extra"))));
    }

    return jsonMap;
}

void WriteToZotero::splitPersonList(const QString& text, QStringList &resultList)
{
    QStringList wordList;
    QString word;
    int bracketCounter = 0;
    resultList.clear();

    for (int pos = 0; pos < text.length(); ++pos) {
        if (text[pos] == '{')
            ++bracketCounter;
        else if (text[pos] == '}')
            --bracketCounter;

        if (text[pos] == ' ' || text[pos] == '\t' || text[pos] == '\n' || text[pos] == '\r') {
            if (word == "and" && bracketCounter == 0) {
                resultList.append(wordList.join(" "));
                wordList.clear();
            } else if (!word.isEmpty())
                wordList.append(word);

            word = "";
        } else
            word.append(text[pos]);
    }

    if (!word.isEmpty())
        wordList.append(word);
    if (!wordList.isEmpty())
        resultList.append(wordList.join(" "));
}

void WriteToZotero::splitName(const QString& name, QString &first, QString &last )
{
    QStringList list = name.split(QLatin1String(", "));

    if(list.size() == 2) {
        last = list.at(0).trimmed();
        first = list.at(1).trimmed();
    }
    else {
        last = name.trimmed();
    }
}
