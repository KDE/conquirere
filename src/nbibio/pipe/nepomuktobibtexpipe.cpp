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

#include "nepomuktobibtexpipe.h"

#include <kbibtex/entry.h>
#include <kbibtex/value.h>

#include "nbib.h"
#include "sync.h"
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Vocabulary/NFO>
#include <Nepomuk/Vocabulary/NCO>
#include <Nepomuk/Vocabulary/NUAO>
#include <Nepomuk/Variant>
#include <Nepomuk/Tag>

#include <KDE/KDebug>

#include <QtCore/QSharedPointer>

using namespace Nepomuk::Vocabulary;

NepomukToBibTexPipe::NepomukToBibTexPipe()
    : m_bibtexFile(0)
    , m_strict(false)
    , m_addNepomukUris(false)
{
}

NepomukToBibTexPipe::~NepomukToBibTexPipe()
{
//    delete m_bibtexFile;
//    m_bibtexFile = 0;
}

void NepomukToBibTexPipe::pipeExport(QList<Nepomuk::Resource> resources)
{
    delete m_bibtexFile;
    m_bibtexFile = new File;

    int citeKeyNumer = 1;
    QString citeRef = QLatin1String("BibTexExport");

    m_percentPerResource = (qreal)resources.size() / 100.0;

    int i = 0;
    foreach(const Nepomuk::Resource & resource, resources) {
        int curProgress = i * m_percentPerResource;
        emit progress(curProgress);
        i++;

        Nepomuk::Resource reference;
        Nepomuk::Resource publication;

        // first check if we operate on a Reference or a Publication
        if(resource.hasType( NBIB::Reference() )) {
            // we have a Reference
            reference = resource;
            publication = reference.property(NBIB::publication()).toResource();
        }
        else {
            //we have a publication and no idea what reference to use with it
            // we will extract as many information as possible anyway
            publication = resource;
        }

        QString citeKey = reference.property(NBIB::citeKey()).toString();
        if(citeKey.isEmpty()) {
            kDebug() << "unknown citeKey for the bibtex pipe export :: create one";
            citeKey = citeRef + QString::number(citeKeyNumer);
            citeKeyNumer++;
        }

        QString entryType = retrieveEntryType(reference, publication);
        if(entryType.isEmpty()) {
            kWarning() << "unknown entry type for the bibtex export with citekey" << citeKey;
            continue;
        }

        //collect nepomuk content
        QSharedPointer<Entry>entry(new Entry);
        Entry *e = entry.data();
        e->setType(entryType);
        e->setId(citeKey);
        collectContent(e, reference, publication);

        if(m_addNepomukUris) {
            Value v1;
            v1.append(QSharedPointer<ValueItem>(new PlainText(publication.resourceUri().toString())));
            e->insert(QLatin1String("nepomuk-publication-uri"), v1);
            if(reference.isValid()) {
                Value v2;
                v2.append(QSharedPointer<ValueItem>(new PlainText(reference.resourceUri().toString())));
                e->insert(QLatin1String("nepomuk-reference-uri"), v2);
            }
        }


        m_bibtexFile->append(entry);
    }
}

File *NepomukToBibTexPipe::bibtexFile() const
{
    return m_bibtexFile;
}

void NepomukToBibTexPipe::useStrictTypes(bool strict)
{
    m_strict = strict;
}

