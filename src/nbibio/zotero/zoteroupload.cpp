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

#include "zoteroupload.h"

#include "core/library.h"
#include "core/projectsettings.h"

#include "nbibio/pipe/nepomuktobibtexpipe.h"

#include "onlinestorage/zotero/zoteroinfo.h"
#include "onlinestorage/zotero/writetozotero.h"
#include "onlinestorage/providersettings.h"

#include <kbibtex/entry.h>
#include <kbibtex/file.h>

#include "dms-copy/simpleresourcegraph.h"
#include "dms-copy/simpleresource.h"
#include "dms-copy/datamanagement.h"
#include "dms-copy/storeresourcesjob.h"
#include <KDE/KJob>
#include "sro/sync/serversyncdata.h"
#include "sro/nbib/publication.h"
#include "sro/nbib/reference.h"
#include "sro/pimo/note.h"
#include "sro/nfo/document.h"

#include "nbib.h"
#include "sync.h"
#include <Nepomuk/Vocabulary/PIMO>
#include <Nepomuk/Vocabulary/NUAO>
#include <Soprano/Vocabulary/NAO>

#include <Nepomuk/Variant>
#include <Nepomuk/Query/Term>
#include <Nepomuk/Query/ResourceTerm>
#include <Nepomuk/Query/ResourceTypeTerm>
#include <Nepomuk/Query/ComparisonTerm>
#include <Nepomuk/Query/LiteralTerm>
#include <Nepomuk/Query/AndTerm>
#include <Nepomuk/Query/OrTerm>
#include <Nepomuk/Query/QueryServiceClient>
#include <Nepomuk/Query/Result>
#include <Nepomuk/Query/QueryParser>

//DEBUG
#include <kbibtex/fileexporterbibtex.h>

using namespace Nepomuk::Vocabulary;
using namespace Soprano::Vocabulary;

ZoteroUpload::ZoteroUpload(QObject *parent)
    : QObject(parent)
    , m_cancel(false)
    , m_syncSteps(0)
    , m_currentStep(0)
    , m_attachmentMode(false)
    , m_systemLibrary(0)
    , m_libraryToSyncWith(0)
    , m_wtz(0)
{
}

ZoteroUpload::~ZoteroUpload()
{
    delete m_wtz;
}

void ZoteroUpload::setProviderDetails(ProviderSyncDetails psd)
{
    m_psd = psd;
}

void ZoteroUpload::setSystemLibrary(Library *sl)
{
    m_systemLibrary = sl;
}

void ZoteroUpload::setLibraryToSyncWith(Library *l)
{
    m_libraryToSyncWith = l;
}

File *ZoteroUpload::corruptedUploads()
{
    return m_corruptedUploads;
}

