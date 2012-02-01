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

#include "zoterodownload.h"

#include "core/library.h"
#include "core/projectsettings.h"

#include "pipe/bibtextonepomukpipe.h"

#include "onlinestorage/zotero/zoteroinfo.h"
#include "onlinestorage/zotero/readfromzotero.h"

#include <kbibtex/file.h>
#include <kbibtex/element.h>
#include <kbibtex/entry.h>

#include "nbib.h"
#include "sync.h"
#include <Nepomuk/Variant>
#include <Nepomuk/Vocabulary/PIMO>
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk/Vocabulary/NUAO>

#include <Nepomuk/Query/QueryServiceClient>
#include <Nepomuk/Query/Result>
#include <Nepomuk/Query/QueryParser>

using namespace Nepomuk::Vocabulary;
using namespace Soprano::Vocabulary;

ZoteroDownload::ZoteroDownload(QObject *parent)
    : QObject(parent)
    , m_cancel(false)
    , m_attachmentMode(false)
    , m_systemLibrary(0)
    , m_libraryToSyncWith(0)
    , m_rfz(0)
    , m_bibCache(0)
    , m_corruptedUploads(0)
{
}

ZoteroDownload::~ZoteroDownload()
{
    delete m_rfz;
    delete m_bibCache;
}

void ZoteroDownload::setProviderDetails(ProviderSyncDetails psd)
{
    m_psd = psd;
}

void ZoteroDownload::setSystemLibrary(Library *sl)
{
    m_systemLibrary = sl;
}

void ZoteroDownload::setLibraryToSyncWith(Library *l)
{
    m_libraryToSyncWith = l;
}

void ZoteroDownload::setCorruptedUploadEntries(File *corruptedOnUpload)
{
    m_corruptedUploads = corruptedOnUpload;
}

void ZoteroDownload::startDownload()
{
    Q_ASSERT( m_systemLibrary != 0);

    m_currentStep = 0;

    if(!m_libraryToSyncWith) {
        m_libraryToSyncWith = m_systemLibrary;
    }

    m_syncSteps = 8;

    m_rfz = new ReadFromZotero;
    m_rfz->setProviderSettings(m_psd);
    m_rfz->setAdoptBibtexTypes(true);

    connect(m_rfz, SIGNAL(progress(int)), this, SLOT(calculateProgress(int)));
    emit progressStatus(i18n("fetch data from Zotero server"));

    if(m_cancel) {
        finishAndCleanUp(); // cancel and clean up correctly
    }

    //lets start by retrieving all items from the server and merge them with the current data
    connect(m_rfz, SIGNAL(finished()), this, SLOT(readDownloadSync()));
    m_rfz->fetchItems(m_psd.collection);
}

void ZoteroDownload::readDownloadSync()
{
    m_bibCache = m_rfz->getFile();

    if(m_cancel) { finishAndCleanUp(); return; }

    // special case if we redo the download because a previous update left us with corrupted data we fix it here.
    // will only solve the error when:
    // * New items are uploaded to the server and successfully created
    // * but the server returned the "internal server error" message instead the item info with the zoterokey + etag value

    m_currentStep++;
    if(m_corruptedUploads && !m_corruptedUploads->isEmpty()) {
        emit progressStatus(i18n("Fix previous corrupted upload"));

        fixCorrputedUpload();
    }

    if(m_cancel) { finishAndCleanUp(); return; }

    emit progressStatus(i18n("sync %1 zotero items with local Nepomuk storage", m_bibCache->size() ));
    m_currentStep++;

    m_tmpUserDeleteRequest.clear();
    findRemovedEntries();

    kDebug() << m_tmpUserDeleteRequest.size() << "items removed on the server remove them in the localstorage too";

    if(m_cancel) { finishAndCleanUp(); return; }

    // now we have all files from the server and those that should be removed
    if(m_tmpUserDeleteRequest.isEmpty()) {
        readDownloadSyncAfterDelete();
        return;
    }

    if(m_psd.askBeforeDeletion) {
        // ask user if he really wants to remove them or upload again next time
        // after this emit we wait for the slot call deleteLocalFiles(...)
        emit askForDeletion(m_tmpUserDeleteRequest);
    }
    else {
        // default is to delete the resource or remove the isRelated connection
        deleteLocalFiles(true);
    }
}

