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

#include "varianttonepomukpipe.h"

#include "nbibio/provider/onlinestorage.h"
#include <nepomuk-webminer/publicationpipe.h>

#include <KDE/KJob>

#include <Nepomuk2/DataManagement>
#include <Nepomuk2/StoreResourcesJob>
#include <KDE/KComponentData>

#include "sro/sync/serversyncdata.h"
#include "sro/pimo/note.h"
#include "sro/nao/tag.h"
#include "sro/nfo/filedataobject.h"

#include "ontology/nbib.h"
#include "ontology/sync.h"
#include <Nepomuk2/Vocabulary/PIMO>
#include <Nepomuk2/Vocabulary/NIE>
#include <Nepomuk2/Vocabulary/NCO>
#include <Nepomuk2/Vocabulary/NFO>
#include <Nepomuk2/Vocabulary/NCAL>
#include <Nepomuk2/Vocabulary/NUAO>
#include <Soprano/Vocabulary/NAO>
#include <Soprano/Vocabulary/RDF>
#include <Nepomuk2/Variant>

#include <Nepomuk2/ResourceManager>
#include <Soprano/Model>
#include <Soprano/QueryResultIterator>
#include <Nepomuk2/DataManagement>

#include <KDE/KDebug>

#include <QtGui/QTextDocument>

using namespace Nepomuk2::Vocabulary;
using namespace Soprano::Vocabulary;

VariantToNepomukPipe::VariantToNepomukPipe(QObject *parent)
    : QObject(parent)
{
}

void VariantToNepomukPipe::pipeExport(QVariantList &publicationList)
{
    emit progress(0);

    //create the collection used for importing
    int maxValue = publicationList.size();
    qreal perResourceProgress = (100.0/(qreal)maxValue);
    qreal currentProgress = 0.0;

    // We must ensure that child notes (from zotero) are always added last, so the QVariantlist must be sorted
    foreach(const QVariant &v, publicationList) {
        QVariantMap publicationEntry = v.toMap();

        QString entryType = publicationEntry.value(QLatin1String("bibtexentrytype")).toString();

        // thats a special case. Zotero allows to store bibtex entries of type note
        // this is not the usual publication/reference but will be created as pimo:Note instead
        if( entryType == QLatin1String("note") ) {
            importNote(publicationEntry);
        }
        else if( entryType == QLatin1String("attachment") ) {
            importAttachment(publicationEntry);
        }
        // this handles all normal bibtex entries via MetaDataExtractors publication pipe
        else {
            QPair<QUrl,QUrl> importedPublication;

            // we have an existing resource just update the sync details
            if( publicationEntry.contains(QLatin1String("nepomuk-reference-uri")) ) {
                importedPublication.first = publicationEntry.value(QLatin1String("nepomuk-publication-uri")).toUrl();
                importedPublication.second = publicationEntry.value(QLatin1String("nepomuk-reference-uri")).toUrl();
            }
            else {
                NepomukWebMiner::Pipe::PublicationPipe publicationPipe;

                // either conquirere or one of the unittests (overrides "metadata-extractor" component)
                publicationPipe.overrideComponentName( KGlobal::mainComponent().componentName() );

                publicationPipe.setProjectPimoThing(m_projectThing);
                publicationPipe.pipeImport( publicationEntry );

                importedPublication = publicationPipe.importedPublication();
            }

            // do the sync part
            if( publicationEntry.contains(QLatin1String("sync-key")) ) {
                Nepomuk2::Resource main(importedPublication.first);
                Nepomuk2::Resource ref(importedPublication.second);
                addStorageSyncDetails(main, ref, publicationEntry);
            }
        }

        currentProgress += perResourceProgress;
        emit progress(currentProgress);
    }

    emit progress(100);
}

void VariantToNepomukPipe::setSyncStorageProvider(OnlineStorage *storage)
{
    m_storage = storage;
}

void VariantToNepomukPipe::setProjectPimoThing(Nepomuk2::Resource projectThing)
{
    m_projectThing = projectThing;
}

