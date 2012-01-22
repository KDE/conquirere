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
#include <Nepomuk/Vocabulary/NUAO>
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

SyncZoteroNepomuk::SyncZoteroNepomuk(QObject *parent)
    : NBibSync(parent)
    , m_rfz(0)
    , m_wtz(0)
    , m_btnp(0)
    , m_ntnp(0)
    , m_syncMode(false)
{
}

SyncZoteroNepomuk::~SyncZoteroNepomuk()
{
    delete m_btnp;
    delete m_ntnp;
}

void SyncZoteroNepomuk::startDownload()
{
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

    //lets start by retrieving all items from the server and merge them with the current files
    connect(m_rfz, SIGNAL(itemsInfo(File)), this, SLOT(readDownloadSync(File)));
    m_rfz->fetchItems(m_psd.collection);
}

void SyncZoteroNepomuk::readDownloadSync(const File &zoteroData)
{
    emit progressStatus(i18n("sync zotero data with local Nepomuk storage"));
    m_curStep++;

    m_tmpUserDeleteRequest.clear();
    findDeletedEntries(zoteroData, m_tmpUserDeleteRequest);

    if(!m_tmpUserDeleteRequest.isEmpty()) {
        qDebug() << m_tmpUserDeleteRequest.size() << "items deleted on the server delete them in the localstorage too";
    }

    m_bibCache.clear();
    m_bibCache.append(zoteroData);
    if(m_psd.askBeforeDeletion && !m_tmpUserDeleteRequest.isEmpty()) {
        emit askForDeletion(m_tmpUserDeleteRequest);
    }
    else {
        deleteLocalFiles(true);
    }
}

void SyncZoteroNepomuk::readDownloadSyncAfterDelete(const File &zoteroData)
{
    File newEntries;
    findDuplicates(zoteroData, newEntries, m_tmpUserMergeRequest);

    if(!m_tmpUserMergeRequest.isEmpty()) {
        qDebug() << "SyncZoteroNepomuk::readDownloadSync user merge request necessary for " << m_tmpUserMergeRequest.size() << "items";

        if(m_psd.mergeMode == Manual) {
            emit userMerge(m_tmpUserMergeRequest);
        }
    }

    // up to this point we have a list of new entries we need to add
    // and a list of items that need an update, that either means merging automatic server/local or let the user decide how to merge
    delete m_btnp;
    m_btnp = new BibTexToNepomukPipe;
    connect(m_btnp, SIGNAL(progress(int)), this, SLOT(calculateProgress(int)));

    emit progressStatus(i18n("push new Zotero data into Nepomuk"));
    m_curStep++;
    if(m_psd.collection.isEmpty()) {
        m_btnp->setSyncDetails(m_psd.url, m_psd.userName);
    }
    else {
        QString url = m_psd.url + QLatin1String("/") + m_psd.collection;
        m_btnp->setSyncDetails(url, m_psd.userName);
    }

    if(m_psd.akonadiContactsUUid > 1) {
    Akonadi::Collection contactBook(m_psd.akonadiContactsUUid);
    if(contactBook.isValid())
        m_btnp->setAkonadiAddressbook(contactBook);
    }

    m_btnp->pipeExport(newEntries);

    m_curStep++;
    emit calculateProgress(50);

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

    if(m_syncMode) {
        startUpload();
    }
}

void SyncZoteroNepomuk::startUpload()
{
    m_wtz = new WriteToZotero;
    m_wtz->setProviderSettings(m_psd);
    m_wtz->setAdoptBibtexTypes(true);
    connect(m_wtz, SIGNAL(progress(int)), this, SLOT(calculateProgress(int)));

    delete m_ntnp;
    m_ntnp = new NepomukToBibTexPipe;
    connect(m_ntnp, SIGNAL(progress(int)), this, SLOT(calculateProgress(int)));

    // step 1 fetch data from nepomuk
    emit progressStatus(i18n("fetch data from Nepomuk"));

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

    Nepomuk::Query::Query query( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NBIB::Reference() ) );
    QList<Nepomuk::Query::Result> queryResult = Nepomuk::Query::QueryServiceClient::syncQuery(query);

    QList<Nepomuk::Resource> exportList;
    foreach(const Nepomuk::Query::Result & r, queryResult) {
        exportList.append(r.resource());
    }

    // step 2 pipe to bibtex
    emit progressStatus(i18n("prepare data for Zotero"));
    m_curStep++;

    if(m_psd.collection.isEmpty()) {
        m_ntnp->setSyncDetails(m_psd.url, m_psd.userName);
    }
    else {
        QString url = m_psd.url + QLatin1String("/") + m_psd.collection;
        m_ntnp->setSyncDetails(url, m_psd.userName);
    }

    m_ntnp->addNepomukUries(true);
    m_ntnp->pipeExport(exportList);
    m_bibCache.clear();
    m_bibCache.append(*m_ntnp->bibtexFile());

    // step 3 upload to zotero
    emit progressStatus(i18n("upload to Zotero"));
    m_curStep++;

    connect(m_wtz, SIGNAL(itemsInfo(File)), this, SLOT(readUploadSync(File)));
    connect(m_wtz, SIGNAL(entryItemUpdated(QString,QString,QString)), this, SLOT(updateEtagValuesAfterUpload(QString,QString,QString)));
    m_wtz->pushItems(m_bibCache, m_psd.collection);
}

