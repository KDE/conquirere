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
#include "nbibio/pipe/nepomuktovariantpipe.h"

#include "ontology/sync.h"
#include "ontology/nbib.h"
#include <Nepomuk2/Vocabulary/NIE>
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk2/Vocabulary/PIMO>
#include <Nepomuk2/Vocabulary/NUAO>

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
    , m_cancel(false)
{
    connect(this, SIGNAL(pushNextItem()), this, SLOT(pushNewItemCache()) );
    connect(this, SIGNAL(deleteNextItem()), this, SLOT(deleteItemsCache()) );

    qRegisterMetaType<SyncMergeDetails>("SyncMergeDetails");
    qRegisterMetaType<QList<SyncMergeDetails> >("QList<SyncMergeDetails>");
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

void NepomukSyncClient::cancel()
{
    m_cancel = true;
    //TODO: emit signal to cancel variantToNepomuk pipe
}

void NepomukSyncClient::importData()
{
    kDebug() << "##### START DOWNLOAD ###### m_currentStep" << m_currentStep;
    clearInternalData();

    if(!m_storage) {
        Q_ASSERT_X( !m_psd.providerId.isEmpty(), "Missing provider Id","no provider specified" );
        //TODO: add more than the zotero storage
        if(m_psd.providerId == QLatin1String("zotero")) {
            m_storage = new ZoteroSync();
        }
        m_storage->setProviderSettings(m_psd);
    }
    else {
        disconnect( m_storage );
    }

    if(m_psd.syncMode != Full_Sync) {
        m_syncSteps = 5;
        m_currentStep = 0;
    }

    calculateProgress(0);

    // Step 1: first download all data from the storage
    connect( m_storage, SIGNAL(finished()), this, SLOT(dataDownloadFinished()) );
    connect( m_storage, SIGNAL(progress(int)), this, SLOT(calculateProgress(int)) );
    m_storage->fetchItems(m_psd.collection);
}

void NepomukSyncClient::dataDownloadFinished()
{
    calculateProgress(100);

    m_cacheDownloaded = m_storage->data();
    kDebug() << "Storage download finished. Fetched items :: " << m_cacheDownloaded.size();

    if(m_cancel) { emit finished(); return; }

    // Step 2: check if we removed some entries from the server and need to remove them locally too
    m_currentStep++;
    if( !m_cacheDownloaded.isEmpty()) {
        findRemovedEntries();
    }
    calculateProgress(100);

    kDebug() << "We need to remove" << m_tmpUserDeleteRequest.size() << "files locally, beacuse they are removed from the server";
    if(m_cancel) { emit finished(); return; }

    // now we have all files from the server and those that should be removed
    if(m_tmpUserDeleteRequest.isEmpty()) {
        m_currentStep++; //no need to delete anything, skip step 3
        readDownloadSyncAfterDelete();
        return;
    }

    //Step 3) let the user decide or simple delete all online removed entries locally too
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
    // to the current "project in use" and is valid for the current zotero sync object
    // but has not a sync-key that is in the list of keys from the "zoteroData" file

    // this means we have more sync objects in nepomuk than returned from the server and thus these elements must be removed locally
    // we fill m_tmpUserDeleteRequest with all nepomu kresources which should be removed

    QString keyFilter; // match only sync resources that are NOT having one of the returned sync-key
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

    calculateProgress(40);

    QString query = "select DISTINCT ?r where {  "
                    "?r a sync:ServerSyncData . "
                    "?r sync:provider ?provider . FILTER regex(?provider, \"" + m_psd.providerId + "\") "
                    "?r sync:userId ?userId . FILTER regex(?userId, \"" + m_psd.userName + "\") "
                    "?r sync:url ?url . FILTER regex(?url, \"" + m_psd.url + "\") "
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
    //TODO: this method needs to be implemented a lot nicer
    if(m_cancel) { emit finished(); return; }

    // step 3 delete either some local resources or the sync part to upload it again
    m_currentStep++;
    calculateProgress(0);

    if(deleteThem) {
        emit status(i18n("Delete %1 items from the Nepomuk storage", m_tmpUserDeleteRequest.size() ));
    }
    else {
        emit status(i18n("Delete %1 zotero sync details from the local Nepomuk storage", m_tmpUserDeleteRequest.size() ));
    }

    //TODO: calculate proper progress in the foreachLoop
    // now go through all entries the user wants to be removed
    foreach(const Nepomuk2::Resource &syncResource, m_tmpUserDeleteRequest) {

        if(m_cancel) { emit finished(); return; }

        QUrl syncDataType = syncResource.property( SYNC::syncDataType() ).toUrl();
        kDebug() << "check sync resource "<< syncResource.uri() << "of type " << syncDataType;

        if(deleteThem) {
            // if we operate on the system library remove the resources completely
            // if we operate only on a project, remove only the isRelated part
            // after all, the resource could be part of a different group too
            // also if we have a resource of type BibResource, remove also any Notes/file resources with it
            if( syncDataType == SYNC::Note()) {
                Nepomuk2::Resource noteResource = syncResource.property( SYNC::note() ).toResource();

                if(m_project.isValid()) {
                    Nepomuk2::removeProperty(QList<QUrl>() << noteResource.uri(), NAO::isRelated(), QVariantList() << m_project.uri());
                }
                else {
                    Nepomuk2::removeResources( QList<QUrl>() << noteResource.uri() << syncResource.uri() );
                }
            }
            else if( syncDataType == SYNC::Attachment()) {
                Nepomuk2::Resource attachmentResource = syncResource.property( SYNC::attachment() ).toResource();

                if(m_project.isValid()) {
                    Nepomuk2::removeProperty(QList<QUrl>() << attachmentResource.uri(), NAO::isRelated(), QVariantList() << m_project.uri());
                }
                else {
                    Nepomuk2::removeResources( QList<QUrl>() << attachmentResource.uri() << syncResource.uri() );
                    // delete file from disk

                    //get the file url
                    QUrl fileUrl = attachmentResource.property(NIE::url()).toUrl();
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
            // BibResource, so delete reference/publication/notes/attachments
            else {
                Nepomuk2::Resource publication = syncResource.property( SYNC::publication() ).toResource();
                Nepomuk2::Resource reference = syncResource.property( SYNC::publication() ).toResource();
                if(m_project.isValid()) {
                    Nepomuk2::removeProperty(QList<QUrl>() << reference.uri() << publication.uri(), NAO::isRelated(), QVariantList() << m_project.uri());
                }
                else {

                    // if the publication has no other reference attached to it, delete it too
                    QList<Nepomuk2::Resource> referencesList = publication.property( NBIB::reference() ).toResourceList();
                    if( referencesList.size() == 1) {
                        Nepomuk2::removeResources( QList<QUrl>() << publication.uri() << syncResource.uri() );

                        // delete all files file from disk
                        QList<Nepomuk2::Resource> fileList = publication.property( NBIB::isPublicationOf()).toResourceList();
                        foreach(const Nepomuk2::Resource &r, fileList) {

                            //get the file url
                            QUrl fileUrl = r.property(NIE::url()).toUrl();
                            QString localFile = fileUrl.toLocalFile();

                            if(localFile.startsWith(QLatin1String("file://"))) {
                                QString localFilePath = localFile.remove( QLatin1String("file://") );
                                QFile localFile( localFilePath );

                                if(!localFile.remove()) {
                                    kDebug() << "file " << localFilePath << "could not be removed";
                                }
                            }
                            Nepomuk2::removeResources( QList<QUrl>() << r.uri() );
                        }

                        // also remove any sync information for child nodes (notes/files)
                        // child files/notes are deleted via nao:hasSubresource when the publication is removed
                        QList<Nepomuk2::Resource> syncRelated = syncResource.property(NAO::isRelated()).toResourceList();
                        QList<QUrl> syncurls;
                        foreach(const Nepomuk2::Resource &r, syncRelated) {
                            syncurls << r.uri();
                        }
                        Nepomuk2::removeResources( QList<QUrl>() << syncurls );
                    }
                    Nepomuk2::removeResources( QList<QUrl>() << reference.uri() << syncResource.uri() );
                }
            }
        }
        else {
            // the user decided to keep the file in his storage so he wants to upload it again next time he uploads.
            // we simply remove the sync::ServerSyncData so the item will be uploaded as a new item into zotero
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
    if(m_cancel) { emit finished(); return; }
    QList<Nepomuk2::Resource> existingItems;

    // Step 4) find duplicates for the merge process
    m_currentStep++;
    calculateProgress(0);

    // find all duplicate entries and merge them if the user specified the UseLocal or UseServer merge
    // fills also m_newEntries
    emit status(i18n("Find duplicated entries") );
    findDuplicates(existingItems);

    if(m_cancel) { emit finished(); return; }

    fixMergingItems();

    if(m_cancel) { emit finished(); return; }

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
        emit status(i18n("push %1 new Zotero items into Nepomuk", m_newEntries.size()));

        //Step 5 import new resources
        m_currentStep++;
        calculateProgress(0);
        importNewResources();

        calculateProgress(50);
    }
    else {
        kDebug() << "no new items for the import found";
        m_currentStep++; // skip step 5 for the push new data to zotero
        calculateProgress(50);
    }

    if(m_cancel) { emit finished(); return; }

    m_downloadFinished = true;
    endSyncStep();
}

void NepomukSyncClient::findDuplicates(QList<Nepomuk2::Resource> &existingItems)
{
    kDebug() << "check duplicates for" << m_cacheDownloaded.size();
    // for each downloaded item from zotero we try to find the item in the local storage
    // we can identify this via the unique zotero Key

    // In the case we found such an entry, we check if the zotero etag value is different
    // If this is the case, the item has been changed on the server side and we need to merge them

    qreal percentPerFile = 100.0/(qreal)m_cacheDownloaded.size();
    qreal curProgress = 0.0;

    foreach(const QVariant &variant, m_cacheDownloaded) {
        QVariantMap entry = variant.toMap();

        //TODO: might be faster if we do 1 query at the beginning and search for the right entry again, than searching for 1 item always in the foreach loop
        // define what we are looking for in the nepomuk storage
        QString itemID = entry.value( QLatin1String("sync-key") ).toString();
        QString query = "select DISTINCT ?r where {  "
                        "?r a sync:ServerSyncData . "
                        "?r sync:provider ?provider . FILTER regex(?provider, \"" + m_psd.providerId + "\") "
                        "?r sync:userId ?userId . FILTER regex(?userId, \"" + m_psd.userName + "\") "
                        "?r sync:url ?url . FILTER regex(?url, \"" + m_psd.url + "\") "
                        "?r sync:id ?synckey . FILTER regex(?synckey, \"" + itemID + "\") "
                        "}";

        Soprano::Model* model = Nepomuk2::ResourceManager::instance()->mainModel();
        Soprano::QueryResultIterator it = model->executeQuery( query, Soprano::Query::QueryLanguageSparql );

        QList<Nepomuk2::Resource> queryResult;
        while( it.next() ) {
            Soprano::BindingSet p = it.current();
            queryResult << Nepomuk2::Resource(p.value(QLatin1String("r")).toString());
        }

        // nothing found, means we have a new entry
        if(queryResult.isEmpty()) {
            kDebug() << "new entry found";
            m_newEntries.append(entry);
        }
        // we found something, means we need to check if it changed on the server
        else {
            if(queryResult.size() > 1) {
                qWarning() << "database error, found more than 1 item to sync the zotero data to. size::" << queryResult.size() << "item" << itemID;
            }

            Nepomuk2::Resource syncResource = queryResult.first();

            QString localEtag = syncResource.property( SYNC::etag() ).toString();
            QString serverEtag = entry.value( QLatin1String("sync-etag") ).toString();

            kDebug() << "existing entry found. Local etag" << localEtag << "| server etag" << serverEtag;

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
                if(reference.isValid()) {
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

            // item changed on the server, let the user decide what to do with it
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

void NepomukSyncClient::fixMergingItems()
{
    if( !m_tmpUserMergeRequest.isEmpty()) {
        kDebug() << "Upload errors that need to be solved/merged" << m_tmpUserMergeRequest.size();

        if(m_psd.mergeMode == Manual) {
            emit userMerge(m_tmpUserMergeRequest);
            // let the user decide what to do, proceede with any other task meanwhile until
            // we need to wait for the user to finish in endSyncStep()
        }
        else {
            kDebug() << "automatic merging of the faulty entries";
            m_mergeFinished = true;
            //FIXME: implement automatic merging
            //TODO: calculate propper progress here
//            foreach(const SyncMergeDetails &sd, m_tmpUserMergeRequest) {
//                mergePipe.merge(sd.syncResource, sd.externalResource, m_psd.mergeMode);
//            }

            m_mergeFinished = true;
            m_tmpUserMergeRequest.clear();
        }
    }
    else {
        m_mergeFinished = true;
    }
}

// small helper function to sort all QVariantMaps entries that have a sync-parent key to the end of the list
// so we are sure that the parent item was created first
bool childItemsLast(const QVariant &s1, const QVariant &s2)
{
    Q_UNUSED(s2)
    QVariantMap entryS1 = s1.toMap();
    bool S1hasParent = entryS1.contains(QLatin1String("sync-key"));

    if(S1hasParent) // items with parents are "higher" thus will be sorted at the end
        return false;
    else
        return true;
}

void NepomukSyncClient::importNewResources()
{
    VariantToNepomukPipe vtnp;
    vtnp.setProjectPimoThing( m_project );
    vtnp.setSyncStorageProvider( m_storage );

    connect(&vtnp, SIGNAL(progress(int)), this, SLOT(calculateProgress(int)));

    emit status(i18n("push %1 new Zotero references into Nepomuk", m_newEntries.size()));
    m_currentStep++;

    // sort entries that need a parent at the end of the list
    qSort(m_newEntries.begin(), m_newEntries.end(), childItemsLast);

    vtnp.pipeExport(m_newEntries);
}

void NepomukSyncClient::mergeFinished()
{
    m_mergeFinished = true;
    endSyncStep();
}

void NepomukSyncClient::endSyncStep()
{
    kDebug() << "#############################################################";
    kDebug() << " End Sync :: " << m_mergeFinished << m_downloadFinished << m_uploadFinished;
    if( !m_mergeFinished ) {
        emit status(i18n("Wait until user merge is finished"));
    }
    else {
        if( m_downloadFinished ) {
            calculateProgress(100);

            clearInternalData();

            if(providerSettings().syncMode == Download_Only) {
                kDebug() << "##### DOWNLOAD FINISHED ###### m_currentStep" << m_currentStep;
                emit finished();
            }
            else {
                exportData();
            }
        }
        else if(m_uploadFinished ) {
            calculateProgress(100);

            clearInternalData();

            kDebug() << "##### Upload FINISHED ###### m_currentStep" << m_currentStep;
            emit finished();
        }
    }
}

void NepomukSyncClient::exportData()
{
    kDebug() << "##################### export disabled #############";
    emit finished();
    return;
    kDebug() << "##### START Upload ###### m_currentStep" << m_currentStep;
    clearInternalData();

    if(!m_storage) {
        Q_ASSERT_X( !m_psd.providerId.isEmpty(), "Missing provider Id","no provider specified" );

        if(m_psd.providerId == QLatin1String("zotero")) {
            m_storage = new ZoteroSync();
        }
        m_storage->setProviderSettings(m_psd);
    }
    else {
        m_storage->disconnect();
    }

    if(m_psd.syncMode != Full_Sync) {
        m_syncSteps = 3;
        m_currentStep = 0;
    }

    kDebug() << "sync steps :: " << m_syncSteps << "current step :: " << m_currentStep;

    emit status( i18n("Fetch data from Nepomuk") );
    calculateProgress(0);

    QString projectFilter;
    if( m_project.isValid() ) {
        projectFilter =  "?r nao:isRelated <" + m_project.uri().toString() + "> . ";
    }

    kDebug() << "1a";
    //----------------------------------------------------------------------------------------------------
    // Step 1a) Get all references
    QString query = "select distinct ?r where {"
                    "?r a nbib:Reference ."
                    + projectFilter +
                    "}";

    Soprano::Model* model = Nepomuk2::ResourceManager::instance()->mainModel();
    Soprano::QueryResultIterator it = model->executeQuery( query, Soprano::Query::QueryLanguageSparql );

    QList<Nepomuk2::Resource> pushNewItems;
    while( it.next() ) {
        Soprano::BindingSet p = it.current();
        pushNewItems << Nepomuk2::Resource(p.value(QLatin1String("r")).toString());
    }

    kDebug() << "Nepomuk resources to upload :: " << pushNewItems.size();

    // transform the new entries to a QVariantList
    NepomukToVariantPipe ntvp;
    ntvp.setSyncProviderDetails(m_storage->providerSettings());
    ntvp.addNepomukUries(true);
    //TODO: add a mode to define how notes are handled (either as noteX = entry or @note{ .. entry)
    ntvp.pipeExport( pushNewItems );

    m_tmpPushItemList = ntvp.variantList();

    calculateProgress(33);

    //----------------------------------------------------------------------------------------------------
    //Step 1b) Get all items that should be removed from the current collection
    //         Means all items that fit the current server sync details that are not in the current project anymore
/*
    if( m_project.isValid() ) {
        //FIXME: check remove from group SPARQL
        QString query3 = "select distinct ?id where {"
                         "?r a nbib:Reference ."
                         "?r sync:serverSyncData ?ssd ."
                         "?ssd sync:id ?id ."
                         "?ssd sync:provider ?provider . FILTER regex(?provider, \"" + m_psd.providerId + "\") "
                         "?ssd sync:userId ?userId . FILTER regex(?userId, \"" + m_psd.userName + "\") "
                         "?ssd sync:url ?url . FILTER regex(?url, \"" + m_psd.url + "\") "
                         "?r nao:isRelated ?project ."
                         //"FILTER !regex(?project, \"<" + m_project.uri().toString() + ">\" ) ."
                         "}";

        Soprano::QueryResultIterator it3 = model->executeQuery( query3, Soprano::Query::QueryLanguageSparql );

        m_pushRemoveFromCollection.clear();
        while( it3.next() ) {
            Soprano::BindingSet p = it3.current();
            m_pushRemoveFromCollection << p.value(QLatin1String("id")).toString();
        }
    }
    */

    calculateProgress(66);
    //----------------------------------------------------------------------------------------------------
    //Step 1c) Get all deleted items
    //         Means all ServerSyncData objects that fit the provider details but have no valid reference/note/attachment anymore
    //         These resources got deleted locally and must be deleted online too

    QString query4 = "select distinct ?r ?id ?etag where {"
            "?r a sync:ServerSyncData."
            "?r sync:id ?id ."
            "?r sync:etag ?etag ."
            "?r sync:provider ?provider . FILTER regex(?provider, \"" + m_psd.providerId + "\") "
            "?r sync:userId ?userId . FILTER regex(?userId, \"" + m_psd.userName + "\") "
            "?r sync:url ?url . FILTER regex(?url, \"" + m_psd.url + "\") "
            "{"
            "  ?r sync:syncDataType sync:Note ."
            "  OPTIONAL { ?r sync:note ?note . }"
            "  FILTER ( !bound(?note)) ."
            "} UNION {"
            "  ?r sync:syncDataType sync:Attachment ."
            "  OPTIONAL { ?r sync:attachment ?attachment . }"
            "  FILTER ( !bound(?attachment)) ."
            "}UNION{"
            "  ?r sync:syncDataType sync:BibResource ."
            "  OPTIONAL { ?r sync:reference ?reference . }"
            "  FILTER ( !bound(?reference)) ."
            "}"
            "}";

    Soprano::QueryResultIterator it4 = model->executeQuery( query4, Soprano::Query::QueryLanguageSparql );

    m_pushDeleteItems.clear();
    while( it4.next() ) {
        Soprano::BindingSet p = it4.current();
        QVariantMap entry;
        entry.insert(QLatin1String("sync-key"), p.value(QLatin1String("id")).toString());
        entry.insert(QLatin1String("sync-etag"), p.value(QLatin1String("etag")).toString());
        entry.insert(QLatin1String("nepomuk-ssd-uri"), p.value(QLatin1String("r")).toString());
        m_pushDeleteItems << entry;
    }

    kDebug() << "Delete " << m_pushDeleteItems.size() << "items from storage";

    calculateProgress(100);
    m_currentStep++;
    calculateProgress(0);

    // now that we have all data from nepomuk we need, start adding it to the onlineprovider
    connect( m_storage, SIGNAL(finished()), this, SLOT(newItemUploadFinished()) );
    connect( m_storage, SIGNAL(error(QString)), this, SLOT(errorDuringUpload(QString)) );

    m_curProgressPerItem = 100.0/m_tmpPushItemList.size();
    m_curProgress = 0;

    pushNewItemCache();
}

void NepomukSyncClient::pushNewItemCache()
{
    m_tmpUserMergeRequest.clear();

    // ok this is called for each item and at the start as well es after each item push request.
    // several things need to be concidered.
    // a) did we finished pushing all nbib:references => exit here start next step (remove ite mfrom collections)
    // b) did we push the all notes/attachments of the current item
    // c) Do we need to push the next nbib:reference

    // Case a) all finished start with next upload (editied items)
    if(m_tmpPushItemList.isEmpty() && m_tmpPushNotesItemList.isEmpty() && m_tmpPushFilesItemList.isEmpty()) {
        disconnect( m_storage, SIGNAL(finished()), this, SLOT(newItemUploadFinished()) );
        disconnect( m_storage, SIGNAL(error(QString)), this, SLOT(errorDuringUpload(QString)) );

        calculateProgress(100);
        m_currentStep++;
        calculateProgress(0);
        fixMergingItems();

        calculateProgress(100);
        connect( m_storage, SIGNAL(finished()), this, SLOT(deleteItemFinished()) );
        connect( m_storage, SIGNAL(error(QString)), this, SLOT(errorDuringDelete(QString)) );
        deleteItemsCache();
        return;
    }

    // Case b) take next note  /file and upload it
    else if( !m_tmpPushNotesItemList.isEmpty() ) {
        m_tmpCurPushedAttachmentItem = m_tmpPushNotesItemList.takeFirst().toMap();

        kDebug() << "upload next note" << m_tmpCurPushedAttachmentItem;
        emit status( i18n("Upload note %1", m_tmpCurPushedAttachmentItem.value(QLatin1String("title")).toString()) );
        m_storage->pushItems( QVariantList() << m_tmpCurPushedAttachmentItem, m_psd.collection );
    }
    else if( !m_tmpPushFilesItemList.isEmpty() ) {
        m_tmpCurPushedAttachmentItem = m_tmpPushFilesItemList.takeFirst().toMap();

        kDebug() << "upload next file";
        emit status( i18n("Upload file %1", m_tmpCurPushedAttachmentItem.value(QLatin1String("url")).toString()) );
        m_storage->pushFile( m_tmpCurPushedAttachmentItem );
    }

    // Case c) no current item to upload take next reference
    else {
        kDebug() << "upload next reference";
        m_tmpCurPushedItem = m_tmpPushItemList.takeFirst().toMap();

        kDebug() << m_tmpCurPushedItem;

        emit status( i18n("Upload item: %1", m_tmpCurPushedItem.value(QLatin1String("title")).toString()) );
        m_storage->pushItems( QVariantList() << m_tmpCurPushedItem, m_psd.collection );
    }
}

void NepomukSyncClient::newItemUploadFinished()
{
    kDebug() << "item upload successful";
    QVariantList retrievedDataList = m_storage->data();

    if( retrievedDataList.isEmpty()) {
        kError() << "no data returned from the storage provider for item";
    }
    else {
        QVariantMap retrievedData = retrievedDataList.first().toMap();

        // Now we concider 2 cases
        // a) we updated a new item to teh server, thus we need to create new item info
        // b) we updated the item on the server, so we just need to update the ServerSyncData object

        if( m_tmpCurPushedItem.contains(QLatin1String("nepomuk-ssd-uri")) ||
                m_tmpCurPushedAttachmentItem.contains(QLatin1String("nepomuk-ssd-uri")) )
        {
            // just update the ServerSyncData with the new etag and updated info
            QUrl nepomukUrl = m_tmpCurPushedItem.value(QLatin1String("nepomuk-ssd-uri")).toUrl();
            if( !nepomukUrl.isValid()) { nepomukUrl = m_tmpCurPushedAttachmentItem.value(QLatin1String("nepomuk-ssd-uri")).toUrl(); }

            if( !nepomukUrl.isValid()) {
                kDebug() << "could not get nepomuk ssd uri from updated resource, can't update SyncDetails";
            }
            else {
                kDebug() << "add updated syncStorage info";

                // Step 1) get the ServerSyncData Object
                Nepomuk2::Resource curServerSyncData = Nepomuk2::Resource(nepomukUrl);

                //Step 2) update etag / updated date on it
                Nepomuk2::setProperty(QList<QUrl>() << curServerSyncData.uri(), SYNC::etag(), QVariantList() << retrievedData.value(QLatin1String("sync-etag")));
                Nepomuk2::setProperty(QList<QUrl>() << curServerSyncData.uri(), NUAO::lastModification(), QVariantList() << retrievedData.value(QLatin1String("sync-updated")));
            }
        }
        else {
            kDebug() << "add NEW syncStorage info";
            // create new ServerSyncData and all the necessary connections

            // as we already have existing Nepomuk resources add the information if available to the retrieved object
            // the VariantToNepomukPipe takes the entry it needs to get the right Nepomuk2::Resource from it again
            retrievedData.insert(QLatin1String("nepomuk-note-uri"), m_tmpCurPushedAttachmentItem.value(QLatin1String("nepomuk-note-uri")).toString());
            retrievedData.insert(QLatin1String("nepomuk-attachment-uri"), m_tmpCurPushedAttachmentItem.value(QLatin1String("nepomuk-attachment-uri")).toString());
            retrievedData.insert(QLatin1String("nepomuk-publication-uri"), m_tmpCurPushedItem.value(QLatin1String("nepomuk-publication-uri")).toString());
            retrievedData.insert(QLatin1String("nepomuk-reference-uri"), m_tmpCurPushedItem.value(QLatin1String("nepomuk-reference-uri")).toString());

            // now add the ServerSyncData to this item
            VariantToNepomukPipe vtnp;
            vtnp.setProjectPimoThing( m_project );
            vtnp.setSyncStorageProvider(m_storage);
            vtnp.pipeExport( QVariantList() << retrievedData );
        }
    }

    // in case we have a reference, check here if we need to upload notes/files now too
    if( !m_tmpCurPushedItem.isEmpty() ) {

        m_curProgress += m_curProgressPerItem;
        calculateProgress(m_curProgress);

        Nepomuk2::Resource publication = Nepomuk2::Resource(m_tmpCurPushedItem.value(QLatin1String("nepomuk-publication-uri")).toString());
        NepomukToVariantPipe ntvp;
        ntvp.setSyncProviderDetails(m_storage->providerSettings());
        ntvp.addNepomukUries(true);

        // get all notes
        //TODO: skip this step for bibtex/mendeley export, define a parameter to say we upload them with the reference rather than as a single object
        QList<Nepomuk2::Resource> relatedList = publication.property(NAO::isRelated()).toResourceList();

        QList<Nepomuk2::Resource> noteList;
        foreach(const Nepomuk2::Resource &r, relatedList) {
            if(r.hasType(PIMO::Note())) {
                noteList << r;
            }
        }

        m_tmpPushNotesItemList.clear();

        if( !noteList.isEmpty()) {
            // transform the new notes to a QVariantList
            ntvp.pipeExport( noteList ); // this adds the parent sync info we just created to it, so we know where to add it it
            m_tmpPushNotesItemList = ntvp.variantList();
        }

        kDebug() << "export " << m_tmpPushNotesItemList.size() << "notes for the current publication" << m_tmpCurPushedItem.value(QLatin1String("title")).toString();

        // get all file attachments
        QList<Nepomuk2::Resource> fileList = publication.property(NBIB::isPublicationOf()).toResourceList();

        m_tmpPushFilesItemList.clear();
        if(!fileList.isEmpty()) {
            ntvp.pipeExport( fileList ); // this adds the parent sync info we just created to it, so we know where to add it it
            m_tmpPushFilesItemList = ntvp.variantList();
        }

        kDebug() << "export " << m_tmpPushFilesItemList.size() << "files for the current publication" << m_tmpCurPushedItem.value(QLatin1String("title")).toString();

        // we setup all note/file upload information, remove m_tmpCurPushedItem information so once note/file upload is finished
        // we can start with the next reference
        m_tmpCurPushedItem.clear();
    }

    m_tmpCurPushedAttachmentItem.clear();

    // upload next item
    emit pushNextItem();
}

void NepomukSyncClient::errorDuringUpload(const QString &msg)
{
    //TODO: present error to the user
    kError() << msg;
    emit pushNextItem();
}

void NepomukSyncClient::itemNeedMerge(const QVariantMap & item)
{
    SyncMergeDetails smd;
    smd.syncResource = Nepomuk2::Resource(item.value(QLatin1String("nepomuk-ssd-uri")).toString());
    smd.externalResource = item;

    m_tmpUserMergeRequest << smd;
}

void NepomukSyncClient::deleteItemsCache()
{
    m_currentStep++;
    calculateProgress(0);
    if(m_pushDeleteItems.isEmpty()) {
        // we finished all upload steps here
        disconnect( m_storage, SIGNAL(finished()), this, SLOT(deleteItemFinished()) );
        disconnect( m_storage, SIGNAL(error(QString)), this, SLOT(errorDuringDelete(QString)) );
        m_uploadFinished = true;
        calculateProgress(100);
        endSyncStep();
    }
    else {
        if(m_psd.askBeforeDeletion) {
            emit askForServerDeletion( m_pushDeleteItems );
        }
        else {
            deleteServerFiles(true);
        }
    }
}

void NepomukSyncClient::deleteServerFiles(bool deleteThem)
{
    if( deleteThem) {
        m_storage->deleteItems( m_pushDeleteItems );
    }
    else {
        deleteItemFinished();
    }
}

void NepomukSyncClient::deleteItemFinished()
{
    //now we can also delete all ServerSyncData resources locally
    QList<QUrl> ssdList;
    foreach(const QVariant &v, m_pushDeleteItems) {
        QVariantMap ssd = v.toMap();
        ssdList << ssd.value(QLatin1String("nepomuk-ssd-uri")).toUrl();
    }

    Nepomuk2::removeResources( ssdList );

    m_pushDeleteItems.clear();

    emit deleteNextItem();
}

void NepomukSyncClient::errorDuringDelete(const QString &msg)
{
    //TODO: present error to the user
    kError() << msg;
    emit deleteNextItem();
}

void NepomukSyncClient::syncData()
{
    clearInternalData();
    Q_ASSERT_X( !m_psd.providerId.isEmpty(), "Missing provider Id","no provider specified" );

    if(m_psd.providerId == QLatin1String("zotero")) {
        m_storage = new ZoteroSync();
    }
    m_storage->setProviderSettings(m_psd);

    m_psd.syncMode = Full_Sync;

    m_syncSteps = 8;
    m_currentStep = 0;

    importData();
}

void NepomukSyncClient::calculateProgress(int value)
{
    qreal curProgress = ((qreal)value * 1.0/m_syncSteps);

    curProgress += (qreal)(100.0/m_syncSteps) * m_currentStep;

    kDebug() << "all steps ::" << m_syncSteps << " curStep" << m_currentStep << " value" << value;
    emit progress(curProgress);
}

void NepomukSyncClient::clearInternalData()
{
    m_cacheDownloaded.clear();
    m_newEntries.clear();
    m_tmpUserDeleteRequest.clear();
    m_tmpUserMergeRequest.clear();
    m_tmpPushItemList.clear();
    m_tmpCurPushedItem.clear();
    m_tmpPushNotesItemList.clear();
    m_tmpPushFilesItemList.clear();
    m_tmpCurPushedAttachmentItem.clear();
    m_pushRemoveFromCollection.clear();
    m_pushDeleteItems.clear();

    m_cancel = false;
    m_mergeFinished = false;
    m_downloadFinished = false;
    m_uploadFinished = false;

    m_curProgressPerItem = 0;
    m_curProgress = 0;
}
