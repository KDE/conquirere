/*
 * Copyright 2011 Jörg Ehrichs <joerg.ehichs@gmx.de>
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

#include "nbibimporterbibtex.h"

#include "encoderlatex.h"

#include "nbib.h"
#include <Nepomuk/Vocabulary/PIMO>
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Vocabulary/NCO>
#include <Nepomuk/Vocabulary/NFO>
#include <Nepomuk/Variant>

#include <Nepomuk/Query/Term>
#include <Nepomuk/Query/ResourceTerm>
#include <Nepomuk/Query/ResourceTypeTerm>
#include <Nepomuk/Query/ComparisonTerm>
#include <Nepomuk/Query/AndTerm>
#include <Nepomuk/Query/QueryServiceClient>
#include <Nepomuk/Query/Result>
#include <Nepomuk/Query/QueryParser>
#include <Nepomuk/Query/LiteralTerm>

#include <QTextStream>
#include <QRegExp>
#include <QDebug>

NBibImporterBibTex::NBibImporterBibTex()
    : NBibImporter()
{
}

bool NBibImporterBibTex::load(QIODevice *iodevice, QStringList *errorLog)
{
    // we start by fetching all contacts for the conflict checking
    // this reduce the need to query nepomuk with every new author again and again
    // the storage got out of sync otherwise
    Nepomuk::Query::ResourceTypeTerm type( Nepomuk::Vocabulary::NCO::Contact() );
    Nepomuk::Query::Query query( type );
    QList<Nepomuk::Query::Result> queryResult = Nepomuk::Query::QueryServiceClient::syncQuery(query);
    foreach(Nepomuk::Query::Result nqr, queryResult) {
        m_allContacts.append(nqr.resource());
    }

    // lets start by seperating each bibtex entry
    // no matter how the bibtext content is formated, each entry
    // starts with a line like "@ENTRYTYPE{ CITEKEY," and ends with "}" on their own line

    QList<Entry> bibEntries;
    QList<Entry> bibEntriesWithCrossref;

    QRegExp entryStart = QRegExp(QLatin1String("@([a-zA-Z]*)\\s*[{]\\s*([^,]*),*"));
    QRegExp entryContent = QRegExp(QLatin1String("^\\s*([a-zA-Z]*)\\s*[=]\\s*(.*)"));
    QRegExp entryEnd = QRegExp(QLatin1String("^\\s*[}]\\s*$"));

    QTextStream textStream(iodevice);

    Entry curEntry;
    QString curKey;
    bool inEntry = false; // we are currently inside a bibtex entry section

    while (!textStream.atEnd()) {
        QString line = textStream.readLine();

        // ignore empty lines
        if(line.isEmpty())
            continue;
        //ignore comment lines %
        //if(line.contains(QRegExp(QLatin1String("^[\\s*%]"))));
        //    continue;

        if(line.contains(entryStart)) {
            if (entryStart.indexIn(line) != -1) {
                curEntry.entryType = entryStart.cap(1);
                curEntry.citeKey = entryStart.cap(2);
                inEntry = true;
            }
        }
        else if(line.contains(entryEnd)) {
            // we check again we we are inside an entry, deals with multiple } at the end
            if(inEntry) {
                QString crossref = curEntry.content.value(QLatin1String("crossref"),QString());
                if(crossref.isEmpty()) {
                    bibEntries.append(curEntry);
                }
                else {
                    bibEntriesWithCrossref.append(curEntry);
                }
                curEntry.content.clear();
            }
            inEntry = false;
            curKey.clear();
        }
        else {
            if(!inEntry) {
                continue;
            }

            if (entryContent.indexIn(line) != -1) {
                QString key = entryContent.cap(1).toLower();
                curKey = key;
                QString value = entryContent.cap(2);

                value = EncoderLaTeX::currentEncoderLaTeX()->decode(value);
                //strip leading {" and trailing "},
                value.remove(QRegExp(QLatin1String(",$")));
                value.remove(QRegExp(QLatin1String("^\\s*[\"|{]|[\"|}]\\s*$")));
                value.remove(QRegExp(QLatin1String("[]{]|[}]"))); // better yet remove all {, }

                QString valueNew = curEntry.content.value(key,QString());

                valueNew.append(value);

                curEntry.content.insert(key,valueNew);
            }
            else {
                //this line adds content to the last used key when it spans several lines
                if(!curKey.isEmpty()) {
                    QString valueNew = curEntry.content.value(curKey,QString());
                    valueNew.append(line);

                    curEntry.content.insert(curKey,valueNew);
                }
                else {
                    qWarning() << "could not parse content line!!";
                    qWarning() << line;
                }
            }
        }
    }

    qDebug() << "import " << bibEntries.size() << "bibtex entries";

    qreal percentperFile = 100/bibEntries.size();
    int fileNumber = 0;

    //now we seperated all entries, time to inspect and import them
    foreach(Entry e, bibEntries) {
        //create a new reference and Publication
        addEntry(e);
        fileNumber++;
        emit progress( percentperFile * fileNumber );

        if(m_cancel) {
            break;
        }
    }

    if(!m_cancel)
        emit progress( 100 );

    return false;
}

void NBibImporterBibTex::addEntry(Entry e)
{
    //we create new resources and find conflicts at the end, the user has to resolve them later on
    Nepomuk::Resource publication = typeToResource(e.entryType.toLower());

    Nepomuk::Resource reference(QUrl(), Nepomuk::Vocabulary::NBIB::Reference());
    reference.setProperty(Nepomuk::Vocabulary::NBIB::publication(), publication);
    reference.setProperty(Nepomuk::Vocabulary::NBIB::citeKey(), e.citeKey);

    //before we go through the whole list one by one, we take care of some special cases

    // I. publisher/school/institution + address
    QString address = e.content.value(QLatin1String("address"),QString());
    if(!address.isEmpty()) {
        QString publisher = e.content.value(QLatin1String("publisher"),QString());
        e.content.remove(QLatin1String("school"));
        if(publisher.isEmpty()) {
            publisher = e.content.value(QLatin1String("school"),QString());
        }
        if(publisher.isEmpty()) {
            publisher = e.content.value(QLatin1String("institution"),QString());
            e.content.remove(QLatin1String("institution"));
        }

        e.content.remove(QLatin1String("address"));
        e.content.remove(QLatin1String("publisher"));

        addPublisher(address, publisher, publication);
    }

    // II. journal + number + volume
    QString journal = e.content.value(QLatin1String("journal"),QString());
    if(!journal.isEmpty()) {
        QString number = e.content.value(QLatin1String("number"),QString());
        QString volume = e.content.value(QLatin1String("volume"),QString());

        e.content.remove(QLatin1String("journal"));
        e.content.remove(QLatin1String("number"));
        e.content.remove(QLatin1String("volume"));

        addJournal(journal, volume, number, publication);
    }

    // III. archivePrefix + eprint

    //now go through the list of all remaining entries minus the crossref
    QString crossref = e.content.value(QLatin1String("crossref"),QString());
    e.content.remove(QLatin1String("crossref"));

    QMapIterator<QString, QString> i(e.content);
    while (i.hasNext()) {
        i.next();
        addContent(i.key(), i.value(), publication, reference, e.entryType.toLower());
    }

    // X. if we have a crossref entry add all missing fields from the crossref to the entry
    addContent(QLatin1String("crossref"), crossref, publication, reference, e.entryType.toLower());
}

Nepomuk::Resource NBibImporterBibTex::typeToResource(const QString & entryType)
{
    Nepomuk::Resource publication;

    if(entryType == QLatin1String("article")) {
        publication = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::Article());
    }
    else if(entryType == QLatin1String("bachelorthesis")) {
        publication = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::BachelorThesis());
    }
    else if(entryType == QLatin1String("book")) {
        publication = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::Book());
    }
    else if(entryType == QLatin1String("inbook")) {
        publication = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::Book());
    }
    else if(entryType == QLatin1String("booklet")) {
        publication = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::Booklet());
    }
    else if(entryType == QLatin1String("collection")) {
        publication = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::Collection());
    }
    else if(entryType == QLatin1String("incollection")) {
        publication = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::Collection());
    }
    else if(entryType == QLatin1String("electronic")) {
        publication = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::Electronic());
    }
    else if(entryType == QLatin1String("inproceedings")) {
        publication = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::InProceedings());
    }
    else if(entryType == QLatin1String("manual")) {
        publication = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::Manual());
    }
    else if(entryType == QLatin1String("mastersthesis")) {
        publication = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::MastersThesis());
    }
    else if(entryType == QLatin1String("phdthesis")) {
        publication = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::PhdThesis());
    }
    else if(entryType == QLatin1String("presentation")) {
        publication = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::Presentation());
    }
    else if(entryType == QLatin1String("proceedings")) {
        publication = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::Proceedings());
    }
    else if(entryType == QLatin1String("script")) {
        publication = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::Script());
    }
    else if(entryType == QLatin1String("techreport")) {
        publication = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::Techreport());
    }
    else if(entryType == QLatin1String("thesis")) {
        publication = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::Thesis());
    }
    else if(entryType == QLatin1String("unpublished")) {
        publication = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::Unpublished());
    }
    else if(entryType == QLatin1String("patent")) {
        publication = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::Patent());
    }
    else {
        // same as @Misc
        publication = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::Publication());
    }

    return publication;
}

void NBibImporterBibTex::addContent(const QString &key, const QString &value, Nepomuk::Resource publication, Nepomuk::Resource reference, const QString & originalEntryType)
{
    if(key == QLatin1String("abstract")) {
        addAbstract(value, publication);
    }
    else if(key == QLatin1String("annote")) {
        addAnnote(value, publication);
    }
    else if(key == QLatin1String("author")) {
        addAuthor(value, publication, reference, originalEntryType);
    }
    else if(key == QLatin1String("booktitle")) {
        addBooktitle(value, publication);
    }
    else if(key == QLatin1String("chapter")) {
        addChapter(value, publication, reference);
    }
    else if(key == QLatin1String("copyrigth")) {
        addCopyrigth(value, publication);
    }
    else if(key == QLatin1String("crossref")) {
        addCrossref(value, publication);
    }
    else if(key == QLatin1String("doi")) {
        addDoi(value, publication);
    }
    else if(key == QLatin1String("edition")) {
        addEdition(value, publication);
    }
    else if(key == QLatin1String("editor")) {
        addEditor(value, publication);
    }
    else if(key == QLatin1String("eprint")) {
        addEprint(value, publication);
    }
    else if(key == QLatin1String("howpublished")) {
        addHowPublished(value, publication);
    }
    else if(key == QLatin1String("institution")) {
        addInstitution(value, publication);
    }
    else if(key == QLatin1String("isbn")) {
        addIsbn(value, publication);
    }
    else if(key == QLatin1String("issn")) {
        addIssn(value, publication);
    }
    else if(key == QLatin1String("language")) {
        addLanguage(value, publication);
    }
    else if(key == QLatin1String("lccn")) {
        addLccn(value, publication);
    }
    else if(key == QLatin1String("month")) {
        addMonth(value, publication);
    }
    else if(key == QLatin1String("mrnumber")) {
        addMrNumber(value, publication);
    }
    else if(key == QLatin1String("note")) {
        addNote(value, publication);
    }
    else if(key == QLatin1String("number")) {
        addNumber(value, publication);
    }
    else if(key == QLatin1String("organization")) {
        addOrganization(value, publication);
    }
    else if(key == QLatin1String("pages")) {
        addPages(value, reference);
    }
    else if(key == QLatin1String("publisher")) {
        addPublisher(value, QString(), publication);
    }
    else if(key == QLatin1String("school")) {
        addSchool(value, publication);
    }
    else if(key == QLatin1String("series")) {
        addSeries(value, publication);
    }
    else if(key == QLatin1String("title")) {
        addTitle(value, publication, reference, originalEntryType);
    }
    else if(key == QLatin1String("type")) {
        addType(value, publication);
    }
    else if(key == QLatin1String("url")) {
        addUrl(value, publication);
    }
    else if(key == QLatin1String("volume")) {
        addVolume(value, publication);
    }
    else if(key == QLatin1String("year")) {
        addYear(value, publication);
    }
    else {
        qDebug() << "NBibImporterBibTex::addContent unknown key ::" << key << value;
    }
}

void NBibImporterBibTex::addAbstract(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NBIB::abstract(), content);
}

void NBibImporterBibTex::addAnnote(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NIE::comment(), content);
}

void NBibImporterBibTex::addAuthor(const QString &content, Nepomuk::Resource publication, Nepomuk::Resource reference, const QString & originalEntryType)
{
    QList<NBibImporterBibTex::Name> allNames = parseName(content);

    //in case of @incollection or @inbook the author is used to identify who wrote the chapter not the complete book/collection
    Nepomuk::Resource authorResource;

    if(originalEntryType == QLatin1String("inbook") || originalEntryType == QLatin1String("incollection") ) {
        Nepomuk::Resource chapter = reference.property(Nepomuk::Vocabulary::NBIB::referencedChapter()).toResource();

        if(!chapter.isValid()) {
            chapter = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::Chapter());
            reference.setProperty(Nepomuk::Vocabulary::NBIB::referencedChapter(), chapter);
            publication.addProperty(Nepomuk::Vocabulary::NBIB::chapter(), chapter);
            chapter.setProperty(Nepomuk::Vocabulary::NBIB::chapterOf(), publication);
        }
        authorResource = chapter;
    }
    else {
        authorResource = publication;
    }

    foreach(NBibImporterBibTex::Name author, allNames) {
        //check if the publisher already exist in the database
        Nepomuk::Resource a;
        foreach(Nepomuk::Resource r, m_allContacts) {
            if(r.property(Nepomuk::Vocabulary::NCO::fullname()).toString() == author.full) {
                a = r;
                break;
            }
        }
        if(!a.isValid()) {
            qDebug() << "create a new Contact resource for " << author.full;
            a = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NCO::Contact());
        }

        a.setProperty(Nepomuk::Vocabulary::NCO::fullname(), author.full);
        qDebug() << "name set for " << author.full << " :: " << a.property(Nepomuk::Vocabulary::NCO::fullname()).toString() << " uri " << a.uri();
        qDebug() << "to rresource " << authorResource.uri();
        if(!author.first.isEmpty())
            a.setProperty(Nepomuk::Vocabulary::NCO::nameGiven(), author.first);
        if(!author.last.isEmpty())
            a.setProperty(Nepomuk::Vocabulary::NCO::nameFamily(), author.last);
        if(!author.middle.isEmpty())
            a.setProperty(Nepomuk::Vocabulary::NCO::nameAdditional(), author.middle);

        authorResource.addProperty(Nepomuk::Vocabulary::NCO::creator(), a);
    }
}

void NBibImporterBibTex::addBooktitle(const QString &content, Nepomuk::Resource publication)
{
    //two specialities occure here
    // I. "booktitle" means the title of a book where "title" than means the title of the chapter
    // this is valid for any publication other than @InProceedings
    // II. "booktitle" marks the title of the @proceedings from an @InProceedings or @Conference

    if(publication.hasType(Nepomuk::Vocabulary::NBIB::InProceedings())) {
        //check if a resource @Proceedings with the name of content exist or create a new one

        // fetcha data
        Nepomuk::Query::ComparisonTerm title( Nepomuk::Vocabulary::NIE::title(), Nepomuk::Query::LiteralTerm( content ) );
        Nepomuk::Query::ResourceTypeTerm type( Nepomuk::Vocabulary::NBIB::Proceedings() );

        Nepomuk::Query::Query query( Nepomuk::Query::AndTerm( type, title ) );

        QList<Nepomuk::Query::Result> queryResult = Nepomuk::Query::QueryServiceClient::syncQuery(query);

        Nepomuk::Resource a;
        if(!queryResult.isEmpty()) {
            if(queryResult.size() > 1) {
                qWarning() << "found more than 1 book/collection with the name " << content;

                //now we search deeper as we do get false results
                // Example A.M. Bronstein and M.M. Bronstein will be found with the same query
                foreach(Nepomuk::Query::Result nqr, queryResult) {
                    if( nqr.resource().genericLabel() == content) {
                        a = nqr.resource();
                    }
                }

                // we found just false results ... create a new one
                if(!a.isValid()) {
                    a = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::Proceedings());
                    a.setProperty(Nepomuk::Vocabulary::NIE::title(), content);
                }
            }
            else {
                a = queryResult.first().resource();
            }
        }
        else {
            a = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::Proceedings());
            a.setProperty(Nepomuk::Vocabulary::NIE::title(), content);
        }

        //at this point we have a valid proceedings entry connect it to the publication
        publication.setProperty(Nepomuk::Vocabulary::NBIB::proceedings(), a);
        a.setProperty(Nepomuk::Vocabulary::NBIB::proceedingsOf(), publication);
    }
    else {
        publication.setProperty(Nepomuk::Vocabulary::NIE::title(), content);
    }
}

void NBibImporterBibTex::addChapter(const QString &content, Nepomuk::Resource publication, Nepomuk::Resource reference)
{
    Nepomuk::Resource c = reference.property(Nepomuk::Vocabulary::NBIB::referencedChapter()).toResource();

    if(!c.isValid()) {
        c = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::Chapter());
        reference.setProperty(Nepomuk::Vocabulary::NBIB::referencedChapter(), c);
        publication.addProperty(Nepomuk::Vocabulary::NBIB::chapter(), c);
        c.setProperty(Nepomuk::Vocabulary::NBIB::chapterOf(), publication);
    }

    c.setProperty( Nepomuk::Vocabulary::NBIB::chapterNumber(), content);
}

void NBibImporterBibTex::addCopyrigth(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NIE::copyright(), content);
}

void NBibImporterBibTex::addCrossref(const QString &content, Nepomuk::Resource publication)
{
    qWarning() << "NBibImporterBibTex::addCrossref needs implementation";
}

void NBibImporterBibTex::addDoi(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NBIB::doi(), content);
}

void NBibImporterBibTex::addEdition(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NBIB::edition(), content);
}

void NBibImporterBibTex::addEditor(const QString &content, Nepomuk::Resource publication)
{
    QList<NBibImporterBibTex::Name> allNames = parseName(content);

    foreach(NBibImporterBibTex::Name editor, allNames) {
        //check if the editor already exist in the database
        Nepomuk::Resource e;

        foreach(Nepomuk::Resource r, m_allContacts) {
            if(r.property(Nepomuk::Vocabulary::NCO::fullname()).toString() == editor.full) {
                e = r;
                break;
            }
        }
        if(!e.isValid()) {
            qDebug() << "create a new Contact resource for " << editor.full;
            e = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NCO::Contact());
        }

        e.setProperty(Nepomuk::Vocabulary::NCO::fullname(), editor.full);
        if(!editor.first.isEmpty())
            e.setProperty(Nepomuk::Vocabulary::NCO::nameGiven(), editor.first);
        if(!editor.last.isEmpty())
            e.setProperty(Nepomuk::Vocabulary::NCO::nameFamily(), editor.last);
        if(!editor.middle.isEmpty())
            e.setProperty(Nepomuk::Vocabulary::NCO::nameAdditional(), editor.middle);

        //publication.addProperty(Nepomuk::Vocabulary::NBIB::editor(), e);
        publication.addProperty(Nepomuk::Vocabulary::NBIB::editor(), e);
    }
}

void NBibImporterBibTex::addEprint(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NBIB::eprint(), content);
}

void NBibImporterBibTex::addHowPublished(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NBIB::publicationMethod(), content);
}

void NBibImporterBibTex::addInstitution(const QString &content, Nepomuk::Resource publication)
{
    addPublisher(content, QString(), publication);
}

void NBibImporterBibTex::addIsbn(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NBIB::isbn(), content);
}

void NBibImporterBibTex::addIssn(const QString &content, Nepomuk::Resource publication)
{
    //fetch already existing Series or create a new one
    Nepomuk::Resource series = publication.property(Nepomuk::Vocabulary::NBIB::inSeries()).toResource();
    if(!series.isValid()) {
        series = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::BookSeries());
        series.setProperty(Nepomuk::Vocabulary::NBIB::seriesOf(), publication);
        publication.setProperty(Nepomuk::Vocabulary::NBIB::inSeries(), series);
    }

    series.setProperty(Nepomuk::Vocabulary::NBIB::issn(), content);
}

void NBibImporterBibTex::addJournal(const QString &journal, const QString &volume, const QString &number, Nepomuk::Resource publication)
{
    //find existing journal or create a new series of them
    Nepomuk::Resource journalResource;
    Nepomuk::Resource journalIssue;

    // fetcha data
    Nepomuk::Query::ComparisonTerm jorunalName( Nepomuk::Vocabulary::NIE::title(), Nepomuk::Query::LiteralTerm( journal ) );
    Nepomuk::Query::ResourceTypeTerm type( Nepomuk::Vocabulary::NBIB::Journal() );

    Nepomuk::Query::Query query( Nepomuk::Query::AndTerm( type, jorunalName ) );

    QList<Nepomuk::Query::Result> queryResult = Nepomuk::Query::QueryServiceClient::syncQuery(query);

    if(!queryResult.isEmpty()) {
        if(queryResult.size() > 1) {
            qWarning() << "found more than 1 journal with the name " << journal;

            //now we search deeper as we do get false results
            // Example A.M. Bronstein and M.M. Bronstein will be found with the same query
            foreach(Nepomuk::Query::Result nqr, queryResult) {
                if( nqr.resource().genericLabel() == journal) {
                    journalResource = nqr.resource();
                }
            }

            // we found just false results ... create a new one
            if(!journalResource.isValid()) {
                journalResource = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::Journal());
            }
        }
        else {
            journalResource = queryResult.first().resource();
        }
    }
    else {
        journalResource = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::Journal());
    }

    journalResource.setProperty(Nepomuk::Vocabulary::NIE::title(), journal);

    // now check if the journalIssue exists already
    QList<Nepomuk::Resource> issues = journalResource.property(Nepomuk::Vocabulary::NBIB::hasIssue()).toResourceList();

    foreach(Nepomuk::Resource issue, issues) {
        QString checkNumber = issue.property(Nepomuk::Vocabulary::NBIB::issueNumber()).toString();
        QString checkVolume = issue.property(Nepomuk::Vocabulary::NBIB::volume()).toString();

        if( checkNumber == number && checkVolume == volume) {
            journalIssue = issue;
            break;
        }
    }

    //if we can't find an existing journal issue, create a new one
    if(!journalIssue.isValid()) {
        journalIssue = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::JournalIssue());
        journalIssue.setProperty(Nepomuk::Vocabulary::NBIB::issueNumber(), number);
        journalIssue.setProperty(Nepomuk::Vocabulary::NBIB::volume(), volume);
        // duplicate title join journal and journalissue, helps to easily identify those two
        // but is more like a better way to create a prefLabel / genericLabel
        journalIssue.setProperty(Nepomuk::Vocabulary::NIE::title(), journal);

        // connect issue <-> journal
        journalIssue.setProperty(Nepomuk::Vocabulary::NBIB::journal(), journalResource);
        journalResource.addProperty(Nepomuk::Vocabulary::NBIB::hasIssue(), journalIssue);
    }

    // now connect the issue to the Publication
    publication.setProperty(Nepomuk::Vocabulary::NBIB::journalIssue(), journalIssue);
    journalIssue.addProperty(Nepomuk::Vocabulary::NBIB::hasArticle(), publication);
}

void NBibImporterBibTex::addLanguage(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NIE::language(), content);
}

void NBibImporterBibTex::addLccn(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NBIB::lccn(), content);
}

void NBibImporterBibTex::addMonth(const QString &content, Nepomuk::Resource publication)
{
    //fetch already existing publication or create a new one
    QString date = publication.property(Nepomuk::Vocabulary::NBIB::publicationDate()).toString();
    QString year = QString::number(0000);
    QString month = QString::number(00);
    QString day = QString::number(00);

    if(!date.isEmpty()) {
        QRegExp rx(QLatin1String("(\\d*)-(\\d*)-(\\d*)*"));
        if (rx.indexIn(date) != -1) {
            year = rx.cap(1);
            month = rx.cap(2);
            day = rx.cap(3);
        }
    }

    QString contentMonth = content.toLower();
    //transform bibtex month to numbers
    if(contentMonth.contains(QLatin1String("jan"))) {
        month = QString::number(1);
    }
    else if(contentMonth.contains(QLatin1String("feb"))) {
        month = QString::number(2);
    }
    else if(contentMonth.contains(QLatin1String("mar"))) {
        month = QString::number(3);
    }
    else if(contentMonth.contains(QLatin1String("apr"))) {
        month = QString::number(4);
    }
    else if(contentMonth.contains(QLatin1String("may"))) {
        month = QString::number(5);
    }
    else if(contentMonth.contains(QLatin1String("jun"))) {
        month = QString::number(6);
    }
    else if(contentMonth.contains(QLatin1String("jul"))) {
        month = QString::number(7);
    }
    else if(contentMonth.contains(QLatin1String("aug"))) {
        month = QString::number(8);
    }
    else if(contentMonth.contains(QLatin1String("sep"))) {
        month = QString::number(9);
    }
    else if(contentMonth.contains(QLatin1String("oct"))) {
        month = QString::number(0);
    }
    else if(contentMonth.contains(QLatin1String("nov"))) {
        month = QString::number(1);
    }
    else if(contentMonth.contains(QLatin1String("dec"))) {
        month = QString::number(12);
    }

    QString newDate = year + QLatin1String("-") + month + QLatin1String("-") + day;
    publication.setProperty(Nepomuk::Vocabulary::NBIB::publicationDate(), newDate);
}

void NBibImporterBibTex::addMrNumber(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NBIB::mrNumber(), content);
}

void NBibImporterBibTex::addNote(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NIE::description(), content);
}

void NBibImporterBibTex::addNumber(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NBIB::issueNumber(), content);
}

void NBibImporterBibTex::addOrganization(const QString &content, Nepomuk::Resource publication)
{
    // create a new organization with the string s as title
    Nepomuk::Resource newOrganizatzion(QUrl(), Nepomuk::Vocabulary::NCO::OrganizationContact());
    newOrganizatzion.setProperty(Nepomuk::Vocabulary::NCO::fullname(), content);

    if(publication.hasType(Nepomuk::Vocabulary::NBIB::InProceedings())) {
        Nepomuk::Resource proc = publication.property(Nepomuk::Vocabulary::NBIB::proceedings()).toResource();

        if(!proc.isValid()) {
            proc = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::Proceedings());
            proc.setProperty(Nepomuk::Vocabulary::NBIB::proceedingsOf(), publication);
            publication.setProperty(Nepomuk::Vocabulary::NBIB::proceedings(), proc);
        }
        proc.setProperty(Nepomuk::Vocabulary::NBIB::organization(), newOrganizatzion);
    }
    else {
        publication.setProperty(Nepomuk::Vocabulary::NBIB::organization(), newOrganizatzion);
    }
}

void NBibImporterBibTex::addPages(const QString &content, Nepomuk::Resource reference)
{
    reference.setProperty(Nepomuk::Vocabulary::NBIB::pages(), content);
}

void NBibImporterBibTex::addPublisher(const QString &publisherString, const QString &address, Nepomuk::Resource publication)
{
    // create the address object
    Nepomuk::Resource addr(QUrl(), Nepomuk::Vocabulary::NCO::PostalAddress());
    //FIXME extendedAddress is not correct, but determining which part of the @p address is the street/location and so on is nearly impossible

    if(!address.isEmpty())
        addr.setProperty(Nepomuk::Vocabulary::NCO::extendedAddress(), address);

    QList<NBibImporterBibTex::Name> allNames = parseName(publisherString);

    foreach(NBibImporterBibTex::Name publisher, allNames) {
        //check if the publisher already exist in the database

        Nepomuk::Resource p;

        foreach(Nepomuk::Resource r, m_allContacts) {
            if(r.property(Nepomuk::Vocabulary::NCO::fullname()).toString() == publisher.full) {
                p = r;
                break;
            }
        }
        if(!p.isValid()) {
            qDebug() << "create a new Contact resource for " << publisher.full;
            p = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NCO::Contact());
        }


        p.setProperty(Nepomuk::Vocabulary::NCO::fullname(), publisher.full);
        if(!publisher.first.isEmpty())
            p.setProperty(Nepomuk::Vocabulary::NCO::nameGiven(), publisher.first);
        if(!publisher.last.isEmpty())
            p.setProperty(Nepomuk::Vocabulary::NCO::nameFamily(), publisher.last);
        if(!publisher.middle.isEmpty())
            p.setProperty(Nepomuk::Vocabulary::NCO::nameAdditional(), publisher.middle);

        p.setProperty(Nepomuk::Vocabulary::NCO::hasPostalAddress(), addr);

        publication.addProperty(Nepomuk::Vocabulary::NCO::publisher(), p);
    }
}

void NBibImporterBibTex::addSchool(const QString &content, Nepomuk::Resource publication)
{
    addPublisher(content, QString(), publication);
}

void NBibImporterBibTex::addSeries(const QString &content, Nepomuk::Resource publication)
{
    //fetch already existing Series or create a new one
    Nepomuk::Resource series = publication.property(Nepomuk::Vocabulary::NBIB::inSeries()).toResource();
    if(!series.isValid()) {
        series = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::BookSeries());
        series.setProperty(Nepomuk::Vocabulary::NBIB::seriesOf(), publication);
        publication.setProperty(Nepomuk::Vocabulary::NBIB::inSeries(), series);
    }

    series.setProperty(Nepomuk::Vocabulary::NIE::title(), content);
}

void NBibImporterBibTex::addTitle(const QString &content, Nepomuk::Resource publication, Nepomuk::Resource reference, const QString & originalEntryType)
{
    // in the case of @InBook or @InCollection title means title of the chapter
    // while booktitle is the actual title of the book
    if(originalEntryType == QLatin1String("inbook") || originalEntryType == QLatin1String("incollection") ) {

        Nepomuk::Resource c = reference.property(Nepomuk::Vocabulary::NBIB::referencedChapter()).toResource();

        if(!c.isValid()) {
            c = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::Chapter());
            reference.setProperty(Nepomuk::Vocabulary::NBIB::referencedChapter(), c);
            publication.addProperty(Nepomuk::Vocabulary::NBIB::chapter(), c);
            c.setProperty(Nepomuk::Vocabulary::NBIB::chapterOf(), publication);
        }

        c.setProperty( Nepomuk::Vocabulary::NIE::title(), content);
    }
    else {
        publication.setProperty(Nepomuk::Vocabulary::NIE::title(), content);
    }
}

void NBibImporterBibTex::addType(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NBIB::type(), content);
}

void NBibImporterBibTex::addUrl(const QString &content, Nepomuk::Resource publication)
{

}

void NBibImporterBibTex::addVolume(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NBIB::volume(), content);
}

void NBibImporterBibTex::addYear(const QString &content, Nepomuk::Resource publication)
{
    //fetch already existing publication or create a new one
    QString date = publication.property(Nepomuk::Vocabulary::NBIB::publicationDate()).toString();
    QString year = QString::number(0000);
    QString month = QString::number(00);
    QString day = QString::number(00);

    if(!date.isEmpty()) {
        QRegExp rx(QLatin1String("(\\d*)-(\\d*)-(\\d*)*"));
        if (rx.indexIn(date) != -1) {
            year = rx.cap(1);
            month = rx.cap(2);
            day = rx.cap(3);
        }
    }

    QString newDate = content.trimmed() + QLatin1String("-") + month + QLatin1String("-") + day;
    publication.setProperty(Nepomuk::Vocabulary::NBIB::publicationDate(), newDate);
}

QList<NBibImporterBibTex::Name> NBibImporterBibTex::parseName(const QString & nameString)
{
    QList<NBibImporterBibTex::Name> allNames;

    //first split several authors
    QStringList authors = nameString.split(QLatin1String(" and "));

    foreach(QString author, authors) {
        //remove unneccesary {}
        author.remove(QLatin1String("{"));
        author.remove(QLatin1String("}"));

        //split names entered with ,
        QStringList names = author.split(QRegExp(QLatin1String("\\s*,\\s*")), QString::SkipEmptyParts);

        Name newname;

        if(names.size() > 1) {
            newname.first = names.takeFirst();
            newname.last = names.takeLast();
            // I assume there is only 1 middle name
            if(!names.isEmpty())
                newname.middle = names.takeFirst();

            newname.full = newname.first + QLatin1String(" ") + newname.middle + QLatin1String(" ") + newname.last;
        }
        else {
            newname.full = names.first();
        }
        allNames.append(newname);
    }

    return allNames;
}
