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

#include <Nepomuk2/Variant>
#include <Nepomuk2/Resource>
#include <Nepomuk2/DataManagement>
#include <Nepomuk2/SimpleResource>
#include <Nepomuk2/SimpleResourceGraph>
#include <Nepomuk2/StoreResourcesJob>
#include <KDE/KJob>

#include "nbib.h"
#include "sro/nbib/series.h"
#include "sro/nbib/article.h"
#include "sro/nbib/journal.h"
#include "sro/nbib/journalissue.h"
#include "sro/nbib/proceedings.h"
#include "sro/nbib/book.h"
#include "sro/nbib/chapter.h"
#include "sro/nbib/techreport.h"
#include "sro/nbib/reference.h"
#include "sro/ncal/event.h"
#include "sro/nco/contact.h"
#include "sro/nco/organizationcontact.h"

#include "nbibio/pipe/nepomuktovariantpipe.h"

#include <QtTest>
#include <QtDebug>

/**
 * @file nepomukvariant.cpp
 * @test UnitTest for the nbibio exporter Nepomuk -> Bibtex
 *       checks: a few defined test cases.
 *
 * for a full data driven test see Test-nbibio.bibtex2bibtex
 */
class NepomukVariant: public QObject
{
    Q_OBJECT

private slots:

    void initTestCase();

    void exportArticleTest();

    void exportBookTest();
    void exportBookletTest();
    void exportInBookTest();
    void exportInCollectionTest();
    void exportProceedingsTest();
    void exportInProceedingsTest();
    void exportTechreportTest();

    void cleanupTestCase();

private:
    void checkEquality(Nepomuk2::Resource ref, const QVariantMap &originalMap);

    QUrl articleReferenceUri;
    QUrl proceedingsReferenceUri;
    QUrl inProceedingsReferenceUri;
    QUrl bookReferenceUri;
    QUrl bookletReferenceUri;
    QUrl inBookReferenceUri;
    QUrl inCollectionReferenceUri;
    QUrl techreportReferenceUri;

};

QTEST_MAIN(NepomukVariant)

