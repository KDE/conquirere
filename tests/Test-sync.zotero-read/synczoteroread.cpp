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

#include "nbibio/storageglobals.h"
#include "nbibio/provider/zotero/zoterosync.h"

#include "nbibio/nepomuksyncclient.h"
#include "ontology/sync.h"
#include "ontology/nbib.h"
#include <Soprano/Vocabulary/NAO>

#include <Nepomuk2/Resource>
#include <Nepomuk2/Variant>

#include <Nepomuk2/ResourceManager>
#include <Soprano/Model>
#include <Soprano/QueryResultIterator>
#include <Nepomuk2/DataManagement>
#include <KDE/KJob>

#include <QtTest>
#include <QtDebug>

using namespace Nepomuk2::Vocabulary;
using namespace Soprano::Vocabulary;

/**
 * @file synczoteroread.cpp
 * @test Test reading from zotero and adding data to Nepomuk
 *       Checks for 1 defiend case if all nepomuk resources together with the SyncResources are created correctly
 *       This includes Note parent items and the download of attachments
 */
class SyncZoteroRead: public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    void importTest();

    void cleanupTestCase();

private:
    NepomukSyncClient client;
    ProviderSyncDetails psd;
    QString itemToFetch;
};

QTEST_MAIN(SyncZoteroRead)

void SyncZoteroRead::initTestCase()
{
    // prepare zotero to have 1 item in the collection 3BEJ4A2M
    // the item needs 2 notes and 1 file attachment
    itemToFetch = QString("S9PIQIPK");
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

    client.setProviderSettings( psd );
}

void SyncZoteroRead::importTest()
{
    QSignalSpy spy(&client, SIGNAL(finished()));
    QSignalSpy spy2(&client, SIGNAL(error(QString)) );
    client.importData();

    while (spy.count() == 0 && spy2.count() == 0) {
        QTest::qWait(200);
    }

    QTest::qWait(2000); // wait a bit more, to finish none-blocking stuff

    QVERIFY2(spy2.count() == 0, "Error during zotero download / item import");

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

}

void SyncZoteroRead::cleanupTestCase()
{
    // remove all data created by this unittest from the nepomuk database again
//    KJob *job = Nepomuk2::removeDataByApplication();
//    if(!job->exec()) {
//        qWarning() << job->errorString();
//        QFAIL("Cleanup did not work");
//    }
}

#include "synczoteroread.moc"
