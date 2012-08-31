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

#include "nbibio/nbibexporterfile.h"

#include <QtTest>
#include <QtDebug>

/**
 * @brief UnitTest for the nbibio exporter Nepomuk -> Bibtex
 *
 * checks: a few defined test cases.
 * for a full data driven test see Test-nbibio.bibtex2bibtex
 */
class NepomukBibtex: public QObject
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
    void checkEquality(Nepomuk2::Resource ref, const QStringList & original);

    QUrl articleReferenceUri;
    QUrl proceedingsReferenceUri;
    QUrl inProceedingsReferenceUri;
    QUrl bookReferenceUri;
    QUrl bookletReferenceUri;
    QUrl inBookReferenceUri;
    QUrl inCollectionReferenceUri;
    QUrl techreportReferenceUri;

};

QTEST_MAIN(NepomukBibtex)

void NepomukBibtex::initTestCase()
{
    //insert some test publication
    Nepomuk2::SimpleResourceGraph graph;

    //##############################################
    // General stuff usefull for any publication
    Nepomuk2::NCO::Contact author;
    author.setFullname(QLatin1String("UNITTEST-Author"));

    Nepomuk2::NCO::Contact bookAuthor;
    bookAuthor.setFullname(QLatin1String("UNITTEST-Author"));

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

    QDateTime publicationDate = QDateTime::fromString("1987-01-01", "yyyy-MM-dd");


    //##############################################
    //# Article in a journal
    Nepomuk2::NBIB::Journal journal;
    journal.setTitle( QLatin1String("UNITTEST-Jounal"));

    Nepomuk2::NBIB::JournalIssue journalIssue;
    //TODO: should we allow to add a title to the sournalIssue? (collection) usually this should be covered by the Journal (Series) the issue is in
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

void NepomukBibtex::checkEquality(Nepomuk2::Resource ref, const QStringList &original)
{
    NBibExporterFile exporter;
    exporter.setFileType( NBibExporterFile::EXPORT_BIBTEX );

    QByteArray byteArray;
    QBuffer iodev(&byteArray);
    iodev.open(QBuffer::ReadWrite);

    QStringList errorLog;
    exporter.save(&iodev, QList<Nepomuk2::Resource>() << ref, &errorLog);

    if(!errorLog.isEmpty()) {
        qDebug() << errorLog;
        QFAIL("Errors occured while exporting the data");
    }

    QTextStream output(&byteArray);

    bool equal = true;
    int line = 0;
    while ( !output.atEnd() && line < original.size()) {
        QString outputLine = output.readLine();

        if(outputLine != original.at(line)) {
            qDebug() << "OUT:" << outputLine << " NOT EQUAL Original: " << original.at(line);
            equal = false;
        }
        line++;
    }

    if(!equal) {
        QFAIL("exported data not equal reference data");
    }
}

void NepomukBibtex::exportArticleTest()
{
    Nepomuk2::Resource ref = Nepomuk2::Resource( articleReferenceUri );

    QStringList originalLine;
    originalLine << "@article{UNITTEST-article,";
    originalLine << "	author = {UNITTEST-Author},";
    originalLine << "	date = {1986-12-31T23:00:00Z},";
    originalLine << "	doi = {10.4204/EPTCS},";
    originalLine << "	editor = {UNITTEST-Editor},";
    originalLine << "	howpublished = {UNITTEST-Howpublished},";
    originalLine << "	issn = {UNITTEST-ISSN},";
    originalLine << "	journal = {UNITTEST-Jounal},";
    originalLine << "	month = {dec},";
    originalLine << "	number = {21},";
    originalLine << "	pages = {1-999},";
    originalLine << "	series = {{UNITTEST-Series}},";
    originalLine << "	title = {{UNITTEST-Article-Title}},";
    originalLine << "	volume = {443},";
    originalLine << "	year = {1986}";
    originalLine << "}";

    checkEquality(ref, originalLine);
}

void NepomukBibtex::exportBookTest()
{
    Nepomuk2::Resource ref = Nepomuk2::Resource( bookReferenceUri );

    QStringList originalLine;
    originalLine << "@book{UNITTEST-book,";
    originalLine << "	author = {UNITTEST-Author},";
    originalLine << "	date = {1986-12-31T23:00:00Z},";
    originalLine << "	edition = {UNITTEST-second},";
    originalLine << "	editor = {UNITTEST-Editor},";
    originalLine << "	issn = {UNITTEST-ISSN},";
    originalLine << "	month = {dec},";
    originalLine << "	number = {UNITTEST-number},";
    originalLine << "	publisher = {UNITTEST-Publisher},";
    originalLine << "	series = {{UNITTEST-Series}},";
    originalLine << "	title = {{UNITTEST-book}},";
    originalLine << "	volume = {UNITTEST-volume},";
    originalLine << "	year = {1986}";
    originalLine << "}";

    checkEquality(ref, originalLine);
}

void NepomukBibtex::exportBookletTest()
{
    Nepomuk2::Resource ref = Nepomuk2::Resource( bookletReferenceUri );

    QStringList originalLine;
    originalLine << "@booklet{UNITTEST-booklet,";
    originalLine << "	author = {UNITTEST-Author},";
    originalLine << "	date = {1986-12-31T23:00:00Z},";
    originalLine << "	edition = {UNITTEST-second},";
    originalLine << "	editor = {UNITTEST-Editor},";
    originalLine << "	howpublished = {UNITTEST-how published},";
    originalLine << "	issn = {UNITTEST-ISSN},";
    originalLine << "	month = {dec},";
    originalLine << "	series = {{UNITTEST-Series}},";
    originalLine << "	title = {{UNITTEST-book}},";
    originalLine << "	year = {1986}";
    originalLine << "}";

    checkEquality(ref, originalLine);
}

void NepomukBibtex::exportInBookTest()
{
    Nepomuk2::Resource ref = Nepomuk2::Resource( inBookReferenceUri );

    QStringList originalLine;
    originalLine << "@inbook{UNITTEST-inBook,";
    originalLine << "	author = {UNITTEST-Author},";
    originalLine << "	chapter = {UNITTEST-II},";
    originalLine << "	date = {1986-12-31T23:00:00Z},";
    originalLine << "	edition = {UNITTEST-second},";
    originalLine << "	editor = {UNITTEST-Editor},";
    originalLine << "	month = {dec},";
    originalLine << "	publisher = {UNITTEST-Publisher},";
    originalLine << "	title = {{UNITTEST-inbook}},";
    originalLine << "	year = {1986}";
    originalLine << "}";

    checkEquality(ref, originalLine);
}

void NepomukBibtex::exportInCollectionTest()
{
    Nepomuk2::Resource ref = Nepomuk2::Resource( inCollectionReferenceUri );

    QStringList originalLine;
    originalLine << "@incollection{UNITTEST-inCollection,";
    originalLine << "	author = {, UNITTEST-Author},";
    originalLine << "	bookauthor = {UNITTEST-Author},";
    originalLine << "	booktitle = {{UNITTEST-incollection}},";
    originalLine << "	chapter = {UNITTEST-II},";
    originalLine << "	chaptername = {UNITTEST-Chapter-Title},";
    originalLine << "	date = {1986-12-31T23:00:00Z},";
    originalLine << "	edition = {UNITTEST-second},";
    originalLine << "	editor = {UNITTEST-Editor},";
    originalLine << "	month = {dec},";
    originalLine << "	publisher = {UNITTEST-Publisher},";
    originalLine << "	title = {{UNITTEST-Chapter-Title}},";
    originalLine << "	year = {1986}";
    originalLine << "}";

    checkEquality(ref, originalLine);
}

void NepomukBibtex::exportProceedingsTest()
{
    Nepomuk2::Resource ref = Nepomuk2::Resource( proceedingsReferenceUri );

    QStringList originalLine;
    originalLine << "@proceedings{UNITTEST-proceedings,";
    originalLine << "	date = {1986-12-31T23:00:00Z},";
    originalLine << "	event = {UNITTEST-Title},";
    originalLine << "	issn = {UNITTEST-ISSN},";
    originalLine << "	month = {dec},";
    originalLine << "	organization = {UNITTEST-Organization},";
    originalLine << "	pages = {1-999},";
    originalLine << "	publisher = {UNITTEST-Publisher},";
    originalLine << "	series = {{UNITTEST-Series}},";
    originalLine << "	title = {{UNITTEST-Title: A Proceedings}},";
    originalLine << "	year = {1986}";
    originalLine << "}";

    checkEquality(ref, originalLine);
}

void NepomukBibtex::exportInProceedingsTest()
{
    Nepomuk2::Resource ref = Nepomuk2::Resource( inProceedingsReferenceUri );

    QStringList originalLine;
    originalLine << "@inproceedings{UNITTEST-inproceedings,";
    originalLine << "	author = {UNITTEST-Author},";
    originalLine << "	booktitle = {{UNITTEST-Title: A Proceedings}},";
    originalLine << "	date = {1986-12-31T23:00:00Z},";
    originalLine << "	doi = {10.4204/EPTCS},";
    originalLine << "	editor = {UNITTEST-Editor},";
    originalLine << "	event = {UNITTEST-Title},";
    originalLine << "	howpublished = {UNITTEST-Howpublished},";
    originalLine << "	issn = {UNITTEST-ISSN},";
    originalLine << "	journal = {UNITTEST-Series},";
    originalLine << "	month = {dec},";
    originalLine << "	pages = {1-999},";
    originalLine << "	title = {{UNITTEST-Title: An Inproceedings}},";
    originalLine << "	year = {1986}";
    originalLine << "}";

    checkEquality(ref, originalLine);
}

void NepomukBibtex::exportTechreportTest()
{
    Nepomuk2::Resource ref = Nepomuk2::Resource( techreportReferenceUri );

    QStringList originalLine;
    originalLine << "@techreport{UNITTEST-techreport,";
    originalLine << "	author = {UNITTEST-Author},";
    originalLine << "	date = {1986-12-31T23:00:00Z},";
    originalLine << "	howpublished = {UNITTEST-how published},";
    originalLine << "	institution = {UNITTEST-Publisher},";
    originalLine << "	month = {dec},";
    originalLine << "	organization = {UNITTEST-Organization},";
    originalLine << "	title = {{UNITTEST-techreport}},";
    originalLine << "	type = {UNITTEST-publication-type},";
    originalLine << "	year = {1986}";
    originalLine << "}";

    checkEquality(ref, originalLine);
}

void NepomukBibtex::cleanupTestCase()
{
    // remove all data created by this unittest from the nepomuk database again
    KJob *job = Nepomuk2::removeDataByApplication();
    if(!job->exec()) {
        qWarning() << job->errorString();
        QFAIL("Cleanup did not work");
    }
}

#include "nepomukbibtex.moc"
