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

#include <nepomukmetadataextractor/publicationpipe.h>

#include <KDE/KJob>

#include <Nepomuk2/DataManagement>
#include <Nepomuk2/SimpleResource>
#include <Nepomuk2/StoreResourcesJob>
#include <KDE/KComponentData>

#include "sro/pimo/note.h"
#include "sro/nao/tag.h"
#include "sro/nfo/filedataobject.h"

#include "nbib.h"
#include "sync.h"
#include <Nepomuk2/Vocabulary/PIMO>
#include <Nepomuk2/Vocabulary/NIE>
#include <Nepomuk2/Vocabulary/NCO>
#include <Nepomuk2/Vocabulary/NFO>
#include <Nepomuk2/Vocabulary/NCAL>
#include <Nepomuk2/Vocabulary/NUAO>
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk2/Variant>

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
        else {
            NepomukMetaDataExtractor::Pipe::PublicationPipe publicationPipe;

            // either conquirere or one of the unittests
            publicationPipe.overrideComponentName( KGlobal::mainComponent().componentName() );

            publicationPipe.setProjectPimoThing(m_projectThing);
            publicationPipe.pipeImport( publicationEntry );

            QPair<QUrl,QUrl> importedPublication = publicationPipe.importedPublication();

            // do the sync part
        }

        currentProgress += perResourceProgress;
        emit progress(currentProgress);
    }

    emit progress(100);

}

void VariantToNepomukPipe::setSyncDetails(const QString &url, const QString &userid)
{
    m_syncUrl = url;
    m_syncUserId = userid;
}

void VariantToNepomukPipe::setProjectPimoThing(Nepomuk2::Resource projectThing)
{
    m_projectThing = projectThing;
}

void VariantToNepomukPipe::importNote(const QVariantMap &noteEntry)
{
    Nepomuk2::SimpleResourceGraph graph;

    kDebug() << "import note " << noteEntry.value("zoterotitle").toString();

    QTextDocument content( noteEntry.value("note").toString() );
    if(content.isEmpty()) {
        return;
    }

    Nepomuk2::PIMO::Note note;
    note.addType(NIE::InformationElement());

    //FIXME: remove zotero parts and make it more general
    note.setProperty( NAO::prefLabel(), noteEntry.value(QLatin1String("zoterotitle")).toString() );
    note.setProperty( NIE::title(), noteEntry.value(QLatin1String("zoterotitle")).toString() );

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

//    Nepomuk2::SimpleResource empty;
//    if(e->contains(QLatin1String("zoterokey"))) {
//        addZoteroSyncDetails(note, empty, e);
//    }

    graph << note;

    Nepomuk2::StoreResourcesJob *srj = Nepomuk2::storeResources(graph,Nepomuk2::IdentifyNew, Nepomuk2::OverwriteProperties);
    connect(srj, SIGNAL(result(KJob*)), this, SLOT(slotSaveToNepomukDone(KJob*)));
    if(!srj->exec()) {
        kDebug() << srj->errorString();
    }
}