QString NepomukToBibTexPipe::retrieveEntryType(Nepomuk::Resource reference, Nepomuk::Resource publication)
{
    QString type;

    if(publication.hasType(NBIB::Dictionary())) {
        QString pages = reference.property(NBIB::pages()).toString();
        Nepomuk::Resource chapter = reference.property(NBIB::referencedPart()).toResource();
        if(!pages.isEmpty() || chapter.isValid()) {
            type = QLatin1String("DictionaryEntry");
        }
        else {
            type = QLatin1String("Dictionary");
        }
    }
    // handle general book/collections then
    else if(publication.hasType(NBIB::Book())) {
        QString pages = reference.property(NBIB::pages()).toString();
        Nepomuk::Resource chapter = reference.property(NBIB::referencedPart()).toResource();
        Nepomuk::Resource chapterAuthor = chapter.property(NCO::creator()).toResource();

        // so we have a @book if no chapter or or pages are specified
        // and a @InBook if they are
        // and @InCollection if the chapter has an author
        if(!pages.isEmpty() || chapter.isValid()) {
            if(chapterAuthor.isValid()) {
                type = QLatin1String("Incollection"); //book ref with defined author in the chapter
            }
            else {
                type = QLatin1String("InBook"); //book ref with chapter/pages
            }
        }
        else {
            Nepomuk::Resource typeResource(publication.resourceType());
            type = typeResource.genericLabel();
        }
    }
    else if(publication.hasType(NBIB::Proceedings())) {
        type = QLatin1String("Proceedings");
    }
    else if(publication.hasType(NBIB::LegalCaseDocument())) {
        type = QLatin1String("Case");
    }
    else if(publication.hasType(NBIB::Blog())) {
        type = QLatin1String("Blog");
    }
    else if(publication.hasType(NBIB::Forum())) {
        type = QLatin1String("Forum");
    }
    else if(publication.hasType(NBIB::Website())) {
        type = QLatin1String("Website");
    }
    // handle special articles
    else if(publication.hasType(NBIB::Article())) {
        Nepomuk::Resource collection = publication.property(NBIB::collection()).toResource();
        if(collection.hasType(NBIB::Proceedings())) {
            type = QLatin1String("Inproceedings"); //article in some proceedings paper
        }
        else {
            type = QLatin1String("Article"); //normal article in a journal, magazine pr newspaper
        }
    }
    // all other cases
    else {
        Nepomuk::Resource typeResource(publication.resourceType());
        type = typeResource.genericLabel();

        Nepomuk::Resource typeResource2(NBIB::Publication());
        if(type == typeResource2.genericLabel()) {
            type = QLatin1String("Misc");
        }
    }

    // if we have strict export, transforn into standard types
    if(m_strict) {
        kDebug() << "strict export is not implemented yet";
    }

    return type;
}

