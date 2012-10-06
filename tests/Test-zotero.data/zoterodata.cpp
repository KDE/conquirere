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

#include "nbibio/bibtex/bibteximporter.h"
#include "nbibio/bibtex/bibtexvariant.h"

#include <QtTest>
#include <QtDebug>

/**
 * @file zoterodata.cpp
 * @test Data driven unittest for the zotero read/write
 *       Checks all supported item types for zotero. Uploads the items and checks if the returned item is the same.
 *       Helps to identify key name translations.
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
    QVariantMap currentItem;
};

QTEST_MAIN(ZoteroData)

void ZoteroData::initTestCase()
{
    psd.userName = QString("879781");
    psd.pwd = QString("Zqbpsll0iJXGuRJbJAHnGern");
    psd.url = QString("users");
    psd.collection = QString("D4EJCUQW");
    psd.syncMode = Download_Only;
    psd.mergeMode = UseServer;
    psd.askBeforeDeletion = false;
    psd.importAttachments = false;
    psd.exportAttachments = false;

    //TODO create new collection for this

    client.setProviderSettings( psd );
}

void ZoteroData::importExportTest_data()
{
    // Get bibfile for the import test
    QStringList errorReadFile;

    QString testFileDir = TESTDATADIR + QLatin1String("/data/generic_zotero.bib");
    BibTexImporter nbImBib;
    nbImBib.readBibFile(testFileDir, &errorReadFile);

    if(!errorReadFile.isEmpty()) {
        qWarning() << errorReadFile;
        QFAIL("Errors occurred while reading the bibfile");
    }

    File *bibFileToCheck = nbImBib.bibFile();
    QVERIFY( bibFileToCheck != 0 );

    // pipe article in the bibfile into a QVariantList
    QVariantList bibList = BibTexVariant::toVariant(*bibFileToCheck);

    if( bibList.isEmpty() ) {
        QFAIL("No publication exported to QVariantList");
    }

    // and add each entry to the TestData
    QTest::addColumn<QVariantMap>("bibentry");
    foreach(const QVariant &v, bibList) {
        QVariantMap entry = v.toMap();
        entry.remove(QLatin1String("note")); // note not supported in normal query, uses extra child item
        QTest::newRow( entry.value(QLatin1String("bibtexentrytype")).toString().toAscii() ) << entry;
    }
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
    currentItem = returnedItem.first().toMap();

    bool error = false;
    QStringList errorString;

    QMapIterator<QString, QVariant> i(bibentry);
    while(i.hasNext()) {
        i.next();

        if( !currentItem.contains(i.key().toLower()) ) {
            error = true;
            errorString << QLatin1String("Key missing ::" + i.key().toAscii());
            errorString << QLatin1String("\n");

        }
        else if( currentItem.value(i.key()) != i.value()) {
            if( i.key() == QLatin1String("bibtexcitekey")) // citekey is the zotero unique id, this changes with every new upload
                continue;

            error = true;
            errorString << i.key() + QLatin1String("-Value not the same :: shouldbe: " + i.value().toString().toAscii() + " | is: " + currentItem.value(i.key()).toString().toAscii());
            errorString << QLatin1String("\n");
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
    client.deleteItems( QVariantList() << currentItem );

    while (spy.count() == 0 && spy2.count() == 0) {
        QTest::qWait(200);
    }

    QVERIFY2( spy2.count() == 0, "Error happend during item removal");
}

#include "zoterodata.moc"