void VariantToNepomukPipe::importNote(const QVariantMap &noteEntry)
{
    Nepomuk2::Resource noteResource;
    if( !noteEntry.contains(QLatin1String("nepomuk-note-uri")) ) {
        Nepomuk2::SimpleResourceGraph graph;

        kDebug() << "import note " << noteEntry.value("sync-title").toString();

        QTextDocument content( noteEntry.value("note").toString() );
        if(content.isEmpty()) {
            return;
        }

        Nepomuk2::PIMO::Note note;
        note.addType(NIE::InformationElement());

        QString title = noteEntry.value(QLatin1String("title")).toString();
        if(title.isEmpty()) {
            title = noteEntry.value(QLatin1String("sync-title")).toString();
        }
        if(title.isEmpty()) {
            title = noteEntry.value(QLatin1String("sync-parent")).toString();
        }

        note.setProperty( NAO::prefLabel(), title );
        note.setProperty( NIE::title(), title );

        note.setProperty( NIE::plainTextContent(), content.toPlainText());
        note.setProperty( NIE::htmlContent(), content.toHtml());

        QStringList keywords = noteEntry.value(QLatin1String("keywords")).toString().split(";");
        foreach(const QString &tagText, keywords) {
            Nepomuk2::NAO::Tag tag;

            tag.addPrefLabel( tagText );
            tag.addProperty( NAO::identifier(), KUrl::fromEncoded(tagText.toLatin1()));

            note.addProperty( NAO::hasTag(), tag.uri());
            note.addProperty( NAO::hasSubResource(), tag.uri());

            graph << tag;
        }

        graph << note;

        Nepomuk2::StoreResourcesJob *srj = Nepomuk2::storeResources(graph,Nepomuk2::IdentifyNew, Nepomuk2::OverwriteProperties);
        if(!srj->exec()) {
            kDebug() << "Import error :: " << srj->errorString();
        }
        QUrl noteUrl = srj->mappings().value( note.uri() );
        noteResource = Nepomuk2::Resource(noteUrl);
    }
    else {
        noteResource = Nepomuk2::Resource(noteEntry.value(QLatin1String("nepomuk-note-uri")).toUrl());
    }

    // now get the uri of the newly created attachment and add the sync details to it
    // and if necessary the parent/child connection
    if(noteEntry.contains(QLatin1String("sync-key"))) {
        addStorageSyncDetails(noteResource, noteEntry);
    }
}

void VariantToNepomukPipe::importAttachment(const QVariantMap &attachmentEntry)
{
    kDebug() << "import attachment" << attachmentEntry.value("title").toString();

    Nepomuk2::Resource attachmentResource;

    if( !attachmentEntry.contains(QLatin1String("nepomuk-attachment-uri")) ) {
        Nepomuk2::SimpleResourceGraph graph;

        Nepomuk2::NFO::FileDataObject attachment;
        attachment.addType(NFO::Document());

        attachment.setProperty( NIE::title(), attachmentEntry.value("title").toString() );

        if(attachmentEntry.contains("sync-attachment-file")) {

            //FIXME: emit download details (status speed/progress etc)
            KUrl downloadedFile = m_storage->downloadFile( attachmentEntry.value("sync-attachment-file").toString(),
                                                           attachmentEntry.value("sync-title").toString()); //TODO: check if filename is always in sync-title or just in Zotero

            //        QString localFilePath = attachmentEntry.value("localFile").toString();
            //        localFilePath.prepend(QLatin1String("file://"));
            attachment.setProperty( NIE::url(), downloadedFile);
            attachment.setProperty( NAO::identifier(), downloadedFile);
        }
        else {
            attachment.addType(NFO::RemoteDataObject());
            QString url = attachmentEntry.value("url").toString();
            attachment.setProperty( NIE::url(), url);
            attachment.setProperty( NAO::identifier(), url);

            kDebug() << "create attachment with url" << url;
        }

        QStringList keywords = attachmentEntry.value(QLatin1String("keywords")).toString().split(";");
        foreach(const QString &tagText, keywords) {
            Nepomuk2::NAO::Tag tag;

            tag.addPrefLabel( tagText );
            tag.addProperty( NAO::identifier(), KUrl::fromEncoded(tagText.toLatin1()));

            attachment.addProperty( NAO::hasTag(), tag.uri());
            attachment.addProperty( NAO::hasSubResource(), tag.uri());

            graph << tag;
        }

        QString accessdate = attachmentEntry.value("accessdate").toString();
        QDateTime dateTime = NepomukWebMiner::Pipe::NepomukPipe::createDateTime( accessdate );

        if(dateTime.isValid()) {
            QString date = dateTime.toString(Qt::ISODate);
            attachment.setProperty( NUAO::lastUsage(), date);
        }
        else {
            kDebug() << "could not parse accessdate" << accessdate;
        }

        QString comment = attachmentEntry.value("note").toString();
        attachment.setProperty( NIE::comment(), comment);

        graph << attachment;

        Nepomuk2::StoreResourcesJob *srj = Nepomuk2::storeResources(graph,Nepomuk2::IdentifyNew, Nepomuk2::OverwriteProperties);
        if(!srj->exec()) {
            kDebug() << srj->errorString();
        }
        QUrl attachmentUrl = srj->mappings().value( attachment.uri() );
        attachmentResource = Nepomuk2::Resource(attachmentUrl);
    }
    else {
        attachmentResource = Nepomuk2::Resource(attachmentEntry.value(QLatin1String("nepomuk-attachment-uri")).toUrl());
    }

    // now get the uri of the newly created attachment and add the sync details to it
    // and if necessary the parent/child connection
    if(attachmentEntry.contains(QLatin1String("sync-key"))) {
        addStorageSyncDetails(attachmentResource, attachmentEntry);
    }
}

