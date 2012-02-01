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

#include "zoterodownload.h"
#include "zoteroupload.h"

#include <QtCore/QTimer>

SyncZoteroNepomuk::SyncZoteroNepomuk(QObject *parent)
    : NBibSync(parent)
    , m_syncMode(false)
    , m_cancel(false)
    , m_zoteroDownload(0)
    , m_zoteroUpload(0)
{
}

SyncZoteroNepomuk::~SyncZoteroNepomuk()
{
//    delete m_zoteroDownload;
    /*
    delete m_rfz;
    delete m_wtz;
    delete m_btnp;
    delete m_ntbp;
    */
}

void SyncZoteroNepomuk::startDownload()
{
    if(!m_syncMode) {
        m_syncSteps = 2;
        m_curStep = 0;
    }
    else {
//        m_curStep++;
    }

    m_zoteroDownload = new ZoteroDownload;

    m_zoteroDownload->setProviderDetails(m_psd);
    m_zoteroDownload->setSystemLibrary(m_systemLibrary);
    m_zoteroDownload->setLibraryToSyncWith(m_libraryToSyncWith);

    //m_zoteroDownload->setCorruptedUploadEntries(File *corruptedOnUpload);

    connect(m_zoteroDownload, SIGNAL(progress(int)), this, SLOT(calculateProgress(int)));
    connect(m_zoteroDownload, SIGNAL(progressStatus(QString)), this, SIGNAL(progressStatus(QString)));
    connect(m_zoteroDownload, SIGNAL(askForLocalDeletion(QList<SyncDetails>)), this, SIGNAL(askForLocalDeletion(QList<SyncDetails>)));
    connect(m_zoteroDownload, SIGNAL(userMerge(QList<SyncDetails>)), this, SIGNAL(userMerge(QList<SyncDetails>)));
    connect(m_zoteroDownload, SIGNAL(finished()), this, SLOT(downloadFinished()));

    m_zoteroDownload->startDownload();
}

void SyncZoteroNepomuk::mergeFinished()
{
    m_zoteroDownload->mergeFinished();
}

void SyncZoteroNepomuk::downloadFinished()
{
    kDebug() << "download finished";

    delete m_zoteroDownload;
    m_zoteroDownload = 0;

    if(m_syncMode) {
        startUpload();
    }
}

void SyncZoteroNepomuk::uploadFinished()
{
    kDebug() << "upload finished";

    delete m_zoteroUpload;
    m_zoteroUpload = 0;
}

void SyncZoteroNepomuk::startAttachmentDownload()
{
    /*
    kDebug() << "start with attachment download";
    m_attachmentMode = true;

    m_rfz = new ReadFromZotero;
    m_rfz->setProviderSettings(m_psd);
    m_rfz->setAdoptBibtexTypes(true);
    connect(m_rfz, SIGNAL(progress(int)), this, SLOT(calculateProgress(int)));

    // if we started the download independently, initialise sync steps for progress calculation
    if(m_curStep == 0) {
        m_syncSteps = 4;
    }
    else {
        m_curStep++;
    }

    emit progressStatus(i18n("Start to download attachments"));

    // restrict zotero download to notes and attachments
//    m_rfz->setSearchFilter(QLatin1String("&itemType=note%20||%20attachment"));
    m_rfz->setSearchFilter(QLatin1String("&itemType=note%20"));

    connect(m_rfz, SIGNAL(itemsInfo(File)), this, SLOT(readDownloadSync(File)));
    m_rfz->fetchItems(m_psd.collection);
    */
}

void SyncZoteroNepomuk::startUpload()
{
    if(!m_syncMode) {
        m_syncSteps = 1;
        m_curStep = 0;
    }
    else {
        m_curStep++;
    }

    m_zoteroUpload = new ZoteroUpload;

    m_zoteroUpload->setProviderDetails(m_psd);
    m_zoteroUpload->setSystemLibrary(m_systemLibrary);
    m_zoteroUpload->setLibraryToSyncWith(m_libraryToSyncWith);

    //m_zoteroDownload->setCorruptedUploadEntries(File *corruptedOnUpload);

    connect(m_zoteroUpload, SIGNAL(progress(int)), this, SLOT(calculateProgress(int)));
    connect(m_zoteroUpload, SIGNAL(progressStatus(QString)), this, SIGNAL(progressStatus(QString)));

    connect(m_zoteroUpload, SIGNAL(askForServerDeletion(QList<SyncDetails>)), this, SIGNAL(askForServerDeletion(QList<SyncDetails>)));
    connect(m_zoteroUpload, SIGNAL(askForGroupRemoval(QList<SyncDetails>)), this, SIGNAL(askForGroupRemoval(QList<SyncDetails>)));

    connect(m_zoteroUpload, SIGNAL(finished()), this, SLOT(uploadFinished()));

    m_zoteroUpload->startUpload();
}

