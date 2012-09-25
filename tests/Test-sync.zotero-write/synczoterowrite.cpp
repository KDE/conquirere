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

#include "../testdatadir.h"

#include "nbibio/storageglobals.h"
#include "nbibio/provider/zotero/zoterosync.h"

#include "nbibio/nepomuksyncclient.h"
#include "ontology/sync.h"
#include "ontology/nbib.h"
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk2/Vocabulary/NIE>
#include <Nepomuk2/Vocabulary/NFO>
#include "sro/nbib/book.h"
#include "sro/nbib/reference.h"
#include "sro/pimo/project.h"
#include "sro/pimo/note.h"
#include "sro/nco/contact.h"
#include "sro/nfo/filedataobject.h"
#include "sro/nco/organizationcontact.h"

#include <Nepomuk2/Variant>
#include <Nepomuk2/Resource>
#include <Nepomuk2/DataManagement>
#include <Nepomuk2/SimpleResource>
#include <Nepomuk2/SimpleResourceGraph>
#include <Nepomuk2/StoreResourcesJob>
#include <KDE/KJob>

#include <QtTest>
#include <QtDebug>

using namespace Nepomuk2::Vocabulary;
using namespace Soprano::Vocabulary;

/**
 * @file synczoteroread.cpp
 * @test Test writing to zotero
 *       Checks for 1 defiend case if all nepomuk resources uploaded and the SyncResources are updated correctly
 *       This includes Note parent items and the upload of attachments
 *
 * 2 book items are uploaded from a mock project
 * One without note/file attachments
 * Another with 2 notes and 1 file attachment
 *
 * After a sucessfull upload check that all nepomuk items have ServerSyncData attached and the linking between the mis correct
 */
class SyncZoteroWrite: public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    void exportTest();

    void cleanupTestCase();

private:
    NepomukSyncClient client;
    ProviderSyncDetails psd;
    Nepomuk2::Resource pimoProject;
};

QTEST_MAIN(SyncZoteroWrite)

