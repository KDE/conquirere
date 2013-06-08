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

#include <QtGui/QMessageBox>
#include <QtTest>
#include <QtDebug>

#include <qtest_kde.h>

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
 * After a successful upload check that all nepomuk items have ServerSyncData attached and the linking between the mis correct
 */
class SyncZoteroWrite: public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    void exportTest();
    void changeTest();
    void deleteTest();
    void serverChangeTest();

    void cleanupTestCase();

    void statusOutput(const QString &msg);
    void errorOutput(const QString &msg);

private:
    NepomukSyncClient client;
    ProviderSyncDetails psd;
    Nepomuk2::Resource pimoProject;
    Nepomuk2::Resource bookRef1;
    Nepomuk2::Resource bookPub1;
    Nepomuk2::Resource br1File;
    Nepomuk2::Resource bookRef2;
    Nepomuk2::Resource bookPub2;
    Nepomuk2::Resource br2Note1;
    Nepomuk2::Resource br2Note2;
};

QTEST_KDEMAIN_CORE(SyncZoteroWrite)

void SyncZoteroWrite::initTestCase()
{
    // prepare zotero to have 1 item in the collection 3BEJ4A2M
    // the item needs 2 notes and 1 file attachment
    psd.providerId = QString("zotero");
    psd.userName = QString("");
    psd.pwd = QString("Zqbpsll0iJXGuRJbJAHnGern");
    psd.url = QString("users");
//    psd.collection = QString("3BEJ4A2M");
    psd.syncMode = Download_Only;
    psd.localStoragePath = QString("~/");
    psd.mergeMode = UseServer;
    psd.askBeforeDeletion = false;
    psd.importAttachments = true;
    psd.exportAttachments = false;

    client.setProviderSettings( psd );
    client.setProject( pimoProject );

    connect(&client, SIGNAL(status(QString)), this, SLOT(statusOutput(QString)) );
    connect(&client, SIGNAL(error(QString)), this, SLOT(errorOutput(QString)) );

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
    book.addType(NBIB::Publication());
    book.setTitle(QLatin1String("UNITTEST-book"));
    book.addCreator( bookAuthor.uri() );
    book.addPublisher( publisher.uri() );

    Nepomuk2::NBIB::Reference bookReference;
    bookReference.setCiteKey(QLatin1String("UNITTEST-bookRef"));
    bookReference.addProperty( NAO::isRelated(), project.uri());

    bookReference.setPublication( book.uri() );
    book.addReference(bookReference.uri());
    book.addProperty(NAO::hasSubResource(), bookReference.uri());
    book.addProperty(NAO::hasSubResource(), publisher.uri());
    book.addProperty(NAO::hasSubResource(), bookAuthor.uri());

    Nepomuk2::NFO::FileDataObject file;
    file.addType(NFO::Document());
    file.setProperty(NIE::title(), QLatin1String("test-pdf-file"));

    QString testFileDir = QLatin1String("file://") + TESTDATADIR + QLatin1String("/data/test-pdf-file.pdf");
    file.setUrl( testFileDir);
    file.setProperty(NIE::identifier(), testFileDir);

    file.setProperty(NBIB::publishedAs(), book.uri());
    book.setProperty(NBIB::isPublicationOf(), file.uri());
    book.addProperty(NAO::hasSubResource(), file.uri());

    graph << book << bookReference << file;

    // create another test book
    Nepomuk2::NBIB::Book book2;
    book2.addType(NBIB::Publication());
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
    book2.addProperty(NAO::hasSubResource(), bookReference2.uri());
    book2.addProperty(NAO::hasSubResource(), publisher.uri());
    book2.addProperty(NAO::hasSubResource(), bookAuthor.uri());
    book2.addProperty(NAO::hasSubResource(), note1.uri());
    book2.addProperty(NAO::hasSubResource(), note2.uri());

    graph << book2 << bookReference2 << note1 << note2;

    Nepomuk2::StoreResourcesJob *srj = Nepomuk2::storeResources(graph,Nepomuk2::IdentifyNew, Nepomuk2::OverwriteProperties);
    if( !srj->exec() ) {
        qWarning() << srj->errorString();
        QFAIL("Could not create test Publication data");
    }
    else {
        // get the pimo project from the return job mappings
        pimoProject = Nepomuk2::Resource( srj->mappings().value( project.uri() ) );

        bookRef1 = Nepomuk2::Resource( srj->mappings().value( bookReference.uri() ) );
        bookPub1 = Nepomuk2::Resource( srj->mappings().value( book.uri() ) );
        br1File = Nepomuk2::Resource( srj->mappings().value( file.uri() ) );

        bookRef2 = Nepomuk2::Resource( srj->mappings().value( bookReference2.uri() ) );
        bookPub2 = Nepomuk2::Resource( srj->mappings().value( book2.uri() ) );
        br2Note1 = Nepomuk2::Resource( srj->mappings().value( note1.uri() ) );
        br2Note2 = Nepomuk2::Resource( srj->mappings().value( note2.uri() ) );
    }
}

