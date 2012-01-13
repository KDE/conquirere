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

#include "onlinestorage/zotero/zoteroinfo.h"
#include "onlinestorage/zotero/readfromzotero.h"
#include "nbibio/nbibimporterbibtex.h"

#include <kbibtex/file.h>
#include <kbibtex/element.h>
#include <kbibtex/entry.h>
#include <kbibtex/fileexporterbibtex.h>

#include <QMetaType>
#include <QMetaTypeId>
#include <QtTest>
#include <QtDebug>
#include <QDateTime>

class BibtexNepomukTest: public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();
    void readZoteroTest_data();

    void init();
    void readZoteroTest();
    void cleanup();

private:
    NBibImporterBibTex *nbImBib;
    QString unitTestCollection;
    ReadFromZotero *rfz;
    File *localFile;
};

QTEST_MAIN(BibtexNepomukTest)

void BibtexNepomukTest::initTestCase()
{
    rfz = 0;
    localFile = 0;
    unitTestCollection = QString("HS6N25ER");


    QString testFile = TESTDATADIR + QLatin1String("/data/generic_zotero.bib");
    QStringList errorReadFile;

    nbImBib = new NBibImporterBibTex;
    nbImBib->readBibFile(testFile, &errorReadFile);

    if(!errorReadFile.isEmpty()) {
        qWarning() << errorReadFile;
        QFAIL("Errors occured while reading the bibfile");
    }

    localFile = nbImBib->bibFile();
    QVERIFY( localFile != 0 );
}

void BibtexNepomukTest::cleanupTestCase()
{
    delete rfz;
}

void BibtexNepomukTest::readZoteroTest_data()
{
    QTest::addColumn<QString>("zoteroKey");

    QTest::newRow("Zotero Book") << "HEH97V8C";
    QTest::newRow("Zotero DictionaryEntry") << "BI9SEMHT";
    QTest::newRow("Zotero Encyclopedia Article") << "7ZRI57QH";
    QTest::newRow("Zotero document") << "U2IFZZRG";
    QTest::newRow("Zotero Conference Paper") << "CX99HFP7";
    QTest::newRow("Zotero Tv BroadCast") << "RF2WJX66";
    QTest::newRow("Zotero Video Recording") << "877ET8KB";
    QTest::newRow("Zotero Presentation") << "Q53S6JI2";
    QTest::newRow("Zotero Audio Recording") << "VU87X4AW";
    QTest::newRow("Zotero Forum Post") << "5VCVP6N2";
    QTest::newRow("Zotero Blog Post") << "DH7CADN9";
    QTest::newRow("Zotero Map") << "Q2PX3TVU";
    QTest::newRow("Zotero Statute") << "BWR3MUIC";
    QTest::newRow("Zotero Patent") << "I3A3J2Z2";
    QTest::newRow("Zotero Case") << "Z3RUXKWF";
    QTest::newRow("Zotero Bill") << "ENQCEJKN";
    QTest::newRow("Zotero Report") << "GAKPWXAC";
    QTest::newRow("Zotero Web Page") << "TJD4BXN3";
    QTest::newRow("Zotero Artwork") << "JMZX67GP";
    QTest::newRow("Zotero Film") << "8I59RGVG";
    QTest::newRow("Zotero Manuscript") << "6RKMKQQX";
    QTest::newRow("Zotero Thesis") << "ZB5J5QZ9";
    QTest::newRow("Zotero Newspaper Article") << "8675CR3X";
    QTest::newRow("Zotero Magazine Article") << "XI4KH4K9";
    QTest::newRow("Zotero Journal Article") << "ZQKK75JX";
    QTest::newRow("Zotero Book Section") << "TN44DMTR";
}

void BibtexNepomukTest::init()
{
    // create new one
    delete rfz;
    rfz = new ReadFromZotero;
    rfz->setAdoptBibtexTypes(true);

    ProviderSyncDetails psd;
    psd.providerInfo = new ZoteroInfo();
    psd.userName = QString("795913");
    psd.pwd = QString("lzhbQu69OUJZ7g0ouvLEjn6N");
    psd.url = QString("users");
    psd.collection = QString("HS6N25ER");
    psd.syncMode = Download_Only;
    psd.mergeMode = UseServer;
    psd.askBeforeDeletion = false;
    psd.importAttachments = false;
    psd.exportAttachments = false;

    rfz->setProviderSettings(psd);
}

void BibtexNepomukTest::readZoteroTest()
{
    //######################################################################################
    //#
    //# Step 1 detch data from zotero for 1 item as specified in QFetch
    //#
    //######################################################################################

    QFETCH(QString, zoteroKey);
    rfz->fetchItem(zoteroKey, unitTestCollection );

    qRegisterMetaType<File>("File");
    QSignalSpy spy(rfz, SIGNAL(itemsInfo(File)));
    while (spy.count() == 0) {
        QTest::qWait(200);
    }

    File result = rfz->getFile();

    QCOMPARE( result.size(), 1 );

    //######################################################################################
    //#
    //# Step 2 checkagainst imported file
    //#
    //######################################################################################
    bool compareTestFailed = false;

    foreach(QSharedPointer<Element> elementImport, result) {
        Entry *entryImport = dynamic_cast<Entry *>(elementImport.data());
        if(!entryImport) continue;

        bool entryExist = false;
        foreach(QSharedPointer<Element> elementExport, *localFile) {
            Entry *entryExport = dynamic_cast<Entry *>(elementExport.data());
            if(!entryExport) continue;

            if(entryImport->id() == entryExport->id()) {
                entryExist = true;

                // check if both have the same type
                if(entryImport->type().toLower() != entryExport->type().toLower()) {
                    qWarning() << entryImport->id() << " || Imported type-> "<< entryImport->type() << " |#| not equal exported |#|" << entryExport->type();
                    compareTestFailed = true;
                }

                // now check each and every key/value if they are the same
                // run over any import entry keys and check if the exported has the same with the same value
                QMapIterator<QString, Value> i(*entryExport);
                while (i.hasNext()) {
                    i.next();

                    Value importedValue = entryImport->value(i.key());
                    Value exportedValue = entryExport->value(i.key());
                    if(PlainTextValue::text(importedValue) != PlainTextValue::text(exportedValue)) {
                        qWarning() << entryImport->id() << " || Imported key " << i.key() << "-> "<< PlainTextValue::text(importedValue) << " |#| not equal exported |#|" << PlainTextValue::text(exportedValue);
                        compareTestFailed = true;
                    }
                }
            }

        }

        if(!entryExist) {
            qWarning() << entryImport->id() << "not found in imported file";
            compareTestFailed = true;
        }
    }

    if(compareTestFailed) {
        QFAIL("Exported data from nepomuk is not equal the imported data from the file");
    }

}

void BibtexNepomukTest::cleanup()
{

}

#include "bibtexnepomuktest.moc"
