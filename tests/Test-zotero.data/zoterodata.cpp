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



//#include "../testdatadir.h"

#include "nbibio/storageglobals.h"
#include "nbibio/provider/zotero/zoterosync.h"

#include <QtTest>
#include <QtDebug>

/**
 * @brief Data driven unittest
 *
 * Checks all supported item types for zotero. Uploads the items and checks if the returned item is the same.
 * Helps to identify key name translations.
 */
class ZoteroData: public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    void importExportTest_data();
    void importExportTest();
    void cleanup();

private:
    ProviderSyncDetails psd;
    ZoteroSync client;
    QString currentId;
};

QTEST_MAIN(ZoteroData)

void ZoteroData::initTestCase()
{
    psd.userName = QString("879781");
    psd.pwd = QString("Zqbpsll0iJXGuRJbJAHnGern");
    psd.url = QString("users");
    psd.collection = QString("VZA7KBSZ"); // data-test from conqtest
    psd.syncMode = Download_Only;
    psd.mergeMode = UseServer;
    psd.askBeforeDeletion = false;
    psd.importAttachments = false;
    psd.exportAttachments = false;

    client.setProviderSettings( psd );
}

void ZoteroData::importExportTest_data()
{
    QTest::addColumn<QVariantMap>("bibentry");

    //TODO: read all zoterotest entries from a file
    QVariantMap artwork;
    artwork.insert(QLatin1String("artworkSize"), QLatin1String("UNITTEST-artworkSize"));
    artwork.insert(QLatin1String("accessDate"), QLatin1String("2011-05-05"));
    artwork.insert(QLatin1String("publicationtype"), QLatin1String("artwork"));
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
    QTest::newRow("artwork") << artwork;
}

void ZoteroData::importExportTest()
{
    QFETCH(QVariantMap, bibentry);
    QSignalSpy spy(&client, SIGNAL(finished()));
    QSignalSpy spy2(&client, SIGNAL(error(QString)));
    client.pushItems( QVariantList() << bibentry, psd.collection);

    while (spy.count() == 0 && spy2.count() == 0) {
        QTest::qWait(200);
    }

    QVERIFY2(spy2.count() == 0, "Error during zotero upload");

    QVariantList returnedItem = client.data();

    QVERIFY2(!returnedItem.isEmpty(), "No items returned");
    QVERIFY2(returnedItem.size() == 1, "Returned more than 1 item for a new item request");

    // now check that each uploaded entry is also in the returned data
    QVariantMap downloadedEntry = returnedItem.first().toMap();
    currentId = downloadedEntry.value(QLatin1String("sync-key")).toString();

    bool error = false;
    QStringList errorString;

    QMapIterator<QString, QVariant> i(bibentry);
    while(i.hasNext()) {
        i.next();

        if( !downloadedEntry.contains(i.key()) ) {
            error = true;
            errorString << QLatin1String("Key missing ::" + i.key().toAscii());

        }
        else if( downloadedEntry.value(i.key()) != i.value()) {
            error = true;
            errorString << QLatin1String("Value not the same :: shouldbe: " + i.value().toString().toAscii() + " | is: " + downloadedEntry.value(i.key()).toString().toAscii());
        }
    }

    if(error) {
        qDebug() << errorString;
        QFAIL("Downloaded Item is different from uploaded");
    }
}

void ZoteroData::cleanup()
{
    QSignalSpy spy(&client, SIGNAL(finished()));
    QSignalSpy spy2(&client, SIGNAL(error(QString)));
    client.deleteItems( QVariantList() << currentId );

    while (spy.count() == 0 && spy2.count() == 0) {
        QTest::qWait(200);
    }

    QVERIFY2( spy2.count() == 0, "Error happend during item removal");
}

#include "zoterodata.moc"