void ZoteroUpload::startUpload()
{
    Q_ASSERT( m_systemLibrary != 0);

    if(m_psd.exportAttachments) {
        m_syncSteps = 11;
    }
    else {
        m_syncSteps = 6;
    }

    m_currentStep = 0;

    if(!m_libraryToSyncWith) {
        m_libraryToSyncWith = m_systemLibrary;
    }

    m_wtz = new WriteToZotero;
    m_wtz->setProviderSettings(m_psd);
    m_wtz->setAdoptBibtexTypes(true);
    connect(m_wtz, SIGNAL(progress(int)), this, SLOT(calculateProgress(int)));

    NepomukToBibTexPipe *ntbp = new NepomukToBibTexPipe;
    connect(ntbp, SIGNAL(progress(int)), this, SLOT(calculateProgress(int)));

    // step 1 fetch data from nepomuk
    emit progressStatus(i18n("fetch reference data from Nepomuk"));

    calculateProgress(0);

    QString projectFilter;
    if( m_libraryToSyncWith->libraryType() == Library_Project) {
        projectFilter =  "?r nbib:publication ?publication . "
                         "?publication pimo:isRelated <" + m_libraryToSyncWith->settings()->projectThing().resourceUri().toString() + "> . ";
    }

    QString query = "select DISTINCT ?r where {"
    + projectFilter +
    "{"
    "?r a nbib:Reference ."
    "}"
    "UNION"
    "{"
    "?r a pimo:Note ."
    "?r nao:isRelated ?x ."
    "?x a nbib:Reference ."
    "}"
    "UNION"
    "{"
    "?r a pimo:Note ."
    "?r sync:serverSyncData ?h . FILTER (bound(?h))"
    "}"
    "}";

    QList<Nepomuk::Query::Result> queryResult = Nepomuk::Query::QueryServiceClient::syncSparqlQuery(query);

    QList<Nepomuk::Resource> exportList;
    foreach(const Nepomuk::Query::Result & r, queryResult) {
        exportList.append(r.resource());
    }

    if(m_cancel) { cleanupAfterUpload(); return; }

    // step 2 pipe to bibtex
    emit progressStatus(i18n("prepare %1 reference items for the Zotero upload", exportList.size()));
    m_currentStep++;

    ntbp->setSyncDetails(m_psd.url, m_psd.userName);

    ntbp->addNepomukUries(true);
    ntbp->pipeExport(exportList);
    m_uploadBibCache = ntbp->bibtexFile();

    //BUG workaround that crash the system otherwise
    // no clue why but if we push m_bibCache into the WriteToZotero class @ m_wtz->pushItems(m_bibCache, m_psd.collection) the
    // QSharedPointers from m_bibCache get invalid and the dynamic_cast crash the system
    // therefore I need to create this twice ... as this function is fast enough its stil ok,
    // but must be fixed nonetheless
    delete ntbp;
    ntbp = new NepomukToBibTexPipe;
    ntbp->setSyncDetails(m_psd.url, m_psd.userName);

    ntbp->addNepomukUries(true);
    ntbp->pipeExport(exportList);
    File *itemsToPush = ntbp->bibtexFile();
    //end bug workaround

    // step 3 upload to zotero
    emit progressStatus(i18n("upload %1 references to Zotero", exportList.size()));
    m_currentStep++;


    if(m_cancel) { cleanupAfterUpload(); return; }

    connect(m_wtz, SIGNAL(finished()), this, SLOT(readUploadSync()));
    connect(m_wtz, SIGNAL(entryItemUpdated(QString,QString,QString)), this, SLOT(updateSyncDetailsToNepomuk(QString,QString,QString)));

    m_wtz->pushItems(itemsToPush, m_psd.collection);
}