void ZoteroDownload::readDownloadSyncAfterDelete()
{
    m_newEntries = new File;
    QList<Nepomuk::Resource> existingItems;

    m_currentStep++;
    calculateProgress(0);

    // find all duplicate entries and merge them if the user specified the UseLocal or UseServer merge
    findDuplicates(existingItems);

    if(!m_tmpUserMergeRequest.isEmpty()) {
        kDebug() << "merge request necessary for " << m_tmpUserMergeRequest.size() << "items";

        if(m_psd.mergeMode == Manual) {
            emit userMerge(m_tmpUserMergeRequest);
        }
        else {
            fixMergingAutomatically();
        }
    }

    // if we operate on a library project add the is related part to all existingItems
    if(m_libraryToSyncWith->libraryType() == Library_Project) {
        foreach(Nepomuk::Resource r, existingItems) {
            m_libraryToSyncWith->addResource(r);
        }
    }

    if(!m_newEntries->isEmpty()) {
        // up to this point we have a list of new entries we need to add
        // and merged all items with the server or local version
        // unless the user wanted to merge on its own
        if(m_attachmentMode) {
            emit progressStatus(i18n("push %1 new Zotero attachments into Nepomuk", m_newEntries->size()));
        }
        else {
            emit progressStatus(i18n("push %1 new Zotero references into Nepomuk", m_newEntries->size()));
        }
        m_currentStep++;
        importNewResources();

        calculateProgress(50);
    }
    else {
        kDebug() << "no new items for the import found";
        m_currentStep++; // skip step for the push new data to zotero
        m_currentStep++; // step for the merging
        calculateProgress(50);
    }

    if(m_cancel) {
        mergeFinished(); // cancel and clean up correctly
    }

    // wait until the user merged all entries on its own
    if(m_tmpUserMergeRequest.size() > 0) {
        emit progressStatus(i18n("wait until user merge is finished"));
    }
    else {
        mergeFinished();
    }
}

void ZoteroDownload::startAttachmentDownload()
{
    m_currentStep++;
    calculateProgress(0);

    emit progressStatus(i18n("fetch zotero attachment infos"));

    kDebug() << "start attachment Download";
}

void ZoteroDownload::deleteLocalFiles(bool deleteThem)
{
    m_currentStep++;
    if(deleteThem)
        emit progressStatus(i18n("delete %1 items from the Nepomuk storage", m_tmpUserDeleteRequest.size() ));
    else
        emit progressStatus(i18n("delete %1 zotero sync details from the local Nepomuk storage", m_tmpUserDeleteRequest.size() ));

    calculateProgress(0);

    // now go through all entries the user wants to be removed
    foreach(SyncDetails sd, m_tmpUserDeleteRequest) {

        if(m_cancel) { return; }

        if(deleteThem) {
            // if we operate on the system library remove the resources completely
            // if we operate only on a project, remove only the isRelated part
            // after all the resource could be part of a different group too
            Nepomuk::Resource mainResource;
            QUrl syncDataType = sd.syncResource.property( SYNC::syncDataType() ).toUrl();
            if( syncDataType == SYNC::Note()) {
                mainResource = sd.syncResource.property( SYNC::note() ).toResource();
            }
            else if( syncDataType == SYNC::Attachment()) {
                mainResource = sd.syncResource.property( SYNC::attachment() ).toResource();
            }
            else {
                mainResource = sd.syncResource.property( NBIB::publication() ).toResource();
            }

            if(m_libraryToSyncWith->libraryType() == Library_Project) {
                m_libraryToSyncWith->removeResource(mainResource);
            }
            else {
                m_systemLibrary->deleteResource(mainResource, true);
            }

            sd.syncResource.remove();
        }
        else {
            if(m_libraryToSyncWith->libraryType() == Library_System) {
                // the user decided to keep the file in his storage so we want to upload it again next time we upload files
                // we simply remove the sync::ServerSyncData so the item will be uploaded as a new item in zotero
                Nepomuk::Resource mainResource;
                Nepomuk::Resource reference;
                QUrl syncDataType = sd.syncResource.property( SYNC::syncDataType() ).toUrl();
                if( syncDataType == SYNC::Note()) {
                    mainResource = sd.syncResource.property( SYNC::note() ).toResource();
                }
                else if( syncDataType == SYNC::Attachment()) {
                    mainResource = sd.syncResource.property( SYNC::attachment() ).toResource();
                }
                else {
                    mainResource = sd.syncResource.property( NBIB::publication() ).toResource();
                    reference = sd.syncResource.property( NBIB::reference()).toResource();
                }

                mainResource.removeProperty( SYNC::serverSyncData());
                reference.removeProperty( SYNC::serverSyncData());
                sd.syncResource.remove();
            }
            // in the case of an project library next time we upload, all items will be added again to the right group
        }
    }

    calculateProgress(1000);
    readDownloadSyncAfterDelete();
}

