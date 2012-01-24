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

#include "synczoteronepomuk.h"

#include "core/library.h"
#include "core/projectsettings.h"
#include "mainui/librarymanager.h"

#include "pipe/bibtextonepomukpipe.h"
#include "pipe/nepomuktobibtexpipe.h"
#include "onlinestorage/zotero/zoteroinfo.h"
#include "onlinestorage/zotero/readfromzotero.h"
#include "onlinestorage/zotero/writetozotero.h"
#include "onlinestorage/providersettings.h"

#include <kbibtex/element.h>
#include <kbibtex/entry.h>
#include <kbibtex/findduplicates.h>

#include "nbib.h"
#include "sync.h"
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Vocabulary/PIMO>
#include <Nepomuk/Vocabulary/NUAO>
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk/Variant>
#include <Nepomuk/Query/Term>
#include <Nepomuk/Query/ResourceTerm>
#include <Nepomuk/Query/ResourceTypeTerm>
#include <Nepomuk/Query/ComparisonTerm>
#include <Nepomuk/Query/NegationTerm>
#include <Nepomuk/Query/LiteralTerm>
#include <Nepomuk/Query/AndTerm>
#include <Nepomuk/Query/OrTerm>
#include <Nepomuk/Query/QueryServiceClient>
#include <Nepomuk/Query/Result>
#include <Nepomuk/Query/QueryParser>

#include <QtCore/QTimer>

SyncZoteroNepomuk::SyncZoteroNepomuk(QObject *parent)
    : NBibSync(parent)
    , m_rfz(0)
    , m_wtz(0)
    , m_btnp(0)
    , m_ntbp(0)
    , m_syncMode(false)
    , m_cancel(false)
{
}

SyncZoteroNepomuk::~SyncZoteroNepomuk()
{
    delete m_rfz;
    delete m_wtz;
    delete m_btnp;
    delete m_ntbp;
}

void SyncZoteroNepomuk::startDownload()
{
    Q_ASSERT( m_systemLibrary != 0);

    if(!m_libraryToSyncWith)
        m_libraryToSyncWith = m_systemLibrary;

    m_rfz = new ReadFromZotero;
    m_rfz->setProviderSettings(m_psd);
    m_rfz->setAdoptBibtexTypes(true);

    connect(m_rfz, SIGNAL(progress(int)), this, SLOT(calculateProgress(int)));
    emit progressStatus(i18n("fetch data from Zotero server"));

    // if we do not use syncmode, we initialize the syncSteps here
    // they are used to properly calculate the current progress
    if(!m_syncMode) {
        m_syncSteps = 4;
        m_curStep = 0;
    }
    else {
        m_curStep++;
    }
    emit calculateProgress(0);

    if(m_cancel)
        mergeFinished(); // cancel and clean up correctly

    //lets start by retrieving all items from the server and merge them with the current data
    connect(m_rfz, SIGNAL(itemsInfo(File)), this, SLOT(readDownloadSync(File)));
    m_rfz->fetchItems(m_psd.collection);
}

void SyncZoteroNepomuk::readDownloadSync(const File &zoteroData)
{
    emit progressStatus(i18n("sync zotero data with local Nepomuk storage"));
    m_curStep++;

    m_tmpUserDeleteRequest.clear();

    findRemovedEntries(zoteroData, m_tmpUserDeleteRequest);

    kDebug() << m_tmpUserDeleteRequest.size() << "items removed on the server remove them in the localstorage too";

    m_bibCache.clear();
    m_bibCache.append(zoteroData);

    if(m_cancel)
        return;

    // now we have all files from the server and those that should be removed
    if(m_tmpUserDeleteRequest.isEmpty()) {
        readDownloadSyncAfterDelete(m_bibCache);
        return;
    }

    if(m_psd.askBeforeDeletion) {
        // ask user if he really wants to remove them or upload again next time
        // after this emit we wait for the slot call deleteLocalFiles(...)
        emit askForDeletion(m_tmpUserDeleteRequest);
    }
    else {
        // default is to delete or remove the isRelated connection
        deleteLocalFiles(true);
    }
}

