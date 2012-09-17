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

#include "nepomuksyncclient.h"

#include "nbibio/provider/onlinestorage.h"
#include "nbibio/provider/zotero/zoterosync.h"
#include "nbibio/pipe/varianttonepomukpipe.h"

#include "ontology/sync.h"
#include <Nepomuk2/Vocabulary/NIE>
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk2/Vocabulary/PIMO>

#include <Nepomuk2/ResourceManager>
#include <Soprano/Model>
#include <Soprano/QueryResultIterator>
#include <Nepomuk2/Resource>
#include <Nepomuk2/Variant>
#include <Nepomuk2/DataManagement>

#include <KDE/KDebug>

#include <QtCore/QFile>

using namespace Nepomuk2::Vocabulary;
using namespace Soprano::Vocabulary;

NepomukSyncClient::NepomukSyncClient(QObject *parent)
    : QObject(parent)
    , m_storage(0)
{
}

void NepomukSyncClient::setProviderSettings(const ProviderSyncDetails &psd)
{
    m_psd = psd;

    Q_ASSERT_X( !m_psd.providerId.isEmpty(), "ProviderSyncDetails is missing the provider id info", "NepomukSyncClient");
}

ProviderSyncDetails NepomukSyncClient::providerSettings() const
{
    return m_psd;
}

void NepomukSyncClient::setProject(const Nepomuk2::Resource &project)
{
    m_project = project;
}

void NepomukSyncClient::importData()
{
    m_attachmentMode = false;

    if(!m_storage) {
        if(m_psd.providerId == QLatin1String("zotero")) {
            m_storage = new ZoteroSync();
        }
        m_storage->setProviderSettings(m_psd);
        m_syncSteps = 8;
        m_currentStep = 0;
    }

    calculateProgress(0);

    // Step 1: first download all data from the storage
    connect(m_storage, SIGNAL(finished()), this, SLOT(dataDownloadFinisher()));
    connect(m_storage, SIGNAL(progress(int)), this, SLOT(calculateProgress(int)) );
    m_storage->fetchItems(m_psd.collection);
}

void NepomukSyncClient::dataDownloadFinisher()
{
    calculateProgress(100);

    m_cacheDownloaded = m_storage->data();
    kDebug() << "Storage download finished. Fetched items :: " << m_cacheDownloaded.size();

    // Step 2: check if we removed some entries from the server and need to remove them locally too
    m_currentStep++;
    calculateProgress(0);
    findRemovedEntries();
    calculateProgress(100);

    // now we have all files from the server and those that should be removed
    if(m_tmpUserDeleteRequest.isEmpty()) {
        readDownloadSyncAfterDelete();
        return;
    }

    if(m_psd.askBeforeDeletion) {
        // ask user if he really wants to remove them or upload again next time
        // after this emit we wait for the slot call deleteLocalFiles(...)
        emit askForLocalDeletion(m_tmpUserDeleteRequest);
    }
    else {
        // default is to delete the resource or remove the isRelated connection
        deleteLocalFiles(true);
    }
}

void NepomukSyncClient::findRemovedEntries()
{
    // ok here we ask nepomuk to give us all sync:ServerSyncData resources that have a publication that is related
    // the the current "project in use" and is valid for the current zotero sync object
    // but has not a zoteroKey that is in the list of key from the "zoteroData" file

    // this means we have more sync objects than returned from the server and thus these elements must be removed locally

    QString keyFilter; // match only sync resources that are NOT having one of the returned zotero key
    foreach(const QVariant &v, m_cacheDownloaded) {
        QVariantMap entry = v.toMap();

        QString itemID = entry.value( QLatin1String("sync-key") ).toString();
        if(!keyFilter.isEmpty())
            keyFilter += " && ";

        keyFilter += "!regex(?synckey, \"" + itemID + "\")";
    }

    QString projectFilter;
    if( m_project.isValid() ) {
        projectFilter =  "?r nbib:publication ?publication . "
                         "?publication pimo:isRelated <" + m_project.uri().toString() + "> . ";
    }

    QString dataTypeFilter;
    if(m_attachmentMode) {
        dataTypeFilter = "?r sync:syncDataType ?dataType . Filter ( regex(?dataType, sync:Attachment) )";
    }
    else {
        dataTypeFilter = "?r sync:syncDataType ?dataType . Filter ( regex(?dataType, sync:BibResource) || regex(?dataType, sync:Note) )";
    }

    calculateProgress(40);

    QString query = "select DISTINCT ?r where {  "
                    "?r a sync:ServerSyncData . "
                    "?r sync:provider ?provider . FILTER regex(?provider, \"" + m_psd.providerId + "\") "
                    "?r sync:userId ?userId . FILTER regex(?userId, \"" + m_psd.userName + "\") "
                    "?r sync:url ?url . FILTER regex(?url, \"" + m_psd.url + "\") "
                    + dataTypeFilter
                    + projectFilter +
                    "?r sync:id ?synckey ."
                    "FILTER (" + keyFilter + ")"
                    "}";

    Soprano::Model* model = Nepomuk2::ResourceManager::instance()->mainModel();
    Soprano::QueryResultIterator it = model->executeQuery( query, Soprano::Query::QueryLanguageSparql );

    while( it.next() ) {
        Soprano::BindingSet p = it.current();
        m_tmpUserDeleteRequest << Nepomuk2::Resource(p.value(QLatin1String("?r")).toString());
    }

    // nothing to return
    // m_tmpUserDeleteRequest has all the resources we operate on in the next step
}