void ZoteroUpload::readUploadSync()
{
    m_corruptedUploads = m_wtz->getFailedPushRequestItems();

    File *newItemWithSyncDetails = m_wtz->getFile();

    kDebug() << newItemWithSyncDetails->size() << "new items where we need to add sync data to";

    if(newItemWithSyncDetails->isEmpty()) { // no new files uploaded
        m_currentStep++; // skip step 4
        removeFilesFromGroup();
        return;
    }

    // step 4 add sync data for the newly created files
    emit progressStatus(i18n("update Nepomuk storage with new sync details"));
    m_currentStep++;

    qreal percentPerFile = 100.0/(qreal)newItemWithSyncDetails->size();
    qreal curProgress = 0.0;

    // now find the real duplicates between the zoteroData entries and the m_bibCache entries
    // foreach entry in zoteroData there is exact 1 entry in m_bibCache
    int entriesFound = 0;
    foreach(const QSharedPointer<Element> &zoteroElement, *newItemWithSyncDetails) {

        if(m_cancel) { cleanupAfterUpload(); return; }

        Entry *zoteroEntry = dynamic_cast<Entry *>(zoteroElement.data());
        if(!zoteroEntry) { continue; }

        QStringList error;
        bool updateSuccessfull = false;
        //now try go through all entries we send
        foreach(const QSharedPointer<Element> &localElement, *m_uploadBibCache) {
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
                // ignore abstract
                if(i.key().startsWith( QLatin1String("abstract") ))
                    continue;
                if(i.key().startsWith( QLatin1String("keywords") )) // might fail because we don't keep the same order
                    continue;

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
                m_uploadBibCache->removeAll(localElement); // remove from local storage, so we don't check it again
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
        calculateProgress(curProgress);
    }

    if(entriesFound != newItemWithSyncDetails->size()) {
        qWarning() << "could not update all new items with its zotero value. missing" << newItemWithSyncDetails->size() - entriesFound << " from" << newItemWithSyncDetails->size();
    }

    removeFilesFromGroup();
}

void ZoteroUpload::removeFilesFromGroup()
{
    // lets reuse the writeToZotero object but disconnect the signals in this case
    disconnect(m_wtz, SIGNAL(finished()), this, SLOT(readUploadSync()));
    disconnect(m_wtz, SIGNAL(entryItemUpdated(QString,QString,QString)), this, SLOT(updateSyncDetailsToNepomuk(QString,QString,QString)));

    if(m_psd.collection.isEmpty()) {
        m_currentStep++; // skip step 5
        removeFilesFromZotero();
        return;
    }

    // step 5 remove items from the current selected group
    emit progressStatus(i18n("Remove files from the Zotero group: %1", m_psd.collection));
    m_currentStep++;
    calculateProgress(0);

    if(m_cancel) { cleanupAfterUpload(); return; }

    // now we fetch nepomuk for all sync:ServerSyncData objects that are not related to the current project
    // or do not have a valid reference/publication anymore
    // as our syncprovider says we sync with a collection, we simple remove all those id's from
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
        // but there is no way to tell if we just removed one item from a group earlier or if it was never in this group before
        query += "?r nbib:reference ?reference . "
                 "Optional { ?reference pimo:isRelated ?pimoRef . }"
                 "FILTER (!bound (?pimoRef)) . ";

    }

    query += "}";

    QList<Nepomuk::Query::Result> queryResult = Nepomuk::Query::QueryServiceClient::syncSparqlQuery(query);

    calculateProgress(50);

    if(queryResult.isEmpty()) { removeFilesFromZotero(); return; }

    kDebug() << queryResult.size() << "items must be removed from server group";

    m_tmpUserDeleteRequest.clear();
    foreach(const Nepomuk::Query::Result &nqr, queryResult) {
        SyncDetails sd;
        sd.syncResource = nqr.resource();
        m_tmpUserDeleteRequest.append(sd);
    }

    if(m_psd.askBeforeDeletion) {
        // ask user if he really wants to remove them or download again next time
        // after this emit we wait for the slot call removeFilesFromGroup(...)
        emit askForGroupRemoval(m_tmpUserDeleteRequest);
    }
    else {
        removeFilesFromGroup(true);
    }
}

void ZoteroUpload::removeFilesFromGroup(bool removeThem)
{
    if(removeThem) {

        QStringList idsToBeRemoved;
        foreach(SyncDetails sd, m_tmpUserDeleteRequest) {

            if(m_cancel) { cleanupAfterUpload(); return; }

            idsToBeRemoved.append( sd.syncResource.property(  SYNC::id() ).toString() );
            QList<QUrl> resUri; resUri << sd.syncResource.uri();
            Nepomuk::removeResources(resUri);
        }

        connect(m_wtz, SIGNAL(finished()), this, SLOT(removeFilesFromZotero()));
        m_wtz->removeItemsFromCollection(idsToBeRemoved, m_psd.collection);
    }
    else {
        // do not remove from server, but remove sync:ServerSyncData objects locally file will be downloaded again next time

        foreach(SyncDetails sd, m_tmpUserDeleteRequest) {
            if(m_cancel) { cleanupAfterUpload(); return; }

            QList<QUrl> resUri; resUri << sd.syncResource.uri();
            Nepomuk::removeResources(resUri);
        }

        removeFilesFromZotero();
    }
}