void SyncZoteroWrite::initTestCase()
{
    // prepare zotero to have 1 item in the collection 3BEJ4A2M
    // the item needs 2 notes and 1 file attachment
    psd.providerId = QString("zotero");
    psd.userName = QString("879781");
    psd.pwd = QString("Zqbpsll0iJXGuRJbJAHnGern");
    psd.url = QString("users");
//    psd.collection = QString("3BEJ4A2M");
    psd.syncMode = Download_Only;
    psd.localStoragePath = QString("~/");
    psd.mergeMode = UseServer;
    psd.askBeforeDeletion = false;
    psd.importAttachments = true;
    psd.exportAttachments = false;

    Nepomuk2::SimpleResourceGraph graph;

    Nepomuk2::NCO::Contact bookAuthor;
    bookAuthor.setFullname(QLatin1String("UNITTEST-BookAuthor"));

    Nepomuk2::NCO::OrganizationContact publisher;
    publisher.setFullname(QLatin1String("UNITTEST-Publisher"));

    graph << bookAuthor << publisher;

    // create a test project
    Nepomuk2::PIMO::Project project;
    project.setProperty( NAO::prefLabel() , QLatin1String("UNITTEST-Project") );

    graph << project;

    // create a test book
    Nepomuk2::NBIB::Book book;
    book.setTitle(QLatin1String("UNITTEST-book"));
    book.addCreator( bookAuthor.uri() );
    book.addPublisher( publisher.uri() );

    Nepomuk2::NBIB::Reference bookReference;
    bookReference.setCiteKey(QLatin1String("UNITTEST-bookRef"));
    bookReference.addProperty( NAO::isRelated(), project.uri());

    bookReference.setPublication( book.uri() );
    book.addReference(bookReference.uri());

    Nepomuk2::NFO::FileDataObject file;
    file.addType(NFO::Document());
    file.setProperty(NIE::title(), QLatin1String("test-pdf-file"));

    QString testFileDir = QLatin1String("file://") + TESTDATADIR + QLatin1String("/data/test-pdf-file.pdf");
    file.setUrl( testFileDir);
    file.setProperty(NIE::identifier(), testFileDir);

    file.setProperty(NBIB::publishedAs(), book.uri());
    book.setProperty(NBIB::isPublicationOf(), file.uri());

    graph << book << bookReference << file;

    // create another test book
    Nepomuk2::NBIB::Book book2;
    book2.setTitle(QLatin1String("UNITTEST-book2"));
    book2.addCreator( bookAuthor.uri() );
    book2.addPublisher( publisher.uri() );

    Nepomuk2::PIMO::Note note1;
    note1.addType(NIE::InformationElement());
    note1.setProperty( NAO::prefLabel() , QLatin1String("UNITTEST-Note1") );
    note1.setProperty( NIE::plainTextContent(), QLatin1String("Note Content 1"));
    book2.addProperty( NAO::isRelated(), note1.uri());
    note1.addProperty( NAO::isRelated(), book2.uri());

    Nepomuk2::PIMO::Note note2;
    note2.addType(NIE::InformationElement());
    note2.setProperty( NAO::prefLabel() , QLatin1String("UNITTEST-Note2") );
    note2.setProperty( NIE::plainTextContent(), QLatin1String("Note Content 2"));
    book2.addProperty( NAO::isRelated(), note2.uri());
    note2.addProperty( NAO::isRelated(), book2.uri());

    Nepomuk2::NBIB::Reference bookReference2;
    bookReference2.setCiteKey(QLatin1String("UNITTEST-bookRef2"));
    bookReference2.addProperty( NAO::isRelated(), project.uri());

    bookReference2.setPublication( book2.uri() );
    book2.addReference(bookReference2.uri());

    graph << book2 << bookReference2 << note1 << note2;

    Nepomuk2::StoreResourcesJob *srj = Nepomuk2::storeResources(graph,Nepomuk2::IdentifyNew, Nepomuk2::OverwriteProperties);
    if( !srj->exec() ) {
        qWarning() << srj->errorString();
        QFAIL("Could not create test Publication data");
    }
    else {
        // get the pimo project from the return job mappings
        pimoProject = Nepomuk2::Resource( srj->mappings().value( project.uri() ) );
    }
}