void ZoteroDownload::mergeFinished()
{
    m_currentStep++;
    calculateProgress(100);

    //we finished everything, so cleanup
    m_rfz->deleteLater();
    m_rfz = 0;

    if(m_cancel) { finishAndCleanUp(); return; }

    if(m_psd.importAttachments && !m_attachmentMode) {
        startAttachmentDownload();
    }
    else {
        finishAndCleanUp();
    }
}

void ZoteroDownload::cancel()
{
    m_cancel = true;

    if(m_rfz)
        m_rfz->cancelDownload();
}

void ZoteroDownload::finishAndCleanUp()
{
    kDebug() << "finishAndCleanUp";
    emit finished();
}

Nepomuk::Resource ZoteroDownload::writeNewSyncDetailsToNepomuk(Entry *localData, const QString &id, const QString &etag, const QString &updated)
{
    // This one is only called when we upload data to the server
    // or if we fix a corrupted upload. (so we downloaded items we uploaded last time but couldn't add sync details before)
    // downloaded stuff is handled by the bibtexToNepomukPipe.cpp directly
    // So we know we must have a valid Nepomuk::Resource attached to the "localData" Entry

    // well we create a new ServerSyncData object to the entry
    Nepomuk::Resource syncDetails = Nepomuk::Resource();
    syncDetails.addType(SYNC::ServerSyncData());

    syncDetails.setProperty( SYNC::url(), m_psd.url);
    syncDetails.setProperty( SYNC::provider(), QString("zotero"));
    syncDetails.setProperty( SYNC::userId(), m_psd.userName);
    syncDetails.setProperty( SYNC::id(), id);
    syncDetails.setProperty( SYNC::etag(), etag);
    syncDetails.setProperty( NUAO::lastModification(), updated);

    // Now theck where this sync data belongs to
    if(localData->type() == QLatin1String("note")) {
        syncDetails.setProperty( SYNC::syncDataType(), SYNC::Note());

        QString noteUri = PlainTextValue::text(localData->value(QLatin1String("nepomuk-note-uri")));
        Nepomuk::Resource note = Nepomuk::Resource(QUrl(noteUri));

        if(!note.exists())
            qWarning() << "Try to add syncDetails to non existent pimo:Note" << noteUri;

        syncDetails.setProperty( SYNC::note(), note);
        note.setProperty( SYNC::serverSyncData(), syncDetails);

    }
    else if(localData->type() == QLatin1String("attachment")) {
        syncDetails.setProperty( SYNC::syncDataType(), SYNC::Attachment());

        QString attachmentUri = PlainTextValue::text(localData->value(QLatin1String("nepomuk-attachment-uri")));
        Nepomuk::Resource attachment = Nepomuk::Resource(QUrl(attachmentUri));

        if(!attachment.exists())
            qWarning() << "Try to add syncDetails to non existent nfo:Document" << attachmentUri;

        syncDetails.setProperty( SYNC::attachment(), attachment);
        attachment.setProperty( SYNC::serverSyncData(), syncDetails);

    }
    else { //BibReference
        syncDetails.setProperty( SYNC::syncDataType(), SYNC::BibResource());

        QString pubUri = PlainTextValue::text(localData->value(QLatin1String("nepomuk-publication-uri")));
        QString refUri = PlainTextValue::text(localData->value(QLatin1String("nepomuk-reference-uri")));

        Nepomuk::Resource publication = Nepomuk::Resource(QUrl(pubUri));
        Nepomuk::Resource reference = Nepomuk::Resource(QUrl(refUri));

        if(!publication.exists())
            qWarning() << "Try to add syncDetails to non existent nbib:Publication" << pubUri;

        if(!reference.exists())
            qWarning() << "Try to add syncDetails to non existent nbib:Reference" << refUri;

        syncDetails.setProperty( NBIB::publication(), publication);
        publication.setProperty( SYNC::serverSyncData(), syncDetails);

        if(reference.isValid()) {
            syncDetails.setProperty( NBIB::reference(), reference);
            reference.setProperty( SYNC::serverSyncData(), syncDetails);
        }
    }

    return syncDetails;
}