void NepomukSyncClient::deleteLocalFiles(bool deleteThem)
{
    // step 3 delete either some local resources or the sync part to upload it again
    m_currentStep++;
    calculateProgress(0);

    if(deleteThem) {
        emit status(i18n("delete %1 items from the Nepomuk storage", m_tmpUserDeleteRequest.size() ));
    }
    else {
        emit status(i18n("delete %1 zotero sync details from the local Nepomuk storage", m_tmpUserDeleteRequest.size() ));
    }

    //TODO: calculate proper progress in the foreachLoop
    // now go through all entries the user wants to be removed
    foreach(const Nepomuk2::Resource &syncResource, m_tmpUserDeleteRequest) {

        if(deleteThem) {
            // if we operate on the system library remove the resources completely
            // if we operate only on a project, remove only the isRelated part
            // after all the resource could be part of a different group too
            Nepomuk2::Resource mainResource;
            QUrl syncDataType = syncResource.property( SYNC::syncDataType() ).toUrl();
            bool deleteFileFromDisk = false;
            if( syncDataType == SYNC::Note()) {
                mainResource = syncResource.property( SYNC::note() ).toResource();
            }
            else if( syncDataType == SYNC::Attachment()) {
                mainResource = syncResource.property( SYNC::attachment() ).toResource();
                deleteFileFromDisk = true;
            }
            else {
                mainResource = syncResource.property( SYNC::publication() ).toResource();
            }

            if(m_project.isValid()) {
                //FIXME: remove project link also from collections / Series that have no other publication in the project
                Nepomuk2::removeProperty(QList<QUrl>() << mainResource.uri(), NAO::isRelated(), QVariantList() << m_project.uri());
            }
            else {
                Nepomuk2::removeResources( QList<QUrl>() << mainResource.uri(), Nepomuk2::RemoveSubResoures );

                if(deleteFileFromDisk) {
                    //get the file url
                    QUrl fileUrl = mainResource.property(NIE::url()).toUrl();
                    QString localFile = fileUrl.toLocalFile();

                    if(localFile.startsWith(QLatin1String("file://"))) {
                        QString localFilePath = localFile.remove( QLatin1String("file://") );
                        QFile localFile( localFilePath );

                        if(!localFile.remove()) {
                            kDebug() << "file " << localFilePath << "could not be removed";
                        }
                    }
                }
            }

            QList<QUrl> resUri; resUri << syncResource.uri();
            Nepomuk2::removeResources(resUri);
        }
        else {
            // the user decided to keep the file in his storage so we want to upload it again next time we upload files
            // we simply remove the sync::ServerSyncData so the item will be uploaded as a new item in zotero
            Nepomuk2::Resource mainResource;
            Nepomuk2::Resource reference;
            QUrl syncDataType = syncResource.property( SYNC::syncDataType() ).toUrl();
            if( syncDataType == SYNC::Note()) {
                mainResource = syncResource.property( SYNC::note() ).toResource();
            }
            else if( syncDataType == SYNC::Attachment()) {
                mainResource = syncResource.property( SYNC::attachment() ).toResource();
            }
            else {
                mainResource = syncResource.property( SYNC::publication() ).toResource();
                reference = syncResource.property( SYNC::reference()).toResource();
            }

            QList<QUrl> mrUri; mrUri << mainResource.uri();
            QVariantList ssdValue; ssdValue << syncResource.uri();
            Nepomuk2::removeProperty(mrUri,SYNC::serverSyncData(), ssdValue);

            QList<QUrl> refUri; refUri << reference.uri();
            Nepomuk2::removeProperty(refUri,SYNC::serverSyncData(), ssdValue);

            QList<QUrl> resUri; resUri << syncResource.uri();
            Nepomuk2::removeResources(resUri);
        }
    }

    calculateProgress(100);
    readDownloadSyncAfterDelete();
}