void VariantToNepomukPipe::addStorageSyncDetails(Nepomuk2::Resource &publicationResource, Nepomuk2::Resource &referenceResource, const QVariantMap &item)
{
    Q_ASSERT_X(m_storage,"Add Sync Details","no online storage set, can't add Sync details");
    ProviderSyncDetails psd = m_storage->providerSettings();

    kDebug() << "add zotero sync details for the publication/reference";
    Nepomuk2::SimpleResourceGraph graph;

    QString id = item.value(QLatin1String("sync-key")).toString();
    QString etag = item.value(QLatin1String("sync-etag")).toString();
    QString updated = item.value(QLatin1String("sync-updated")).toString();

    Nepomuk2::SYNC::ServerSyncData serverSyncData;
    serverSyncData.addType(NIE::DataObject());

    // first set an identifier, when the object already exist we merge them together
    QString identifier = psd.providerId + psd.userName + psd.url + id;
    serverSyncData.addProperty( NAO::identifier(), KUrl::fromEncoded( identifier.toLatin1()) );

    // now we set the new values
    serverSyncData.setProvider( psd.providerId );
    serverSyncData.setUrl( psd.url );
    serverSyncData.setUserId( psd.userName );
    serverSyncData.setId( id );
    serverSyncData.setEtag( etag );
    serverSyncData.setProperty( NUAO::lastModification(), updated);

    serverSyncData.setSyncDataType( SYNC::BibResource() );                   // here mainResource is a nbib:Publication and referenceResource the nbib:Reference

    //publicationResource.setProperty(SYNC::serverSyncData(), serverSyncData );
    serverSyncData.setPublication( publicationResource.uri() );

    //referenceResource.setProperty(SYNC::serverSyncData(), serverSyncData );
    serverSyncData.setReference( referenceResource.uri() );

    graph << serverSyncData;


    Nepomuk2::StoreResourcesJob *srj = Nepomuk2::storeResources(graph,Nepomuk2::IdentifyNew, Nepomuk2::OverwriteProperties);
    if(!srj->exec()) {
        kDebug() << srj->errorString();
    }
    else {
        kDebug() << "Publication sync resource created sucessfully";

        QUrl syncUrl = srj->mappings().value( serverSyncData.uri() );

        // link back publication and reference to the newly created sync object.
        // seems this can't be done in one go, as the DMS complains when creating SimpleResources with the existing nbib:Publication/nbib:Reference
        // during the save.
        //TODO: check if this two stage resource ading is really necessary or using existing resource in SimpleResource should work
        KJob *job = Nepomuk2::addProperty(QList<QUrl>() << publicationResource.uri() << referenceResource.uri(),SYNC::serverSyncData(), QVariantList() << syncUrl);
        if(!job->exec()) {
            kDebug() << job->errorString();
        }
    }
}