void ZoteroDownload::findRemovedEntries()
{
    // ok here we ask nepomuk to give us all sync:ServerSyncData resources that have a publication that is related
    // the the current "project in use" and is valid for the current zotero sync object
    // but has not a zoteroKey that is in the list of key from the "zoteroData" file

    // this means we have more sync objects than returned from the server and thus these elements must be removed locally

    QString keyFilter; // match only sync resources that are NOT having one of the returned zotero key
    foreach(QSharedPointer<Element> zoteroElement, *m_bibCache) {
        Entry *zoteroEntry = dynamic_cast<Entry *>(zoteroElement.data());
        if(!zoteroEntry) { continue; }

        QString itemID = PlainTextValue::text(zoteroEntry->value( QLatin1String("zoterokey")) );
        if(!keyFilter.isEmpty())
            keyFilter += " && ";

        keyFilter += "!regex(?zoterokey, \"" + itemID + "\")";
    }

    QString projectFilter;
    if( m_libraryToSyncWith->libraryType() == Library_Project) {
        projectFilter =  "?r nbib:publication ?publication . "
                         "?publication pimo:isRelated <" + m_libraryToSyncWith->settings()->projectThing().resourceUri().toString() + "> . ";
    }

    QString dataTypeFilter;
    if(m_attachmentMode) {
        dataTypeFilter = "?r sync:syncDataType ?dataType . Filter ( regex(?dataType, sync:Attachment) )";
    }
    else {
        dataTypeFilter = "?r sync:syncDataType ?dataType . Filter ( regex(?dataType, sync:BibResource) || regex(?dataType, sync:Note) )";
    }

    QString query = "select DISTINCT ?r where {  "
                    "?r a sync:ServerSyncData . "
                    "?r sync:provider ?provider . FILTER regex(?provider, \"" + m_psd.providerInfo->providerId() + "\") "
                    "?r sync:userId ?userId . FILTER regex(?userId, \"" + m_psd.userName + "\") "
                    "?r sync:url ?url . FILTER regex(?url, \"" + m_psd.url + "\") "
                    + dataTypeFilter
                    + projectFilter +
                    "?r sync:id ?zoterokey ."
                    "FILTER (" + keyFilter + ")"
                    "}";

    QList<Nepomuk::Query::Result> queryResult = Nepomuk::Query::QueryServiceClient::syncSparqlQuery(query);

    // the results contain now only those syncDataObjects that are part of the Project (or system library) and are not
    // part of the zotero group (or on the zotero server at all, depending what we synced with)
    foreach(const Nepomuk::Query::Result &nqr, queryResult) {
        Nepomuk::Resource syncRes = nqr.resource();

        SyncDetails sd;
        sd.syncResource = syncRes;
        sd.externalResource = 0;
        m_tmpUserDeleteRequest.append(sd);
    }

    // nothing to return
    // userDeleteRequest has all the resources we operate on in the next step
}

