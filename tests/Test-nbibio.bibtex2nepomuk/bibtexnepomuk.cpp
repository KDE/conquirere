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

using namespace Nepomuk2::Vocabulary;
using namespace Soprano::Vocabulary;

/**
 * @brief UnitTest for the nbibio exporter Bibtex -> Nepomuk
 *
 * checks: a few defined test cases.
 * for a full data driven test see Test-nbibio.bibtex2bibtex
 */
class BibtexNepomuk: public QObject
{
    Q_OBJECT

private slots:

    void initTestCase();

    void importBenchmarkTest();
    void importArticleTest();
    //TODO: add more tests:: check publication types / more special cases / file/link addition

    void cleanupTestCase();

private:
    void checknames(const Nepomuk2::Resource &r, QStringList &names, const QUrl &property);

    NBibImporterBibTex nbImBib;
    File *importedFile;

};

QTEST_MAIN(BibtexNepomuk)

void BibtexNepomuk::initTestCase()
{
    //read in the bibfile
    QStringList errorReadFile;

    QString testFileDir = TESTDATADIR + QLatin1String("/data/generic_bibtex.bib");
    nbImBib.readBibFile(testFileDir, &errorReadFile);

    if(!errorReadFile.isEmpty()) {
        qWarning() << errorReadFile;
        QFAIL("Errors occurred while reading the bibfile");
    }

    importedFile = nbImBib.bibFile();
    QVERIFY( importedFile != 0 );
}

void BibtexNepomuk::importBenchmarkTest()
{
    // push all items into nepomuk
    QBENCHMARK_ONCE {
        QStringList errorReadFile;

        nbImBib.pipeToNepomuk();

        if(!errorReadFile.isEmpty()) {
            qWarning() << errorReadFile;
            QFAIL("Errors occurred while importing the data to Nepomuk");
        }
    }
}

void BibtexNepomuk::checknames(const Nepomuk2::Resource &r, QStringList &names, const QUrl &property)
{
    QList<Nepomuk2::Resource> contactList = r.property( property ).toResourceList();
    foreach(const Nepomuk2::Resource &r2, contactList) {
        QString name = r2.property(NCO::fullname()).toString();

        foreach(QString checkname, names) {
            if(name == checkname) {
                names.removeOne(checkname);
            }
        }
    }
}

