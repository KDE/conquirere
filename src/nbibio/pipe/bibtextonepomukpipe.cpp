/*
 * Copyright 2011 Jörg Ehrichs <joerg.ehrichs@gmx.de>
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

#include "bibtextonepomukpipe.h"

#include <kbibtex/entry.h>

#include "nbib.h"
#include <Nepomuk/Vocabulary/PIMO>
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Vocabulary/NCO>
#include <Nepomuk/Vocabulary/NFO>
#include <Nepomuk/Variant>
#include <Nepomuk/Tag>

#include <Nepomuk/Query/QueryServiceClient>
#include <Nepomuk/Query/Result>
#include <Nepomuk/Query/Term>
#include <Nepomuk/Query/LiteralTerm>
#include <Nepomuk/Query/ResourceTerm>
#include <Nepomuk/Query/ResourceTypeTerm>
#include <Nepomuk/Query/ComparisonTerm>
#include <Nepomuk/Query/AndTerm>

#include <Akonadi/Item>
#include <KABC/Addressee>
#include <Akonadi/ItemFetchJob>
#include <Akonadi/ItemCreateJob>
#include <Akonadi/CollectionCreateJob>
#include <Akonadi/CollectionFetchJob>
#include <Akonadi/CollectionFetchScope>

#include <QtCore/QDebug>

BibTexToNepomukPipe::BibTexToNepomukPipe()
{
}

BibTexToNepomukPipe::~BibTexToNepomukPipe()
{

}

void BibTexToNepomukPipe::pipeExport(File & bibEntries)
{
    emit progress(0);

    //create the collection used for importing

    // we start by fetching all contacts for the conflict checking
    // this reduce the need to query nepomuk with every new author again and again
    Nepomuk::Query::ResourceTypeTerm type( Nepomuk::Vocabulary::NCO::Contact() );
    Nepomuk::Query::Query query( type );
    QList<Nepomuk::Query::Result> queryResult = Nepomuk::Query::QueryServiceClient::syncQuery(query);
    foreach(const Nepomuk::Query::Result & nqr, queryResult) {
        QString fullname = nqr.resource().property(Nepomuk::Vocabulary::NCO::fullname()).toString();
        m_allContacts.insert(fullname, nqr.resource());
    }
    qDebug() << "fetched all" << queryResult.size() << "contacts";
    Nepomuk::Query::ResourceTypeTerm type2( Nepomuk::Vocabulary::NBIB::Publication() );
    Nepomuk::Query::Query query2( type2 );
    QList<Nepomuk::Query::Result> queryResult2 = Nepomuk::Query::QueryServiceClient::syncQuery(query2);
    foreach(const Nepomuk::Query::Result & nqr, queryResult2) {
        m_allPublications.append(nqr.resource());
    }
    qDebug() << "fetched all publications";
    Nepomuk::Query::ResourceTypeTerm type3( Nepomuk::Vocabulary::NBIB::Reference() );
    Nepomuk::Query::Query query3( type3 );
    QList<Nepomuk::Query::Result> queryResult3 = Nepomuk::Query::QueryServiceClient::syncQuery(query3);
    foreach(const Nepomuk::Query::Result & nqr, queryResult3) {
        m_allReferences.append(nqr.resource());
    }
    qDebug() << "fetched all refrences";

    int maxValue = bibEntries.size();
    qreal perFileProgress = (100.0/(qreal)maxValue);

    int i = 0;
    foreach(Element *e, bibEntries ) {
        Entry *entry = dynamic_cast<Entry *>(e);

        if(entry) {
            if(entry->contains(Entry::ftCrossRef)) {
                Entry *entry2 = Entry::resolveCrossref(*entry, &bibEntries);
                import(entry2);
            }
            else {
                import(entry);
            }
        }
        i++;

        int p = i * perFileProgress;

        emit progress(p);
    }

    emit progress(100);
}


void BibTexToNepomukPipe::setAkonadiAddressbook(Akonadi::Collection & addressbook)
{
    m_addressbook = addressbook;
}

void BibTexToNepomukPipe::import(Entry *e)
{
    //do not check duplicates, just add new resources to the system storage
    QUrl typeUrl = typeToUrl(e->type());
    Nepomuk::Resource publication = Nepomuk::Resource(QUrl(), typeUrl);

    Nepomuk::Resource reference = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::Reference());
    reference.setProperty(Nepomuk::Vocabulary::NBIB::citeKey(), e->id());
    reference.setLabel(e->id()); // adds no real value, but looks nicer in the Nepomuk shell
    reference.setProperty(Nepomuk::Vocabulary::NBIB::publication(), publication);
    publication.addProperty(Nepomuk::Vocabulary::NBIB::reference(), reference);

    //before we go through the whole list one by one, we take care of some special cases

    // I. publisher/school/institution + address
    //    means address belongs to publisher
    if(e->contains(QLatin1String("address"))) {
        Value publisher;
        if(e->contains(QLatin1String("publisher")))
            publisher = e->value(QLatin1String("publisher"));
        else if(e->contains(QLatin1String("school")))
            publisher = e->value(QLatin1String("school"));
        else if(e->contains(QLatin1String("institution")))
            publisher = e->value(QLatin1String("institution"));

        addPublisher(publisher, e->value(QLatin1String("address")),  publication);

        e->remove(QLatin1String("institution"));
        e->remove(QLatin1String("publisher"));
        e->remove(QLatin1String("school"));
        e->remove(QLatin1String("address"));
    }

    // II. journal + number + volume
    if(e->contains(QLatin1String("journal"))) {

        addJournal(e->value(QLatin1String("journal")),
                   e->value(QLatin1String("volume")),
                   e->value(QLatin1String("number")),
                   publication);

        e->remove(QLatin1String("journal"));
        e->remove(QLatin1String("number"));
        e->remove(QLatin1String("volume"));
    }

    // III. archivePrefix + eprint

    //now go through the list of all remaining entries
    QMapIterator<QString, Value> i(*e);
    while (i.hasNext()) {
        i.next();
        addContent(i.key(), i.value(), publication, reference, e->type());
    }
}

QUrl BibTexToNepomukPipe::typeToUrl(const QString & entryType)
{
    if(entryType == QLatin1String("article")) {
        return Nepomuk::Vocabulary::NBIB::Article();
    }
    else if(entryType == QLatin1String("bachelorthesis")) {
        return Nepomuk::Vocabulary::NBIB::BachelorThesis();
    }
    else if(entryType == QLatin1String("book")) {
        return Nepomuk::Vocabulary::NBIB::Book();
    }
    else if(entryType == QLatin1String("inbook")) {
        return  Nepomuk::Vocabulary::NBIB::Book();
    }
    else if(entryType == QLatin1String("booklet")) {
        return Nepomuk::Vocabulary::NBIB::Booklet();
    }
    else if(entryType == QLatin1String("collection")) {
        return Nepomuk::Vocabulary::NBIB::Collection();
    }
    else if(entryType == QLatin1String("incollection")) {
        return Nepomuk::Vocabulary::NBIB::Collection();
    }
    else if(entryType == QLatin1String("electronic")) {
        return Nepomuk::Vocabulary::NBIB::Electronic();
    }
    else if(entryType == QLatin1String("inproceedings")) {
        return Nepomuk::Vocabulary::NBIB::Article();
    }
    else if(entryType == QLatin1String("manual")) {
        return Nepomuk::Vocabulary::NBIB::Manual();
    }
    else if(entryType == QLatin1String("mastersthesis")) {
        return Nepomuk::Vocabulary::NBIB::MastersThesis();
    }
    else if(entryType == QLatin1String("phdthesis")) {
        return Nepomuk::Vocabulary::NBIB::PhdThesis();
    }
    else if(entryType == QLatin1String("presentation")) {
        return Nepomuk::Vocabulary::NBIB::Presentation();
    }
    else if(entryType == QLatin1String("proceedings")) {
        return Nepomuk::Vocabulary::NBIB::Proceedings();
    }
    else if(entryType == QLatin1String("script")) {
        return Nepomuk::Vocabulary::NBIB::Script();
    }
    else if(entryType == QLatin1String("techreport")) {
        return Nepomuk::Vocabulary::NBIB::Techreport();
    }
    else if(entryType == QLatin1String("thesis")) {
        return Nepomuk::Vocabulary::NBIB::Thesis();
    }
    else if(entryType == QLatin1String("unpublished")) {
        return Nepomuk::Vocabulary::NBIB::Unpublished();
    }
    else if(entryType == QLatin1String("patent")) {
        return Nepomuk::Vocabulary::NBIB::Patent();
    }
    else {
        // same as @Misc
        return Nepomuk::Vocabulary::NBIB::Publication();
    }
}


void BibTexToNepomukPipe::addContent(const QString &key, const Value &value, Nepomuk::Resource publication, Nepomuk::Resource reference, const QString & originalEntryType)
{
    qDebug() << "BibTexToNepomukPipe::addContent :: " << key << PlainTextValue::text(value);
    if(key == QLatin1String("abstract")) {
        addAbstract(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("annote")) {
        addAnnote(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("author")) {
        addAuthor(value, publication, reference, originalEntryType);
    }
    else if(key == QLatin1String("booktitle")) {
        addBooktitle(PlainTextValue::text(value), publication, originalEntryType);
    }
    else if(key == QLatin1String("chapter")) {
        addChapter(PlainTextValue::text(value), publication, reference);
    }
    else if(key == QLatin1String("copyrigth")) {
        addCopyrigth(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("doi")) {
        addDoi(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("edition")) {
        addEdition(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("editor")) {
        addEditor(value, publication);
    }
    else if(key == QLatin1String("eprint")) {
        addEprint(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("howpublished")) {
        addHowPublished(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("institution")) {
        addInstitution(value, publication);
    }
    else if(key == QLatin1String("isbn")) {
        addIsbn(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("issn")) {
        addIssn(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("language")) {
        addLanguage(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("lccn")) {
        addLccn(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("month")) {
        addMonth(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("mrnumber")) {
        addMrNumber(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("note")) {
        addNote(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("number")) {
        addNumber(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("organization")) {
        addOrganization(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("pages")) {
        addPages(PlainTextValue::text(value), reference);
    }
    else if(key == QLatin1String("pubmed")) {
        addPubMed(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("publisher")) {
        Value empty;
        addPublisher(value, empty, publication);
    }
    else if(key == QLatin1String("school")) {
        addSchool(value, publication);
    }
    else if(key == QLatin1String("series")) {
        addSeries(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("title")) {
        addTitle(PlainTextValue::text(value), publication, reference, originalEntryType);
    }
    else if(key == QLatin1String("type")) {
        addType(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("url")) {
        addUrl(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("volume")) {
        addVolume(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("year")) {
        addYear(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("keywords")) {
        addKewords(value, publication);
    }
    else {
        qDebug() << "BibTexToNepomukPipe::addContent unknown key ::" << key << PlainTextValue::text(value);
    }
}

void BibTexToNepomukPipe::addPublisher(const Value &publisherValue, const Value &addressValue, Nepomuk::Resource publication)
{
    QString address = PlainTextValue::text(addressValue);

    // create the address object
    Nepomuk::Resource addr(QUrl(), Nepomuk::Vocabulary::NCO::PostalAddress());
    //FIXME extendedAddress is not correct, but determining which part of the @p address is the street/location and so on is nearly impossible

    if(!address.isEmpty())
        addr.setProperty(Nepomuk::Vocabulary::NCO::extendedAddress(), address);

    foreach(ValueItem *publisherItem, publisherValue) {
        //transform KBibTex representation of the name into my own Name
        Name publisher;
        Person *person = dynamic_cast<Person *>(publisherItem);
        if(person) {
            publisher.first = person->firstName();
            publisher.last = person->lastName();
            publisher.suffix = person->suffix();
            publisher.full = publisher.first + QLatin1String(" ") + publisher.last + QLatin1String(" ") + publisher.suffix;
        }
        else {
            publisher.full = PlainTextValue::text(*publisherItem);
        }

        //check if the publisher already exist in the database
        Nepomuk::Resource p = m_allContacts.value(publisher.full, Nepomuk::Resource());

        if(!p.isValid()) {
            qDebug() << "create a new Contact resource for " << publisher.full;
            // publisher could be a person or a organization, use Contact and let the user define it later on if he wishes
            p = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NCO::Contact());

            p.setProperty(Nepomuk::Vocabulary::NCO::fullname(), publisher.full);
            if(!publisher.first.isEmpty())
                p.setProperty(Nepomuk::Vocabulary::NCO::nameGiven(), publisher.first);
            if(!publisher.last.isEmpty())
                p.setProperty(Nepomuk::Vocabulary::NCO::nameFamily(), publisher.last);
            if(!publisher.suffix.isEmpty())
                p.setProperty(Nepomuk::Vocabulary::NCO::nameHonorificSuffix(), publisher.suffix);

            m_allContacts.insert(publisher.full,p);
        }

        Nepomuk::Resource existingAddr = p.property(Nepomuk::Vocabulary::NCO::hasPostalAddress()).toResource();
        if(!existingAddr.isValid())
            p.setProperty(Nepomuk::Vocabulary::NCO::hasPostalAddress(), addr);

        publication.addProperty(Nepomuk::Vocabulary::NCO::publisher(), p);
    }
}

void BibTexToNepomukPipe::addJournal(const Value &journalValue, const Value &volumeValue, const Value &numberValue, Nepomuk::Resource publication)
{
    QString journalName = PlainTextValue::text(journalValue);
    QString volume = PlainTextValue::text(volumeValue);
    QString number = PlainTextValue::text(numberValue);

    //find existing journal or create a new series of them
    Nepomuk::Resource journalResource;
    Nepomuk::Resource journalIssue;

    // fetcha data
    Nepomuk::Query::ComparisonTerm jorunalName( Nepomuk::Vocabulary::NIE::title(), Nepomuk::Query::LiteralTerm( journalName ) );
    Nepomuk::Query::ResourceTypeTerm type( Nepomuk::Vocabulary::NBIB::Journal() );

    Nepomuk::Query::Query query( Nepomuk::Query::AndTerm( type, jorunalName ) );

    QList<Nepomuk::Query::Result> queryResult = Nepomuk::Query::QueryServiceClient::syncQuery(query);

    if(!queryResult.isEmpty()) {
        if(queryResult.size() > 1) {
            qWarning() << "found more than 1 journal with the name " << journalName;

            //now we search deeper as we do get false results
            // Example A.M. Bronstein and M.M. Bronstein will be found with the same query
            foreach(const Nepomuk::Query::Result & nqr, queryResult) {
                if( nqr.resource().genericLabel() == journalName) {
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

    journalResource.setProperty(Nepomuk::Vocabulary::NIE::title(), journalName);

    // now check if the journalIssue exists already
    QList<Nepomuk::Resource> issues = journalResource.property(Nepomuk::Vocabulary::NBIB::seriesOf()).toResourceList();

    foreach(const Nepomuk::Resource & issue, issues) {
        QString checkNumber = issue.property(Nepomuk::Vocabulary::NBIB::number()).toString();
        QString checkVolume = issue.property(Nepomuk::Vocabulary::NBIB::volume()).toString();

        if( checkNumber == number && checkVolume == volume) {
            journalIssue = issue;
            break;
        }
    }

    //if we can't find an existing journal issue, create a new one
    if(!journalIssue.isValid()) {
        journalIssue = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::JournalIssue());
        journalIssue.setProperty(Nepomuk::Vocabulary::NBIB::number(), number);
        journalIssue.setProperty(Nepomuk::Vocabulary::NBIB::volume(), volume);
        // duplicate title join journal and journalissue, helps to easily identify those two
        // but is more like a better way to create a prefLabel / genericLabel
        journalIssue.setProperty(Nepomuk::Vocabulary::NIE::title(), journalName);

        // connect issue <-> journal
        journalIssue.setProperty(Nepomuk::Vocabulary::NBIB::inSeries(), journalResource);
        journalResource.addProperty(Nepomuk::Vocabulary::NBIB::seriesOf(), journalIssue);
    }

    // now connect the issue to the Publication/Collection
    publication.setProperty(Nepomuk::Vocabulary::NBIB::collection(), journalIssue);
    journalIssue.addProperty(Nepomuk::Vocabulary::NBIB::article(), publication);
}

void BibTexToNepomukPipe::addAbstract(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NBIB::abstract(), content);
}

void BibTexToNepomukPipe::addAnnote(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NIE::comment(), content);
}

void BibTexToNepomukPipe::addAuthor(const Value &contentValue, Nepomuk::Resource publication, Nepomuk::Resource reference, const QString & originalEntryType)
{
    //in case of @incollection or @inbook the author is used to identify who wrote the chapter not the complete book/collection
    Nepomuk::Resource authorResource;

    if(originalEntryType == QLatin1String("inbook") || originalEntryType == QLatin1String("incollection") ) {
        Nepomuk::Resource chapter = reference.property(Nepomuk::Vocabulary::NBIB::referencedPart()).toResource();

        if(!chapter.isValid()) {
            chapter = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::Chapter());
            reference.setProperty(Nepomuk::Vocabulary::NBIB::referencedPart(), chapter);
            publication.addProperty(Nepomuk::Vocabulary::NBIB::documentPart(), chapter);
            chapter.setProperty(Nepomuk::Vocabulary::NBIB::documentPartOf(), publication);
        }
        authorResource = chapter;
    }
    else {
        authorResource = publication;
    }

    foreach(ValueItem *authorItem, contentValue) {
        //transform KBibTex representation of the name into my own Name
        Name author;
        Person *person = dynamic_cast<Person *>(authorItem);
        if(person) {
            author.first = person->firstName();
            author.last = person->lastName();
            author.suffix = person->suffix();
            author.full = author.first + QLatin1String(" ") + author.last + QLatin1String(" ") + author.suffix;
        }
        else {
            author.full = PlainTextValue::text(*authorItem);
        }

        //check if the publisher already exist in the database
        Nepomuk::Resource a = m_allContacts.value(author.full, Nepomuk::Resource());

        if(!a.isValid()) {
            qDebug() << "create a new Contact resource for " << author.full;

            if(m_addressbook.isValid()) {
                qDebug() << "add author to akonadi";
                KABC::Addressee addr;
                addr.setFamilyName( author.last );
                addr.setGivenName( author.first );
                addr.setAdditionalName( author.suffix );
                addr.setName( author.full );
                addr.setFormattedName( author.full );

                Akonadi::Item item;
                item.setMimeType( KABC::Addressee::mimeType() );
                item.setPayload<KABC::Addressee>( addr );

                Akonadi::ItemCreateJob *job = new Akonadi::ItemCreateJob( item, m_addressbook );

                if ( !job->exec() ) {
                    qDebug() << "Error:" << job->errorString();
                }

                // akonadi saves its contacts with a specific nepomuk uri, we use it here to
                // connect the resource to the publication
                // akonadi will then always update this resource
                a = Nepomuk::Resource(job->item().url(), Nepomuk::Vocabulary::NCO::PersonContact());
                //a.setProperty("http://akonadi-project.org/ontologies/aneo#akonadiItemId", job->item().id());

                a.setProperty(Nepomuk::Vocabulary::NCO::fullname(), author.full);

                if(!author.first.isEmpty())
                    a.setProperty(Nepomuk::Vocabulary::NCO::nameGiven(), author.first);
                if(!author.last.isEmpty())
                    a.setProperty(Nepomuk::Vocabulary::NCO::nameFamily(), author.last);
                if(!author.suffix.isEmpty())
                    a.setProperty(Nepomuk::Vocabulary::NCO::nameAdditional(), author.suffix);

                qDebug() << "akonadi/nepomuk id" << job->item().url() << a.isValid() << a.resourceUri();
            }
            else {
                a = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NCO::PersonContact());

                a.setProperty(Nepomuk::Vocabulary::NCO::fullname(), author.full);

                if(!author.first.isEmpty())
                    a.setProperty(Nepomuk::Vocabulary::NCO::nameGiven(), author.first);
                if(!author.last.isEmpty())
                    a.setProperty(Nepomuk::Vocabulary::NCO::nameFamily(), author.last);
                if(!author.suffix.isEmpty())
                    a.setProperty(Nepomuk::Vocabulary::NCO::nameAdditional(), author.suffix);
            }

            m_allContacts.insert(author.full,a);
        }


        authorResource.addProperty(Nepomuk::Vocabulary::NCO::creator(), a);
    }
}

void BibTexToNepomukPipe::addBooktitle(const QString &content, Nepomuk::Resource publication, const QString & originalEntryType)
{
    //two specialities occur here
    // I. "booktitle" means the title of a book where "title" than means the title of the chapter
    // this is valid for any publication other than @InProceedings
    // II. "booktitle" marks the title of the @proceedings from an @InProceedings or @Conference

    if(originalEntryType == QLatin1String("inproceedings")) {
        //check if a resource @Proceedings with the name of content exist or create a new one

        // fetcha data
        Nepomuk::Query::ComparisonTerm title( Nepomuk::Vocabulary::NIE::title(), Nepomuk::Query::LiteralTerm( content ) );
        Nepomuk::Query::ResourceTypeTerm type( Nepomuk::Vocabulary::NBIB::Proceedings() );

        Nepomuk::Query::Query query( Nepomuk::Query::AndTerm( type, title ) );

        QList<Nepomuk::Query::Result> queryResult = Nepomuk::Query::QueryServiceClient::syncQuery(query);

        Nepomuk::Resource proceedingsResource;
        if(!queryResult.isEmpty()) {
            if(queryResult.size() > 1) {
                qWarning() << "found more than 1 proceedings with the name " << content;

                //now we search deeper as we do get false results
                // Example A.M. Bronstein and M.M. Bronstein will be found with the same query
                foreach(const Nepomuk::Query::Result & nqr, queryResult) {
                    if( nqr.resource().genericLabel() == content) {
                        proceedingsResource = nqr.resource();
                    }
                }

                // we found just false results ... create a new one
                if(!proceedingsResource.isValid()) {
                    proceedingsResource = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::Proceedings());
                    proceedingsResource.setProperty(Nepomuk::Vocabulary::NIE::title(), content);
                }
            }
            else {
                qWarning() << "found another proceedings with the name " << content;
                proceedingsResource = queryResult.first().resource();
            }
        }
        else {
            qWarning() << "found no existing proceedings with the name " << content << "create new one";
            proceedingsResource = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::Proceedings());
            proceedingsResource.setProperty(Nepomuk::Vocabulary::NIE::title(), content);
        }

        //at this point we have a valid proceedings entry connect it to the publication
        //The publication (@inproceedings) is an article while the @Proceedings is a collection
        publication.setProperty(Nepomuk::Vocabulary::NBIB::collection(), proceedingsResource);
        proceedingsResource.addProperty(Nepomuk::Vocabulary::NBIB::article(), publication);
    }
    else {
        publication.setProperty(Nepomuk::Vocabulary::NIE::title(), content);
    }
}

void BibTexToNepomukPipe::addChapter(const QString &content, Nepomuk::Resource publication, Nepomuk::Resource reference)
{
    // If we import some thing we assume no reference already existied and we have a new one
    // thus referencedPart() is not valid
    // if it is valid we assume this was already a Chapter rather than an generic nbib:DocumentPart
    // if above is not true, we should throw an error message
    Nepomuk::Resource chapterResource = reference.property(Nepomuk::Vocabulary::NBIB::referencedPart()).toResource();

    if(!chapterResource.isValid()) {
        chapterResource = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::Chapter());
        reference.setProperty(Nepomuk::Vocabulary::NBIB::referencedPart(), chapterResource);
        publication.addProperty(Nepomuk::Vocabulary::NBIB::documentPart(), chapterResource);
        chapterResource.setProperty(Nepomuk::Vocabulary::NBIB::documentPartOf(), publication);
    }
    else {
        if(!chapterResource.hasType(Nepomuk::Vocabulary::NBIB::Chapter())) {
            qWarning() << "BibTexToNepomukPipe::addChapter tries to add a chapterNumber to a nbib:DocumentPart Resource that is not a nbib:Chapter";
        }
    }

    chapterResource.setProperty( Nepomuk::Vocabulary::NBIB::chapterNumber(), content);
}

void BibTexToNepomukPipe::addCopyrigth(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NIE::copyright(), content);
}

void BibTexToNepomukPipe::addDoi(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NBIB::doi(), content);
}

void BibTexToNepomukPipe::addEdition(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NBIB::edition(), content);
}

void BibTexToNepomukPipe::addEditor(const Value &contentValue, Nepomuk::Resource publication)
{
    foreach(ValueItem *editorItem, contentValue) {
        //transform KBibTex representation of the name into my own Name
        Name editor;
        Person *person = dynamic_cast<Person *>(editorItem);
        if(person) {
            editor.first = person->firstName();
            editor.last = person->lastName();
            editor.suffix = person->suffix();
            editor.full = editor.first + QLatin1String(" ") + editor.last + QLatin1String(" ") + editor.suffix;
        }
        else {
            editor.full = PlainTextValue::text(*editorItem);
        }

        //check if the editor already exist in the database

        Nepomuk::Resource e = m_allContacts.value(editor.full, Nepomuk::Resource());

        if(!e.isValid()) {
            qDebug() << "create a new Contact resource for " << editor.full;
            e = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NCO::PersonContact());

            if(m_addressbook.isValid()) {
                qDebug() << "add editor to akonadi";
                KABC::Addressee addr;
                addr.setFamilyName( editor.last );
                addr.setGivenName( editor.first );
                addr.setAdditionalName( editor.suffix );
                addr.setName( editor.full );
                addr.setFormattedName( editor.full );

                Akonadi::Item item;
                item.setMimeType( KABC::Addressee::mimeType() );
                item.setPayload<KABC::Addressee>( addr );

                Akonadi::ItemCreateJob *job = new Akonadi::ItemCreateJob( item, m_addressbook );

                if ( !job->exec() ) {
                    qDebug() << "Error:" << job->errorString();
                }

                // akonadi saves its contacts with a specific nepomuk uri, we use it here to
                // connect the resource to the publication
                // akonadi will then always update this resource
                //QUrl akonadiUrl(QString("akonadi:?item=%1").arg(job->item().id()));
                e = Nepomuk::Resource(job->item().url(), Nepomuk::Vocabulary::NCO::PersonContact());
                //e.setProperty("http://akonadi-project.org/ontologies/aneo#akonadiItemId", job->item().id());

                e.setProperty(Nepomuk::Vocabulary::NCO::fullname(), editor.full);

                if(!editor.first.isEmpty())
                    e.setProperty(Nepomuk::Vocabulary::NCO::nameGiven(), editor.first);
                if(!editor.last.isEmpty())
                    e.setProperty(Nepomuk::Vocabulary::NCO::nameFamily(), editor.last);
                if(!editor.suffix.isEmpty())
                    e.setProperty(Nepomuk::Vocabulary::NCO::nameAdditional(), editor.suffix);

                qDebug() << "akonadi/nepomuk id" << job->item().url() << e.isValid();
            }
            else {
                e = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NCO::PersonContact());

                e.setProperty(Nepomuk::Vocabulary::NCO::fullname(), editor.full);

                if(!editor.first.isEmpty())
                    e.setProperty(Nepomuk::Vocabulary::NCO::nameGiven(), editor.first);
                if(!editor.last.isEmpty())
                    e.setProperty(Nepomuk::Vocabulary::NCO::nameFamily(), editor.last);
                if(!editor.suffix.isEmpty())
                    e.setProperty(Nepomuk::Vocabulary::NCO::nameAdditional(), editor.suffix);
            }

            m_allContacts.insert(editor.full, e);
        }

        publication.addProperty(Nepomuk::Vocabulary::NBIB::editor(), e);
    }
}

void BibTexToNepomukPipe::addEprint(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NBIB::eprint(), content);
}

void BibTexToNepomukPipe::addHowPublished(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NBIB::publicationMethod(), content);
}

void BibTexToNepomukPipe::addInstitution(const Value &content, Nepomuk::Resource publication)
{
    Value empty;
    addPublisher(content, empty, publication);
}

void BibTexToNepomukPipe::addIsbn(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NBIB::isbn(), content);
}

void BibTexToNepomukPipe::addIssn(const QString &content, Nepomuk::Resource publication)
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

void BibTexToNepomukPipe::addLanguage(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NIE::language(), content);
}

void BibTexToNepomukPipe::addLccn(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NBIB::lccn(), content);
}

void BibTexToNepomukPipe::addMonth(const QString &content, Nepomuk::Resource publication)
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


    if(year.size() != 4) {
        year.prepend(QLatin1String("00"));
    }
    if(month.size() != 2) {
        month.prepend(QLatin1String("0"));
    }
    if(day.size() != 2) {
        day.prepend(QLatin1String("0"));
    }

    QString newDate = year + QLatin1String("-") + month + QLatin1String("-") + day + QLatin1String("T00:00:00");
    publication.setProperty(Nepomuk::Vocabulary::NBIB::publicationDate(), newDate);
}

void BibTexToNepomukPipe::addMrNumber(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NBIB::mrNumber(), content);
}

void BibTexToNepomukPipe::addNote(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NIE::description(), content);
}

void BibTexToNepomukPipe::addNumber(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NBIB::number(), content);
}

void BibTexToNepomukPipe::addOrganization(const QString &content, Nepomuk::Resource publication)
{
    //check if the organization already exist in the database
    Nepomuk::Resource organizationResource = m_allContacts.value(content, Nepomuk::Resource());

    if(!organizationResource.isValid()) {
        qDebug() << "create a new OrganizationContact resource for " << content;
        organizationResource = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NCO::OrganizationContact());
        organizationResource.setProperty(Nepomuk::Vocabulary::NCO::fullname(), content);

        m_allContacts.insert(content, organizationResource);
    }
    else {
        qDebug() << "use existing Organization resource for " << content;
    }

    if(publication.hasType(Nepomuk::Vocabulary::NBIB::Article())) {
        Nepomuk::Resource proceedings = publication.property(Nepomuk::Vocabulary::NBIB::collection()).toResource();
        if(!proceedings.isValid()) {
            proceedings = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::Proceedings());
            proceedings.addProperty(Nepomuk::Vocabulary::NBIB::article(), publication);
            publication.setProperty(Nepomuk::Vocabulary::NBIB::collection(), proceedings);
        }
        proceedings.setProperty(Nepomuk::Vocabulary::NBIB::organization(), organizationResource);
    }
    else {
        publication.setProperty(Nepomuk::Vocabulary::NBIB::organization(), organizationResource);
    }
}

void BibTexToNepomukPipe::addPages(const QString &content, Nepomuk::Resource reference)
{
    reference.setProperty(Nepomuk::Vocabulary::NBIB::pages(), content);
}

void BibTexToNepomukPipe::addPubMed(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NBIB::pubMed(), content);
}

void BibTexToNepomukPipe::addSchool(const Value &content, Nepomuk::Resource publication)
{
    Value empty;
    addPublisher(content, empty, publication);
}

void BibTexToNepomukPipe::addSeries(const QString &content, Nepomuk::Resource publication)
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

void BibTexToNepomukPipe::addTitle(const QString &content, Nepomuk::Resource publication, Nepomuk::Resource reference, const QString & originalEntryType)
{
    // in the case of @InCollection title means title of the chapter
    // while booktitle is the actual title of the book
    if(originalEntryType == QLatin1String("incollection") ) {

        Nepomuk::Resource chapterResource = reference.property(Nepomuk::Vocabulary::NBIB::referencedPart()).toResource();

        if(!chapterResource.isValid()) {
            chapterResource = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::Chapter());
            reference.setProperty(Nepomuk::Vocabulary::NBIB::referencedPart(), chapterResource);
            publication.addProperty(Nepomuk::Vocabulary::NBIB::documentPart(), chapterResource);
            chapterResource.setProperty(Nepomuk::Vocabulary::NBIB::documentPartOf(), publication);
        }

        chapterResource.setProperty( Nepomuk::Vocabulary::NIE::title(), content);
    }
    else {
        publication.setProperty(Nepomuk::Vocabulary::NIE::title(), content);
    }
}

void BibTexToNepomukPipe::addType(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NBIB::type(), content);
}

void BibTexToNepomukPipe::addUrl(const QString &content, Nepomuk::Resource publication)
{
    qDebug() << "BibTexToNepomukPipe::addUrl :: missing implementation currently";
}

void BibTexToNepomukPipe::addVolume(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NBIB::volume(), content);
}

void BibTexToNepomukPipe::addYear(const QString &content, Nepomuk::Resource publication)
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

    if(year.size() != 4) {
        year.prepend(QLatin1String("00"));
    }
    if(month.size() != 2) {
        month.prepend(QLatin1String("0"));
    }
    if(day.size() != 2) {
        day.prepend(QLatin1String("0"));
    }

    QString newDate = content.trimmed() + QLatin1String("-") + month + QLatin1String("-") + day + QLatin1String("T00:00:00");
    publication.setProperty(Nepomuk::Vocabulary::NBIB::publicationDate(), newDate);
}

void BibTexToNepomukPipe::addKewords(const Value &content, Nepomuk::Resource publication)
{
    foreach(ValueItem *vi, content) {
        Keyword *k = dynamic_cast<Keyword *>(vi);
        Nepomuk::Tag tag(k->text());
        tag.setLabel(k->text());
        publication.addTag(tag);
    }
}