void ZoteroDownload::findDuplicates(QList<Nepomuk::Resource> &existingItems)
{
    // for each downloaded item from zotero we try to find the item in the local storage
    // we can itentify this via the unique zotero Key

    // In the case we found such an entry, we check if the zotero etag value is different
    // If this is the case, the item has been changed on the server side and we need to merge them

    qreal percentPerFile = 100.0/(qreal)m_bibCache->size();
    qreal curProgress = 0.0;

    foreach(QSharedPointer<Element> element, *m_bibCache) {

        if(m_cancel) { return; }

        Entry *entry = dynamic_cast<Entry *>(element.data());
        if(!entry) { continue; }

        // define what we are looking for in the nepomuk storage
        QString itemID = PlainTextValue::text(entry->value( QLatin1String("zoterokey")) );
        QString query = "select DISTINCT ?r where {  "
                        "?r a sync:ServerSyncData . "
                        "?r sync:provider ?provider . FILTER regex(?provider, \"" + m_psd.providerInfo->providerId() + "\") "
                        "?r sync:userId ?userId . FILTER regex(?userId, \"" + m_psd.userName + "\") "
                        "?r sync:url ?url . FILTER regex(?url, \"" + m_psd.url + "\") "
                        "?r sync:id ?zoterokey . FILTER regex(?zoterokey, \"" + itemID + "\") "
                        "}";

        QList<Nepomuk::Query::Result> queryResult = Nepomuk::Query::QueryServiceClient::syncSparqlQuery(query);

        // nothing found, means we have a new entry
        if(queryResult.isEmpty()) {
            m_newEntries->append(element);
        }
        // we found something, means we need to check if it changed on the server
        else {
            if(queryResult.size() > 1) {
                qWarning() << "database error, found more than 1 item to sync the zotero data to. size::" << queryResult.size() << "item" << itemID;
            }

            Nepomuk::Resource syncResource = queryResult.first().resource();

            QString localEtag = syncResource.property( SYNC::etag()).toString();
            QString serverEtag = PlainTextValue::text(entry->value(QLatin1String("zoteroetag")));

            // Here we change some values depending if the syncdata represents a BibResource, Note or Attachment.
            // Because when we check the isRelated  part we need to check different resources in this case
            // also while for note/attachment the resource itself must be valid
            // when it comes to the BibResource the reference and publication must be valid
            Nepomuk::Resource resToCheckIsRelatedRelationship;
            QList<Nepomuk::Resource> addToExisting;
            bool validExistingData = false;

            QUrl resourceType = syncResource.property(SYNC::syncDataType()).toUrl();

            if(resourceType == SYNC::BibResource()) {
                Nepomuk::Resource publication = syncResource.property( NBIB::publication()).toResource();
                Nepomuk::Resource reference = syncResource.property( NBIB::reference()).toResource();
                if(reference.isValid() && publication.isValid()) {
                    addToExisting.append(publication);
                    addToExisting.append(reference);
                    validExistingData = true;

                    resToCheckIsRelatedRelationship = reference;
                }
            }
            else if(resourceType == SYNC::Note()) {
                Nepomuk::Resource note = syncResource.property( SYNC::note()).toResource();
                if(note.isValid()) {
                    addToExisting.append(note);
                    validExistingData = true;
                    resToCheckIsRelatedRelationship = note;
                }
            }
            else if(resourceType == SYNC::Attachment()) {
                Nepomuk::Resource attachment = syncResource.property( SYNC::attachment()).toResource();
                if(attachment.isValid()) {
                    addToExisting.append(attachment);
                    validExistingData = true;
                    resToCheckIsRelatedRelationship = attachment;
                }
            }

            // the actual check if the syncData points to still existing or already deleted Nepomuk::Resource objects
            if(validExistingData) {
                if(m_libraryToSyncWith->libraryType() == Library_Project) {
                    QList<Nepomuk::Resource> relatedTo = resToCheckIsRelatedRelationship.property( PIMO::isRelated()).toResourceList();

                    if( !relatedTo.contains(m_libraryToSyncWith->settings()->projectThing()) ) {
                        // so if the item is not related to the project that we sync with this group anymore
                        // this means in conclusion we removed it from the project and should therefore remove it also from
                        // the server group
                        continue;
                    }
                }
                existingItems.append(addToExisting);
            }
            else {
                // if we removed the reference or both we ignore this, they will be removed from the server side as soon as
                // we send an upload request to the server
                continue;
            }

            if(localEtag != serverEtag) {
                if(!syncResource.isValid()) {
                    qWarning() << "ServerSyncData has no valid publication connected to it!";
                }

                SyncDetails sd;
                sd.syncResource = syncResource;
                sd.externalResource = entry;
                m_tmpUserMergeRequest.append(sd);
            }
        }
    }

    curProgress += percentPerFile;
    calculateProgress(curProgress);
}