void BibtexNepomuk::importArticleTest()
{
    //TODO: limit search by creation date just in case the article exist already from a previous entry
    // search for the imported reference
    QString query = QString::fromLatin1("select ?r where {"
                                        "?r a nbib:Reference ."
                                        "OPTIONAL { ?r nbib:citeKey ?t . }"
                                        "FILTER regex(?t, \"^UNITTEST-article\") ."
                                        "}");
    Soprano::Model* model = Nepomuk2::ResourceManager::instance()->mainModel();
    Soprano::QueryResultIterator it = model->executeQuery( query, Soprano::Query::QueryLanguageSparql );

    QString referenceUri;
    int count = 0;
    while( it.next() ) {
        count++;
        Soprano::BindingSet p = it.current();
        referenceUri = p.value("r").toString();
    }

    QVERIFY2(count == 1, "Could not find the right article reference in the Nepomuk database");

    //####################################################################
    //# now the actual check if the data is like it should be

    //####################################################################
    //# The reference
    Nepomuk2::Resource reference(referenceUri);
    QString citekey = reference.property(NBIB::citeKey()).toString();
    QCOMPARE(citekey, QString("UNITTEST-article"));

    QString pages = reference.property(NBIB::pages()).toString();
    QCOMPARE(pages, QString::fromUtf8("1–999"));

    //####################################################################
    //# The Chapter
    Nepomuk2::Resource chapter = reference.property(NBIB::referencedPart()).toResource();
    QString chapterNumber = chapter.property(NBIB::chapterNumber()).toString();
    QCOMPARE(chapterNumber, QString::fromUtf8("UNITTEST-II ÄÖÜ"));

    //####################################################################
    //# The publication
    Nepomuk2::Resource publication = reference.property(NBIB::publication()).toResource();
    QString title = publication.property(NIE::title()).toString();
    QCOMPARE(title, QString::fromUtf8("UNITTEST-Title A: An Article ÄÖÜ"));

    QDateTime publicationDate = QDateTime::fromString(publication.property(NBIB::publicationDate()).toString(), Qt::ISODate);
    QString year = publicationDate.toString("yyyy");
    QCOMPARE(year, QString::fromUtf8("1987"));
    //TODO: compare month in unittest

    QString type = publication.property(NBIB::publicationType()).toString();
    QCOMPARE(type, QString::fromUtf8("UNITTEST-Type ÄÖÜ"));

    QString howpublished = publication.property(NBIB::publicationMethod()).toString();
    QCOMPARE(howpublished, QString::fromUtf8("UNITTEST-Howpublished ÄÖÜ"));

    QString doi = publication.property(NBIB::doi()).toString();
    QCOMPARE(doi, QString::fromUtf8("10.4204/EPTCS"));

    QString edition = publication.property(NBIB::edition()).toString();
    QCOMPARE(edition, QString::fromUtf8("UNITTEST-Second ÄÖÜ"));

    QStringList publisherNames;
    publisherNames << QString::fromUtf8("UNITTEST-Publisher ÄÖÜ");
    checknames(publication, publisherNames, NCO::publisher());

    if(!publisherNames.isEmpty()) {
        qDebug() << "missing names" << publisherNames;
        QFAIL("Not all publisher names could be found");
    }
    //TODO: check publisher address

    QStringList editorNames;
    editorNames << QString::fromUtf8("UNITTEST-E.D.I. UNITTEST-Thör");
    editorNames << QString::fromUtf8("UNITTEST-E. UNITTEST-di UNITTEST-Thor");
    checknames(publication, editorNames, NBIB::editor());

    if(!editorNames.isEmpty()) {
        qDebug() << "missing names" << editorNames;
        QFAIL("Not all editor names could be found");
    }

    QStringList authorNames;
    authorNames << QString::fromUtf8("UNITTEST-A. UNITTEST-Ut");
    authorNames << QString::fromUtf8("UNITTEST-H. UNITTEST-Ör");
    authorNames << QString::fromUtf8("UNITTEST-Co UNITTEST-Author");
    authorNames << QString::fromUtf8("L. Tøffner-Clausen");
    checknames(publication, authorNames, NCO::creator());

    if(!authorNames.isEmpty()) {
        qDebug() << "missing names" << authorNames;
        QFAIL("Not all author names could be found");
    }

    bool notefound = false;
    QList<Nepomuk2::Resource> relatedlist = publication.property(NAO::isRelated()).toResourceList();
    foreach(const Nepomuk2::Resource &r, relatedlist) {
        QString plainTextContent = r.property(NIE::plainTextContent()).toString();
        if(plainTextContent == QString::fromUtf8("UNITTEST-Note ÄÖÜ")) {
               notefound = true;
                break;
        }
    }

    if(!notefound) {
        QFAIL("Note note found with content \"UNITTEST-Note ÄÖÜ\"");
    }

    bool linkFound = false;
    QList<Nepomuk2::Resource> linklist = publication.property(NIE::links()).toResourceList();
    foreach(const Nepomuk2::Resource &r, linklist) {

        QString url = r.property(NIE::url()).toString();
        if(url == QString::fromUtf8("http://arxiv.org/abs/1009.3306")) {
               linkFound = true;
                break;
        }
    }

    if(!linkFound) {
        QFAIL("url found with link \"http://arxiv.org/abs/1009.3306\"");
    }

    //####################################################################
    //# The series
    Nepomuk2::Resource series = publication.property(NBIB::inSeries()).toResource();
    QString seriesTitle = series.property(NIE::title()).toString();
    QCOMPARE(seriesTitle, QString::fromUtf8("UNITTEST-Series ÄÖÜ"));
    QList<Nepomuk2::Resource> publicationList = series.property(NBIB::seriesOf()).toResourceList();
    if( !publicationList.contains(publication) ) {
        QFAIL("Backlink broken. series does not link back to the publication");
    }

    //####################################################################
    //# collection (journalIssue)
    Nepomuk2::Resource journalIssue = publication.property(NBIB::collection()).toResource();
    QString number = journalIssue.property(NBIB::number()).toString();
    QCOMPARE(number, QString::fromUtf8("21"));

    QString volume = journalIssue.property(NBIB::volume()).toString();
    QCOMPARE(volume, QString::fromUtf8("443"));
    QList<Nepomuk2::Resource> articleList = journalIssue.property(NBIB::article()).toResourceList();
    if( !articleList.contains(publication) ) {
        QFAIL("Backlink broken. journalIssue does not link back to the article");
    }

    //####################################################################
    //# Series of the issue (journal)
    Nepomuk2::Resource journal = journalIssue.property(NBIB::inSeries()).toResource();
    QString journalName = journal.property(NIE::title()).toString();
    QCOMPARE(journalName, QString::fromUtf8("UNITTEST-Journal ÄÖÜ"));
    QList<Nepomuk2::Resource> issueList = journal.property(NBIB::seriesOf()).toResourceList();
    if( !issueList.contains(journalIssue) ) {
        QFAIL("Backlink broken. journal does not link back to journalissue");
    }
}

void BibtexNepomuk::cleanupTestCase()
{
    // remove all data created by this unittest from the nepomuk database again
    KJob *job = Nepomuk2::removeDataByApplication();
    if(!job->exec()) {
        qWarning() << job->errorString();
        QFAIL("Cleanup did not work");
    }
}

#include "nepomukbibtex.moc"