void SyncZoteroNepomuk::readUploadSync(const File &zoteroData)
{
    if(zoteroData.isEmpty()) {
        m_curStep++;
        emit calculateProgress(100);
        //we finished everything, so cleanup
        m_wtz->deleteLater();
        m_wtz = 0;

        //delete m_ntnp;
        m_ntnp = 0;

        m_syncMode = false;
        return;
    }

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
                writeSyncDetailsToNepomuk(localEntry,
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

    //we finished everything, so cleanup
    m_wtz->deleteLater();
    m_wtz = 0;

    //delete m_ntnp;
    m_ntnp = 0;

    m_syncMode = false;
    emit calculateProgress(100);
}

void SyncZoteroNepomuk::startSync()
{
    m_syncMode = true;
    m_syncSteps = 9;
    m_curStep = 0;
    startDownload();
}

void SyncZoteroNepomuk::deleteLocalFiles(bool deleteThem)
{
    foreach(SyncDetails sd, m_tmpUserDeleteRequest) { // krazy:exclude=foreach
        if(deleteThem) {
            Nepomuk::Resource publication = sd.syncResource.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();
            qDebug() << "delete publication" << publication.resourceUri();
            publication.remove();
            // delete the series of the publication if no other publication is in the series
            Nepomuk::Resource series = publication.property(Nepomuk::Vocabulary::NBIB::inSeries()).toResource();
            QList<Nepomuk::Resource> seriesPubilcations = series.property(Nepomuk::Vocabulary::NBIB::seriesOf()).toResourceList();
            if(seriesPubilcations.isEmpty())
                series.remove();
            // delete the collection of the publication if no other article is in the collection
            Nepomuk::Resource collection = publication.property(Nepomuk::Vocabulary::NBIB::collection()).toResource();
            QList<Nepomuk::Resource> articles = collection.property(Nepomuk::Vocabulary::NBIB::article()).toResourceList();
            if(articles.isEmpty())
                collection.remove();
            Nepomuk::Resource reference = sd.syncResource.property(Nepomuk::Vocabulary::NBIB::reference()).toResource();
            reference.remove();
            sd.syncResource.remove();
        }
        else {
            Nepomuk::Resource publication = sd.syncResource.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();
            publication.removeProperty(Nepomuk::Vocabulary::SYNC::serverSyncData());
            Nepomuk::Resource reference = sd.syncResource.property(Nepomuk::Vocabulary::NBIB::reference()).toResource();
            reference.removeProperty(Nepomuk::Vocabulary::SYNC::serverSyncData());
            sd.syncResource.remove();
        }
    }

    readDownloadSyncAfterDelete(m_bibCache);
}

void SyncZoteroNepomuk::findDuplicates(const File &zoteroData, File &newEntries, QList<SyncDetails> &userMergeRequest)
{
    // for each downloaded item from zotero we try to find the item in the local storage
    // we can itendify this via the unique zotero Key

    // In the case we found such an entry, we check if the zotero etag value is different
    // If this is the case, the item has been changed on the server side and we need to merge them

    qreal percentPerFile = 100.0/(qreal)newEntries.size();
    qreal curProgress = 0.0;

    foreach(QSharedPointer<Element> element, zoteroData) {
        Entry *entry = dynamic_cast<Entry *>(element.data());
        if(!entry) { continue; }

        // define what we are looking for in the nepomuk storage
        Nepomuk::Query::AndTerm andTerm;
        andTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::SYNC::ServerSyncData() ) );
        andTerm.addSubTerm( Nepomuk::Query::ComparisonTerm( Nepomuk::Vocabulary::SYNC::provider(), Nepomuk::Query::LiteralTerm( "zotero" ) ));
        andTerm.addSubTerm( Nepomuk::Query::ComparisonTerm( Nepomuk::Vocabulary::SYNC::userId(), Nepomuk::Query::LiteralTerm( m_psd.userName ) ));
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

            if(localEtag != serverEtag) {
                if(!syncResource.isValid()) {
                    qWarning() << "ServerSyncData has no valid publication connected to it!";
                }

                switch(m_psd.mergeMode) {
                case UseServer:
                {
                    BibTexToNepomukPipe mergePipe;
                    if(m_psd.collection.isEmpty()) {
                        mergePipe.setSyncDetails(m_psd.url, m_psd.userName);
                    }
                    else {
                        QString url = m_psd.url + QLatin1String("/") + m_psd.collection;
                        mergePipe.setSyncDetails(url, m_psd.userName);
                    }
                    mergePipe.merge(syncResource, entry, false);
                    break;
                }
                case UseLocal:
                {
                    BibTexToNepomukPipe mergePipe;
                    if(m_psd.collection.isEmpty()) {
                        mergePipe.setSyncDetails(m_psd.url, m_psd.userName);
                    }
                    else {
                        QString url = m_psd.url + QLatin1String("/") + m_psd.collection;
                        mergePipe.setSyncDetails(url, m_psd.userName);
                    }
                    mergePipe.merge(syncResource, entry, true);
                    break;
                }
                case Manual:
                    SyncDetails sd;
                    sd.syncResource = syncResource;
                    sd.externalResource = entry;
                    userMergeRequest.append(sd);
                    break;
                }
            }
        }

        curProgress += percentPerFile;
        emit calculateProgress(curProgress);
    }
}