void VariantToNepomukPipe::importAttachment(const QVariantMap &attachmentEntry)
{
    Nepomuk2::SimpleResourceGraph graph;

    Nepomuk2::NFO::FileDataObject attachment;
    attachment.addType(NFO::Document());

    attachment.setProperty( NIE::title(), attachmentEntry.value("title").toString() );

    if(attachmentEntry.contains("zoteroAttachmentFile")) {

        QString localFilePath = attachmentEntry.value("localFile").toString();
        localFilePath.prepend(QLatin1String("file://"));
        attachment.setProperty( NIE::url(), localFilePath);
        attachment.setProperty( NAO::identifier(), localFilePath);

        kDebug() << "create attachment with url" << localFilePath;
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
    QDateTime dateTime = NepomukMetaDataExtractor::Pipe::NepomukPipe::createDateTime( accessdate );

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

//    Nepomuk2::SimpleResource empty;
//    if(e->contains(QLatin1String("zoterokey"))) {
//        addZoteroSyncDetails(attachment, empty, e);
//    }

    Nepomuk2::StoreResourcesJob *srj = Nepomuk2::storeResources(graph,Nepomuk2::IdentifyNew, Nepomuk2::OverwriteProperties);
    connect(srj, SIGNAL(result(KJob*)), this, SLOT(slotSaveToNepomukDone(KJob*)));
    if(!srj->exec()) {
        kDebug() << srj->errorString();
    }
}
/*
void BibTexToNepomukPipe::addZoteroSyncDetails(Nepomuk2::SimpleResource &mainResource, Nepomuk2::SimpleResource &referenceResource,Entry *e)
{
    //FIXME: Find a better way to do the syncing/information adding of onlinestorages
    return; // disabled for now
    QString id = PlainTextValue::text(e->value(QLatin1String("zoterokey")));
    QString etag = PlainTextValue::text(e->value(QLatin1String("zoteroetag")));
    QString updated = PlainTextValue::text(e->value(QLatin1String("zoteroupdated")));
    QString parentId = PlainTextValue::text(e->value(QLatin1String("zoteroparent")));

    e->remove(QLatin1String("zoterokey"));
    e->remove(QLatin1String("zoteroetag"));
    e->remove(QLatin1String("zoteroupdated"));
    e->remove(QLatin1String("zoteroparent"));

    Nepomuk2::SYNC::ServerSyncData serverSyncData;
    serverSyncData.addType(NIE::DataObject());

    // first set an identifier, when the object already exist we merge them together
    QString identifier = QLatin1String("zotero") + m_syncUserId + m_syncUrl + id;
    serverSyncData.addProperty( NAO::identifier(), KUrl::fromEncoded( identifier.toLatin1()) );
    serverSyncData.setProperty(NIE::url(), KUrl::fromEncoded(identifier.toLatin1())); // we need the url to make this unique and not merge it wth something else

    // now we set the new values
    serverSyncData.setProvider( QLatin1String("zotero") );
    serverSyncData.setUrl( m_syncUrl );
    serverSyncData.setUserId( m_syncUserId );
    serverSyncData.setId( id );
    serverSyncData.setEtag( etag );
    serverSyncData.setProperty( NUAO::lastModification(), updated);

    // now depending on what kind of mainResource we have, we add another TypeClass
    // helps to find the right data later on again and create the right links between Resource and syncData
    if(e->type() == QLatin1String("note")) {
        serverSyncData.setSyncDataType( SYNC::Note() );
        serverSyncData.setNote( mainResource.uri() );
        mainResource.setProperty(SYNC::serverSyncData(), serverSyncData.uri() );
    }
    else if(e->type() == QLatin1String("attachment")) {
        serverSyncData.setSyncDataType( SYNC::Attachment() );
        serverSyncData.setAttachment( mainResource.uri() );
        mainResource.setProperty(SYNC::serverSyncData(), serverSyncData );
    }
    else {
        serverSyncData.setSyncDataType( SYNC::BibResource() );
        serverSyncData.setPublication( mainResource.uri() );
        mainResource.setProperty(SYNC::serverSyncData(), serverSyncData );

        if(referenceResource.isValid()) {
            serverSyncData.setReference( referenceResource.uri() );
            referenceResource.setProperty(SYNC::serverSyncData(), serverSyncData );
        }
    }

    m_currentGraph << serverSyncData;

    // check if the current item we added is a child item and need to be added to a parent too
    if( parentId.isEmpty() ) {
        return;
    }

    //now we do have a parent id, so find the parent and make the important isRealated relation
    QString query = "select DISTINCT ?r where {  "
                     "?r a sync:ServerSyncData . "
                     "?r sync:provider ?provider . FILTER regex(?provider, \"zotero\") "
                     "?r sync:userId ?userId . FILTER regex(?userId, \"" + m_syncUserId + "\") "
                     "?r sync:url ?url . FILTER regex(?url, \"" + m_syncUrl + "\") "
                     "?r sync:id ?id . FILTER regex(?id, \""+ parentId + "\") "
                     "}";

    QList<Nepomuk2::Query::Result> results = Nepomuk2::Query::QueryServiceClient::syncSparqlQuery(query);

    if(results.size() > 1 || results.isEmpty()) {
        kDebug() << "could not find the right sync details for the current parent item query" << "zotero" << m_syncUserId << m_syncUrl << parentId;
        return;
    }

    Nepomuk2::Resource parentSyncResourceNepomuk = results.first().resource();
    Nepomuk2::SimpleResource parentSyncResource(results.first().resource().uri());

    QUrl syncDataType = parentSyncResourceNepomuk.property(SYNC::syncDataType()).toUrl();

    if(syncDataType == SYNC::Attachment()) { //ignore
    }
    else if(syncDataType == SYNC::Note()) { //ignore
    }
    else if(syncDataType == SYNC::BibResource()) {
        // here we add the information that a note as related(a child) of the reference and the publication.
        // and that the attachment (nfo:FileDataObject) is the publicationOf/publishedAs of the nbib:Publication

        if( mainResource.contains(RDF::type(), PIMO::Note())) {
            Nepomuk2::Resource parentPublicationxx = parentSyncResourceNepomuk.property(SYNC::publication()).toResource();
            Nepomuk2::SimpleResource parentPublication(parentPublicationxx.uri());
            parentPublication.addProperty( NAO::isRelated(), mainResource.uri());
            mainResource.addProperty( NAO::isRelated(), parentPublication.uri());

            parentPublication.addProperty( NAO::hasSubResource(), mainResource.uri()); //delete note when publication is deleted

            Nepomuk2::Resource parentReferencexx = parentSyncResourceNepomuk.property(SYNC::reference()).toResource();
            Nepomuk2::SimpleResource parentReference(parentReferencexx.uri());
            parentReference.addProperty( NAO::isRelated(), mainResource.uri());
            mainResource.addProperty( NAO::isRelated(), parentReference.uri());
            m_currentGraph << parentReference << parentPublication;
        }
        else if( mainResource.contains(RDF::type(), NFO::FileDataObject()) || mainResource.contains(RDF::type(), NFO::RemoteDataObject())) {
            Nepomuk2::Resource parentPublicationxx = parentSyncResourceNepomuk.property(SYNC::publication()).toResource();
            Nepomuk2::SimpleResource parentPublication(parentPublicationxx.uri());
            parentPublication.addProperty( NBIB::isPublicationOf(), mainResource.uri());
            mainResource.addProperty( NBIB::publishedAs(), parentPublication.uri());

            parentPublication.addProperty( NAO::hasSubResource(), mainResource.uri()); // delete file when publication is deleted
            m_currentGraph << parentPublication;
        }
    }

    // this creates the link for the syncResources so we know how they are connected
    // connect child syncDetails to its parent syncDetails
    serverSyncData.setProperty(NAO::isRelated(), parentSyncResource );
    parentSyncResource.setProperty( SYNC::provider(), QLatin1String("zotero")); // we need to add some kind of property, or the resource is invalid

    m_currentGraph << serverSyncData << parentSyncResource;

}
*/
