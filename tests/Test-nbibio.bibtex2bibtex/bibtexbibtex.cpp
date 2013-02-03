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

#include <kbibtex/file.h>
#include <kbibtex/element.h>
#include <kbibtex/entry.h>

#include "nbibio/bibtex/bibteximporter.h"
#include "nbibio/bibtex/bibtexvariant.h"
#include "nbibio/pipe/nepomuktovariantpipe.h"

#include <Nepomuk2/ResourceManager>
#include <Soprano/Model>
#include <Soprano/QueryResultIterator>
#include <Nepomuk2/DataManagement>
#include <KDE/KJob>

#include <Nepomuk2/Variant>
#include <Nepomuk2/Resource>

#include "nbib.h"
#include <Nepomuk2/Vocabulary/NCO>
#include <Nepomuk2/Vocabulary/NIE>
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk2/Vocabulary/NCAL>

#include <QtTest>
#include <QtDebug>
#include <qtest_kde.h>

using namespace Nepomuk2::Vocabulary;
using namespace Soprano::Vocabulary;

/**
 * @file bibtexbibtex.cpp
 * @test UnitTest for the nbibio exporter/importer Bibtex -> Nepomuk -> Bibtex.
 *       checks: full data driven test see data folder for the used bibtex files
 */
class BibtexBibtex: public QObject
{
    Q_OBJECT

private slots:

    void importExportTest_data();

    void importExportTest();
    void cleanup();

    void cleanupTestCase();
};

QTEST_KDEMAIN_CORE(BibtexBibtex)

void BibtexBibtex::importExportTest_data()
{
    QTest::addColumn<QString>("bibfile");

    QString testFileDir = TESTDATADIR + QLatin1String("/data");

    QTest::newRow("generic standard bibtex") << QString("%1/generic_bibtex_single.bib").arg(testFileDir);
    QTest::newRow("generic standard bibtex") << QString("%1/generic_bibtex.bib").arg(testFileDir);
    QTest::newRow("generic extended bibtex") << QString("%1/generic_extended_bibtex.bib").arg(testFileDir);
    QTest::newRow("generic name import") << QString("%1/generic_names_bibtex.bib").arg(testFileDir);
    QTest::newRow("generic zotero import") << QString("%1/generic_zotero.bib").arg(testFileDir);
//    QTest::newRow("generic zotero import") << QString("%1/generic_zotero_extended.bib").arg(testFileDir);
}