void NepomukVariant::initTestCase()
{
    //insert some test publication
    Nepomuk2::SimpleResourceGraph graph;

    //##############################################
    // General stuff usefull for any publication
    Nepomuk2::NCO::Contact author;
    author.setFullname(QLatin1String("UNITTEST-Author"));

    Nepomuk2::NCO::Contact bookAuthor;
    bookAuthor.setFullname(QLatin1String("UNITTEST-BookAuthor"));

    Nepomuk2::NCO::Contact seriesAuthor;
    seriesAuthor.setFullname(QLatin1String("UNITTEST-SeriesAuthor"));

    Nepomuk2::NCO::Contact editor;
    editor.setFullname(QLatin1String("UNITTEST-Editor"));

    Nepomuk2::NCO::OrganizationContact publisher;
    publisher.setFullname(QLatin1String("UNITTEST-Publisher"));
    //TODO: add publisher address

    Nepomuk2::NCO::OrganizationContact organization;
    organization.setFullname(QLatin1String("UNITTEST-Organization"));
    //TODO: add publisher address

    Nepomuk2::NCAL::Event event;
    event.setTitle(QLatin1String("UNITTEST-Title"));

    Nepomuk2::NBIB::Series series;
    series.setTitle(QLatin1String("UNITTEST-Series"));
    series.setIssn(QLatin1String("UNITTEST-ISSN"));

    QDateTime publicationDate = QDateTime::fromString("1986-04-03T12:12:12Z", Qt::ISODate);

    //##############################################
    //# Article in a journal
    Nepomuk2::NBIB::Journal journal;
    journal.setTitle( QLatin1String("UNITTEST-Jounal"));

    Nepomuk2::NBIB::JournalIssue journalIssue;
    //TODO: should we allow to add a title to the journalIssue? (collection) usually this should be covered by the Journal (Series) the issue is in
    //journalIssue.setTitle( QLatin1String("UNITTEST-JounalIssue"));
    journalIssue.setVolume(QString("443"));
    journalIssue.setNumber(QString("21"));
    journalIssue.addPublisher( publisher.uri() );

    journalIssue.setInSeries( journal.uri() );
    journal.addSeriesOf( journalIssue.uri() );

    Nepomuk2::NBIB::Article article;
    article.setTitle(QLatin1String("UNITTEST-Article-Title"));
    article.setPublicationDate( publicationDate );
    article.setDoi(QLatin1String("10.4204/EPTCS"));
    article.setPublicationMethod(QString("UNITTEST-Howpublished"));

    article.addCreator( author.uri() );
    article.addEditor( editor.uri() );

    journalIssue.addArticle( article.uri() );
    article.setCollection(journalIssue.uri());
    article.setInSeries( series.uri() );
    series.addSeriesOf( article.uri() );

    Nepomuk2::NBIB::Reference articleReference;
    articleReference.setPages(QLatin1String("1-999"));
    articleReference.setCiteKey(QLatin1String("UNITTEST-article"));

    articleReference.setPublication(article.uri());
    article.addReference(articleReference.uri());


    //######################################
    //# Proceedings
    Nepomuk2::NBIB::Proceedings proceedings;
    proceedings.setTitle(QLatin1String("UNITTEST-Title: A Proceedings"));
    proceedings.setPublicationDate( publicationDate );
    proceedings.setOrganization( organization.uri() );
    proceedings.addPublisher( publisher.uri() );

    proceedings.setEvent( event.uri() );
    event.addProperty( Nepomuk2::Vocabulary::NBIB::eventPublication(), proceedings.uri());

    series.addSeriesOf(proceedings.uri());
    proceedings.setInSeries(series.uri());

    Nepomuk2::NBIB::Reference proceedingsReference;
    proceedingsReference.setPages(QLatin1String("1-999"));
    proceedingsReference.setCiteKey(QLatin1String("UNITTEST-proceedings"));

    proceedingsReference.setPublication(proceedings.uri());
    proceedings.addReference(proceedingsReference.uri());


    //######################################
    //# Article InProceedings
    Nepomuk2::NBIB::Article inProceedings;
    inProceedings.setTitle(QLatin1String("UNITTEST-Title: An Inproceedings"));
    inProceedings.setPublicationDate( publicationDate );
    inProceedings.setDoi(QLatin1String("10.4204/EPTCS"));
    inProceedings.setPublicationMethod(QString("UNITTEST-Howpublished"));

    proceedings.addArticle( inProceedings.uri() );
    inProceedings.setCollection( proceedings.uri() );

    inProceedings.addCreator( author.uri() );
    inProceedings.addEditor( editor.uri() );

    Nepomuk2::NBIB::Reference inProceedingsReference;
    inProceedingsReference.setPages(QLatin1String("1-999"));
    inProceedingsReference.setCiteKey(QLatin1String("UNITTEST-inproceedings"));

    inProceedingsReference.setPublication(inProceedings.uri());
    inProceedings.addReference(inProceedingsReference.uri());


    //######################################
    //# Book
    Nepomuk2::NBIB::Book book;
    book.setTitle(QLatin1String("UNITTEST-book"));
    book.setEdition(QLatin1String("UNITTEST-second"));
    book.setPublicationDate( publicationDate );
    book.addCreator( author.uri() );
    book.addEditor( editor.uri() );
    book.addPublisher( publisher.uri() );
    book.setVolume(QLatin1String("UNITTEST-volume"));
    book.setNumber(QLatin1String("UNITTEST-number"));

    book.setInSeries( series.uri() );
    series.addSeriesOf( book.uri() );

    Nepomuk2::NBIB::Reference bookReference;
    bookReference.setCiteKey(QLatin1String("UNITTEST-book"));

    bookReference.setPublication( book.uri() );
    book.addReference(bookReference.uri());

    //######################################
    //# Booklet (book without publisher)
    Nepomuk2::NBIB::Book booklet;
    booklet.setTitle(QLatin1String("UNITTEST-book"));
    booklet.setEdition(QLatin1String("UNITTEST-second"));
    booklet.setPublicationDate( publicationDate );
    booklet.addCreator( author.uri() );
    booklet.addEditor( editor.uri() );
    booklet.setPublicationMethod( QLatin1String("UNITTEST-how published") );

    booklet.setInSeries( series.uri() );
    series.addSeriesOf( booklet.uri() );

    Nepomuk2::NBIB::Reference bookletReference;
    bookletReference.setCiteKey(QLatin1String("UNITTEST-booklet"));

    bookletReference.setPublication( booklet.uri() );
    booklet.addReference(bookletReference.uri());


    //######################################
    //# inBook (chapter in book but untitled)
    Nepomuk2::NBIB::Chapter chapter;
    chapter.setChapterNumber(QString("UNITTEST-II"));
    chapter.setPageStart(45);
    chapter.setPageEnd(67);

    Nepomuk2::NBIB::Book inBook;
    inBook.setTitle(QLatin1String("UNITTEST-inbook"));
    inBook.setEdition(QLatin1String("UNITTEST-second"));
    inBook.addCreator( author.uri() );
    inBook.addEditor( editor.uri() );
    inBook.addPublisher( publisher.uri() );
    inBook.setPublicationDate( publicationDate );

    inBook.addDocumentPart( chapter.uri() );
    chapter.setDocumentPartOf( inBook.uri() );

    Nepomuk2::NBIB::Reference inBookReference;
    inBookReference.setCiteKey(QLatin1String("UNITTEST-inBook"));
    inBookReference.setReferencedPart(chapter.uri());

    inBookReference.setPublication( inBook.uri() );
    inBook.addReference(inBookReference.uri());


    //######################################
    //# inCollection (chapter in book having its own title)
    Nepomuk2::NBIB::Chapter chapter2;
    chapter2.setTitle(QLatin1String("UNITTEST-Chapter-Title"));
    chapter2.setChapterNumber(QString("UNITTEST-II"));
    chapter2.setPageStart(45);
    chapter2.setPageEnd(67);
    chapter2.addCreator( author.uri() );

    Nepomuk2::NBIB::Book inCollection;
    inCollection.setTitle(QLatin1String("UNITTEST-incollection"));
    inCollection.setEdition(QLatin1String("UNITTEST-second"));
    inCollection.addCreator( bookAuthor.uri() );
    inCollection.addEditor( editor.uri() );
    inCollection.addPublisher( publisher.uri() );
    inCollection.setPublicationDate( publicationDate );

    inCollection.addDocumentPart( chapter2.uri() );
    chapter2.setDocumentPartOf( inCollection.uri() );

    Nepomuk2::NBIB::Reference inCollectionReference;
    inCollectionReference.setCiteKey(QLatin1String("UNITTEST-inCollection"));
    inCollectionReference.setReferencedPart(chapter2.uri());

    inCollectionReference.setPublication( inCollection.uri() );
    inCollection.addReference(inCollectionReference.uri());

    //######################################
    //# Techreport
    Nepomuk2::NBIB::Techreport techreport;
    techreport.setTitle(QLatin1String("UNITTEST-techreport"));
    techreport.setPublicationDate( publicationDate );
    techreport.addCreator( author.uri() );
    techreport.addPublisher( publisher.uri() );
    techreport.setOrganization( organization.uri() );
    techreport.setPublicationMethod( QLatin1String("UNITTEST-how published") );
    techreport.setPublicationType(QLatin1String("UNITTEST-publication-type"));

    Nepomuk2::NBIB::Reference techreportReference;
    techreportReference.setCiteKey(QLatin1String("UNITTEST-techreport"));

    techreportReference.setPublication( techreport.uri() );
    techreport.addReference(techreportReference.uri());


    // Now add all the stuff to the graph
    graph << author << bookAuthor << seriesAuthor << editor << publisher << organization << event << series;
    graph << journal << journalIssue << article << articleReference;
    graph << proceedings << proceedingsReference;
    graph << inProceedings << inProceedingsReference;
    graph << book << bookReference;
    graph << booklet << bookletReference;
    graph << chapter << inBook << inBookReference;
    graph << chapter2 << inCollection << inCollectionReference;
    graph << techreport << techreportReference;

    Nepomuk2::StoreResourcesJob* srj = Nepomuk2::storeResources(graph,Nepomuk2::IdentifyNone);

    if(!srj->exec()) {
        qDebug() << srj->errorString();
        QFAIL("Could not insert test publication into Nepomuk");
    }

    // save the real nepomuk uris for later use
    articleReferenceUri = srj->mappings().value(articleReference.uri());
    bookReferenceUri = srj->mappings().value(bookReference.uri());
    bookletReferenceUri = srj->mappings().value(bookletReference.uri());
    inBookReferenceUri = srj->mappings().value(inBookReference.uri());
    proceedingsReferenceUri = srj->mappings().value(proceedingsReference.uri());
    inProceedingsReferenceUri = srj->mappings().value(inProceedingsReference.uri());
    inCollectionReferenceUri = srj->mappings().value(inCollectionReference.uri());
    techreportReferenceUri = srj->mappings().value(techreportReference.uri());
}

