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


#include "nbibio/pipe/varianttonepomukpipe.h"

#include <Nepomuk2/Variant>
#include <Nepomuk2/Resource>
#include <Nepomuk2/DataManagement>
#include <Nepomuk2/SimpleResource>
#include <Nepomuk2/SimpleResourceGraph>
#include <Nepomuk2/ResourceManager>
#include <Soprano/Model>
#include <Soprano/QueryResultIterator>
#include <KDE/KJob>

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
 * @brief UnitTest for the nbibio exporter Nepomuk -> Bibtex
 *
 * checks: a few defined test cases.
 * for a full data driven test see Test-nbibio.bibtex2bibtex
 */
class VariantNepomuk: public QObject
{
    Q_OBJECT

private slots:

    void initTestCase();

    void importArticleTest();

    //TODO: Add more publication types for unittest nepomuk import
//    void exportBookTest();
//    void exportBookletTest();
//    void exportInBookTest();
//    void exportInCollectionTest();
//    void exportProceedingsTest();
//    void exportInProceedingsTest();
//    void exportTechreportTest();

    void cleanupTestCase();

private:
    void checknames(const Nepomuk2::Resource &r, QStringList &names, const QUrl &property);
};

QTEST_MAIN(VariantNepomuk)

void VariantNepomuk::initTestCase()
{
}

void VariantNepomuk::checknames(const Nepomuk2::Resource &r, QStringList &names, const QUrl &property)
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