void NepomukToBibTexPipe::collectContent(Entry *e, Nepomuk::Resource reference, Nepomuk::Resource publication)
{
    setTitle(e, publication, reference);
    setChapter(e, reference);

    // if the chapter has an author attached to it , don't search author of the publication
    // solves the incollection special case where the author of the chapter is not the one from the whole book is ment
    if(!e->contains(QLatin1String("author"))){
        setContact(e, publication, NCO::creator(), Entry::ftAuthor);
    }
    else {
        setContact(e, publication, NCO::creator(), QLatin1String("bookAuthor"));
    }

    setContact(e, publication, NBIB::editor(), Entry::ftEditor);setContact(e, publication, NBIB::translator(), QLatin1String("translator"));
    setContact(e, publication, NBIB::contributor(), QLatin1String("contributor"));
    setContact(e, publication, NBIB::reviewedAuthor(), QLatin1String("reviewedauthor"));
    setContact(e, publication, NBIB::commenter(), QLatin1String("commenter"));
    setContact(e, publication, NBIB::attorneyAgent(), QLatin1String("attorneyagent"));
    setContact(e, publication, NBIB::counsel(), QLatin1String("counsel"));
    setContact(e, publication, NBIB::coSponsor(), QLatin1String("cosponsor"));

    setSeriesEditors(e, publication);
    setPublisher(e, publication);
    setOrganization(e, publication);

    setPublicationDate(e, publication);
    setUrl(e, publication);
    setSeries(e, publication);
    setISSN(e, publication);
    setJournal(e, publication);
    setKewords(e, publication);
    setEvent(e, publication);
    setCode(e, publication);
    setCodeNumber(e, publication);
    setCodeVolume(e, publication);
    setReporter(e, publication);
    setReporterVolume(e, publication);
    setArticleType(e, publication);
    setAssignee(e, publication);

    setValue(e, publication, NBIB::edition(), QLatin1String("edition"));
    setValue(e, publication, NBIB::volume(), Entry::ftVolume);
    setValue(e, publication, NBIB::number(), Entry::ftNumber);
    setValue(e, publication, NIE::copyright(), QLatin1String("copyright"));
    setValue(e, publication, NBIB::publicationMethod(), QLatin1String("howpublished"));
    setValue(e, publication, NBIB::type(), QLatin1String("type"));
    setValue(e, publication, NUAO::lastUsage(), QLatin1String("accessdate"));
    setValue(e, publication, NBIB::publicationDate(), QLatin1String("date"));
    setValue(e, publication, NBIB::eprint(), QLatin1String("eprint"));
    setValue(e, publication, NBIB::isbn(), QLatin1String("isbn"));
    setValue(e, publication, NBIB::lccn(), QLatin1String("lccn"));
    setValue(e, publication, NBIB::mrNumber(), QLatin1String("mrnumber"));
    setValue(e, publication, NBIB::pubMed(), QLatin1String("pubmed"));
    setValue(e, publication, NBIB::doi(), QLatin1String("doi"));
    setValue(e, publication, NBIB::abstract(), QLatin1String("abstract"));
    setValue(e, publication, NIE::description(), QLatin1String("note"));
    setValue(e, publication, NIE::comment(), QLatin1String("annote"));
    setValue(e, publication, NIE::language(), QLatin1String("language"));
    setValue(e, reference, NBIB::pages(), QLatin1String("pages"));
    setValue(e, publication, NBIB::priorityNumbers(), QLatin1String("prioritynumbers"));
    setValue(e, publication, NBIB::applicationNumber(), QLatin1String("applicationnumber"));
    setValue(e, publication, NBIB::patentReferences(), QLatin1String("references"));
    setValue(e, publication, NBIB::legalStatus(), QLatin1String("legalstatus"));
    setValue(e, publication, NBIB::filingDate(), QLatin1String("filingdate"));
    setValue(e, publication, NBIB::archive(), QLatin1String("archive"));
    setValue(e, publication, NBIB::archiveLocation(), QLatin1String("archiveLocation"));
    setValue(e, publication, NBIB::libraryCatalog(), QLatin1String("libraryCatalog"));
    setValue(e, publication, NBIB::shortTitle(), QLatin1String("shorttitle"));
    setValue(e, publication, NBIB::numberOfPages(), QLatin1String("numpages"));
    setValue(e, publication, NBIB::numberOfVolumes(), QLatin1String("numberofvolumes"));
    setValue(e, publication, NBIB::mapScale(), QLatin1String("scale"));
    setValue(e, publication, NBIB::history(), QLatin1String("history"));

    // Zotero additions
    setSyncDetails(e, publication);
}

void NepomukToBibTexPipe::setTitle(Entry *e, Nepomuk::Resource publication, Nepomuk::Resource reference)
{
    QString title;
    QString booktitle;

    // handle special case where "title=" is name of the chapter and "booktitle=" is the name of the book
    if(e->type() == QLatin1String("Incollection") || e->type() == QLatin1String("DictionaryEntry")) {
        booktitle = publication.property(NIE::title()).toString();

        Nepomuk::Resource chapter = reference.property(NBIB::referencedPart()).toResource();
        title = chapter.property(NIE::title()).toString();
    }
    else {
        title = publication.property(NIE::title()).toString();

        if(publication.hasType(NBIB::Article())) {
            Nepomuk::Resource collection = publication.property(NBIB::collection()).toResource();
            booktitle = collection.property(NIE::title()).toString();
        }
    }

    if(!title.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(title)));
        e->insert(Entry::ftTitle, v);
    }

    if(!booktitle.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(booktitle)));
        e->insert(Entry::ftBookTitle, v);
    }
}