void BibtexBibtex::importExportTest()
{
    QDateTime startDate = QDateTime::currentDateTime();
    //######################################################################################
    //#
    //# Step 1 read data from testfile
    //#
    //######################################################################################

    QFETCH(QString, bibfile);
    QStringList errorReadFile;

    BibTexImporter bti;
    bti.readBibFile(bibfile, &errorReadFile);

    if(!errorReadFile.isEmpty()) {
        qWarning() << errorReadFile;
        QFAIL("Errors occurred while reading the bibfile");
    }

    File *importedFile = bti.bibFile();
    QVERIFY( importedFile != 0 );

    //######################################################################################
    //#
    //# Step 2 up to this point we read all data correctly, put it to nepomuk now
    //#
    //######################################################################################

    QStringList errorImportData;
    bti.pipeToNepomuk(&errorImportData);

    if(!errorReadFile.isEmpty()) {
        qWarning() << errorReadFile;
        QFAIL("Errors occurred while importing the bibfile to Nepomuk");
    }

    //######################################################################################
    //#
    //# Step 3 fetch all newly created references
    //#
    //######################################################################################

    QString query = QString::fromLatin1("select ?r ?t ?created where {"
                                        "?r a nbib:Reference ."
                                        "?r nbib:citeKey ?t ."

                                        "?r nao:created ?created ."
                                        "FILTER ( ?created > \"" + startDate.toString(Qt::ISODate).toLatin1() + "\"^^xsd:dateTime ) ."
                                        "}");

    qDebug() << "get all references from: " << query;
    Soprano::Model* model = Nepomuk2::ResourceManager::instance()->mainModel();
    Soprano::QueryResultIterator it = model->executeQuery( query, Soprano::Query::QueryLanguageSparql );

    QList<Nepomuk2::Resource> references;

    while( it.next() ) {
        Soprano::BindingSet p = it.current();
        KUrl uri = KUrl(p.value("r").toString());
        references.append( Nepomuk2::Resource( uri ) );
    }

    QVERIFY2( !references.isEmpty(), "Could not find imported references in the Nepomuk database");

    //######################################################################################
    //#
    //# Step 4 export created data to bibfile again
    //#
    //######################################################################################

    // Transform Nepomuk Resources to QVariantList
    NepomukToVariantPipe ntvp;
    ntvp.addNepomukUries(false);
    ntvp.pipeExport(references);

    QVariantList list = ntvp.variantList();
    File *exportedFile = BibTexVariant::fromVariant(list);

    QCOMPARE( exportedFile->size(), importedFile->size() );

    //######################################################################################
    //#
    //# Step 5 check if imported data equals exported data
    //#
    //######################################################################################

    bool compareTestFailed = false;

    foreach(QSharedPointer<Element> elementImport, *importedFile) {
        Entry *entryImport = dynamic_cast<Entry *>(elementImport.data());
        if(!entryImport) continue;

        bool entryExist = false;
        foreach(QSharedPointer<Element> elementExport, *exportedFile) {
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
                // run over any import entry keys and check if the exported has the same key with the same value
                QMapIterator<QString, Value> i(*entryImport);
                while (i.hasNext()) {
                    i.next();

                    Value importedValue = entryImport->value(i.key());
                    Value exportedValue = entryExport->value(i.key());

                    // some special cases with person/keywords, as value contains multiple valueitems where the order is not equal
                    if(i.key() == QString("keywords") || i.key() == QString("author") || i.key() == QString("editor")) {
                        foreach(QSharedPointer<ValueItem> vi, importedValue) {
                            if(!exportedValue.containsPattern( PlainTextValue::text(*vi.data()) )) {
                                qWarning() << entryImport->id() << " || Local key " << i.key() << "-> "<< PlainTextValue::text(importedValue) << " |#| not equal nepomuk exported |#|" << PlainTextValue::text(exportedValue);
                                compareTestFailed = true;
                            }
                        }
                    }
                    else if( i.key() == QString("date") || i.key() == QString("accessdate") || i.key() == QString("filingdate") ) {
                        if(!PlainTextValue::text(exportedValue).contains(PlainTextValue::text(importedValue))) {
                            qWarning() << entryImport->id() << " || Local key " << i.key() << "-> "<< PlainTextValue::text(importedValue) << " |#| not equal nepomuk exported |#|" << PlainTextValue::text(exportedValue);
                            compareTestFailed = true;
                        }
                    }
                    else if(PlainTextValue::text(importedValue).toLower() != PlainTextValue::text(exportedValue).toLower()) {
                        qWarning() << entryImport->id() << " || Local key " << i.key() << "-> "<< PlainTextValue::text(importedValue) << " |#| not equal nepomuk exported |#|" << PlainTextValue::text(exportedValue);
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

    QVERIFY2(compareTestFailed == false, "Exported data from nepomuk is not equal the imported data from the file");
}

void BibtexBibtex::cleanup()
{
    // remove all data created by this unittest from the nepomuk database again
    KJob *job = Nepomuk2::removeDataByApplication();
    if(!job->exec()) {
        qWarning() << job->errorString();
        QFAIL("Cleanup did not work");
    }
}

void BibtexBibtex::cleanupTestCase()
{
    // there shouldn't be anything left, as all data should be removed by the cleanup()
    // but just in case

    // remove all data created by this unittest from the nepomuk database again
//    KJob *job = Nepomuk2::removeDataByApplication();
//    if(!job->exec()) {
//        qWarning() << job->errorString();
//        QFAIL("Cleanup did not work");
//    }
}

#include "bibtexbibtex.moc"