void SyncZoteroWrite::exportTest()
{
    QSignalSpy spy(&client, SIGNAL(finished()));
    QSignalSpy spy2(&client, SIGNAL(error(QString)) );

    client.exportData();

    while (spy.count() == 0 && spy2.count() == 0) {
        QTest::qWait(200);
    }

    QVERIFY2(spy2.count() == 0, "Error during zotero download / item import");

    // now check that the ServerSyncData got added correctly to all items

    //----------------------------------------------------------------------
    // BookRef1 + File
    //----------------------------------------------------------------------
    Nepomuk2::Resource ssdBook1 = bookRef1.property(SYNC::serverSyncData()).toResource();
    QVERIFY2(ssdBook1.isValid() && ssdBook1.exists(), "Book reference 1 has no valid ServerSyncData");

    Nepomuk2::Resource ssdBR1File = br1File.property(SYNC::serverSyncData()).toResource();
    QVERIFY2(ssdBR1File.isValid() && ssdBR1File.exists(), "File has no valid ServerSyncData");

    Nepomuk2::Resource ssdFileParent = ssdBR1File.property(NAO::isRelated()).toResource();
    QVERIFY2(ssdFileParent.uri() == ssdBook1.uri(), "File ServerSyncData parent is not BookRef ServerSyncParent");

    //----------------------------------------------------------------------
    // BookRef2 + Notes 1 and Note 2
    //----------------------------------------------------------------------
    Nepomuk2::Resource ssdBook2 = bookRef2.property(SYNC::serverSyncData()).toResource();
    QVERIFY2(ssdBook2.isValid() && ssdBook2.exists(), "Book reference 2 has no valid ServerSyncData");

    Nepomuk2::Resource ssdBR2Note1 = br2Note1.property(SYNC::serverSyncData()).toResource();
    QVERIFY2(ssdBR2Note1.isValid() && ssdBR2Note1.exists(), "Note1 has no valid ServerSyncData");

    Nepomuk2::Resource ssdBR2Note2 = br2Note2.property(SYNC::serverSyncData()).toResource();
    QVERIFY2(ssdBR2Note2.isValid() && ssdBR2Note2.exists(), "Note2 has no valid ServerSyncData");

    //TODO: check note parent
}

void SyncZoteroWrite::changeTest()
{
    Nepomuk2::setProperty(QList<QUrl>() << bookPub1.uri(), NIE::title(), QVariantList() << QLatin1String("CHANGED-TITLE_Book1"));
    Nepomuk2::setProperty(QList<QUrl>() << br1File.uri(), NIE::title(), QVariantList() << QLatin1String("CHANGED pdf NAME"));
    Nepomuk2::setProperty(QList<QUrl>() << bookPub2.uri(), NIE::title(), QVariantList() << QLatin1String("CHANGED-TITLE_Book2"));
    Nepomuk2::setProperty(QList<QUrl>() << br2Note1.uri(), NIE::plainTextContent(), QVariantList() << QLatin1String("CHANGED note 1 content"));

    QTest::qWait(5000); // wait for job to be finished

//    NepomukSyncClient client2;

//    client2.setProviderSettings( psd );
//    client2.setProject( pimoProject );

    QSignalSpy spy(&client, SIGNAL(finished()));
    QSignalSpy spy2(&client, SIGNAL(error(QString)) );

    client.exportData();

    while (spy.count() == 0 && spy2.count() == 0) {
        QTest::qWait(200);
    }

    QVERIFY2(spy2.count() == 0, "Error during zotero download / item import");

    //TODO: download data from zotero and check that it got changed
}

void SyncZoteroWrite::deleteTest()
{
    // delete book 1 and check if it will be deleted online too
    KJob *job = Nepomuk2::removeResources(QList<QUrl>() << bookPub1.uri() );
    job->exec();
    KJob *job2 = Nepomuk2::removeResources(QList<QUrl>() << bookRef1.uri() );
    job2->exec();

//    NepomukSyncClient client2;

//    client2.setProviderSettings( psd );
//    client2.setProject( pimoProject );

    QSignalSpy spy(&client, SIGNAL(finished()));
    QSignalSpy spy2(&client, SIGNAL(error(QString)) );

    client.exportData();

    while (spy.count() == 0 && spy2.count() == 0) {
        QTest::qWait(200);
    }

    QVERIFY2(spy2.count() == 0, "Error during zotero item deletion");
}

void SyncZoteroWrite::serverChangeTest()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::information(0, QString("QMessageBox::information()"), QString("Please change the item on the server"));
    if (reply != QMessageBox::Ok) {
        qDebug() << "cancel";
        QFAIL("Failed to change item on the server, can't test this case");
    }

//    NepomukSyncClient client2;

//    client2.setProviderSettings( psd );
//    client2.setProject( pimoProject );

    QSignalSpy spy(&client, SIGNAL(finished()));
    QSignalSpy spy2(&client, SIGNAL(error(QString)) );

    client.exportData();

    while (spy.count() == 0 && spy2.count() == 0) {
        QTest::qWait(200);
    }

    QVERIFY2(spy2.count() == 0, "Error during zotero download / item import");


}

void SyncZoteroWrite::cleanupTestCase()
{
    // remove all data created by this unittest from the nepomuk database again
    KJob *job = Nepomuk2::removeDataByApplication();
    if(!job->exec()) {
        qWarning() << job->errorString();
        QFAIL("Cleanup did not work");
    }
}
void SyncZoteroWrite::statusOutput(const QString &msg)
{
    qDebug() << "STATUS :: " << msg;
}

void SyncZoteroWrite::errorOutput(const QString &msg)
{
    qDebug() << "ERROR :: " << msg;
}

#include "synczoterowrite.moc"
