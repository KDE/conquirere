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

#include <QtTest>
#include <QtDebug>

/**
 * @file zoterosimple.cpp
 * @test Unittest to test the Zotero provider functionality
 *       Checks collections creation/change/removal and for 1 item creation/change/removal
 *
 * For a data driven test that checks all supported item types see Test-zotero.data
 */
class ZoteroSimple: public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    void createCollectionTest();
    void editCollectionTest();

    void createItemTest();
    void changeItemTest();

    void addItemToCollectionTest();
    void removeItemFromCollectionTest();

    void deleteCollectionTest();
    void deleteItemTest();

    void cleanupTestCase();

private:
    ZoteroSync client;
    CollectionInfo m_currentCollection;
    QVariantMap m_currentItem;
};

QTEST_MAIN(ZoteroSimple)

void ZoteroSimple::initTestCase()
{
    ProviderSyncDetails psd;
    psd.userName = QString("879781");
    psd.pwd = QString("Zqbpsll0iJXGuRJbJAHnGern");
    psd.url = QString("users");
    psd.collection = QString("9EIQZBK5"); // UNITTEST4 from conqtest
    psd.syncMode = Download_Only;
    psd.mergeMode = UseServer;
    psd.askBeforeDeletion = false;
    psd.importAttachments = false;
    psd.exportAttachments = false;

    client.setProviderSettings( psd );
}

void ZoteroSimple::createCollectionTest()
{
    CollectionInfo ci;
    ci.name = QLatin1String("UNITTEST-WRITE-TEST");

    QSignalSpy spy(&client, SIGNAL(finished()));
    QSignalSpy spy2(&client, SIGNAL(error(QString)));
    client.createCollection( ci );

    while (spy.count() == 0 && spy2.count() == 0) {
        QTest::qWait(200);
    }
    QVERIFY2(spy2.count() == 0, "Error during zotero upload");

    QVERIFY2( !client.collectionInfo().isEmpty(), "No collection information returned");

    m_currentCollection = client.collectionInfo().first();

    QVERIFY2( !m_currentCollection.name.isEmpty(), "Returned Collection has no name");
    QCOMPARE( ci.name, m_currentCollection.name);
    QVERIFY2( !m_currentCollection.id.isEmpty(), "Returned Collection has no unique id");
}

void ZoteroSimple::editCollectionTest()
{
    QVERIFY2(!m_currentCollection.id.isEmpty(), "edit collection works only if createCollection did not fail");

    m_currentCollection.name = QLatin1String("UNITTEST-CHANGE-TEST");
    QSignalSpy spy(&client, SIGNAL(finished()));
    QSignalSpy spy2(&client, SIGNAL(error(QString)));
    client.editCollection(m_currentCollection);

    while (spy.count() == 0 && spy2.count() == 0) {
        QTest::qWait(200);
    }

    QVERIFY2(spy2.count() == 0, "Error during zotero upload");

    QVERIFY2( !client.collectionInfo().isEmpty(), "No collection information returned");
    CollectionInfo changedCollection = client.collectionInfo().first();

    QVERIFY2( !changedCollection.name.isEmpty(), "Returned Collection has no name");
    QCOMPARE( changedCollection.name, m_currentCollection.name);
    QVERIFY2( !changedCollection.id.isEmpty(), "Returned Collection has no unique id");

    m_currentCollection = changedCollection;
}

void ZoteroSimple::createItemTest()
{
    QVariantMap artwork;
    artwork.insert(QLatin1String("artworkSize"), QLatin1String("UNITTEST-artworkSize"));
    artwork.insert(QLatin1String("accessDate"), QLatin1String("2011-05-05"));
    artwork.insert(QLatin1String("bibtexentrytype"), QLatin1String("artwork"));
    artwork.insert(QLatin1String("language"), QLatin1String("UNITTEST-language"));
    artwork.insert(QLatin1String("title"), QLatin1String("UNITTEST-title"));
    artwork.insert(QLatin1String("archiveLocation"), QLatin1String("UNITTEST-archiveLocation"));
    //artwork.insert(QLatin1String("notes"), QLatin1String("UNITTEST-"));
    artwork.insert(QLatin1String("copyright"), QLatin1String("UNITTEST-copyright"));
    artwork.insert(QLatin1String("libraryCatalog"), QLatin1String("UNITTEST-libraryCatalog"));
    artwork.insert(QLatin1String("shortTitle"), QLatin1String("UNITTEST-shortTitle"));
    artwork.insert(QLatin1String("date"), QLatin1String("UNITTEST-date"));
    artwork.insert(QLatin1String("keywords"), QLatin1String("UNITTEST-Tag1;UNITTEST-Tag2;UNITTEST-Tag3"));
    artwork.insert(QLatin1String("url"), QLatin1String("UNITTEST-url"));
    artwork.insert(QLatin1String("artworkMedium"), QLatin1String("UNITTEST-artworkMedium"));
    artwork.insert(QLatin1String("abstract"), QLatin1String("UNITTEST-abstract"));
    artwork.insert(QLatin1String("author"), QLatin1String("UNITTEST-AFist1 UNITTEST-ALast1;UNITTEST-AFist2 UNITTEST-ALast2"));
    artwork.insert(QLatin1String("contributor"), QLatin1String("UNITTEST-CFist1 UNITTEST-CLast1;UNITTEST-CFist2 UNITTEST-CLast2"));
    artwork.insert(QLatin1String("extra"), QLatin1String("UNITTEST-extra"));
    artwork.insert(QLatin1String("archive"), QLatin1String("UNITTEST-archive"));
    artwork.insert(QLatin1String("lccn"), QLatin1String("UNITTEST-lccn"));
    //artwork.insert(QLatin1String("attachments"), QLatin1String("UNITTEST-"));

    QSignalSpy spy(&client, SIGNAL(finished()));
    QSignalSpy spy2(&client, SIGNAL(error(QString)));
    client.pushItems( QVariantList() << artwork,QLatin1String("UC4BABIM"));

    while (spy.count() == 0 && spy2.count() == 0) {
        QTest::qWait(200);
    }

    QVERIFY2(spy2.count() == 0, "Error during zotero upload");

    QVariantList returnedItem = client.data();

    QVERIFY2(!returnedItem.isEmpty(), "No items returned");
    QVERIFY2(returnedItem.size() == 1, "Returned more than 1 item for a new item request");

    m_currentItem = returnedItem.first().toMap();

    QMapIterator<QString,QVariant> i(artwork);
    bool error = false;
    QStringList errorMsg;
    while(i.hasNext()) {
        i.next();

        if( !m_currentItem.contains(i.key())) {
            error = true;
            errorMsg << QString("Returned item is missing key" + i.key().toAscii());
        }
        if( m_currentItem.value(i.key()) != i.value()) {
            error = true;
            errorMsg << QString("Values not the same for key" + i.key().toAscii() + " is:" + m_currentItem.value(i.key()).toString() + " actual:" + i.value().toString());

        }
    }

    if (error) {
        qDebug() << errorMsg;
        QFAIL("Values for returned item not the same");
    }
}