void ZoteroDownload::fixCorrputedUpload()
{
    qreal curProgress = 0.0;
    qreal percentPerFile = 100.0/(qreal)m_bibCache->size();

    //find the real duplicates between the zoteroData entries and the m_corruptedUploads entries
    int entriesFound = 0;
    foreach(QSharedPointer<Element> corruptedElement, *m_corruptedUploads) {
        Entry *corruptedEntry = dynamic_cast<Entry *>(corruptedElement.data());
        if(!corruptedEntry) { continue; }

        QStringList error;
        bool updateSuccessfull = false;

        //now try to go through all entries we got from the server
        foreach(const QSharedPointer<Element> &zoteroElement, *m_bibCache) {
            Entry *zoteroEntry = dynamic_cast<Entry *>(zoteroElement.data());
            if(!zoteroEntry) { continue; }

            bool duplicateFound = true;
            // and foreach entry compare all key/value pairs with each other
            // except the zotero keys
            QMapIterator<QString, Value> i(*corruptedEntry);
            while (i.hasNext()) {
                i.next();

                //ignore zotero keys
                if(i.key().startsWith( QLatin1String("zotero") ))
                    continue;
                // ignore abstract
                if(i.key().startsWith( QLatin1String("abstract") ))
                    continue;
                if(i.key().startsWith( QLatin1String("keywords") )) // might fail because we don't keep the same order
                    continue;

                //get local value for currentkey
                Value zoteroValue = zoteroEntry->value(i.key());

                //now check if both entries are the same
                // if not, stop the while loop and check the next entry
                if(PlainTextValue::text(zoteroValue) != PlainTextValue::text(i.value())) {
                    error << "entries not the same #### Key::" +  i.key() + '\n';
                    error << "ZOTERO :: " <<  PlainTextValue::text(zoteroValue) + "|--| LOCAL ::" + PlainTextValue::text(i.value()) + '\n';
                    duplicateFound = false;
                    break;
                }
            }

            // if we checked all key/value pairs and don't get the duplicateFound=false result
            // we found the right entry
            // add the zotero sync data to it
            if(duplicateFound) {
                writeNewSyncDetailsToNepomuk(corruptedEntry, zoteroEntry->id(),
                                             PlainTextValue::text(zoteroEntry->value(QLatin1String("zoteroetag"))),
                                             PlainTextValue::text(zoteroEntry->value(QLatin1String("zoteroupdated"))));

                m_bibCache->removeAll(zoteroElement); // remove from local storage, so we don't check it again
                m_corruptedUploads->removeAll(corruptedElement);
                updateSuccessfull = true;
                break;
            }
        }

        if(!updateSuccessfull) {
            qWarning() << "could not add zotero sync details to the right item, duplicate not found!" << corruptedEntry->id();
            qDebug() << error;
        }
        else {
            error.clear();
            entriesFound++;
        }

        curProgress += percentPerFile;
        calculateProgress(curProgress);
    }

    if(entriesFound != m_corruptedUploads->size()) {
        qWarning() << "could not update all corrupted entries from the last update :: " << m_corruptedUploads->size() - entriesFound << "are missing";
    }
}

void ZoteroDownload::fixMergingAutomatically()
{
    BibTexToNepomukPipe mergePipe;
    mergePipe.setSyncDetails(m_psd.url, m_psd.userName);

    // do the automatic sync use either the version from zotero or keep the local changes
    foreach(const SyncDetails &sd, m_tmpUserMergeRequest) {
        switch(m_psd.mergeMode) {
        case UseServer:
            mergePipe.merge(sd.syncResource, sd.externalResource, false);
            break;
        case UseLocal:
            mergePipe.merge(sd.syncResource, sd.externalResource, true);
            break;
        case Manual: // not handled here
            break;
        }
    }

    m_tmpUserMergeRequest.clear();
}

// small helper function to sort all bibtex entries that have a zoteroparent key to the end of the list
// so we are sure that the parent item was created first
bool childItemsLast(const QSharedPointer<Element> &s1, const QSharedPointer<Element> &s2)
{
    Entry *entryS1 = dynamic_cast<Entry *>(s1.data());
    bool S1hasParent = entryS1->contains(QLatin1String("zoteroparent"));

    if(S1hasParent) // items with parent are "higher" thus will be sorted at the end
        return false;
    else
        return true;
}

void ZoteroDownload::importNewResources()
{
    BibTexToNepomukPipe *btnp = new BibTexToNepomukPipe;

    if(m_libraryToSyncWith->libraryType() == Library_Project) {
        // relate all new items also to the project
        btnp->setProjectPimoThing(m_libraryToSyncWith->settings()->projectThing());
    }

    connect(btnp, SIGNAL(progress(int)), this, SLOT(calculateProgress(int)));

    emit progressStatus(i18n("push %1 new Zotero references into Nepomuk", m_newEntries->size()));
    m_currentStep++;

    btnp->setSyncDetails(m_psd.url, m_psd.userName);

    if(m_psd.akonadiContactsUUid > 1) {
        Akonadi::Collection contactBook(m_psd.akonadiContactsUUid);
        if(contactBook.isValid()) {
            btnp->setAkonadiAddressbook(contactBook);
        }
    }

    // sort entries that need a parent at the end of the list
    qSort(m_newEntries->begin(), m_newEntries->end(), childItemsLast);

    btnp->pipeExport(*m_newEntries);

    delete btnp;
    delete m_newEntries;
    m_newEntries = 0;
}

void ZoteroDownload::calculateProgress(int value)
{
    qreal curProgress = ((qreal)value * 1.0/m_syncSteps);

    curProgress += (qreal)(100.0/m_syncSteps) * m_currentStep;

    kDebug() << curProgress << m_syncSteps << m_currentStep;

    emit progress(curProgress);
}