void ZoteroUpload::removeFilesFromZotero()
{
    disconnect(m_wtz, SIGNAL(finished()), this, SLOT(removeFilesFromZotero()));

    if(!m_psd.collection.isEmpty()) {
        m_currentStep++; // skip step 6
        cleanupAfterUpload();
        return;
    }

    calculateProgress(100);

    // step 6 delete items completely from the server
    emit progressStatus(i18n("Remove files from the Zotero server"));
    m_currentStep++;

    calculateProgress(0);

    if(m_cancel) {
        cleanupAfterUpload();
    }

    // now get all the entries that we need to remove from the server completely
    // this is only the case for items removed from a syncprovider that does not relate to a specific collection
    // after all we might have this item in other colletions

    QString query = "select DISTINCT ?r where {"
                    "?r a sync:ServerSyncData ."
                    "?r sync:provider ?provider . FILTER regex(?provider, \"" + m_psd.providerInfo->providerId() + "\") "
                    "?r sync:userId ?userId . FILTER regex(?userId, \"" + m_psd.userName + "\") "
                    "?r sync:url ?url . FILTER regex(?url, \"" + m_psd.url + "\") "
                    "{"
                        "?r sync:syncDataType sync:Attachment ."
                        " Optional {?r sync:attachment ?attachment . } FILTER (!bound(?attachment) )"
                    "}"
                    "UNION"
                    "{"
                        "?r sync:syncDataType sync:Note ."
                        "Optional {?r sync:note ?note . } FILTER (!bound(?note) )"
                    "}"
                    "UNION"
                    "{"
                        "?r sync:syncDataType sync:BibResource ."
                        "Optional {?r nbib:reference ?reference . } FILTER (!bound(?reference) )"
                    "}"
                    "}";

    QList<Nepomuk::Query::Result> queryResult = Nepomuk::Query::QueryServiceClient::syncSparqlQuery(query);

    calculateProgress(50);

    if(queryResult.isEmpty()) { cleanupAfterUpload(); return; }

    kDebug() << queryResult.size() << "items must be removed from the server completely";

    m_tmpUserDeleteRequest.clear();
    foreach(const Nepomuk::Query::Result &nqr, queryResult) {
        SyncDetails sd;
        sd.syncResource = nqr.resource();
        m_tmpUserDeleteRequest.append(sd);
    }

    if(m_psd.askBeforeDeletion) {
        // ask user if he really wants to remove them or download again next time
        // after this emit we wait for the slot call removeFilesFromZotero(...)
        emit askForServerDeletion(m_tmpUserDeleteRequest);
    }
    else {
        removeFilesFromZotero(true);
    }
}

void ZoteroUpload::removeFilesFromZotero(bool removeThem)
{
    if(removeThem) {
        QList<QPair<QString, QString> > idsToBeRemoved;
        foreach(SyncDetails sd, m_tmpUserDeleteRequest) {
            Nepomuk::Resource syncRes = sd.syncResource;
            QPair<QString, QString> item;
            item.first = syncRes.property(  SYNC::id() ).toString();
            item.second = syncRes.property(  SYNC::etag() ).toString();
            idsToBeRemoved.append(item);
            m_syncDataToBeRemoved.append(syncRes);

            QList<QUrl> resUri; resUri << sd.syncResource.uri();
            Nepomuk::removeResources(resUri);
        }

        connect(m_wtz, SIGNAL(finished()), this, SLOT(cleanupAfterUpload()));
        m_wtz->deleteItems(idsToBeRemoved);
    }
    else {
        // do not remove from server, but remove sync:ServerSyncData objects locally file will be downloaded again next time
        foreach(SyncDetails sd, m_tmpUserDeleteRequest) {
            if(m_cancel) { cleanupAfterUpload(); return; }

            QList<QUrl> resUri; resUri << sd.syncResource.uri();
            Nepomuk::removeResources(resUri);
        }

        cleanupAfterUpload();
    }
}

void ZoteroUpload::cleanupAfterUpload()
{
    calculateProgress(100);

    kDebug() << "cleanup and start attachment upload";
    foreach(Nepomuk::Resource r, m_syncDataToBeRemoved) {
        r.remove();
    }
    m_syncDataToBeRemoved.clear();
    m_tmpUserDeleteRequest.clear();
    delete m_uploadBibCache;

    m_wtz->deleteLater();
    m_wtz = 0;

    if(m_psd.exportAttachments && !m_attachmentMode) {
        startAttachmentUpload();
    }
    else {
        emit finished();
    }
}