void SyncZoteroNepomuk::startAttachmentUpload()
{
    /*
    Q_ASSERT( m_systemLibrary != 0);

    m_attachmentMode = true;

    if(!m_libraryToSyncWith)
        m_libraryToSyncWith = m_systemLibrary;

    m_wtz = new WriteToZotero;
    m_wtz->setProviderSettings(m_psd);
    m_wtz->setAdoptBibtexTypes(true);
    connect(m_wtz, SIGNAL(progress(int)), this, SLOT(calculateProgress(int)));

    // step 1 fetch data from nepomuk
    emit progressStatus(i18n("fetch attachment data from Nepomuk"));

    // if we do not use syncmode or as a followup to normal upload sync, we initialize the syncSteps here
    // they are used to properly calculate the current progress
    if(m_curStep == 0) {
        m_syncSteps = 6;
    }
    else {
        m_curStep++;
    }

    calculateProgress(0);

    // now fetch all pimo:Notes connect to the references
    // and all nfo:FileDataObject resources (local and remote) of the publication

    QString projectFilter;
    if( m_libraryToSyncWith->libraryType() == Library_Project) {
        projectFilter =  "?x pimo:isRelated <" + m_libraryToSyncWith->settings()->projectThing().resourceUri().toString() + "> . ";
    }

    QString query = "select ?r where {"
                    "?x a nbib:Reference ."
                    + projectFilter +
//                    "{"
                       "?x nao:isRelated ?r . "
                       "?r a pimo:Note ."
//                    "}"
//                     "UNION"
//                  "{"
//                       "?x nbib:publication ?p ."
//                       "?p nbib:isPublicationOf ?r ."
//                       "?r a nfo:FileDataObject ."
//                    "}"
                    "}";

    QList<Nepomuk::Query::Result> queryResult = Nepomuk::Query::QueryServiceClient::syncSparqlQuery(query);

    if(queryResult.isEmpty()) {
        kDebug() << "no attachments found that need to be uploaded";
        m_curStep++; //skip prepare data step
        m_curStep++; // skip upload attachment
        cleanupAfterUpload();
        return;
    }

    // step 2 transform into bibtex style
    emit progressStatus(i18n("prepare attachment data for Zotero"));
    m_curStep++;

    m_attachmentsToUpload.clear();
    foreach(const Nepomuk::Query::Result & r, queryResult) {
        m_attachmentsToUpload.append( transformAttachmentToBibTeX(r.resource()) );
    }
    kDebug() << "start uploading " << m_attachmentsToUpload.size() << "attachments";

    // step 3 upload to zotero
    emit progressStatus(i18n("upload attachments to Zotero"));
    m_curStep++;

    if(m_cancel) {
        readUploadSync(); // cancel and cleanup
    }

    connect(m_wtz, SIGNAL(finished()), this, SLOT(uploadNextAttachment()));
    connect(m_wtz, SIGNAL(entryItemUpdated(QString,QString,QString)), this, SLOT(updateSyncDetailsToNepomuk(QString,QString,QString)));

    File *narf = new File(); // ... not good but get problems with dereferencing qsharedpointern otherwise..
    narf->append(m_attachmentsToUpload.takeFirst());
    m_wtz->pushItems(*narf, m_psd.collection);
    */
}

void SyncZoteroNepomuk::uploadNextAttachment()
{
    /*
    if(m_attachmentsToUpload.isEmpty()) {
        cleanupAfterUpload();
    }
    else {
        File *narf = new File();
        narf->append(m_attachmentsToUpload.takeFirst());
        m_wtz->pushItems(*narf, m_psd.collection);
    }
    */
}

void SyncZoteroNepomuk::startSync()
{
    m_syncMode = true;
    m_syncSteps = 2;
    m_curStep = 0;
    startDownload();
}

void SyncZoteroNepomuk::cancel()
{
    if(m_zoteroDownload) {
        m_zoteroDownload->cancel();
    }
    if(m_zoteroUpload) {
        m_zoteroUpload->cancel();
    }

    m_cancel = true;
}

void SyncZoteroNepomuk::deleteLocalFiles(bool deleteThem)
{
    m_zoteroDownload->deleteLocalFiles(deleteThem);
}