void NepomukToBibTexPipe::setChapter(Entry *e, Nepomuk::Resource reference)
{
    QString chapterName;
    Nepomuk::Resource chapter = reference.property(NBIB::referencedPart()).toResource();
    QString chapterNumber = chapter.property(NBIB::chapterNumber()).toString();

    // handle special case where "title=" is name of the chapter and "booktitle=" is the name of the book
    // thus "chapter=" is just chapter number not connection of both
    if(e->type() == QLatin1String("Incollection")) {
        chapterName = chapterNumber;
    }
    else {
        QString chapterTitle = chapter.property(NIE::title()).toString();
        if(!chapterNumber.isEmpty() && !chapterTitle.isEmpty()) {
            chapterName.prepend(QLatin1String(" : "));
        }

        chapterName.prepend(chapterNumber);
        chapterName.append(chapterTitle);
    }

    if(!chapterName.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(chapterName)));
        e->insert(Entry::ftChapter, v);
    }

    // now if the chapter has authors attached, add then as "author=" instead of the
    // authors from the publication. This is the case for InCollection
    // the publication authors will be added as "bookauthor=" later on

    QList<Nepomuk::Resource> authors = chapter.property(NCO::creator()).toResourceList();

    Value v;
    if(!authors.isEmpty()) {
        foreach(const Nepomuk::Resource & a, authors) {
            QString firstName = a.property(NCO::nameGiven()).toString();
            QString lastName = a.property(NCO::nameFamily()).toString();
            QString suffix = a.property(NCO::nameHonorificSuffix()).toString();
            if(firstName.isEmpty())
                firstName = a.property(NCO::fullname()).toString();

            Person *p = new Person(firstName, lastName, suffix);
            v.append(QSharedPointer<ValueItem>(p));
        }

        if(!v.isEmpty()) {
            e->insert(Entry::ftAuthor, v);
        }
    }
}

void NepomukToBibTexPipe::setSeriesEditors(Entry *e, Nepomuk::Resource publication)
{
    Nepomuk::Resource series =  publication.property(NBIB::inSeries()).toResource();
    if(!series.isValid()) {
        Nepomuk::Resource collection =  publication.property(NBIB::collection()).toResource();
        series =  collection.property(NBIB::inSeries()).toResource();
    }

    setContact(e, series, NBIB::editor(), QLatin1String("seriesEditor"));
}

void NepomukToBibTexPipe::setPublicationDate(Entry *e, Nepomuk::Resource publication)
{
    QString pdString = publication.property(NBIB::publicationDate()).toString();

    QRegExp rx(QLatin1String("(\\d*)-(\\d*)-(\\d*)*"));
    QString year;
    QString month;
    QString day;
    if (rx.indexIn(pdString) != -1) {
        year = rx.cap(1);
        month = rx.cap(2);
    }
    else {
        return;
    }

    QString string;
    if(!month.isEmpty()) {
        int mInt = month.toInt();
        QString monthName;

        switch(mInt) {
        case 1:
            monthName.append(QLatin1String("jan"));
            break;
        case 2:
            monthName.append(QLatin1String("feb"));
            break;
        case 3:
            monthName.append(QLatin1String("mar"));
            break;
        case 4:
            monthName.append(QLatin1String("apr"));
            break;
        case 5:
            monthName.append(QLatin1String("may"));
            break;
        case 6:
            monthName.append(QLatin1String("jun"));
            break;
        case 7:
            monthName.append(QLatin1String("jul"));
            break;
        case 8:
            monthName.append(QLatin1String("aug"));
            break;
        case 9:
            monthName.append(QLatin1String("sep"));
            break;
        case 10:
            monthName.append(QLatin1String("oct"));
            break;
        case 11:
            monthName.append(QLatin1String("nov"));
            break;
        case 12:
            monthName.append(QLatin1String("dec"));
            break;
        }

        if(!monthName.isEmpty()) {
            Value v;
            v.append(QSharedPointer<ValueItem>(new PlainText(monthName)));
            e->insert(Entry::ftMonth, v);
        }
    }

    if(!year.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(year)));
        e->insert(Entry::ftYear, v);

    }
}

void NepomukToBibTexPipe::setPublisher(Entry *e, Nepomuk::Resource publication)
{
    QString publisherEntry = QLatin1String("publisher");
    QList<Nepomuk::Resource> publisher = publication.property(NCO::publisher()).toResourceList();

    if(publication.hasType(NBIB::Thesis())) {
        publisherEntry = QLatin1String("school");
    }
    else if(publication.hasType(NBIB::Techreport())) {
        publisherEntry = QLatin1String("institution");
    }

    QString names;
    QString address;
    if(!publisher.isEmpty()) {
        foreach(const Nepomuk::Resource & p, publisher) {
            //TODO don't rely only on fullname of NCO::Contact
            names.append(p.property(NCO::fullname()).toString());
            names.append(QLatin1String(" and "));


            Nepomuk::Resource existingAddr = p.property(NCO::hasPostalAddress()).toResource();
            if(existingAddr.isValid())
                address.append(existingAddr.property(NCO::extendedAddress()).toString());
        }

        names.chop(5);

        if(!names.isEmpty()) {
            Value v;
            v.append(QSharedPointer<ValueItem>(new PlainText(names)));
            e->insert(publisherEntry, v);
        }
        if(!address.isEmpty()) {
            Value v;
            v.append(QSharedPointer<ValueItem>(new PlainText(address)));
            e->insert(Entry::ftAddress, v);
        }
    }
}