void ZoteroUpload::startAttachmentUpload()
{
    m_attachmentMode = true;
    kDebug() << "start attachment upload ...";

    emit progressStatus(i18n("prepare attachments for Zotero upload"));

    m_currentStep++;
    calculateProgress(0);

    m_wtz = new WriteToZotero;
    m_wtz->setProviderSettings(m_psd);
    m_wtz->setAdoptBibtexTypes(true);
    connect(m_wtz, SIGNAL(progress(int)), this, SLOT(calculateProgress(int)));

    // select all nfo:RemoteDataObject that are connected to any nbib:Publication
    // of any nbib:Reference we want to update these are threated as "links" in zotero
    // actuall file uploads should be done in a second step

    QString query = "select DISTINCT ?r ?url where {"
//                    "{"
                    "?r a nfo:RemoteDataObject ."
//                    "}"
//                    "UNION"
//                    "{"
//                    "?r a nfo:FileDataObject ."
//                    "}"
                    "?r nbib:publishedAs ?x ."
                    "?x a nbib:Publication ."
                    "?r nie:url ?url ."
                    "}";

    QList<Nepomuk::Query::Result> queryResult = Nepomuk::Query::QueryServiceClient::syncSparqlQuery(query);

    QList<Nepomuk::Resource> exportList;
    foreach(const Nepomuk::Query::Result & r, queryResult) {
        exportList.append(r.resource());
    }

    if(m_cancel) { cleanupAfterUpload(); return; }

    // step 2 pipe to bibtex
    emit progressStatus(i18n("prepare %1 link attachments for the Zotero upload", exportList.size()));

    NepomukToBibTexPipe *ntbp = new NepomukToBibTexPipe;
    connect(ntbp, SIGNAL(progress(int)), this, SLOT(calculateProgress(int)));

    ntbp->setSyncDetails(m_psd.url, m_psd.userName);

    ntbp->addNepomukUries(true);
    ntbp->pipeExport(exportList);
    m_uploadBibCache = ntbp->bibtexFile();

    //BUG workaround that crash the system otherwise
    // no clue why but if we push m_bibCache into the WriteToZotero class @ m_wtz->pushItems(m_bibCache, m_psd.collection) the
    // QSharedPointers from m_bibCache get invalid and the dynamic_cast crash the system
    // therefore I need to create this twice ... as this function is fast enough its stil ok,
    // but must be fixed nonetheless
    delete ntbp;
    ntbp = new NepomukToBibTexPipe;
    ntbp->setSyncDetails(m_psd.url, m_psd.userName);

    ntbp->addNepomukUries(true);
    ntbp->pipeExport(exportList);
    File *itemsToPush = ntbp->bibtexFile();
    //end bug workaround

    // step 3 upload to zotero
    emit progressStatus(i18n("upload %1 link attachments to Zotero", exportList.size()));
    m_currentStep++;


    if(m_cancel) { cleanupAfterUpload(); return; }

    connect(m_wtz, SIGNAL(finished()), this, SLOT(readUploadSync()));
    connect(m_wtz, SIGNAL(entryItemUpdated(QString,QString,QString)), this, SLOT(updateSyncDetailsToNepomuk(QString,QString,QString)));

    m_wtz->pushItems(itemsToPush, m_psd.collection);
}

void ZoteroUpload::cancel()
{
    m_cancel = true;

    if(m_wtz) {
        m_wtz->cancelUpload();
    }
}

void ZoteroUpload::updateSyncDetailsToNepomuk(const QString &id, const QString &etag, const QString &updated)
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
        kDebug() << "could not find the right sync details for the current item query" << m_psd.providerInfo->providerId() << m_psd.userName << m_psd.collection << id;
        return;
    }

    Nepomuk::Resource syncDetails = results.first().resource();

    // update changed etag / modification date
    QList<QUrl> ssdUri; ssdUri << syncDetails.uri();
    QVariantList etagValue; etagValue << etag;
    QVariantList lmValue; lmValue << updated;
    Nepomuk::setProperty(ssdUri, SYNC::etag(), etagValue);
    Nepomuk::setProperty(ssdUri, NUAO::lastModification(), lmValue);
}