void SyncZoteroNepomuk::readDownloadSyncAfterDelete(const File &zoteroData)
{
    File newEntries;
    QList<Nepomuk::Resource> existingItems;
    // find all duplicate entries and merge them if the user specified the UseLocal or UseServer merge
    findDuplicates(zoteroData, newEntries, m_tmpUserMergeRequest, existingItems);

    if(!m_tmpUserMergeRequest.isEmpty()) {
        kDebug() << " merge request necessary for " << m_tmpUserMergeRequest.size() << "items";

        if(m_psd.mergeMode == Manual) {
            emit userMerge(m_tmpUserMergeRequest);
        }
        else {
            // do the automatic sync use eithe rthe version from zotero or keep the local changes
            foreach(const SyncDetails &sd, m_tmpUserMergeRequest) {
                BibTexToNepomukPipe mergePipe;
                mergePipe.setSyncDetails(m_psd.url, m_psd.userName);

                switch(m_psd.mergeMode) {
                case UseServer:
                    mergePipe.merge(sd.syncResource, sd.externalResource, false);
                    break;
                case UseLocal:
                    mergePipe.merge(sd.syncResource, sd.externalResource, true);
                    break;
                case Manual:
                    break;
                }
            }
        }
    }

    // if we operate on a library project add the is related part to all existingItems
    if(m_libraryToSyncWith->libraryType() == Library_Project) {
        foreach(Nepomuk::Resource r, existingItems) {
            m_libraryToSyncWith->addResource(r);
        }
    }

    if(!newEntries.isEmpty()) {
        // up to this point we have a list of new entries we need to add
        // and merged all items with the server or local version unless the user wanted to merge on its own
        delete m_btnp;
        m_btnp = new BibTexToNepomukPipe;

        if(m_libraryToSyncWith->libraryType() == Library_Project) {
            // relate all new items also to the project
            m_btnp->setProjectPimoThing(m_libraryToSyncWith->settings()->projectThing());
        }

        connect(m_btnp, SIGNAL(progress(int)), this, SLOT(calculateProgress(int)));

        emit progressStatus(i18n("push new Zotero data into Nepomuk"));
        m_curStep++;

        m_btnp->setSyncDetails(m_psd.url, m_psd.userName);

        if(m_psd.akonadiContactsUUid > 1) {
        Akonadi::Collection contactBook(m_psd.akonadiContactsUUid);
        if(contactBook.isValid())
            m_btnp->setAkonadiAddressbook(contactBook);
        }

        m_btnp->pipeExport(newEntries);

        m_curStep++;
        emit calculateProgress(50);
    }
    else {
        kDebug() << "no new items for the import found";
        m_curStep++; // step for the push new data to zotero
        m_curStep++; // step for the merging
        emit calculateProgress(50);
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

void SyncZoteroNepomuk::mergeFinished()
{
    emit calculateProgress(100);
    //we finished everything, so cleanup
    delete m_btnp;
    m_btnp = 0;
    m_rfz->deleteLater();
    m_rfz = 0;

    if(m_cancel) {
        return;
        emit syncFinished();
    }

    if(m_syncMode) {
        // give nepomuk some time to recognize that we pushed new items into it
        QTimer::singleShot(5000, this, SLOT(startUpload()));
    }
    else {
        emit syncFinished();
    }
}

void SyncZoteroNepomuk::startUpload()
{
    Q_ASSERT( m_systemLibrary != 0);

    if(!m_libraryToSyncWith)
        m_libraryToSyncWith = m_systemLibrary;

    m_wtz = new WriteToZotero;
    m_wtz->setProviderSettings(m_psd);
    m_wtz->setAdoptBibtexTypes(true);
    connect(m_wtz, SIGNAL(progress(int)), this, SLOT(calculateProgress(int)));

    delete m_ntbp;
    m_ntbp = new NepomukToBibTexPipe;
    connect(m_ntbp, SIGNAL(progress(int)), this, SLOT(calculateProgress(int)));

    // step 1 fetch data from nepomuk
    emit progressStatus(i18n("fetch data from Nepomuk"));

    // if we do not use syncmode, we initialize the syncSteps here
    // they are used to properly calculate the current progress
    if(!m_syncMode) {
        m_syncSteps = 6;
        m_curStep = 0;
    }
    else {
        m_curStep++;
    }

    emit calculateProgress(0);

    Nepomuk::Query::AndTerm andTerm;
    andTerm.addSubTerm(  Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NBIB::Reference() ) );

    // if we sync with a Library project restrict the nepomuk resources
    // to only thouse in this project
    if(m_libraryToSyncWith->libraryType() == Library_Project) {
        Nepomuk::Query::OrTerm orTerm;
        orTerm.addSubTerm( Nepomuk::Query::ComparisonTerm( Soprano::Vocabulary::NAO::hasTag(),
        Nepomuk::Query::ResourceTerm( m_libraryToSyncWith->settings()->projectTag() )));
        orTerm.addSubTerm( Nepomuk::Query::ComparisonTerm( Nepomuk::Vocabulary::PIMO::isRelated(),
        Nepomuk::Query::ResourceTerm(m_libraryToSyncWith->settings()->projectThing() )));
        andTerm.addSubTerm(orTerm);
    }

    Nepomuk::Query::Query query( andTerm );
    QList<Nepomuk::Query::Result> queryResult = Nepomuk::Query::QueryServiceClient::syncQuery(query);

    QList<Nepomuk::Resource> exportList;
    foreach(const Nepomuk::Query::Result & r, queryResult) {
        exportList.append(r.resource());
    }

    // step 2 pipe to bibtex
    emit progressStatus(i18n("prepare data for Zotero"));
    m_curStep++;

    m_ntbp->setSyncDetails(m_psd.url, m_psd.userName);

    m_ntbp->addNepomukUries(true);
    m_ntbp->pipeExport(exportList);
    m_bibCache.clear();
    m_bibCache.append(*m_ntbp->bibtexFile());

    // step 3 upload to zotero
    emit progressStatus(i18n("upload to Zotero"));
    m_curStep++;


    if(m_cancel) {
        File empty;
        readUploadSync(empty); // cancel and cleanup
    }

    connect(m_wtz, SIGNAL(itemsInfo(File)), this, SLOT(readUploadSync(File)));
    connect(m_wtz, SIGNAL(entryItemUpdated(QString,QString,QString)), this, SLOT(updateSyncDetailsToNepomuk(QString,QString,QString)));

    m_wtz->pushItems(m_bibCache, m_psd.collection);
}

void SyncZoteroNepomuk::readUploadSync(const File &zoteroData)
{
    if(zoteroData.isEmpty()) {
        removeFilesFromGroup();
        return;
    }

    // step 4 update changed etag values from changed items on the server
    emit progressStatus(i18n("update Nepomuk storage with new sync details"));
    m_curStep++;

    qreal percentPerFile = 100.0/(qreal)zoteroData.size();
    qreal curProgress = 0.0;

    //step one get all entries which are new to the storage
    foreach(const QSharedPointer<Element> &element, m_bibCache) {
        Entry *entry = dynamic_cast<Entry *>(element.data());
        if(!entry) { continue; }

        if(!entry->contains(QLatin1String("zoteroKey"))) {
            continue;
        }
        m_bibCache.removeAll(element);
    }

    // now find the real duplicates between the zoteroData entries and the m_bibCache entries
    // foreach entry in zoteroData there is exact 1 entry in m_bibCache
    int entriesFound = 0;
    foreach(const QSharedPointer<Element> &zoteroElement, zoteroData) {
        Entry *zoteroEntry = dynamic_cast<Entry *>(zoteroElement.data());
        if(!zoteroEntry) { continue; }

        QStringList error;
        bool updateSuccessfull = false;
        //now try go through all entries we send
        foreach(const QSharedPointer<Element> &localElement, m_bibCache) {
            Entry *localEntry = dynamic_cast<Entry *>(localElement.data());
            if(!localEntry) { continue; }

            bool duplicateFound = true;
            // and foreach entry compare all key/value pairs with each other
            // except the zotero keys
            QMapIterator<QString, Value> i(*zoteroEntry);
            while (i.hasNext()) {
                i.next();

                //ignore zotero keys
                if(i.key().startsWith( QLatin1String("zotero") ))
                    continue;
//                if(i.key().startsWith( QLatin1String("articletype") ))
//                    continue;

                //get local value for currentkey
                Value localValue = localEntry->value(i.key());

                //now check if both entries are the same
                // if not, stop the while loop and check the next entry
                if(PlainTextValue::text(localValue) != PlainTextValue::text(i.value())) {
                    error << "entries not the same #### Key::" +  i.key() + '\n';
                    error << PlainTextValue::text(localValue) + "|--|" + PlainTextValue::text(i.value()) + '\n';
                    duplicateFound = false;
                    break;
                }
            }

            // if we checked all key/value pairs and don't get the duplicateFound=false result
            // we found the right entry
            // add the zotero sync data to it
            if(duplicateFound) {
                writeNewSyncDetailsToNepomuk(localEntry,
                                             zoteroEntry->id(),
                                             PlainTextValue::text(zoteroEntry->value(QLatin1String("zoteroetag"))),
                                             PlainTextValue::text(zoteroEntry->value(QLatin1String("zoteroupdated"))));
                m_bibCache.removeAll(localElement); // remove from local storage, so we don't check it again
                updateSuccessfull = true;
                break;
            }
        }

        if(!updateSuccessfull) {
            qWarning() << "could not add zotero sync details to the right item, duplicate not found!" << zoteroEntry->id();
            qDebug() << error;
        }
        else {
            error.clear();
            entriesFound++;
        }

        curProgress += percentPerFile;
        emit calculateProgress(curProgress);
    }

    if(entriesFound != zoteroData.size()) {
        qWarning() << "could not update all new items with its zotero value. missing" << zoteroData.size() - entriesFound << " from" << zoteroData.size();
    }

    removeFilesFromGroup();
}

void SyncZoteroNepomuk::removeFilesFromGroup()
{
    // lets reuse the writeToZotero object but disconnect the signals in this case
    disconnect(m_wtz, SIGNAL(itemsInfo(File)), this, SLOT(readUploadSync(File)));
    disconnect(m_wtz, SIGNAL(entryItemUpdated(QString,QString,QString)), this, SLOT(updateSyncDetailsToNepomuk(QString,QString,QString)));

    if(m_psd.collection.isEmpty()) {
        kDebug() << "m_psd.collection.isEmpty()) {";
        m_curStep++; // skip step 5
        removeFilesFromZotero();
        return;
    }

    // step 5 remove items from the current selected group
    emit progressStatus(i18n("Remove files from the Zotero group: %1", m_psd.collection));
    m_curStep++;
    emit calculateProgress(0);

    if(m_cancel) {
        cleanupAfterUpload();
    }

    // now we fetch nepomuk for all sync:ServerSyncData objects that are not related to the current project
    // or do not have a valid reference/publication anymore
    // as our syncprovider says we sync with a collection, we simple remove it all those id's from
    // the collection then.

    QString query = "select DISTINCT ?r where {"
                    "?r a sync:ServerSyncData ."
                    "?r sync:provider ?provider . FILTER regex(?provider, \"" + m_psd.providerInfo->providerId() + "\") "
                    "?r sync:userId ?userId . FILTER regex(?userId, \"" + m_psd.userName + "\") "
                    "?r sync:url ?url . FILTER regex(?url, \"" + m_psd.url + "\") ";

    if(m_libraryToSyncWith->libraryType() == Library_System ) {
        // get all ServerSyncData that have no valid reference anymore but
        // correspond to the current syncprovider
        query += "OPTIONAL { ?r nbib:reference ?reference } ."
                 "FILTER (!bound(?reference) ) .";
    }
    else {
        // get all ServerSyncData objects that have a reference that is not part of
        // the current project anymore
        // This will be a huge list if we have lots of items
        // as it really gets all items that are not in the group
        // but there is no way to tell if we just removed one item from a group earlyer or if it was never in this group bef
        query += "?r nbib:reference ?reference . "
                 "Optional { ?reference pimo:isRelated ?pimoRef . }"
                 "FILTER (!bound (?pimoRef)) . ";

    }

    query += "}";


    QList<Nepomuk::Query::Result> queryResult = Nepomuk::Query::QueryServiceClient::syncSparqlQuery(query);

    emit calculateProgress(50);

    if(queryResult.isEmpty()) {
        removeFilesFromZotero();
        return;
    }

    QStringList idsToBeRemoved;
    foreach(const Nepomuk::Query::Result &nqr, queryResult) {
        Nepomuk::Resource syncRes = nqr.resource();
        idsToBeRemoved.append( syncRes.property( Nepomuk::Vocabulary::SYNC::id() ).toString() );
    }

    connect(m_wtz, SIGNAL(itemsInfo(File)), this, SLOT(removeFilesFromZotero()));
    m_wtz->removeItemsFromCollection(idsToBeRemoved, m_psd.collection);
}

void SyncZoteroNepomuk::removeFilesFromZotero()
{
    disconnect(m_wtz, SIGNAL(itemsInfo(File)), this, SLOT(removeFilesFromZotero()));

    if(!m_psd.collection.isEmpty()) {
        m_curStep++; // skip step 6
        cleanupAfterUpload();
        return;
    }

    emit calculateProgress(100);

    // step 6 delete items completely from teh server
    emit progressStatus(i18n("Remove files from the Zotero server"));
    m_curStep++;

    emit calculateProgress(0);

    if(m_cancel) {
        cleanupAfterUpload();
    }

    // now get all the entries that we need to remove from the server completely
    // this is only the case for items removed fro a syncprovider that does not relate to a specific collection
    // after all we might have this item in other colletions

    QString query = "select DISTINCT ?r where {"
                    "?r a sync:ServerSyncData ."
                    "?r sync:provider ?provider . FILTER regex(?provider, \"" + m_psd.providerInfo->providerId() + "\") "
                    "?r sync:userId ?userId . FILTER regex(?userId, \"" + m_psd.userName + "\") "
                    "?r sync:url ?url . FILTER regex(?url, \"" + m_psd.url + "\") "
                    "OPTIONAL { ?r nbib:reference ?reference } ."
                    "FILTER (!bound(?reference) ) ."
                    "}";


    QList<Nepomuk::Query::Result> queryResult = Nepomuk::Query::QueryServiceClient::syncSparqlQuery(query);

    emit calculateProgress(50);

    if(queryResult.isEmpty()) {
        cleanupAfterUpload();
        return;
    }

    QList<QPair<QString, QString> > idsToBeRemoved;
    foreach(const Nepomuk::Query::Result &nqr, queryResult) {
        Nepomuk::Resource syncRes = nqr.resource();
        QPair<QString, QString> item;
        item.first = syncRes.property( Nepomuk::Vocabulary::SYNC::id() ).toString();
        item.second = syncRes.property( Nepomuk::Vocabulary::SYNC::etag() ).toString();
        idsToBeRemoved.append(item);
    }

    connect(m_wtz, SIGNAL(itemsInfo(File)), this, SLOT(cleanupAfterUpload()));
    m_wtz->deleteItems(idsToBeRemoved);
}

void SyncZoteroNepomuk::cleanupAfterUpload()
{
    //we finished everything, so cleanup
    m_wtz->deleteLater();
    m_wtz = 0;

    //delete m_ntnp;
    m_ntbp = 0;

    m_syncMode = false;
    emit calculateProgress(100);
    emit syncFinished();
}

void SyncZoteroNepomuk::startSync()
{
    m_syncMode = true;
    m_syncSteps = 11;
    m_curStep = 0;
    startDownload();
}

void SyncZoteroNepomuk::cancel()
{
    m_cancel = true;
    if(m_rfz)
        m_rfz->cancelDownload();
    if(m_wtz)
        m_wtz->cancelUpload();
}

void SyncZoteroNepomuk::deleteLocalFiles(bool deleteThem)
{
    // now move through all all entries the user wants to be removed
    foreach(SyncDetails sd, m_tmpUserDeleteRequest) { // krazy:exclude=foreach

        if(m_cancel) { return; }

        if(deleteThem) {
            // if we operate on the system library remove the resources completely
            // if we operate only on a project, remove only the isRelated part
            // after all the resource could be part of a different group too
            Nepomuk::Resource publication = sd.syncResource.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();

            if(m_libraryToSyncWith->libraryType() == Library_Project) {
                m_libraryToSyncWith->removeResource(publication);
            }
            else {
                m_systemLibrary->deleteResource(publication);
                sd.syncResource.remove();
            }
        }
        else {
            if(m_libraryToSyncWith->libraryType() == Library_System) {
                // the user decided to keep the file in his storage so we want to upload it again next time we upload files
                // we simply remove the sync::ServerSyncData so the item will be uploaded as a new item in zotero
                Nepomuk::Resource publication = sd.syncResource.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();
                publication.removeProperty(Nepomuk::Vocabulary::SYNC::serverSyncData());
                Nepomuk::Resource reference = sd.syncResource.property(Nepomuk::Vocabulary::NBIB::reference()).toResource();
                reference.removeProperty(Nepomuk::Vocabulary::SYNC::serverSyncData());
                sd.syncResource.remove();
            }
            // in the case of an project library next time we upload all items in the project will be added
            // again to the group, so will this also
        }
    }

    readDownloadSyncAfterDelete(m_bibCache);
}

void SyncZoteroNepomuk::findDuplicates(const File &zoteroData, File &newEntries, QList<SyncDetails> &mergeRequest, QList<Nepomuk::Resource> &existingItems)
{
    // for each downloaded item from zotero we try to find the item in the local storage
    // we can itendify this via the unique zotero Key

    // In the case we found such an entry, we check if the zotero etag value is different
    // If this is the case, the item has been changed on the server side and we need to merge them

    qreal percentPerFile = 100.0/(qreal)zoteroData.size();
    qreal curProgress = 0.0;

    foreach(QSharedPointer<Element> element, zoteroData) {

        if(m_cancel)
            return;

        Entry *entry = dynamic_cast<Entry *>(element.data());
        if(!entry) { continue; }

        // define what we are looking for in the nepomuk storage
        Nepomuk::Query::AndTerm andTerm;
        andTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::SYNC::ServerSyncData() ) );
        andTerm.addSubTerm( Nepomuk::Query::ComparisonTerm( Nepomuk::Vocabulary::SYNC::provider(), Nepomuk::Query::LiteralTerm( "zotero" ) ));
        andTerm.addSubTerm( Nepomuk::Query::ComparisonTerm( Nepomuk::Vocabulary::SYNC::userId(), Nepomuk::Query::LiteralTerm( m_psd.userName ) ));
        andTerm.addSubTerm( Nepomuk::Query::ComparisonTerm( Nepomuk::Vocabulary::SYNC::url(), Nepomuk::Query::LiteralTerm( m_psd.url ) ));
        QString itemID = PlainTextValue::text(entry->value( QLatin1String("zoterokey")) );
        andTerm.addSubTerm( Nepomuk::Query::ComparisonTerm( Nepomuk::Vocabulary::SYNC::id(),  Nepomuk::Query::LiteralTerm( itemID )));
        Nepomuk::Query::Query query( andTerm );

        QList<Nepomuk::Query::Result> queryResult = Nepomuk::Query::QueryServiceClient::syncQuery(query);

        // nothing found, means we have a new entry
        if(queryResult.isEmpty()) {
            newEntries.append(element);
        }
        // we found something, means we need to check if it changed on the server
        else {
            if(queryResult.size() > 1) {
                qWarning() << "database error, found more than 1 item to sync the zotero data to size::" << queryResult.size();
            }

            Nepomuk::Resource syncResource = queryResult.first().resource();

            QString localEtag = syncResource.property(Nepomuk::Vocabulary::SYNC::etag()).toString();
            QString serverEtag = PlainTextValue::text(entry->value(QLatin1String("zoteroetag")));

            Nepomuk::Resource publication = syncResource.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();
            Nepomuk::Resource reference = syncResource.property(Nepomuk::Vocabulary::NBIB::reference()).toResource();

            if(reference.isValid() && publication.isValid()) {
                if(m_libraryToSyncWith->libraryType() == Library_Project) {
                    QList<Nepomuk::Resource> relatedTo = reference.property(Nepomuk::Vocabulary::PIMO::isRelated()).toResourceList();

                    if( !relatedTo.contains(m_libraryToSyncWith->settings()->projectThing()) ) {
                        // so if the item is not realted to the project that we sync with this group anymore
                        // this means in conclusion we removed it from the project and should therefore remove it also from
                        // the server group
                        continue;
                    }
                }
                existingItems.append(publication);
                existingItems.append(reference);
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
                mergeRequest.append(sd);
                break;
            }
        }
    }

    curProgress += percentPerFile;
    emit calculateProgress(curProgress);
}