void VariantToNepomukPipe::addStorageSyncDetails(Nepomuk2::Resource attachment, const QVariantMap &item)
{
    Q_ASSERT_X(m_storage,"Add Sync Details","no online storage set, can't add Sync details");
    ProviderSyncDetails psd = m_storage->providerSettings();

    kDebug() << "add zotero sync details for the attachment";

    QString id = item.value(QLatin1String("sync-key")).toString();
    QString etag = item.value(QLatin1String("sync-etag")).toString();
    QString updated = item.value(QLatin1String("sync-updated")).toString();
    QString parentId = item.value(QLatin1String("sync-parent")).toString();
    QString itemType = item.value(QLatin1String("bibtexentrytype")).toString();

    Nepomuk2::SimpleResourceGraph graph;

    Nepomuk2::SYNC::ServerSyncData serverSyncData;
    serverSyncData.addType(NIE::DataObject());

    // first set an identifier, when the object already exist we merge them together
    QString identifier = psd.providerId + psd.userName + psd.url + id;
    serverSyncData.addProperty( NAO::identifier(), KUrl::fromEncoded( identifier.toLatin1()) );

    // now we set the new values
    serverSyncData.setProvider( psd.providerId );
    serverSyncData.setUrl( psd.url );
    serverSyncData.setUserId( psd.userName );
    serverSyncData.setId( id );
    serverSyncData.setEtag( etag );
    serverSyncData.setProperty( NUAO::lastModification(), updated);

    // now depending on what kind of mainResource we have, we set a different SyncDataType
    // helps to find the right data later on again and create the right links between Resource and syncData
    if(itemType == QLatin1String("note")) {
        serverSyncData.setSyncDataType( SYNC::Note() );
        serverSyncData.setNote( attachment.uri() );
    }
    else if(itemType == QLatin1String("attachment")) {
        serverSyncData.setSyncDataType( SYNC::Attachment() );
        serverSyncData.setAttachment( attachment.uri() );
    }

    graph << serverSyncData;

    // save graph
    Nepomuk2::StoreResourcesJob *srj = Nepomuk2::storeResources(graph,Nepomuk2::IdentifyNew, Nepomuk2::OverwriteProperties);
    if(!srj->exec()) { kDebug() << srj->errorString(); return; }

    // add  backlinks
    QUrl syncUrl = srj->mappings().value( serverSyncData.uri() );
    KJob *job = Nepomuk2::addProperty(QList<QUrl>() << attachment.uri(), SYNC::serverSyncData(), QVariantList() << syncUrl);
    if(!job->exec()) { kDebug() << job->errorString(); return; }

    //----------------------------------------------------------------------------
    // If note/attachment is not the child of a publication/reference
    // we stop here. This can happen, but not that often.
    //----------------------------------------------------------------------------
    if( parentId.isEmpty() ) {
        kDebug() << "no parent id available";
        return;
    }
    else {
        kDebug() << "parent found, try to add it";
    }

    //----------------------------------------------------------------------------
    // Add note/file attachment as a parent child to the publication/reference.
    // This is nearly always the case for zotero
    //----------------------------------------------------------------------------

    //now we do have a parent id, so find the parent and make the important isRealated relation
    QString query = "select DISTINCT ?r where {  "
                     "?r a sync:ServerSyncData . "
                     "?r sync:provider ?provider . FILTER regex(?provider, \"" + psd.providerId + "\") "
                     "?r sync:userId ?userId . FILTER regex(?userId, \"" + psd.userName + "\") "
                     "?r sync:url ?url . FILTER regex(?url, \"" + psd.url + "\") "
                     "?r sync:id ?id . FILTER regex(?id, \""+ parentId + "\") "
                     "}";

    Soprano::Model* model = Nepomuk2::ResourceManager::instance()->mainModel();
    Soprano::QueryResultIterator it = model->executeQuery( query, Soprano::Query::QueryLanguageSparql );

    QList<Nepomuk2::Resource> results;
    while( it.next() ) {
        Soprano::BindingSet p = it.current();
        results << Nepomuk2::Resource( QUrl(p.value(QLatin1String("r")).toString()) );
    }

    if(results.size() > 1 || results.isEmpty()) {
        kDebug() << "could not find the right sync details for the current parent item query" << "zotero" << psd.userName << psd.url << parentId;
        return;
    }

    Nepomuk2::Resource parentSyncResourceNepomuk = results.first();
        // here we add the information that a note is related (a child) of the reference and the publication.
        // and that the attachment (nfo:FileDataObject) is the publicationOf/publishedAs of the nbib:Publication

    if( attachment.hasType(PIMO::Note()) ) {
        kDebug() << "add parent to pimo:Note";
        // connect Note <-> publication
        Nepomuk2::Resource parentPublication = parentSyncResourceNepomuk.property(SYNC::publication()).toResource();
        Nepomuk2::addProperty(QList<QUrl>() << parentPublication.uri(), NAO::hasSubResource(), QVariantList() << attachment.uri()); //delete note when publication is deleted
        Nepomuk2::addProperty(QList<QUrl>() << attachment.uri(), NAO::isRelated(), QVariantList() << parentPublication.uri());

        // connect Note <-> reference
        //TODO: check if a zotero Note should be related to a reference too and not only the publication
        //            Nepomuk2::Resource parentReference = parentSyncResourceNepomuk.property(SYNC::reference()).toResource();
        //            Nepomuk2::addProperty(QList<QUrl>() << parentReference.uri(), NAO::hasSubResource(), QVariantList() << attachment.uri()); //delete note when publication is deleted
        //            Nepomuk2::addProperty(QList<QUrl>() << attachment.uri(), NAO::hasSubResource(), QVariantList() << parentReference.uri());
    }
    else if( attachment.hasType(NFO::Document()) ) {
        kDebug() << "add parent to file attachment";
        Nepomuk2::Resource parentPublication = parentSyncResourceNepomuk.property(SYNC::publication()).toResource();

        KJob *job1 = Nepomuk2::addProperty(QList<QUrl>() << parentPublication.uri(), NBIB::isPublicationOf(), QVariantList() << attachment.uri());
        if(!job1->exec()) { kDebug() << job1->errorString(); return; }
        KJob *job2 =  Nepomuk2::addProperty(QList<QUrl>() << attachment.uri(), NBIB::publishedAs(), QVariantList() << parentPublication.uri());
        if(!job2->exec()) { kDebug() << job2->errorString(); return; }
        KJob *job3 = Nepomuk2::addProperty(QList<QUrl>() << parentPublication.uri(), NAO::hasSubResource(), QVariantList() << attachment.uri()); // delete file when publication is deleted
        if(!job3->exec()) { kDebug() << job3->errorString(); return; }
        KJob *job4 = Nepomuk2::addProperty(QList<QUrl>() << attachment.uri(), NAO::isRelated(), QVariantList() << parentPublication.uri());
        if(!job4->exec()) { kDebug() << job4->errorString(); return; }
    }
    else {
        kDebug() << "can't find type of the attachment!";
    }

    // this creates the link for the syncResources so we know how they are connected
    // connect child syncDetails to its parent syncDetails
    KJob *job2 = Nepomuk2::addProperty(QList<QUrl>() << syncUrl, NAO::isRelated(), QVariantList() << parentSyncResourceNepomuk.uri());
    if(!job2->exec()) { kDebug() << job2->errorString(); return; }
    KJob *job3 = Nepomuk2::addProperty(QList<QUrl>() << parentSyncResourceNepomuk.uri(), NAO::isRelated(), QVariantList() << syncUrl);
    if(!job3->exec()) { kDebug() << job3->errorString(); return; }
}