void NepomukToBibTexPipe::setOrganization(Entry *e, Nepomuk::Resource publication)
{
    Nepomuk::Resource org;
    if(publication.hasType(NBIB::Article())) {
        Nepomuk::Resource collectionResource = publication.property(NBIB::collection()).toResource();
        if(collectionResource.hasType(NBIB::Proceedings())) {
            org = collectionResource.property(NBIB::organization()).toResource();
        }
    }
    else {
        org = publication.property(NBIB::organization()).toResource();
    }

    QString string = org.property(NCO::fullname()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(string)));
        e->insert(QLatin1String("organization"), v);
    }
}

void NepomukToBibTexPipe::setUrl(Entry *e, Nepomuk::Resource publication)
{
    QList<Nepomuk::Resource> objectList = publication.property(NBIB::isPublicationOf()).toResourceList();

    QString urlList;
    foreach(const Nepomuk::Resource &dataObjects, objectList) {
        QUrl url = dataObjects.property(NIE::url()).toUrl();

        urlList.append(url.toString());
        urlList.append(QLatin1String(", "));
    }

    urlList.chop(2);
    if(!urlList.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(urlList)));
        e->insert(Entry::ftUrl, v);
    }
}

void NepomukToBibTexPipe::setSeries(Entry *e, Nepomuk::Resource publication)
{
    Nepomuk::Resource series = publication.property(NBIB::inSeries()).toResource();
    QString string = series.property(NIE::title()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(string)));
        e->insert(Entry::ftSeries, v);
    }
}

void NepomukToBibTexPipe::setJournal(Entry *e, Nepomuk::Resource publication)
{
    Nepomuk::Resource journalIssue = publication.property(NBIB::collection()).toResource();

    if(!journalIssue.isValid()) {
        return; // no journal available for his resource
    }

    // if we have a JournalIssue, get the journal contact and number/volume
    QString journalNumber = journalIssue.property(NBIB::number()).toString(); //Issue number
    QString journalVolume = journalIssue.property(NBIB::volume()).toString();

    Nepomuk::Resource journal = journalIssue.property(NBIB::inSeries()).toResource();
    QString journalName = journal.property(NIE::title()).toString();;

    if(!journalNumber.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(journalNumber)));
        e->insert(Entry::ftNumber, v);
    }
    if(!journalVolume.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(journalVolume)));
        e->insert(Entry::ftVolume, v);
    }
    if(!journalName.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(journalName)));
        e->insert(Entry::ftJournal, v);
    }
}

void NepomukToBibTexPipe::setISSN(Entry *e, Nepomuk::Resource publication)
{
    Nepomuk::Resource series = publication.property(NBIB::inSeries()).toResource();
    QString issnString = series.property(NBIB::issn()).toString();

    // handles article of a collection in a series which has the issn attached
    if(issnString.isEmpty()) {
        Nepomuk::Resource collection = publication.property(NBIB::collection()).toResource();
        Nepomuk::Resource series2 = collection.property(NBIB::inSeries()).toResource();
        issnString = series2.property(NBIB::issn()).toString();
    }

    if(!issnString.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(issnString)));
        e->insert(QLatin1String("issn"), v);
    }
}

void NepomukToBibTexPipe::setEvent(Entry *e, Nepomuk::Resource publication)
{
    Nepomuk::Resource event = publication.property(NBIB::event()).toResource();

    if(!event.isValid()) {
        Nepomuk::Resource collection = publication.property(NBIB::collection()).toResource();
        event = collection.property(NBIB::event()).toResource();
    }

    QString string = event.property(NIE::title()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(string)));
        e->insert(QLatin1String("event"), v);
    }
}