void ZoteroUpload::writeNewSyncDetailsToNepomuk(Entry *localData, const QString &id, const QString &etag, const QString &updated)
{
    // This one is only called when we upload data to the server
    // or if we fix a corrupted upload. (so we downloaded items we uploaded last time but couldn't add sync details before)
    // downloaded stuff is handled by the bibtexToNepomukPipe.cpp directly
    // So we know we must have a valid Nepomuk::Resource attached to the "localData" Entry

    // well we create a new ServerSyncData object to the entry

    Nepomuk::SimpleResourceGraph graph;
    Nepomuk::SYNC::ServerSyncData serverSyncData;

    serverSyncData.setUrl( m_psd.url );
    serverSyncData.setProvider( QString("zotero") );
    serverSyncData.setUserId( m_psd.userName );
    serverSyncData.setId( id );
    serverSyncData.setEtag( etag );
    serverSyncData.setProperty( NUAO::lastModification(), updated);

    // Now theck where this sync data belongs to
    if(localData->type() == QLatin1String("note")) {
        serverSyncData.setSyncDataType( SYNC::Note() );

        QString noteResourceUri = PlainTextValue::text(localData->value(QLatin1String("nepomuk-note-uri")));
        Nepomuk::SimpleResource noteRes( noteResourceUri );
        Nepomuk::PIMO::Note note(noteRes);
        //BUG we need to set some property otherwise the DataManagement server complains the resource is invalid
        QDateTime datetime = QDateTime::currentDateTimeUtc();
        noteRes.setProperty( NUAO::lastModification(), datetime.toString("yyyy-MM-ddTHH:mm:ssZ"));

        note.setProperty( SYNC::serverSyncData(), serverSyncData.uri() );
        serverSyncData.setNote(note.uri()  );

        graph << note;
    }
    else if(localData->type() == QLatin1String("attachment")) {
        serverSyncData.setSyncDataType( SYNC::Attachment() );

        QString attachmentResourceUri = PlainTextValue::text(localData->value(QLatin1String("nepomuk-note-uri")));
        Nepomuk::SimpleResource attachmentRes( attachmentResourceUri );
        Nepomuk::NFO::Document attachment(attachmentRes);
        //BUG we need to set some property otherwise the DataManagement server complains the resource is invalid
        QDateTime datetime = QDateTime::currentDateTimeUtc();
        attachmentRes.setProperty( NUAO::lastModification(), datetime.toString("yyyy-MM-ddTHH:mm:ssZ"));

        attachment.setProperty( SYNC::serverSyncData(), serverSyncData.uri() );
        serverSyncData.setAttachment( attachment.uri() );

        graph << attachment;
    }
    else { //BibReference
        serverSyncData.setSyncDataType( SYNC::BibResource() );

        QString pubUri = PlainTextValue::text(localData->value(QLatin1String("nepomuk-publication-uri")));

        Nepomuk::SimpleResource publicationRes( pubUri );
        Nepomuk::NBIB::Publication publication(publicationRes);
        //BUG we need to set some property otherwise the DataManagement server complains the resource is invalid
        QDateTime datetime = QDateTime::currentDateTimeUtc();
        publicationRes.setProperty( NUAO::lastModification(), datetime.toString("yyyy-MM-ddTHH:mm:ssZ"));

        publication.setProperty( SYNC::serverSyncData(), serverSyncData.uri() );
        serverSyncData.setProperty( SYNC::publication(), publication.uri() );

        graph << publication;

        QString refUri = PlainTextValue::text(localData->value(QLatin1String("nepomuk-reference-uri")));
        if(!refUri.isEmpty()) {
            Nepomuk::SimpleResource referenceRes( refUri );
            Nepomuk::NBIB::Reference reference(referenceRes);
            //BUG we need to set some property otherwise the DataManagement server complains the resource is invalid
            QDateTime datetime = QDateTime::currentDateTimeUtc();
            referenceRes.setProperty( NUAO::lastModification(), datetime.toString("yyyy-MM-ddTHH:mm:ssZ"));

            reference.setProperty( SYNC::serverSyncData(), serverSyncData.uri() );
            serverSyncData.setProperty( SYNC::reference(), reference.uri() );

            graph << reference;
        }
    }

    graph << serverSyncData;

    //blocking graph save
    Nepomuk::StoreResourcesJob *srj = Nepomuk::storeResources(graph, Nepomuk::IdentifyNone);
    if( !srj->exec() ) {
        kWarning() << "could not new ServerSyncData" << srj->errorString();
        return;
    }
}

void ZoteroUpload::calculateProgress(int value)
{
    qreal curProgress = ((qreal)value * 1.0/m_syncSteps);

    curProgress += (qreal)(100.0/m_syncSteps) * m_currentStep;

//    kDebug() << curProgress << m_currentStep << m_syncSteps;

    emit progress(curProgress);
}