void SyncZoteroNepomuk::findRemovedEntries(const File &zoteroData, QList<SyncDetails> &userDeleteRequest)
{
    // ok here we ask nepomuk to give us all sync:ServerSyncData resources that have a publication that is related
    // the the current "project in use" and is valid for the current zotero sync object
    // but has not a zoteroKey that is in the list of key from the "zoteroData" file

    // this means we have more sync objects than returned from the server and thus these elements must be removed locally

    QString keyFilter; // match only sync resources that are NOT having one of the returned zotero key
    foreach(QSharedPointer<Element> zoteroElement, zoteroData) {
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

    QString query = "select DISTINCT ?r where {  "
                    "?r a sync:ServerSyncData . "
                    "?r sync:provider ?provider . FILTER regex(?provider, \"" + m_psd.providerInfo->providerId() + "\") "
                    "?r sync:userId ?userId . FILTER regex(?userId, \"" + m_psd.userName + "\") "
                    "?r sync:url ?url . FILTER regex(?url, \"" + m_psd.url + "\") "
                    + projectFilter +
                    "?r sync:id ?zoterokey ."
                    "FILTER (" + keyFilter + ")"
                    "}";

    QList<Nepomuk::Query::Result> queryResult = Nepomuk::Query::QueryServiceClient::syncSparqlQuery(query);

    // the results contain now only those syncDataObjects that are part of the Project (or system library) and are not
    // part of the zotero group (or on the zotero server at all)
    foreach(const Nepomuk::Query::Result &nqr, queryResult) {
        Nepomuk::Resource syncRes = nqr.resource();

        SyncDetails sd;
        sd.syncResource = syncRes;
        sd.externalResource = 0;
        userDeleteRequest.append(sd);
    }

    // nothing to return
    // userDeleteRequest has all the resources we operate on in the next step
}

void SyncZoteroNepomuk::updateSyncDetailsToNepomuk(const QString &id, const QString &etag, const QString &updated)
{
    //step 1 get Nepomuk resource
    QString query = "select DISTINCT ?r where {  "
                     "?r a sync:ServerSyncData . "
                     "?r sync:provider ?provider . FILTER regex(?provider, \"" + m_psd.providerInfo->providerId() + "\") "
                     "?r sync:userId ?userId . FILTER regex(?userId, \"" + m_psd.userName + "\") "
                     "?r sync:url ?url . FILTER regex(?url, \"" + m_psd.url + "\") "
                     "?r sync:id ?id . FILTER regex(?id, \""+ id + "\") "
                     "}";

    QList<Nepomuk::Query::Result> results = Nepomuk::Query::QueryServiceClient::syncSparqlQuery(query);

    if(results.size() > 1 || results.isEmpty()) {
        kDebug() << "could not find the right sync details for the current item query" << m_psd.providerInfo->providerId() << m_psd.userName << m_psd.collection <<id;
        return;
    }

    Nepomuk::Resource syncDetails = results.first().resource();
    Nepomuk::Resource publication = syncDetails.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();
    Nepomuk::Resource reference = syncDetails.property(Nepomuk::Vocabulary::NBIB::reference()).toResource();

    if(!publication.isValid()) {
        qWarning() << "writeSyncDetailsToNepomuk no valid publication found" << publication.resourceUri();
    }
    if(!reference.isValid()) {
        qWarning() << "writeSyncDetailsToNepomuk no valid reference found" << reference.resourceUri();
    }

    syncDetails.setProperty(Nepomuk::Vocabulary::SYNC::url(), m_psd.url);

    syncDetails.setProperty(Nepomuk::Vocabulary::SYNC::provider(), m_psd.providerInfo->providerId());
    syncDetails.setProperty(Nepomuk::Vocabulary::SYNC::userId(), m_psd.userName);
    syncDetails.setProperty(Nepomuk::Vocabulary::SYNC::id(), id);
    syncDetails.setProperty(Nepomuk::Vocabulary::SYNC::etag(), etag);
    syncDetails.setProperty(Nepomuk::Vocabulary::NUAO::lastModification(), updated);
    syncDetails.setProperty(Nepomuk::Vocabulary::NBIB::publication(), publication);
}

void SyncZoteroNepomuk::writeNewSyncDetailsToNepomuk(Entry *localData, const QString &id, const QString &etag, const QString &updated)
{
    //step 1 get Nepomuk resource
    QString pubUri = PlainTextValue::text(localData->value(QLatin1String("nepomuk-publication-uri")));
    QString refUri = PlainTextValue::text(localData->value(QLatin1String("nepomuk-reference-uri")));
    QUrl pubUriUrl = QUrl(pubUri);
    QUrl refUriUrl = QUrl(refUri);
    Nepomuk::Resource publication = Nepomuk::Resource(pubUriUrl);
    Nepomuk::Resource reference = Nepomuk::Resource(refUriUrl);

    Nepomuk::Resource syncDetails;
    if(!publication.isValid()) {
        qWarning() << "writeSyncDetailsToNepomuk no valid publication found" << pubUri;
    }
    else {
        syncDetails = publication.property(Nepomuk::Vocabulary::SYNC::serverSyncData()).toResource();
    }
    if(!reference.isValid()) {
        qWarning() << "writeSyncDetailsToNepomuk no valid reference found" << refUri;
    }
    else if(!syncDetails.isValid()) {
        syncDetails = publication.property(Nepomuk::Vocabulary::SYNC::serverSyncData()).toResource();
    }

    if(!syncDetails.isValid()) {
        syncDetails = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::SYNC::ServerSyncData());
    }

    syncDetails.setProperty(Nepomuk::Vocabulary::SYNC::url(), m_psd.url);

    syncDetails.setProperty(Nepomuk::Vocabulary::SYNC::provider(), QString("zotero"));
    syncDetails.setProperty(Nepomuk::Vocabulary::SYNC::userId(), m_psd.userName);
    syncDetails.setProperty(Nepomuk::Vocabulary::SYNC::id(), id);
    syncDetails.setProperty(Nepomuk::Vocabulary::SYNC::etag(), etag);
    syncDetails.setProperty(Nepomuk::Vocabulary::NUAO::lastModification(), updated);
    syncDetails.setProperty(Nepomuk::Vocabulary::NBIB::publication(), publication);

    publication.setProperty(Nepomuk::Vocabulary::SYNC::serverSyncData(), syncDetails);

    if(reference.isValid()) {
        syncDetails.setProperty(Nepomuk::Vocabulary::NBIB::reference(), reference);
        reference.setProperty(Nepomuk::Vocabulary::SYNC::serverSyncData(), syncDetails);
    }
}