void NepomukToBibTexPipe::setCode(Entry *e, Nepomuk::Resource publication)
{
    Nepomuk::Resource codeOfLaw = publication.property(NBIB::codeOfLaw()).toResource();

    setValue(e, codeOfLaw, NIE::title(), QLatin1String("code"));
}

void NepomukToBibTexPipe::setCodeNumber(Entry *e, Nepomuk::Resource publication)
{
    Nepomuk::Resource codeOfLaw = publication.property(NBIB::codeOfLaw()).toResource();

    setValue(e, codeOfLaw, NBIB::codeNumber(), QLatin1String("codenumber"));
}

void NepomukToBibTexPipe::setCodeVolume(Entry *e, Nepomuk::Resource publication)
{
    Nepomuk::Resource codeOfLaw = publication.property(NBIB::codeOfLaw()).toResource();

    setValue(e, codeOfLaw, NBIB::volume(), QLatin1String("codevolume"));
}

void NepomukToBibTexPipe::setReporter(Entry *e, Nepomuk::Resource publication)
{
    Nepomuk::Resource courtReporter = publication.property(NBIB::courtReporter()).toResource();

    setValue(e, courtReporter, NIE::title(), QLatin1String("reporter"));
}

void NepomukToBibTexPipe::setReporterVolume(Entry *e, Nepomuk::Resource publication)
{
    Nepomuk::Resource courtReporter = publication.property(NBIB::courtReporter()).toResource();

    setValue(e, courtReporter, NBIB::volume(), QLatin1String("reportervolume"));
}

void NepomukToBibTexPipe::setAssignee(Entry *e, Nepomuk::Resource publication)
{
    Nepomuk::Resource assignee = publication.property(NBIB::assignee()).toResource();

    Value v;
    if(assignee.hasType(NCO::PersonContact())) {
        QString firstName = assignee.property(NCO::nameGiven()).toString();
        QString lastName = assignee.property(NCO::nameFamily()).toString();
        QString suffix = assignee.property(NCO::nameHonorificSuffix()).toString();
        if(firstName.isEmpty() || lastName.isEmpty()) {
            QString fullname = assignee.property(NCO::fullname()).toString();
            if(fullname.contains(QLatin1String(","))) {
                QStringList nameSplitted = fullname.split(QLatin1String(","));
                lastName = nameSplitted.first();
                firstName = nameSplitted.last();
            }
            else {
                QStringList nameSplitted = fullname.split(QLatin1String(" "));
                lastName = nameSplitted.first();
                firstName = nameSplitted.last();
            }
        }
        firstName = firstName.trimmed();
        lastName = lastName.trimmed();

        Person *p = new Person(firstName, lastName, suffix);
        v.append(QSharedPointer<ValueItem>(p));

    }
    else {
        QString name = assignee.property(NCO::fullname()).toString();
        if(!name.isEmpty())
            v.append(QSharedPointer<ValueItem>(new PlainText(name)));
    }

    if(!v.isEmpty()) {
        e->insert(QLatin1String("assignee"), v);
    }
}

void NepomukToBibTexPipe::setKewords(Entry *e, Nepomuk::Resource publication)
{
    QList<Nepomuk::Tag> tags = publication.tags();

    Value v;
    foreach(const Nepomuk::Tag & tag, tags) {
        Keyword *p = new Keyword(tag.genericLabel());
        v.append(QSharedPointer<ValueItem>(p));
    }

    if(!v.isEmpty()) {
        e->insert(Entry::ftKeywords, v);
    }
}

void NepomukToBibTexPipe::setSyncDetails(Entry *e, Nepomuk::Resource publication)
{
    QList<Nepomuk::Resource> sycList = publication.property(SYNC::serverSyncData()).toResourceList();

    // only add the sync details the the right storage
    foreach(const Nepomuk::Resource &r, sycList) {
        if(r.property(SYNC::provider()).toString() != QString("zotero")) { //TODO make this possible for others too
            continue;
        }
        if(r.property(SYNC::userId()).toString() != m_syncUserId) {//TODO make this possible for others too
            continue;
        }
        if(r.property(SYNC::url()).toString() != m_syncUrl) {//TODO make this possible for others too
            continue;
        }

        //now we have the right object, write down sync details
        QString etag = r.property(SYNC::etag()).toString();
        Value v1;
        v1.append(QSharedPointer<ValueItem>(new PlainText(etag)));
        e->insert(QLatin1String("zoteroEtag"), v1);

        QString key = r.property(SYNC::id()).toString();
        Value v2;
        v2.append(QSharedPointer<ValueItem>(new PlainText(key)));
        e->insert(QLatin1String("zoteroKey"), v2);

        QString updated = r.property(NUAO::lastModification()).toString();
        Value v3;
        v3.append(QSharedPointer<ValueItem>(new PlainText(updated)));
        e->insert(QLatin1String("zoteroUpdated"), v3);

        break;
    }
}

