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

#include <kbibtex/fileexporterbibtex.h>
#include <QtTest>
#include <QtDebug>
#include <qtest_kde.h>

/**
 *@file bibtexvarianttest.cpp
 * @test UnitTest for the bibtex importer Bibtex -> QVariantList
 *       checks: a one defined test cases.
 *
 * for a full data driven test see Test-nbibio.bibtex2bibtex
 */
class BibtexVariantTest: public QObject
{
    Q_OBJECT

private slots:

    void initTestCase();

    void importTest();
    void exportTest();

    void cleanupTestCase();

private:
    QStringList bibFileToStringList(const File &bibFile);
    BibTexImporter nbImBib;
    File *bibFileToCheck;
    QVariantList bibEntryListToCheck;
};

QTEST_KDEMAIN_CORE(BibtexVariantTest)

void BibtexVariantTest::initTestCase()
{
    // Get bibfile for the import test
    QStringList errorReadFile;

    QString testFileDir = TESTDATADIR + QLatin1String("/data/generic_bibtex_single.bib");
    nbImBib.readBibFile(testFileDir, &errorReadFile);

    if(!errorReadFile.isEmpty()) {
        qWarning() << errorReadFile;
        QFAIL("Errors occurred while reading the bibfile");
    }

    bibFileToCheck = nbImBib.bibFile();
    QVERIFY( bibFileToCheck != 0 );


    // create the QVariantList for the export test
    QVariantMap entryMap;
    entryMap.insert("bibtexentrytype", QString::fromUtf8("article"));
    entryMap.insert("bibtexcitekey", QString::fromUtf8("UNITTEST-article"));
    entryMap.insert("author", QString::fromUtf8("UNITTEST-A. UNITTEST-Ut; UNITTEST-H. UNITTEST-Ör; UNITTEST-Co UNITTEST-Author; L. Tøffner-Clausen"));
    entryMap.insert("journal", QString::fromUtf8("UNITTEST-Journal"));
    entryMap.insert("keywords", QString::fromUtf8("UNITTEST-Keyword1; UNITTEST-Keyword2; UNITTEST-Keyword3"));
    entryMap.insert("pages", QString::fromUtf8("1–999"));
    entryMap.insert("title", QString::fromUtf8("UNITTEST-Title A: An Article ÄÖÜ"));
    entryMap.insert("url", QString::fromUtf8("http://arxiv.org/abs/1009.3306"));
    entryMap.insert("year", QString::fromUtf8("1987"));
    bibEntryListToCheck.append(entryMap);
}

void BibtexVariantTest::importTest()
{
    // pipe article in the bibfile into a QVariantList
    QVariantList bibList = BibTexVariant::toVariant(*bibFileToCheck);

    if( bibList.isEmpty() ) {
        QFAIL("No publication exported to QVariantList");
    }

    QVariantMap importedMap = bibList.first().toMap();

    QMapIterator<QString, QVariant> i(bibEntryListToCheck.first().toMap());
     while (i.hasNext()) {
         i.next();

         if(importedMap.contains(i.key())) {
             QCOMPARE(importedMap.value(i.key()).toString(),
                     i.value().toString() );
         }
         else { QFAIL("Missing Key:: \"" + i.key().toLatin1() + "\""); }
     }
}

QStringList BibtexVariantTest::bibFileToStringList(const File &bibFile)
{
    QStringList sourceList;

    FileExporterBibTeX exporter;
    QBuffer buffer;
    buffer.open(QBuffer::WriteOnly);
    exporter.save(&buffer, &bibFile);
    buffer.close();

    buffer.open(QBuffer::ReadOnly);
    QTextStream ts(&buffer);
    while(!ts.atEnd()) {
        QString line = ts.readLine();
        line=line.remove(QChar('{'));
        line=line.remove(QChar('}'));
        sourceList.append(line);
    }
    buffer.close();

    return sourceList;
}

void BibtexVariantTest::exportTest()
{
    // pipe the QVariantList to a bibtex file
    File *bibFile = BibTexVariant::fromVariant(bibEntryListToCheck);

    // create the source output for both and compare line by line
    QStringList originalFile = bibFileToStringList(*bibFileToCheck);
    QStringList exportedFile = bibFileToStringList(*bibFile);

    if( exportedFile.isEmpty() ) {
        QFAIL("No data exported from QVariantList to the BibTeX File");
    }

    //Now iterate over all lines in the  original file and check if the exported has the same line
    int line = 0;
    foreach(const QString &originalLine, originalFile) {
        QCOMPARE(originalLine, exportedFile.at(line));
        line++;
    }

    delete bibFile;
}

void BibtexVariantTest::cleanupTestCase()
{
    //delete bibFileToCheck; // why does this crash the test?
}

#include "bibtexvarianttest.moc"
