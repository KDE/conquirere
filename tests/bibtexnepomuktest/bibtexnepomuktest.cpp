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

#include "nbibio/nbibimporterbibtex.h"
#include "nbibio/pipe/nepomuktobibtexpipe.h"

#include "nbib.h"
#include <Soprano/Vocabulary/NAO>

#include <Nepomuk/Query/ComparisonTerm>
#include <Nepomuk/Query/LiteralTerm>
#include <Nepomuk/Query/ResourceTypeTerm>
#include <Nepomuk/Query/AndTerm>

#include <Nepomuk/Query/QueryServiceClient>
#include <Nepomuk/Query/Query>
#include <Nepomuk/Query/Result>
#include <Nepomuk/Query/QueryParser>

#include <QtTest>
#include <QtDebug>
#include <QDateTime>

class BibtexNepomukTest: public QObject
{
    Q_OBJECT
private slots:
    void importToNepomuk();
    void exportToFile();
    void checkExportedFile();
    void cleanupTestCase();

private:
    QDateTime startDate;
    QDateTime endDate;

    NBibImporterBibTex nbImBib;
    NepomukToBibTexPipe ntnp;
    File *importedFile;
    File exportedFile;
};

QTEST_MAIN(BibtexNepomukTest)

void BibtexNepomukTest::importToNepomuk()
{
    QStringList errorReadFile;
    QString testFile = TESTDATADIR + QLatin1String("data/generic.bib");
    nbImBib.readBibFile(testFile, &errorReadFile);

    if(!errorReadFile.isEmpty())
        qWarning() << errorReadFile;

    QVERIFY( errorReadFile.isEmpty() );

    importedFile = nbImBib.bibFile();
    QVERIFY( importedFile != 0 );

    // up to this point we read all data correctly, put it to nepomuk now
    QStringList errorImportData;
    startDate = QDateTime::currentDateTime();
    nbImBib.pipeToNepomuk(&errorImportData);
    endDate = QDateTime::currentDateTime();

    if(!errorReadFile.isEmpty())
        qWarning() << errorReadFile;

    QVERIFY( errorReadFile.isEmpty() );
}

void BibtexNepomukTest::exportToFile()
{
    // Step 1 fetch all references we created
    const Nepomuk::Query::LiteralTerm dateFrom( startDate );
    const Nepomuk::Query::LiteralTerm dateTo( endDate );

    Nepomuk::Query::ComparisonTerm lastModifiedStart = Soprano::Vocabulary::NAO::created() > dateFrom;
    Nepomuk::Query::ComparisonTerm lastModifiedEnd = Soprano::Vocabulary::NAO::created() < dateTo;
    Nepomuk::Query::AndTerm andTerm;

    andTerm.addSubTerm(lastModifiedStart);
    andTerm.addSubTerm(lastModifiedEnd);
    andTerm.addSubTerm(Nepomuk::Query::ResourceTypeTerm(Nepomuk::Vocabulary::NBIB::Reference()));

    Nepomuk::Query::Query query( andTerm );
    QList<Nepomuk::Query::Result> queryResult = Nepomuk::Query::QueryServiceClient::syncQuery(query);

    QCOMPARE( importedFile->size(), queryResult.size() );

    QList<Nepomuk::Resource> references;
    foreach(const Nepomuk::Query::Result & r, queryResult) {
        references.append(r.resource());
    }

    // Step 2 export
    ntnp.addNepomukUries(false);
    ntnp.pipeExport(references);

    exportedFile = ntnp.bibtexFile();

    QCOMPARE( importedFile->size(), exportedFile.size() );
}

void BibtexNepomukTest::checkExportedFile()
{
    bool compareTestFailed = false;

    foreach(QSharedPointer<Element> elementImport, *importedFile) {
        Entry *entryImport = dynamic_cast<Entry *>(elementImport.data());
        if(!entryImport)
            continue;

        bool entryExist = false;
        foreach(QSharedPointer<Element> elementExport, exportedFile) {
            Entry *entryExport = dynamic_cast<Entry *>(elementExport.data());
            if(!entryExport)
                continue;

            if(entryImport->id() == entryExport->id()) {
                entryExist = true;

                // check if both have the same type
                if(entryImport->type().toLower() != entryExport->type().toLower()) {
                    qWarning() << entryImport->id() << " || Imported type-> "<< entryImport->type() << " |#| not equal exported |#|" << entryExport->type();
                    compareTestFailed = true;
                }

                // now check each and every key/value if they are the same
                // run over any import entry keys and check if the exported has the same with the same value
                QMapIterator<QString, Value> i(*entryImport);
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

void BibtexNepomukTest::cleanupTestCase()
{
    // create our range
    const Nepomuk::Query::LiteralTerm dateFrom( startDate );
    const Nepomuk::Query::LiteralTerm dateTo( endDate );

    Nepomuk::Query::ComparisonTerm lastModifiedStart = Soprano::Vocabulary::NAO::created() > dateFrom;
    Nepomuk::Query::ComparisonTerm lastModifiedEnd = Soprano::Vocabulary::NAO::created() < dateTo;
    Nepomuk::Query::AndTerm andTerm;

    andTerm.addSubTerm(lastModifiedStart);
    andTerm.addSubTerm(lastModifiedEnd);

    // fetch data
    Nepomuk::Query::Query query( andTerm );

    QList<Nepomuk::Query::Result> queryResult = Nepomuk::Query::QueryServiceClient::syncQuery(query);

    foreach(const Nepomuk::Query::Result & r, queryResult) {
        r.resource().remove();
    }
}

#include "bibtexnepomuktest.moc"