void NepomukSyncClient::readDownloadSyncAfterDelete()
{
    QList<Nepomuk2::Resource> existingItems;

    // step 4 find duplicates for the merge process
    m_currentStep++;
    calculateProgress(0);

    // find all duplicate entries and merge them if the user specified the UseLocal or UseServer merge
    // fills also m_newEntries
    emit status(i18n("Find duplicated entries") );
    findDuplicates(existingItems);

    if(!m_tmpUserMergeRequest.isEmpty()) {
        if(m_psd.mergeMode == Manual) {
            emit userMerge(m_tmpUserMergeRequest);
        }
        else {
            fixMergingAutomatically();
        }
    }

    // if we operate on a library project add the is related part to all existingItems
    if(m_project.isValid()) {
        foreach(const Nepomuk2::Resource &r, existingItems) {
            //TODO: add also collections/series to library (use library manager?)
            Nepomuk2::addProperty(QList<QUrl>() << r.uri(), NAO::isRelated(), QVariantList() << m_project.uri());
        }
    }

    if(!m_newEntries.isEmpty()) {
        // up to this point we have a list of new entries we need to add
        // and merged all items with the server or local version
        // unless the user wanted to merge on its own
        if(m_attachmentMode) {
            emit status(i18n("push %1 new Zotero attachments into Nepomuk", m_newEntries.size()));
        }
        else {
            emit status(i18n("push %1 new Zotero references into Nepomuk", m_newEntries.size()));
        }
        //Step 5 import new resources
        m_currentStep++;
        calculateProgress(0);
        if(m_attachmentMode) {
//            importNewAttachments();

//            if(!m_newEntriesToDownload->isEmpty()) {
//                downloadNextAttachment();
//                return;
//            }
        }
        else {
            importNewResources();
        }

        calculateProgress(50);
    }
    else {
        kDebug() << "no new items for the import found";
        m_currentStep++; // skip step 5 for the push new data to zotero
        m_currentStep++; // step for 6 the merging
        calculateProgress(50);
    }

//    if(m_cancel) { mergeFinished(); }

    // wait until the user merged all entries on its own
    if(m_tmpUserMergeRequest.size() > 0) {
        emit status(i18n("wait until user merge is finished"));
    }
    else {
        mergeFinished();
    }
}