void SyncZoteroWrite::exportTest()
{
    QSignalSpy spy(&client, SIGNAL(finished()));
    QSignalSpy spy2(&client, SIGNAL(error(QString)) );

    client.setProviderSettings( psd );
    client.setProject( pimoProject );
    client.exportData();

    while (spy.count() == 0 && spy2.count() == 0) {
        QTest::qWait(200);
    }

    QVERIFY2(spy2.count() == 0, "Error during zotero download / item import");

    /*
    // now check that all data got added to nepomuk correctly

    // search for the imported reference
    QString query = QString::fromLatin1("select ?r where {"
                                        "?r a sync:ServerSyncData ."

                                        "?r nao:identifier ?id ."
                                        "FILTER regex(?id, \"^zotero" + psd.userName.toAscii() + psd.url.toAscii() + itemToFetch.toAscii() + "\") ."
                                        "}");

    qDebug() << "Find SyncResource for :: " << query;
    Soprano::Model* model = Nepomuk2::ResourceManager::instance()->mainModel();
    Soprano::QueryResultIterator it = model->executeQuery( query, Soprano::Query::QueryLanguageSparql );

    Nepomuk2::Resource syncData;
    int count = 0;
    while( it.next() ) {
        count++;
        Soprano::BindingSet p = it.current();
        syncData = Nepomuk2::Resource(p.value("r").toString());
        break; // can't be more than 1 anyway
    }

    QVERIFY2(syncData.isValid() && syncData.exists(), "Could not get the ServerSyncData resource");

    //----------------------------------------------------------------------
    // first check if the reference was added correctly
    //----------------------------------------------------------------------
    Nepomuk2::Resource reference = syncData.property(SYNC::reference()).toResource();
    QVERIFY2(reference.isValid() && reference.exists(), "Reference does not exist");

    Nepomuk2::Resource refsyncResource = reference.property(SYNC::serverSyncData()).toResource();
    QVERIFY2(refsyncResource.isValid() && refsyncResource.exists(), "Reference does not link to the ServerSyncData Object");
    QCOMPARE(refsyncResource.uri(), syncData.uri());

    // check that the reference citekey is the ZoteroID
    QString citekey = reference.property(NBIB::citeKey()).toString();
    QCOMPARE(citekey, itemToFetch);

    Nepomuk2::Resource refPub = reference.property(NBIB::publication()).toResource();
    QVERIFY2(refPub.isValid() && refPub.exists(), "Could not get the connected publication from the reference");

    //----------------------------------------------------------------------
    // now check that the ServerSyncData has "the" publication attached
    //----------------------------------------------------------------------
    Nepomuk2::Resource publication = syncData.property(SYNC::publication()).toResource();
    QVERIFY2(publication.isValid() && publication.exists(), "Publication does not exist");

    Nepomuk2::Resource pubsyncResource = publication.property(SYNC::serverSyncData()).toResource();
    QVERIFY2(pubsyncResource.isValid() && pubsyncResource.exists(), "publication does not link to the ServerSyncData Object");
    QCOMPARE(pubsyncResource.uri(), syncData.uri());

    //----------------------------------------------------------------------
    // now check that the Notes File attachments are added correctly
    //----------------------------------------------------------------------

    // we must have 2 note serverSync Details and 1 file attachment connected via nao:isRelated
    QList<Nepomuk2::Resource> childSyncList = syncData.property(NAO::isRelated()).toResourceList();
    QVERIFY2(childSyncList.size() == 3, "Could not find both notes and the file attachment via nao:isRelated");

    // now iterate over all childs and check if they are added correctly
    foreach(const Nepomuk2::Resource &syncChild, childSyncList) {

        // check that the child links back to the parent
        Nepomuk2::Resource parentFromChildResource = syncChild.property(NAO::isRelated()).toResource();
        QCOMPARE(parentFromChildResource.uri(), syncData.uri());

        QUrl childSyncType = syncChild.property(SYNC::syncDataType()).toUrl();

        if( childSyncType == SYNC::Note() ) {
            //the note syncDetails have a pimo:Note connected that is related to the publication
            Nepomuk2::Resource note = syncChild.property(SYNC::note()).toResource();
            QVERIFY( note.isValid() && note.exists() );

            QUrl noteParentPublication = note.property(NAO::isRelated()).toResource().uri();
            QCOMPARE(noteParentPublication, publication.uri());

            // also the note should link to its serverSyncData obejct
            QUrl noteSyncUrl = note.property(SYNC::serverSyncData()).toResource().uri();
            QCOMPARE(noteSyncUrl, syncChild.uri());

        }
        else if( childSyncType == SYNC::Attachment() ) {
            //the note syncDetails have a nfo:Document connected that is related to the publication
            Nepomuk2::Resource attachment = syncChild.property(SYNC::attachment()).toResource();
            QVERIFY( attachment.isValid() && attachment.exists() );

            // the attachment is connected to the publication
            QUrl attachmentPublicationUrl = attachment.property(NBIB::isPublicationOf()).toResource().uri();
            QCOMPARE(attachmentPublicationUrl, publication.uri());

            // and vise versa
            QUrl publicationAttachmentUrl = publication.property(NBIB::publishedAs()).toResource().uri();
            QCOMPARE(publicationAttachmentUrl, attachment.uri());

            // also the file should link to its serverSyncData obejct
            QUrl attachmentSyncUrl = attachment.property(SYNC::serverSyncData()).toResource().uri();
            QCOMPARE(attachmentSyncUrl, syncChild.uri());
        }
        else {
            qDebug() << syncChild;
            QFAIL("unknown syncDataType");
        }
    }
    */

}

void SyncZoteroWrite::cleanupTestCase()
{
    // remove all data created by this unittest from the nepomuk database again
//    KJob *job = Nepomuk2::removeDataByApplication();
//    if(!job->exec()) {
//        qWarning() << job->errorString();
//        QFAIL("Cleanup did not work");
//    }
}

#include "synczoterowrite.moc"
