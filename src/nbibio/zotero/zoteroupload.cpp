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

void ZoteroUpload::startUpload()
{
    Q_ASSERT( m_systemLibrary != 0);

    m_syncSteps = 6;
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

    Nepomuk::Query::AndTerm andTerm;
    andTerm.addSubTerm(  Nepomuk::Query::ResourceTypeTerm(  NBIB::Reference() ) );

    // if we sync with a Library project restrict the nepomuk resources
    // to only thouse in this project
    if(m_libraryToSyncWith->libraryType() == Library_Project) {
        Nepomuk::Query::OrTerm orTerm;
        orTerm.addSubTerm( Nepomuk::Query::ComparisonTerm(  PIMO::isRelated(),
                           Nepomuk::Query::ResourceTerm(m_libraryToSyncWith->settings()->projectThing() )));
        andTerm.addSubTerm(orTerm);
    }

    Nepomuk::Query::Query query( andTerm );
    QList<Nepomuk::Query::Result> queryResult = Nepomuk::Query::QueryServiceClient::syncQuery(query);

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
    m_bibCache = ntbp->bibtexFile();

    //BUG workaround that carsh the system otherwise
    // no clue why put if we push m_bibCache into the WriteToZotero class @ m_wtz->pushItems(m_bibCache, m_psd.collection) the
    // QSharedPointers from m_bibCache get invalid and the dynamic_csat crash the system
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
        foreach(const QSharedPointer<Element> &localElement, *m_bibCache) {
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
                if(i.key().startsWith( QLatin1String("keywords") )) // might fail because we don#t keep the same order
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
                m_bibCache->removeAll(localElement); // remove from local storage, so we don't check it again
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
        // but there is no way to tell if we just removed one item from a group earlyer or if it was never in this group bef
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
            sd.syncResource.remove();
        }

        connect(m_wtz, SIGNAL(finished()), this, SLOT(removeFilesFromZotero()));
        m_wtz->removeItemsFromCollection(idsToBeRemoved, m_psd.collection);
    }
    else {
        // do not remove from server, but remove sync:ServerSyncData objects locally file will be downloaded again next time

        foreach(SyncDetails sd, m_tmpUserDeleteRequest) {
            if(m_cancel) { cleanupAfterUpload(); return; }

            sd.syncResource.remove();
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
    kDebug() << "remove " << m_tmpUserDeleteRequest.size() << "items?" << removeThem;
    if(removeThem) {
        QList<QPair<QString, QString> > idsToBeRemoved;
        foreach(SyncDetails sd, m_tmpUserDeleteRequest) {
            Nepomuk::Resource syncRes = sd.syncResource;
            QPair<QString, QString> item;
            item.first = syncRes.property(  SYNC::id() ).toString();
            item.second = syncRes.property(  SYNC::etag() ).toString();
            idsToBeRemoved.append(item);
            m_syncDataToBeRemoved.append(syncRes);

            sd.syncResource.remove();
        }

        connect(m_wtz, SIGNAL(finished()), this, SLOT(cleanupAfterUpload()));
        m_wtz->deleteItems(idsToBeRemoved);
    }
    else {

        // do not remove from server, but remove sync:ServerSyncData objects locally file will be downloaded again next time
        foreach(SyncDetails sd, m_tmpUserDeleteRequest) {
            if(m_cancel) { cleanupAfterUpload(); return; }

            sd.syncResource.remove();
        }

        cleanupAfterUpload();
    }
}

void ZoteroUpload::cleanupAfterUpload()
{
    kDebug() << "cleanup and start child / note upload";
    foreach(Nepomuk::Resource r, m_syncDataToBeRemoved) {
        r.remove();
    }
    m_syncDataToBeRemoved.clear();
    m_tmpUserDeleteRequest.clear();

    m_wtz->deleteLater();
    m_wtz = 0;

    emit finished();
}

void ZoteroUpload::startAttachmentUpload()
{

}

void ZoteroUpload::uploadNextAttachment()
{

}

void ZoteroUpload::cancel()
{
    m_cancel = true;

    if(m_wtz)
        m_wtz->cancelUpload();
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
    syncDetails.setProperty( SYNC::etag(), etag);
    syncDetails.setProperty( NUAO::lastModification(), updated);
}

Nepomuk::Resource ZoteroUpload::writeNewSyncDetailsToNepomuk(Entry *localData, const QString &id, const QString &etag, const QString &updated)
{
    kDebug() << "local entry" << localData->type() << id;
    // This one is only called when we upload new data to the server
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

//QSharedPointer<Element> ZoteroUpload::transformAttachmentToBibTeX(Nepomuk::Resource resource)
//{

//}

void ZoteroUpload::calculateProgress(int value)
{
    qreal curProgress = ((qreal)value * 1.0/m_syncSteps);

    curProgress += (qreal)(100.0/m_syncSteps) * m_currentStep;

//    kDebug() << curProgress << m_syncSteps << m_currentStep;

    emit progress(curProgress);
}