void NepomukVariant::checkEquality(Nepomuk2::Resource ref, const QVariantMap &originalMap)
{
    // Pipe Nepomuk to QVariantList
    NepomukToVariantPipe ntvp;
    ntvp.pipeExport(QList<Nepomuk2::Resource>() << ref);

    QVariantList exportedList = ntvp.variantList();

    if(exportedList.isEmpty()) {
        QFAIL("No publication was exported");
    }

    QVariantMap exportedMap = exportedList.first().toMap();

    // now see if for each key in the originalMap we hava similar entry in the exported map
    QMapIterator<QString, QVariant> i( originalMap );
     while (i.hasNext()) {
         i.next();

         if(exportedMap.contains(i.key())) {
             QCOMPARE(exportedMap.value(i.key()).toString(),
                     i.value().toString() );
         }
         else { QFAIL("Missing Key:: \"" + i.key().toLatin1() + "\""); }
     }
}

void NepomukVariant::exportArticleTest()
{
    Nepomuk2::Resource ref = Nepomuk2::Resource( articleReferenceUri );

    QVariantMap entryMap;
    entryMap.insert(QLatin1String("bibtexentrytype"), QString::fromUtf8("article"));
    entryMap.insert(QLatin1String("bibtexcitekey"), QString::fromUtf8("UNITTEST-article"));
    entryMap.insert(QLatin1String("author"), QString::fromUtf8("UNITTEST-Author"));
    entryMap.insert(QLatin1String("date"), QString::fromUtf8("1986-04-03T12:12:12Z"));
    entryMap.insert(QLatin1String("doi"), QString::fromUtf8("10.4204/EPTCS"));
    entryMap.insert(QLatin1String("editor"), QString::fromUtf8("UNITTEST-Editor"));
    entryMap.insert(QLatin1String("howpublished"), QString::fromUtf8("UNITTEST-Howpublished"));
    entryMap.insert(QLatin1String("issn"), QString::fromUtf8("UNITTEST-ISSN"));
    entryMap.insert(QLatin1String("journal"), QString::fromUtf8("UNITTEST-Jounal"));
    entryMap.insert(QLatin1String("month"), QString::fromUtf8("apr"));
    entryMap.insert(QLatin1String("number"), QString::fromUtf8("21"));
    entryMap.insert(QLatin1String("pages"), QString::fromUtf8("1-999"));
    entryMap.insert(QLatin1String("series"), QString::fromUtf8("UNITTEST-Series"));
    entryMap.insert(QLatin1String("title"), QString::fromUtf8("UNITTEST-Article-Title"));
    entryMap.insert(QLatin1String("volume"), QString::fromUtf8("443"));
    entryMap.insert(QLatin1String("year"), QString::fromUtf8("1986"));

    checkEquality(ref, entryMap);
}

void NepomukVariant::exportBookTest()
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

void NepomukVariant::exportBookletTest()
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

void NepomukVariant::exportInBookTest()
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

void NepomukVariant::exportInCollectionTest()
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

void NepomukVariant::exportProceedingsTest()
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

void NepomukVariant::exportInProceedingsTest()
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

void NepomukVariant::exportTechreportTest()
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

void NepomukVariant::cleanupTestCase()
{
    // remove all data created by this unittest from the nepomuk database again
    KJob *job = Nepomuk2::removeDataByApplication();
    if(!job->exec()) {
        qWarning() << job->errorString();
        QFAIL("Cleanup did not work");
    }
}

#include "nepomukvariant.moc"