void SyncZoteroNepomuk::findDeletedEntries(const File &zoteroData, QList<SyncDetails> &userDeleteRequest)
{
    // here we find any sync:ServerSyncData object in teh nepomuk storage that does not have one of the
    // zoteroKeys in zoteroData
    Nepomuk::Query::AndTerm andTerm;
    andTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::SYNC::ServerSyncData() ) );
    andTerm.addSubTerm( Nepomuk::Query::ComparisonTerm( Nepomuk::Vocabulary::SYNC::provider(), Nepomuk::Query::LiteralTerm( "zotero" ) ));
    andTerm.addSubTerm( Nepomuk::Query::ComparisonTerm( Nepomuk::Vocabulary::SYNC::userId(), Nepomuk::Query::LiteralTerm( m_psd.userName ) ));

    QStringList idLookup;

    foreach(QSharedPointer<Element> zoteroElement, zoteroData) {
        Entry *zoteroEntry = dynamic_cast<Entry *>(zoteroElement.data());
        if(!zoteroEntry) { continue; }

        QString itemID = PlainTextValue::text(zoteroEntry->value( QLatin1String("zoterokey")) );
        idLookup.append(itemID);
    }
    Nepomuk::Query::Query query( andTerm );
    QList<Nepomuk::Query::Result> queryResult = Nepomuk::Query::QueryServiceClient::syncQuery(query);

    foreach(const Nepomuk::Query::Result &nqr, queryResult) {
        Nepomuk::Resource syncRes = nqr.resource();
        QString id = syncRes.property(Nepomuk::Vocabulary::SYNC::id()).toString();
        if(!idLookup.contains(id)) {
            SyncDetails sd;
            sd.syncResource = syncRes;
            sd.externalResource = 0;
            userDeleteRequest.append(sd);
        }
    }
}

void SyncZoteroNepomuk::updateEtagValuesAfterUpload(const QString &id, const QString &etag, const QString &updated)
{
    // first get the updated entry in the m_bibCache
    foreach(const QSharedPointer<Element> &localElement, m_bibCache) {
        Entry *localEntry = dynamic_cast<Entry *>(localElement.data());
        if(!localEntry) { continue; }
        if(id == PlainTextValue::text(localEntry->value(QLatin1String("zoteroKey")))) {
            writeSyncDetailsToNepomuk(localEntry, id, etag, updated);
        }
    }
}

void SyncZoteroNepomuk::writeSyncDetailsToNepomuk(Entry *localData, const QString &id, const QString &etag, const QString &updated)
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

    if(m_psd.collection.isEmpty()) {
        syncDetails.setProperty(Nepomuk::Vocabulary::SYNC::url(), m_psd.url);
    }
    else {
        QString url = m_psd.url + QLatin1String("/") + m_psd.collection;
        syncDetails.setProperty(Nepomuk::Vocabulary::SYNC::url(), url);
    }

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