void VariantNepomuk::importArticleTest()
{
    QVariantList list;
    QVariantMap entryMap;
    entryMap.insert(QLatin1String("bibtexentrytype"), QString::fromUtf8("article"));
    entryMap.insert(QLatin1String("bibtexcitekey"), QString::fromUtf8("UNITTEST-article"));
    entryMap.insert(QLatin1String("address"), QString::fromUtf8("Address"));
    entryMap.insert(QLatin1String("author"), QString::fromUtf8("UNITTEST-A. UNITTEST-Ut; UNITTEST-H. UNITTEST-Ör; UNITTEST-Co UNITTEST-Author; L. Tøffner-Clausen"));
    entryMap.insert(QLatin1String("chapter"), QString::fromUtf8("UNITTEST-II ÄÖÜ"));
    entryMap.insert(QLatin1String("doi"), QString::fromUtf8("10.4204/EPTCS"));
    entryMap.insert(QLatin1String("edition"), QString::fromUtf8("UNITTEST-Second ÄÖÜ"));
    entryMap.insert(QLatin1String("howpublished"), QString::fromUtf8("UNITTEST-Howpublished ÄÖÜ"));
    entryMap.insert(QLatin1String("editor"), QString::fromUtf8("UNITTEST-E.D.I. UNITTEST-Thör; UNITTEST-E. UNITTEST-di UNITTEST-Thor"));
    entryMap.insert(QLatin1String("journal"), QString::fromUtf8("UNITTEST-Journal ÄÖÜ"));
    entryMap.insert(QLatin1String("month"), QString::fromUtf8("jan"));
    entryMap.insert(QLatin1String("note"), QString::fromUtf8("UNITTEST-Note ÄÖÜ"));
    entryMap.insert(QLatin1String("number"), QString::fromUtf8("21"));
    entryMap.insert(QLatin1String("pages"), QString::fromUtf8("1--999"));
    entryMap.insert(QLatin1String("publisher"), QString::fromUtf8("UNITTEST-Publisher ÄÖÜ"));
    entryMap.insert(QLatin1String("series"), QString::fromUtf8("UNITTEST-Series ÄÖÜ"));
    entryMap.insert(QLatin1String("title"), QString::fromUtf8("UNITTEST-Title A: An Article ÄÖÜ"));
    entryMap.insert(QLatin1String("type"), QString::fromUtf8("UNITTEST-Type ÄÖÜ"));
    entryMap.insert(QLatin1String("volume"), QString::fromUtf8("443"));
    entryMap.insert(QLatin1String("year"), QString::fromUtf8("1987"));
    entryMap.insert(QLatin1String("url"), QString::fromUtf8("http://arxiv.org/abs/1009.3306"));
    entryMap.insert(QLatin1String("keywords"), QString::fromUtf8("UNITTEST-Keyword1; UNITTEST-Keyword2; UNITTEST-Keyword3"));

    list.append(entryMap);

    VariantToNepomukPipe vtnp;
    vtnp.pipeExport( list );

    // now try to find the resource in Nepomuk again

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
    QCOMPARE(pages, QString::fromUtf8("1--999"));

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
/*
void VariantNepomuk::exportBookTest()
{
    Nepomuk2::Resource ref = Nepomuk2::Resource( bookReferenceUri );

    QVariantMap entryMap;
    entryMap.insert(QLatin1String("bibtexentrytype"), QString::fromUtf8("book"));
    entryMap.insert(QLatin1String("bibtexcitekey"), QString::fromUtf8("UNITTEST-book"));
    entryMap.insert(QLatin1String("author"), QString::fromUtf8("UNITTEST-Author"));
    entryMap.insert(QLatin1String("date"), QString::fromUtf8("1986-04-03T12:12:12Z"));
    entryMap.insert(QLatin1String("edition"), QString::fromUtf8("UNITTEST-second"));
    entryMap.insert(QLatin1String("editor"), QString::fromUtf8("UNITTEST-Editor"));
    entryMap.insert(QLatin1String("issn"), QString::fromUtf8("UNITTEST-ISSN"));
    entryMap.insert(QLatin1String("month"), QString::fromUtf8("apr"));
    entryMap.insert(QLatin1String("number"), QString::fromUtf8("UNITTEST-number"));
    entryMap.insert(QLatin1String("publisher"), QString::fromUtf8("UNITTEST-Publisher"));
    entryMap.insert(QLatin1String("series"), QString::fromUtf8("UNITTEST-Series"));
    entryMap.insert(QLatin1String("title"), QString::fromUtf8("UNITTEST-book"));
    entryMap.insert(QLatin1String("volume"), QString::fromUtf8("UNITTEST-volume"));
    entryMap.insert(QLatin1String("year"), QString::fromUtf8("1986"));

    checkEquality(ref, entryMap);
}

void VariantNepomuk::exportBookletTest()
{
    Nepomuk2::Resource ref = Nepomuk2::Resource( bookletReferenceUri );

    QVariantMap entryMap;
    entryMap.insert(QLatin1String("bibtexentrytype"), QString::fromUtf8("booklet"));
    entryMap.insert(QLatin1String("bibtexcitekey"), QString::fromUtf8("UNITTEST-booklet"));
    entryMap.insert(QLatin1String("author"), QString::fromUtf8("UNITTEST-Author"));
    entryMap.insert(QLatin1String("date"), QString::fromUtf8("1986-04-03T12:12:12Z"));
    entryMap.insert(QLatin1String("edition"), QString::fromUtf8("UNITTEST-second"));
    entryMap.insert(QLatin1String("editor"), QString::fromUtf8("UNITTEST-Editor"));
    entryMap.insert(QLatin1String("howpublished"), QString::fromUtf8("UNITTEST-how published"));
    entryMap.insert(QLatin1String("issn"), QString::fromUtf8("UNITTEST-ISSN"));
    entryMap.insert(QLatin1String("month"), QString::fromUtf8("apr"));
    entryMap.insert(QLatin1String("series"), QString::fromUtf8("UNITTEST-Series"));
    entryMap.insert(QLatin1String("title"), QString::fromUtf8("UNITTEST-book"));
    entryMap.insert(QLatin1String("year"), QString::fromUtf8("1986"));

    checkEquality(ref, entryMap);
}

void VariantNepomuk::exportInBookTest()
{
    Nepomuk2::Resource ref = Nepomuk2::Resource( inBookReferenceUri );

    QVariantMap entryMap;
    entryMap.insert(QLatin1String("bibtexentrytype"), QString::fromUtf8("inbook"));
    entryMap.insert(QLatin1String("bibtexcitekey"), QString::fromUtf8("UNITTEST-inBook"));
    entryMap.insert(QLatin1String("author"), QString::fromUtf8("UNITTEST-Author"));
    entryMap.insert(QLatin1String("date"), QString::fromUtf8("1986-04-03T12:12:12Z"));
    entryMap.insert(QLatin1String("edition"), QString::fromUtf8("UNITTEST-second"));
    entryMap.insert(QLatin1String("editor"), QString::fromUtf8("UNITTEST-Editor"));
    entryMap.insert(QLatin1String("month"), QString::fromUtf8("apr"));
    entryMap.insert(QLatin1String("publisher"), QString::fromUtf8("UNITTEST-Publisher"));
    entryMap.insert(QLatin1String("title"), QString::fromUtf8("UNITTEST-inbook"));
    entryMap.insert(QLatin1String("year"), QString::fromUtf8("1986"));

    checkEquality(ref, entryMap);
}

void VariantNepomuk::exportInCollectionTest()
{
    Nepomuk2::Resource ref = Nepomuk2::Resource( inCollectionReferenceUri );

    QVariantMap entryMap;
    entryMap.insert(QLatin1String("bibtexentrytype"), QString::fromUtf8("incollection"));
    entryMap.insert(QLatin1String("bibtexcitekey"), QString::fromUtf8("UNITTEST-inCollection"));
    entryMap.insert(QLatin1String("author"), QString::fromUtf8("UNITTEST-Author"));
    entryMap.insert(QLatin1String("bookauthor"), QString::fromUtf8("UNITTEST-BookAuthor"));
    entryMap.insert(QLatin1String("booktitle"), QString::fromUtf8("UNITTEST-incollection"));
    entryMap.insert(QLatin1String("chapter"), QString::fromUtf8("UNITTEST-II"));
    entryMap.insert(QLatin1String("chaptername"), QString::fromUtf8("UNITTEST-Chapter-Title"));
    entryMap.insert(QLatin1String("date"), QString::fromUtf8("1986-04-03T12:12:12Z"));
    entryMap.insert(QLatin1String("edition"), QString::fromUtf8("UNITTEST-second"));
    entryMap.insert(QLatin1String("editor"), QString::fromUtf8("UNITTEST-Editor"));
    entryMap.insert(QLatin1String("month"), QString::fromUtf8("apr"));
    entryMap.insert(QLatin1String("publisher"), QString::fromUtf8("UNITTEST-Publisher"));
    entryMap.insert(QLatin1String("title"), QString::fromUtf8("UNITTEST-Chapter-Title"));
    entryMap.insert(QLatin1String("year"), QString::fromUtf8("1986"));

    checkEquality(ref, entryMap);
}

void VariantNepomuk::exportProceedingsTest()
{
    Nepomuk2::Resource ref = Nepomuk2::Resource( proceedingsReferenceUri );

    QVariantMap entryMap;
    entryMap.insert(QLatin1String("bibtexentrytype"), QString::fromUtf8("proceedings"));
    entryMap.insert(QLatin1String("bibtexcitekey"), QString::fromUtf8("UNITTEST-proceedings"));
    entryMap.insert(QLatin1String("date"), QString::fromUtf8("1986-04-03T12:12:12Z"));
    entryMap.insert(QLatin1String("event"), QString::fromUtf8("UNITTEST-Title"));
    entryMap.insert(QLatin1String("issn"), QString::fromUtf8("UNITTEST-ISSN"));
    entryMap.insert(QLatin1String("month"), QString::fromUtf8("apr"));
    entryMap.insert(QLatin1String("organization"), QString::fromUtf8("UNITTEST-Organization"));
    entryMap.insert(QLatin1String("pages"), QString::fromUtf8("1-999"));
    entryMap.insert(QLatin1String("publisher"), QString::fromUtf8("UNITTEST-Publisher"));
    entryMap.insert(QLatin1String("series"), QString::fromUtf8("UNITTEST-Series"));
    entryMap.insert(QLatin1String("title"), QString::fromUtf8("UNITTEST-Title: A Proceedings"));
    entryMap.insert(QLatin1String("year"), QString::fromUtf8("1986"));

    checkEquality(ref, entryMap);
}

void VariantNepomuk::exportInProceedingsTest()
{
    Nepomuk2::Resource ref = Nepomuk2::Resource( inProceedingsReferenceUri );

    QVariantMap entryMap;
    entryMap.insert(QLatin1String("bibtexentrytype"), QString::fromUtf8("inproceedings"));
    entryMap.insert(QLatin1String("bibtexcitekey"), QString::fromUtf8("UNITTEST-inproceedings"));
    entryMap.insert(QLatin1String("author"), QString::fromUtf8("UNITTEST-Author"));
    entryMap.insert(QLatin1String("booktitle"), QString::fromUtf8("UNITTEST-Title: A Proceedings"));
    entryMap.insert(QLatin1String("date"), QString::fromUtf8("1986-04-03T12:12:12Z"));
    entryMap.insert(QLatin1String("doi"), QString::fromUtf8("10.4204/EPTCS"));
    entryMap.insert(QLatin1String("editor"), QString::fromUtf8("UNITTEST-Editor"));
    entryMap.insert(QLatin1String("event"), QString::fromUtf8("UNITTEST-Title"));
    entryMap.insert(QLatin1String("howpublished"), QString::fromUtf8("UNITTEST-Howpublished"));
    entryMap.insert(QLatin1String("issn"), QString::fromUtf8("UNITTEST-ISSN"));
    entryMap.insert(QLatin1String("journal"), QString::fromUtf8("UNITTEST-Series"));
    entryMap.insert(QLatin1String("month"), QString::fromUtf8("apr"));
    entryMap.insert(QLatin1String("pages"), QString::fromUtf8("1-999"));
    entryMap.insert(QLatin1String("title"), QString::fromUtf8("UNITTEST-Title: An Inproceedings"));
    entryMap.insert(QLatin1String("year"), QString::fromUtf8("1986"));

    checkEquality(ref, entryMap);
}

void VariantNepomuk::exportTechreportTest()
{
    Nepomuk2::Resource ref = Nepomuk2::Resource( techreportReferenceUri );

    QVariantMap entryMap;
    entryMap.insert(QLatin1String("bibtexentrytype"), QString::fromUtf8("techreport"));
    entryMap.insert(QLatin1String("bibtexcitekey"), QString::fromUtf8("UNITTEST-techreport"));
    entryMap.insert(QLatin1String("author"), QString::fromUtf8("UNITTEST-Author"));
    entryMap.insert(QLatin1String("date"), QString::fromUtf8("1986-04-03T12:12:12Z"));
    entryMap.insert(QLatin1String("howpublished"), QString::fromUtf8("UNITTEST-how published"));
    entryMap.insert(QLatin1String("institution"), QString::fromUtf8("UNITTEST-Publisher"));
    entryMap.insert(QLatin1String("month"), QString::fromUtf8("apr"));
    entryMap.insert(QLatin1String("organization"), QString::fromUtf8("UNITTEST-Organization"));
    entryMap.insert(QLatin1String("title"), QString::fromUtf8("UNITTEST-techreport"));
    entryMap.insert(QLatin1String("type"), QString::fromUtf8("UNITTEST-publication-type"));
    entryMap.insert(QLatin1String("year"), QString::fromUtf8("1986"));

    checkEquality(ref, entryMap);
}
*/
void VariantNepomuk::cleanupTestCase()
{
    // remove all data created by this unittest from the nepomuk database again
//    KJob *job = Nepomuk2::removeDataByApplication();
//    if(!job->exec()) {
//        qWarning() << job->errorString();
//        QFAIL("Cleanup did not work");
//    }
}

#include "variantnepomuk.moc"