void SyncZoteroNepomuk::deleteServerFiles(bool deleteThem)
{
    m_zoteroUpload->removeFilesFromZotero(deleteThem);
}

void SyncZoteroNepomuk::deleteFromGroup(bool deleteThem)
{
    m_zoteroUpload->removeFilesFromGroup(deleteThem);
}

//QSharedPointer<Element> SyncZoteroNepomuk::transformAttachmentToBibTeX(Nepomuk::Resource resource)
//{
//    /*
//    Entry *newEntry = new Entry;

//    if(!resource.hasType(PIMO::Note())) {
//        qWarning() << resource.type() << "resource not supported by the zotero api currently, can only upload pimo:Notes";
//    }

//    // as for pimo note, I simply need the text as the title will be generated automatically from it :(
//    // so thanks to zotero wie will very likely loose information here

//    newEntry->setType(QLatin1String("note"));

//    // note content
//    Value content;
//    QString noteContent = resource.property(NIE::plainTextContent()).toString();
//    content.append(QSharedPointer<ValueItem>( new PlainText(noteContent)));
//    newEntry->insert(QLatin1String("note"), content);

//    // the tags
//    QList<Nepomuk::Tag> tags = resource.tags();

//    Value v;
//    foreach(const Nepomuk::Tag & tag, tags) {
//        Keyword *p = new Keyword(tag.genericLabel());
//        v.append(QSharedPointer<ValueItem>(p));
//    }

//    if(!v.isEmpty()) {
//        newEntry->insert(Entry::ftKeywords, v);
//    }

//    // if available zotero details
//    QList<Nepomuk::Resource> sycList = resource.property(SYNC::serverSyncData()).toResourceList();

//    if(!sycList.isEmpty()) {
//        // only add the sync details of the right storage
//        foreach(const Nepomuk::Resource &r, sycList) {
//            if(r.property(SYNC::provider()).toString() != QString("zotero")) {
//                continue;
//            }
//            if(r.property(SYNC::userId()).toString() != m_psd.userName) {
//                continue;
//            }
//            if(r.property(SYNC::url()).toString() != m_psd.url) {
//                continue;
//            }

//            //now we have the right object, write down sync details
//            QString etag = r.property(SYNC::etag()).toString();
//            Value v1;
//            v1.append(QSharedPointer<ValueItem>(new PlainText(etag)));
//            newEntry->insert(QLatin1String("zoteroEtag"), v1);

//            QString key = r.property(SYNC::id()).toString();
//            Value v2;
//            v2.append(QSharedPointer<ValueItem>(new PlainText(key)));
//            newEntry->insert(QLatin1String("zoteroKey"), v2);

//            QString updated = r.property(NUAO::lastModification()).toString();
//            Value v3;
//            v3.append(QSharedPointer<ValueItem>(new PlainText(updated)));
//            newEntry->insert(QLatin1String("zoteroUpdated"), v3);

//            // now find the parent item we want to put this info in

//            Nepomuk::Resource parentSyncResource = r.property(NAO::isRelated()).toResource();
//            QString parentID = parentSyncResource.property(SYNC::id()).toString();
//            Value v4;
//            v4.append(QSharedPointer<ValueItem>(new PlainText(parentID)));
//            newEntry->insert(QLatin1String("zoteroParent"), v4);

//            kDebug() << "found sync resource with parent" << parentID;

//            break;
//        }
//    }
//    else {
//        //BUg will horrible fail if the note is attached to more than 1 reference
//        Nepomuk::Resource parentResource = resource.property(NAO::isRelated()).toResource();
//        sycList = parentResource.property(SYNC::serverSyncData()).toResourceList();

//        // only add the sync details of the right storage
//        foreach(const Nepomuk::Resource &r, sycList) {
//            if(r.property(SYNC::provider()).toString() != QString("zotero")) {
//                continue;
//            }
//            if(r.property(SYNC::userId()).toString() != m_psd.userName) {
//                continue;
//            }
//            if(r.property(SYNC::url()).toString() != m_psd.url) {
//                continue;
//            }

//            QString parentID = r.property(SYNC::id()).toString();
//            Value v4;
//            v4.append(QSharedPointer<ValueItem>(new PlainText(parentID)));
//            newEntry->insert(QLatin1String("zoteroParent"), v4);

//            kDebug() << "found parent" << parentID << "for new note";
//        }
//    }

//    QSharedPointer<Element> element = QSharedPointer<Element>(newEntry);

//    return element;
//    */
//}