void NepomukSyncClient::findDuplicates(QList<Nepomuk2::Resource> &existingItems)
{
    // for each downloaded item from zotero we try to find the item in the local storage
    // we can itentify this via the unique zotero Key

    // In the case we found such an entry, we check if the zotero etag value is different
    // If this is the case, the item has been changed on the server side and we need to merge them

    qreal percentPerFile = 100.0/(qreal)m_cacheDownloaded.size();
    qreal curProgress = 0.0;

    foreach(const QVariant &variant, m_cacheDownloaded) {
        QVariantMap entry = variant.toMap();

        // define what we are looking for in the nepomuk storage
        QString itemID = entry.value( QLatin1String("sync-key") ).toString();
        QString query = "select DISTINCT ?r where {  "
                        "?r a sync:ServerSyncData . "
                        "?r sync:provider ?provider . FILTER regex(?provider, \"" + m_psd.providerId + "\") "
                        "?r sync:userId ?userId . FILTER regex(?userId, \"" + m_psd.userName + "\") "
                        "?r sync:url ?url . FILTER regex(?url, \"" + m_psd.url + "\") "
                        "?r sync:id ?zoterokey . FILTER regex(?zoterokey, \"" + itemID + "\") "
                        "}";

        Soprano::Model* model = Nepomuk2::ResourceManager::instance()->mainModel();
        Soprano::QueryResultIterator it = model->executeQuery( query, Soprano::Query::QueryLanguageSparql );

        QList<Nepomuk2::Resource> queryResult;
        while( it.next() ) {
            Soprano::BindingSet p = it.current();
            queryResult << Nepomuk2::Resource(p.value(QLatin1String("?r")).toString());
        }

        // nothing found, means we have a new entry
        if(queryResult.isEmpty()) {
            m_newEntries.append(entry);
        }
        // we found something, means we need to check if it changed on the server
        else {
            if(queryResult.size() > 1) {
                qWarning() << "database error, found more than 1 item to sync the zotero data to. size::" << queryResult.size() << "item" << itemID;
            }

            Nepomuk2::Resource syncResource = queryResult.first();

            QString localEtag = syncResource.property( SYNC::etag()).toString();
            QString serverEtag = entry.value( QLatin1String("sync-etag") ).toString();

            // Here we change some values depending if the syncdata represents a BibResource, Note or Attachment.
            // Because when we check the isRelated  part we need to check different resources in this case
            // also while for note/attachment the resource itself must be valid
            // when it comes to the BibResource the reference and publication must be valid
            Nepomuk2::Resource resToCheckIsRelatedRelationship;
            QList<Nepomuk2::Resource> addToExisting;
            bool validExistingData = false;

            QUrl resourceType = syncResource.property(SYNC::syncDataType()).toUrl();

            if(resourceType == SYNC::BibResource()) {
                Nepomuk2::Resource publication = syncResource.property( SYNC::publication()).toResource();
                Nepomuk2::Resource reference = syncResource.property( SYNC::reference()).toResource();
                if(reference.isValid() && publication.isValid()) {
                    addToExisting.append(publication);
                    addToExisting.append(reference);
                    validExistingData = true;

                    resToCheckIsRelatedRelationship = reference;
                }
            }
            else if(resourceType == SYNC::Note()) {
                Nepomuk2::Resource note = syncResource.property( SYNC::note()).toResource();
                if(note.isValid()) {
                    addToExisting.append(note);
                    validExistingData = true;
                    resToCheckIsRelatedRelationship = note;
                }
            }
            else if(resourceType == SYNC::Attachment()) {
                Nepomuk2::Resource attachment = syncResource.property( SYNC::attachment()).toResource();
                if(attachment.isValid()) {
                    addToExisting.append(attachment);
                    validExistingData = true;
                    resToCheckIsRelatedRelationship = attachment;
                }
            }

            // the actual check if the syncData points to still existing or already deleted Nepomuk2::Resource objects
            if(validExistingData) {
                if(m_project.isValid()) {
                    QList<Nepomuk2::Resource> relatedTo = resToCheckIsRelatedRelationship.property( PIMO::isRelated()).toResourceList();

                    if( !relatedTo.contains(m_project) ) {
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

                SyncMergeDetails sd;
                sd.syncResource = syncResource;
                sd.externalResource = entry;
                m_tmpUserMergeRequest.append(sd);
            }
        }
    }

    curProgress += percentPerFile;
    calculateProgress(curProgress);
}

void NepomukSyncClient::fixMergingAutomatically()
{
    //FIXME: Merge QVariantList with entry from Nepomuk -> VariantToNepomukPipe ?
    /*
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
    */

    m_tmpUserMergeRequest.clear();
}

// small helper function to sort all QVariantMaps entries that have a sync-parent key to the end of the list
// so we are sure that the parent item was created first
bool childItemsLast(const QVariant &s1, const QVariant &s2)
{
    QVariantMap entryS1 = s1.toMap();
    bool S1hasParent = entryS1.contains(QLatin1String("sync-key"));

    if(S1hasParent) // items with parents are "higher" thus will be sorted at the end
        return false;
    else
        return true;
}

void NepomukSyncClient::importNewResources()
{
    kDebug() << "import new items" << m_newEntries;

    VariantToNepomukPipe vtnp;
    vtnp.setProjectPimoThing(m_project);
    vtnp.setSyncDetails(m_psd.url, m_psd.userName, m_psd.providerId);

    connect(&vtnp, SIGNAL(progress(int)), this, SLOT(calculateProgress(int)));

    emit status(i18n("push %1 new Zotero references into Nepomuk", m_newEntries.size()));
    m_currentStep++;

    // sort entries that need a parent at the end of the list
    qSort(m_newEntries.begin(), m_newEntries.end(), childItemsLast);

    vtnp.pipeExport(m_newEntries);
}

void NepomukSyncClient::mergeFinished()
{
    //Step 6 merge finished
    m_currentStep++;
    calculateProgress(100);

    //we finished everything, so cleanup
    m_cacheDownloaded.clear();
    m_newEntries.clear();
    m_tmpUserDeleteRequest.clear();
    m_tmpUserMergeRequest.clear();

    if(m_psd.importAttachments && !m_attachmentMode) {
//        startAttachmentDownload();
    }
    else {
//        calculateProgress(100);
        emit finished();
    }
}


void NepomukSyncClient::exportData()
{
    if(!m_storage) {
        if(m_psd.providerId == QLatin1String("zotero")) {
            m_storage = new ZoteroSync();
        }
        m_storage->setProviderSettings(m_psd);
    }

}

void NepomukSyncClient::syncData()
{
    if(m_psd.providerId == QLatin1String("zotero")) {
        m_storage = new ZoteroSync();
    }
    m_storage->setProviderSettings(m_psd);

}

void NepomukSyncClient::cancel()
{

}

void NepomukSyncClient::calculateProgress(int value)
{
    qreal curProgress = ((qreal)value * 1.0/m_syncSteps);

    curProgress += (qreal)(100.0/m_syncSteps) * m_currentStep;

//    kDebug() << curProgress << m_syncSteps << m_currentStep;

    emit progress(curProgress);
}