void ZoteroSimple::changeItemTest()
{
    QVERIFY2(!m_currentItem.isEmpty(), "change item works only if createItem did not fail");

    m_currentItem.insert(QLatin1String("title"), QLatin1String("UNITTEST-title-CHANGED"));

    QSignalSpy spy(&client, SIGNAL(finished()));
    QSignalSpy spy2(&client, SIGNAL(error(QString)));
    client.pushItems(QVariantList() << m_currentItem);

    while (spy.count() == 0 && spy2.count() == 0) {
        QTest::qWait(200);
    }

    QVERIFY2(spy2.count() == 0, "Error during zotero upload");

    QVariantList returnedItem = client.data();

    QVERIFY2(!returnedItem.isEmpty(), "No items returned");
    QVERIFY2(returnedItem.size() == 1, "Returned more than 1 item for a new item request");

    QVariantMap changedItem = returnedItem.first().toMap();

    QCOMPARE(changedItem.value(QLatin1String("title")).toString(),  QLatin1String("UNITTEST-title-CHANGED"));

    m_currentItem = changedItem;
}

void ZoteroSimple::addItemToCollectionTest()
{
    QVERIFY2(!m_currentItem.isEmpty(), "addItemToCollection works only if createItem did not fail");
    QVERIFY2(!m_currentCollection.id.isEmpty(), "addItemToCollection works only if createCollection did not fail");

    QStringList ids;
    ids << m_currentItem.value(QLatin1String("sync-key")).toString();

    QSignalSpy spy(&client, SIGNAL(finished()));
    QSignalSpy spy2(&client, SIGNAL(error(QString)));
    client.addItemsToCollection( ids, m_currentCollection.id);

    while (spy.count() == 0 && spy2.count() == 0) {
        QTest::qWait(200);
    }

    QVERIFY2(spy2.count() == 0, "Error during zotero upload");

    //TODO: check that the item got really added to the collection
}

void ZoteroSimple::removeItemFromCollectionTest()
{
    QVERIFY2(!m_currentItem.isEmpty(), "addItemToCollection works only if createItem did not fail");
    QVERIFY2(!m_currentCollection.id.isEmpty(), "addItemToCollection works only if createCollection did not fail");

    QStringList ids;
    ids << m_currentItem.value(QLatin1String("sync-key")).toString();

    QSignalSpy spy(&client, SIGNAL(finished()));
    QSignalSpy spy2(&client, SIGNAL(error(QString)));
    client.removeItemsFromCollection(ids, m_currentCollection.id);

    while (spy.count() == 0 && spy2.count() == 0) {
        QTest::qWait(200);
    }

    QVERIFY2(spy2.count() == 0, "Error during zotero upload");

    //TODO: check that the item got really removed from the collection
}

void ZoteroSimple::deleteCollectionTest()
{
    QVERIFY2(!m_currentCollection.id.isEmpty(), "Delete collection works only if createCollection did not fail");

    QSignalSpy spy(&client, SIGNAL(finished()));
    QSignalSpy spy2(&client, SIGNAL(error(QString)));
    client.deleteCollection(m_currentCollection);

    while (spy.count() == 0 && spy2.count() == 0) {
        QTest::qWait(200);
    }

    QVERIFY2(spy2.count() == 0, "Error during zotero upload");
    //CollectionInfo changedCollection = wtz.collectionInfo();

    //TODO: test that collection got deleted

}

void ZoteroSimple::deleteItemTest()
{
    QVERIFY2(!m_currentItem.isEmpty(), "delete item works only if createItem did not fail");

    QSignalSpy spy(&client, SIGNAL(finished()));
    QSignalSpy spy2(&client, SIGNAL(error(QString)));
    client.deleteItems( QVariantList() << m_currentItem );

    while (spy.count() == 0 && spy2.count() == 0) {
        QTest::qWait(200);
    }

    QVERIFY2(spy2.count() == 0, "Error during zotero upload");
    //TODO: Verfiy that the item got deleted properly
}

void ZoteroSimple::cleanupTestCase()
{
}

#include "zoterosimple.moc"