void NepomukToBibTexPipe::setSyncDetails(const QString &url, const QString &userid)
{
    m_syncUrl = url;
    m_syncUserId = userid;
}

void NepomukToBibTexPipe::addNepomukUries(bool addThem)
{
    m_addNepomukUris = addThem;
}

void NepomukToBibTexPipe::setArticleType(Entry *e, Nepomuk::Resource publication)
{
    if(publication.hasType(NBIB::Article())) {
        QString articleType;
        Nepomuk::Resource collection = publication.property(NBIB::collection()).toResource();
        if(collection.hasType(NBIB::JournalIssue())) {
            articleType = QLatin1String("journal"); //article in some proceedings paper
        }
        else if(collection.hasType(NBIB::NewspaperIssue())) {
            articleType = QLatin1String("newspaper"); //normal article in a journal or magazine
        }
        else if(collection.hasType(NBIB::MagazinIssue())) {
            articleType = QLatin1String("magazine"); //normal article in a journal or magazine
        }
        else if(collection.hasType(NBIB::Encyclopedia())) {
            articleType = QLatin1String("encyclopedia"); //normal article in an ancyclopedia
        }
        else if(collection.hasType(NBIB::Blog())) {
            articleType = QLatin1String("blog");
        }
        else if(collection.hasType(NBIB::Forum())) {
            articleType = QLatin1String("forum");
        }
        else if(collection.hasType(NBIB::Webpage())) {
            articleType = QLatin1String("webpage");
        }

        if(!articleType.isEmpty()) {
            Value v;
            v.append(QSharedPointer<ValueItem>(new PlainText(articleType)));
            e->insert(QLatin1String("articletype"), v);
        }
    }
}

void NepomukToBibTexPipe::setValue(Entry *e, Nepomuk::Resource publication, QUrl property, const QString &bibkey)
{
    QString string = publication.property(property).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(string)));
        e->insert(bibkey, v);
    }
}

void NepomukToBibTexPipe::setContact(Entry *e, Nepomuk::Resource publication, QUrl property, const QString &bibkey)
{
    QList<Nepomuk::Resource> authors = publication.property(property).toResourceList();

    Value v;
    if(!authors.isEmpty()) {
        foreach(const Nepomuk::Resource & a, authors) {
            QString firstName = a.property(NCO::nameGiven()).toString();
            QString lastName = a.property(NCO::nameFamily()).toString();
            QString suffix = a.property(NCO::nameHonorificSuffix()).toString();

            if(firstName.isEmpty() || lastName.isEmpty()) {
                kDebug() << "could not split firstname / lastname component, try to find it automatically fro mthe fullname";
                QString fullname = a.property(NCO::fullname()).toString();

                // assume fullname as "LastName, Firstname"
                if(fullname.contains(QLatin1String(","))) {
                    QStringList nameSplitted = fullname.split(QLatin1String(","));
                    lastName = nameSplitted.first();
                    firstName = nameSplitted.last();
                }
                // assume fullname as "Firstname LastName"
                else {
                    QStringList nameSplitted = fullname.split(QLatin1String(" "));
                    if(nameSplitted.size() >= 2) {
                        firstName = nameSplitted.first();
                        lastName = nameSplitted.last();
                    }
                    else {
                        //firstName;
                        lastName = nameSplitted.first();
                    }
                }
            }

            firstName = firstName.trimmed();
            lastName = lastName.trimmed();

            Person *p = new Person(firstName, lastName, suffix);
            v.append(QSharedPointer<ValueItem>(p));
        }

        if(!v.isEmpty()) {
            e->insert(bibkey, v);
        }
    }
}
