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

#include <Nepomuk2/Resource>
#include <Nepomuk2/Variant>

#include <QtTest>
#include <QtDebug>

using namespace Nepomuk2::Vocabulary;

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
    itemToFetch = QString("KVV2PS5Z");

    psd.userName = QString("879781");
    psd.pwd = QString("Zqbpsll0iJXGuRJbJAHnGern");
    psd.url = QString("users");
    psd.collection = QString("3BEJ4A2M");
    psd.syncMode = Download_Only;
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

    QVERIFY2(spy2.count() == 0, "Error during zotero download / item import");

    // now check that all data got added to nepomuk correctly

    // search for the imported reference
    QString query = QString::fromLatin1("select ?r where {"
                                        "?r a sync:ServerSyncData ."

                                        "?r nao:identifier ?id ."
                                        "FILTER regex(?id, \"^zotero" + psd.userName.toAscii() + psd.url.toAscii() + itemToFetch.toAscii() + ") ."
                                        "}");

    qDebug() << "Find SyncResource for :: " << query;
    Soprano::Model* model = Nepomuk2::ResourceManager::instance()->mainModel();
    Soprano::QueryResultIterator it = model->executeQuery( query, Soprano::Query::QueryLanguageSparql );

    Nepomuk2::Resource syncData;
    int count = 0;
    while( it.next() ) {
        count++;
        Soprano::BindingSet p = it.current();
        syncData = epomuk2::Resource(p.value("r").toString());
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

    Nepomuk2::Resource refPub = reference.property(NBIB::publication());
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
    // now check that the Notes are added correctly
    //----------------------------------------------------------------------

    //TODO: clarify how this really works
    // notes/attachements via nao:isRelated connection between the ServerSyncData obejcts (to save the parent/child structure and keep the child unique id)
    // check that all of this links correctly.
//    QList<Nepomuk2::Resource> noteSyncList = syncData.property(SYNC::note()).toResourceList();
//    QVERIFY2(noteSyncList.size() == 2, "Could not find both attached notes");

//    foreach(const Nepomuk2::Resource &syncNote, noteSyncList) {
//        // each Note should link to its parent


//        Nepomuk2::Resource noteSyncResource = note.property(SYNC::serverSyncData()).toResource();
//        QVERIFY2(noteSyncResource.isValid() && noteSyncResource.exists(), "note does not link to the ServerSyncData Object");
//        QCOMPARE(noteSyncResource.uri(), syncData.uri());
//    }

}

void SyncZoteroRead::cleanupTestCase()
{
    // remove all data created by this unittest from the nepomuk database again
    KJob *job = Nepomuk2::removeDataByApplication();
    if(!job->exec()) {
        qWarning() << job->errorString();
        QFAIL("Cleanup did not work");
    }
}

#include "synczoteroread.moc"
