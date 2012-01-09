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

#include <QtCore/QSharedPointer>
#include <QtCore/QDebug>

NepomukToBibTexPipe::NepomukToBibTexPipe()
    :m_strict(false)
    , m_addNepomukUris(false)
{
}

NepomukToBibTexPipe::~NepomukToBibTexPipe()
{

}

void NepomukToBibTexPipe::pipeExport(QList<Nepomuk::Resource> resources)
{
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
        if(resource.hasType( Nepomuk::Vocabulary::NBIB::Reference() )) {
            // we have a Reference
            reference = resource;
            publication = reference.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();
        }
        else {
            //we have a publication and no idea what reference to use with it
            // we will extract as many information as possible anyway
            publication = resource;
        }

        QString citeKey = reference.property(Nepomuk::Vocabulary::NBIB::citeKey()).toString();
        if(citeKey.isEmpty()) {
            qDebug() << "unknown citeKey for the bibtex pipe export :: create one";
            citeKey = citeRef + QString::number(citeKeyNumer);
            citeKeyNumer++;
        }

        QString entryType = retrieveEntryType(reference, publication);
        if(entryType.isEmpty()) {
            qWarning() << "unknown entry type for the bibtex export with citekey" << citeKey;
            continue;
        }

        //collect nepomuk content
        Entry *e = new Entry;
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
        m_bibtexFile.append(e);
    }
}

File NepomukToBibTexPipe::bibtexFile() const
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

    // handle some special NBIB::Collection types first
    if(publication.hasType(Nepomuk::Vocabulary::NBIB::Encyclopedia())) {
        type = QLatin1String("Encyclopedia");
    }
    else if(publication.hasType(Nepomuk::Vocabulary::NBIB::Dictionary())) {
        QString pages = reference.property(Nepomuk::Vocabulary::NBIB::pages()).toString();
        Nepomuk::Resource chapter = reference.property(Nepomuk::Vocabulary::NBIB::referencedPart()).toResource();
        if(!pages.isEmpty() || chapter.isValid()) {
            type = QLatin1String("DictionaryEntry");
        }
        else {
            type = QLatin1String("Dictionary");
        }
    }
    // handle general book/collections then
    else if(publication.hasType(Nepomuk::Vocabulary::NBIB::Book()) || publication.hasType(Nepomuk::Vocabulary::NBIB::Collection())) {
        QString pages = reference.property(Nepomuk::Vocabulary::NBIB::pages()).toString();
        Nepomuk::Resource chapter = reference.property(Nepomuk::Vocabulary::NBIB::referencedPart()).toResource();
        Nepomuk::Resource chapterAuthor = chapter.property(Nepomuk::Vocabulary::NCO::creator()).toResource();

        if(!pages.isEmpty() || chapter.isValid()) {
            if(chapterAuthor.isValid()) {
                type = QLatin1String("Incollection"); //book ref with defined author in the chapter
            }
            else {
                type = QLatin1String("Inbook"); //book ref with chapter/pages
            }
        }
        else {
            Nepomuk::Resource typeResource(publication.type());
            type = typeResource.genericLabel();
        }
    }
    // handle special articles
    else if(publication.hasType(Nepomuk::Vocabulary::NBIB::Article())) {
        Nepomuk::Resource collection = publication.property(Nepomuk::Vocabulary::NBIB::collection()).toResource();
        if(collection.hasType(Nepomuk::Vocabulary::NBIB::Proceedings())) {
            type = QLatin1String("Inproceedings"); //article in some proceedings paper
        }
        else if(collection.hasType(Nepomuk::Vocabulary::NBIB::Encyclopedia())) {
            type = QLatin1String("EncyclopediaArticle"); //article in some proceedings paper
        }
        else {
            type = QLatin1String("Article"); //normal article in a journal or magazine
        }
    }
    // all other cases
    else {
        Nepomuk::Resource typeResource(publication.type());
        type = typeResource.genericLabel();

        Nepomuk::Resource typeResource2(Nepomuk::Vocabulary::NBIB::Publication());
        if(type == typeResource2.genericLabel()) {
            type = QLatin1String("Misc");
        }
    }

    // if we have strict export, transforn into standard types
    if(m_strict) {
        qDebug() << "strict export is not implemented yet";
    }

    return type;
}

void NepomukToBibTexPipe::collectContent(Entry *e, Nepomuk::Resource reference, Nepomuk::Resource publication)
{
    setTitle(e, publication, reference);
    setChapter(e, reference);

    // if the chapter has an author attached to it , don't search author of the publication
    // solves the incollection special case where the author of the chapter is not the one from the whole collection is ment
    if(!e->contains(QLatin1String("author"))){
        setAuthors(e, publication);
    }
    else {
        setBookAuthors(e, publication);
    }

    setEditors(e, publication);
    setSeriesEditors(e, publication);
    setPublicationDate(e, publication);
    setPublisher(e, publication);
    setOrganization(e, publication);
    setUrl(e, publication);
    setSeries(e, publication);
    setEdition(e, publication);
    setJournal(e, publication);
    setVolume(e, publication);
    setNumber(e, publication);
    setCopyright(e, publication);
    setPublicationMethod(e, publication);
    setType(e, publication);
    setLastAccessed(e, publication);
    setDate(e, publication);
    setEPrint(e, publication);
    setISBN(e, publication);
    setISSN(e, publication);
    setLCCN(e, publication);
    setMRNumber(e, publication);
    setPubMed(e, publication);
    setDOI(e, publication);
    setAbstract(e, publication);
    setNote(e, publication);
    setAnnote(e, publication);
    setLanguage(e, publication);
    setPages(e, reference);
    setKewords(e, publication);
    setEvent(e, publication);
    setCode(e, publication);
    setCodeNumber(e, publication);
    setCodeVolume(e, publication);
    setReporter(e, publication);
    setReporterVolume(e, publication);
    setPriorityNumbers(e, publication);
    setApplicationNumber(e, publication);
    setPatentReferences(e, publication);
    setLegalStatus(e, publication);
    setFilingDate(e, publication);
    setAssignee(e, publication);

    // Zotero additions
    setArticleType(e, publication);
    setSyncDetails(e, publication);
}

void NepomukToBibTexPipe::setTitle(Entry *e, Nepomuk::Resource publication, Nepomuk::Resource reference)
{
    QString title;
    QString booktitle;
    Nepomuk::Resource collection = publication.property(Nepomuk::Vocabulary::NBIB::collection()).toResource();

    if(publication.hasType(Nepomuk::Vocabulary::NBIB::Article())) {
        booktitle = collection.property(Nepomuk::Vocabulary::NIE::title()).toString();
        title = publication.property(Nepomuk::Vocabulary::NIE::title()).toString();
    }
    else{
        //ignore the case of a reference with a chapter (inbook/incollection)
        Nepomuk::Resource documentPart = reference.property(Nepomuk::Vocabulary::NBIB::referencedPart()).toResource();

        if(!documentPart.hasType(Nepomuk::Vocabulary::NBIB::Chapter())) {
            title = publication.property(Nepomuk::Vocabulary::NIE::title()).toString();
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
    QString chapterEntry = QLatin1String("chapter");
    Nepomuk::Resource publication = reference.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();
    if(publication.hasType(Nepomuk::Vocabulary::NBIB::Collection())) {
        chapterEntry = QLatin1String("title");
    }

    Nepomuk::Resource chapter = reference.property(Nepomuk::Vocabulary::NBIB::referencedPart()).toResource();
    QString chapterTitle = chapter.property(Nepomuk::Vocabulary::NIE::title()).toString();
    QString chapterNumber = chapter.property(Nepomuk::Vocabulary::NBIB::chapterNumber()).toString();
    chapterTitle.prepend(chapterNumber);

    Nepomuk::Resource author = chapter.property(Nepomuk::Vocabulary::NCO::creator()).toResource();
    QString chapterAuthor = author.property(Nepomuk::Vocabulary::NCO::fullname()).toString();

    Nepomuk::Resource book = reference.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();
    QString bookTitle = book.property(Nepomuk::Vocabulary::NIE::title()).toString();

    if(!chapterTitle.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(chapterTitle)));
        e->insert(chapterEntry, v);
        Value v2;
        v2.append(QSharedPointer<ValueItem>(new PlainText(bookTitle)));
        if(publication.hasType(Nepomuk::Vocabulary::NBIB::Bill())) {
            e->insert(Entry::ftTitle, v2);
        }
        else {
            e->insert(Entry::ftBookTitle, v2);
        }

        if(!chapterAuthor.isEmpty()) {
            Value v;
            v.append(QSharedPointer<ValueItem>(new PlainText(chapterAuthor)));
            e->insert(Entry::ftAuthor, v);
        }
    }
}

void NepomukToBibTexPipe::setAuthors(Entry *e, Nepomuk::Resource publication)
{
    QList<Nepomuk::Resource> authors = publication.property(Nepomuk::Vocabulary::NCO::creator()).toResourceList();

    Value v;
    if(!authors.isEmpty()) {
        foreach(const Nepomuk::Resource & a, authors) {
            QString firstName = a.property(Nepomuk::Vocabulary::NCO::nameGiven()).toString();
            QString lastName = a.property(Nepomuk::Vocabulary::NCO::nameFamily()).toString();
            QString suffix = a.property(Nepomuk::Vocabulary::NCO::nameHonorificSuffix()).toString();
            if(firstName.isEmpty())
                firstName = a.property(Nepomuk::Vocabulary::NCO::fullname()).toString();

            Person *p = new Person(firstName, lastName, suffix);
            v.append(QSharedPointer<ValueItem>(p));
        }

        if(!v.isEmpty()) {
            e->insert(Entry::ftAuthor, v);
        }
    }
}

void NepomukToBibTexPipe::setBookAuthors(Entry *e, Nepomuk::Resource publication)
{
    QList<Nepomuk::Resource> authors = publication.property(Nepomuk::Vocabulary::NCO::creator()).toResourceList();

    Value v;
    if(!authors.isEmpty()) {
        foreach(const Nepomuk::Resource & a, authors) {
            QString firstName = a.property(Nepomuk::Vocabulary::NCO::nameGiven()).toString();
            QString lastName = a.property(Nepomuk::Vocabulary::NCO::nameFamily()).toString();
            QString suffix = a.property(Nepomuk::Vocabulary::NCO::nameHonorificSuffix()).toString();
            if(firstName.isEmpty())
                firstName = a.property(Nepomuk::Vocabulary::NCO::fullname()).toString();

            Person *p = new Person(firstName, lastName, suffix);
            v.append(QSharedPointer<ValueItem>(p));
        }

        if(!v.isEmpty()) {
            e->insert(QLatin1String("bookAuthor"), v);
        }
    }
}

void NepomukToBibTexPipe::setEditors(Entry *e, Nepomuk::Resource publication)
{
    QList<Nepomuk::Resource> authors = publication.property(Nepomuk::Vocabulary::NBIB::editor()).toResourceList();

    Value v;
    if(!authors.isEmpty()) {
        foreach(const Nepomuk::Resource & a, authors) {
            QString firstName = a.property(Nepomuk::Vocabulary::NCO::nameGiven()).toString();
            QString lastName = a.property(Nepomuk::Vocabulary::NCO::nameFamily()).toString();
            QString suffix = a.property(Nepomuk::Vocabulary::NCO::nameHonorificSuffix()).toString();
            if(firstName.isEmpty())
                firstName = a.property(Nepomuk::Vocabulary::NCO::fullname()).toString();

            Person *p = new Person(firstName, lastName, suffix);
            v.append(QSharedPointer<ValueItem>(p));
        }

        if(!v.isEmpty()) {
            e->insert(Entry::ftEditor, v);
        }
    }
}

void NepomukToBibTexPipe::setSeriesEditors(Entry *e, Nepomuk::Resource publication)
{
    Nepomuk::Resource series =  publication.property(Nepomuk::Vocabulary::NBIB::inSeries()).toResource();
    QList<Nepomuk::Resource> authors = series.property(Nepomuk::Vocabulary::NBIB::editor()).toResourceList();

    Value v;
    if(!authors.isEmpty()) {
        foreach(const Nepomuk::Resource & a, authors) {
            QString firstName = a.property(Nepomuk::Vocabulary::NCO::nameGiven()).toString();
            QString lastName = a.property(Nepomuk::Vocabulary::NCO::nameFamily()).toString();
            QString suffix = a.property(Nepomuk::Vocabulary::NCO::nameHonorificSuffix()).toString();
            if(firstName.isEmpty())
                firstName = a.property(Nepomuk::Vocabulary::NCO::fullname()).toString();

            Person *p = new Person(firstName, lastName, suffix);
            v.append(QSharedPointer<ValueItem>(p));
        }

        if(!v.isEmpty()) {
            e->insert(QLatin1String("seriesEditor"), v);
        }
    }
}

void NepomukToBibTexPipe::setPublicationDate(Entry *e, Nepomuk::Resource publication)
{
    QString pdString = publication.property(Nepomuk::Vocabulary::NBIB::publicationDate()).toString();

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
    QList<Nepomuk::Resource> publisher;

    if(publication.hasType(Nepomuk::Vocabulary::NBIB::Thesis())) {
        publisherEntry = QLatin1String("school");
        publisher = publication.property(Nepomuk::Vocabulary::NCO::publisher()).toResourceList();
    }
    else if(publication.hasType(Nepomuk::Vocabulary::NBIB::Techreport())) {
        publisherEntry = QLatin1String("institution");
        publisher = publication.property(Nepomuk::Vocabulary::NCO::publisher()).toResourceList();
    }
    else {
        publisher = publication.property(Nepomuk::Vocabulary::NCO::publisher()).toResourceList();
    }

    QString names;
    QString address;
    if(!publisher.isEmpty()) {
        foreach(const Nepomuk::Resource & p, publisher) {
            //TODO don't rely only on fullname of NCO::Contact
            names.append(p.property(Nepomuk::Vocabulary::NCO::fullname()).toString());
            names.append(QLatin1String(" and "));


            Nepomuk::Resource existingAddr = p.property(Nepomuk::Vocabulary::NCO::hasPostalAddress()).toResource();
            if(existingAddr.isValid())
                address.append(existingAddr.property(Nepomuk::Vocabulary::NCO::extendedAddress()).toString());
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
    if(publication.hasType(Nepomuk::Vocabulary::NBIB::Article())) {
        Nepomuk::Resource collectionResource = publication.property(Nepomuk::Vocabulary::NBIB::collection()).toResource();
        if(collectionResource.hasType(Nepomuk::Vocabulary::NBIB::Proceedings())) {
            org = collectionResource.property(Nepomuk::Vocabulary::NBIB::organization()).toResource();
        }
    }
    else {
        org = publication.property(Nepomuk::Vocabulary::NBIB::organization()).toResource();
    }

    QString string = org.property(Nepomuk::Vocabulary::NCO::fullname()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(string)));
        e->insert(QLatin1String("organization"), v);
    }
}

void NepomukToBibTexPipe::setUrl(Entry *e, Nepomuk::Resource publication)
{
    QList<Nepomuk::Resource> objectList = publication.property(Nepomuk::Vocabulary::NBIB::isPublicationOf()).toResourceList();

    QString urlList;
    foreach(const Nepomuk::Resource &dataObjects, objectList) {
        QUrl url = dataObjects.property(Nepomuk::Vocabulary::NIE::url()).toUrl();

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
    Nepomuk::Resource series = publication.property(Nepomuk::Vocabulary::NBIB::inSeries()).toResource();
    QString string = series.property(Nepomuk::Vocabulary::NIE::title()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(string)));
        e->insert(Entry::ftSeries, v);
    }
}

void NepomukToBibTexPipe::setEdition(Entry *e, Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::edition()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(string)));
        e->insert(QLatin1String("edition"), v);
    }
}

void NepomukToBibTexPipe::setJournal(Entry *e, Nepomuk::Resource publication)
{
    Nepomuk::Resource journalIssue = publication.property(Nepomuk::Vocabulary::NBIB::collection()).toResource();

    if(!journalIssue.isValid()) {
        return; // no journal available for his resource
    }

    // if we have a JournalIssue, get the journal contact and number/volume
    QString journalNumber = journalIssue.property(Nepomuk::Vocabulary::NBIB::number()).toString(); //Issue number
    QString journalVolume = journalIssue.property(Nepomuk::Vocabulary::NBIB::volume()).toString();

    Nepomuk::Resource journal = journalIssue.property(Nepomuk::Vocabulary::NBIB::inSeries()).toResource();
    QString journalName = journal.property(Nepomuk::Vocabulary::NIE::title()).toString();;

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

void NepomukToBibTexPipe::setVolume(Entry *e, Nepomuk::Resource publication)
{
    // this only returns proper values if the volume is not used to identify a journal
    // journal volumes are captured by the getJournal method
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::volume()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(string)));
        e->insert(Entry::ftVolume, v);
    }
}

void NepomukToBibTexPipe::setNumber(Entry *e, Nepomuk::Resource publication)
{
    // this only returns proper values if the number is not used to identify a journal
    // journal numbers are captured by the getJournal method
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::number()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(string)));
        e->insert(Entry::ftNumber, v);
    }
}

void NepomukToBibTexPipe::setPublicationMethod(Entry *e, Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::publicationMethod()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(string)));
        e->insert(QLatin1String("howpublished"), v);
    }
}

void NepomukToBibTexPipe::setType(Entry *e, Nepomuk::Resource publication)
{
    // not type like @Article @Book but something that defines a Report/techreport in deeper detail
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::type()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(string)));
        e->insert(QLatin1String("type"), v);
    }
}

void NepomukToBibTexPipe::setCopyright(Entry *e, Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NIE::copyright()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(string)));
        e->insert(QLatin1String("copyrigth"), v);
    }
}

void NepomukToBibTexPipe::setLastAccessed(Entry *e, Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NUAO::lastUsage()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(string)));
        e->insert(QLatin1String("accessdate"), v);
    }
}

void NepomukToBibTexPipe::setDate(Entry *e, Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::publicationDate()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(string)));
        e->insert(QLatin1String("date"), v);
    }
}

void NepomukToBibTexPipe::setLanguage(Entry *e, Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NIE::language()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(string)));
        e->insert(QLatin1String("language"), v);
    }
}

void NepomukToBibTexPipe::setEPrint(Entry *e, Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::eprint()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(string)));
        e->insert(QLatin1String("eprint"), v);
    }
}

void NepomukToBibTexPipe::setISBN(Entry *e, Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::isbn()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(string)));
        e->insert(QLatin1String("isbn"), v);
    }
}

void NepomukToBibTexPipe::setISSN(Entry *e, Nepomuk::Resource publication)
{
    Nepomuk::Resource series = publication.property(Nepomuk::Vocabulary::NBIB::inSeries()).toResource();
    QString string = series.property(Nepomuk::Vocabulary::NBIB::issn()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(string)));
        e->insert(QLatin1String("issn"), v);
    }
}

void NepomukToBibTexPipe::setLCCN(Entry *e, Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::lccn()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(string)));
        e->insert(QLatin1String("lccn"), v);
    }
}

void NepomukToBibTexPipe::setMRNumber(Entry *e, Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::mrNumber()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(string)));
        e->insert(QLatin1String("mrnumber"), v);
    }
}

void NepomukToBibTexPipe::setDOI(Entry *e, Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::doi()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(string)));
        e->insert(QLatin1String("doi"), v);
    }
}

void NepomukToBibTexPipe::setPubMed(Entry *e, Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::pubMed()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(string)));
        e->insert(QLatin1String("pubmed"), v);
    }
}

void NepomukToBibTexPipe::setAbstract(Entry *e, Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::abstract()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(string)));
        e->insert(QLatin1String("abstract"), v);
    }
}

void NepomukToBibTexPipe::setNote(Entry *e, Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NIE::description()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(string)));
        e->insert(QLatin1String("note"), v);
    }
}

void NepomukToBibTexPipe::setAnnote(Entry *e, Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NIE::comment()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(string)));
        e->insert(QLatin1String("annote"), v);
    }
}

void NepomukToBibTexPipe::setPages(Entry *e, Nepomuk::Resource reference)
{
    QString string = reference.property(Nepomuk::Vocabulary::NBIB::pages()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(string)));
        e->insert(QLatin1String("pages"), v);
    }
}

void NepomukToBibTexPipe::setNumOfPages(Entry *e, Nepomuk::Resource reference)
{
    QString string = reference.property(Nepomuk::Vocabulary::NBIB::numberOfPages()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(string)));
        e->insert(QLatin1String("numpages"), v);
    }
}

void NepomukToBibTexPipe::setEvent(Entry *e, Nepomuk::Resource publication)
{
    Nepomuk::Resource event = publication.property(Nepomuk::Vocabulary::NBIB::event()).toResource();

    if(!event.isValid())
        return;

    QString string = event.property(Nepomuk::Vocabulary::NIE::title()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(string)));
        e->insert(QLatin1String("event"), v);
    }
}

void NepomukToBibTexPipe::setCode(Entry *e, Nepomuk::Resource publication)
{
    Nepomuk::Resource codeOfLaw = publication.property(Nepomuk::Vocabulary::NBIB::codeOfLaw()).toResource();
    QString string = codeOfLaw.property(Nepomuk::Vocabulary::NIE::title()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(string)));
        e->insert(QLatin1String("code"), v);
    }
}

void NepomukToBibTexPipe::setCodeNumber(Entry *e, Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::codeNumber()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(string)));
        e->insert(QLatin1String("codenumber"), v);
    }
}

void NepomukToBibTexPipe::setCodeVolume(Entry *e, Nepomuk::Resource publication)
{
    Nepomuk::Resource codeOfLaw = publication.property(Nepomuk::Vocabulary::NBIB::codeOfLaw()).toResource();
    QString string = codeOfLaw.property(Nepomuk::Vocabulary::NBIB::volume()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(string)));
        e->insert(QLatin1String("codevolume"), v);
    }
}

void NepomukToBibTexPipe::setReporter(Entry *e, Nepomuk::Resource publication)
{
    Nepomuk::Resource courtReporter = publication.property(Nepomuk::Vocabulary::NBIB::courtReporter()).toResource();
    QString string = courtReporter.property(Nepomuk::Vocabulary::NIE::title()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(string)));
        e->insert(QLatin1String("reporter"), v);
    }
}

void NepomukToBibTexPipe::setReporterVolume(Entry *e, Nepomuk::Resource publication)
{
    Nepomuk::Resource courtReporter = publication.property(Nepomuk::Vocabulary::NBIB::courtReporter()).toResource();
    QString string = courtReporter.property(Nepomuk::Vocabulary::NBIB::volume()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(string)));
        e->insert(QLatin1String("reportervolume"), v);
    }
}

void NepomukToBibTexPipe::setApplicationNumber(Entry *e, Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::applicationNumber()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(string)));
        e->insert(QLatin1String("applicationnumber"), v);
    }
}

void NepomukToBibTexPipe::setPatentReferences(Entry *e, Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::patentReferences()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(string)));
        e->insert(QLatin1String("references"), v);
    }
}

void NepomukToBibTexPipe::setLegalStatus(Entry *e, Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::legalStatus()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(string)));
        e->insert(QLatin1String("legalstatus"), v);
    }
}

void NepomukToBibTexPipe::setFilingDate(Entry *e, Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::filingDate()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(string)));
        e->insert(QLatin1String("filingdate"), v);
    }
}

void NepomukToBibTexPipe::setAssignee(Entry *e, Nepomuk::Resource publication)
{
    Nepomuk::Resource assignee = publication.property(Nepomuk::Vocabulary::NBIB::assignee()).toResource();

    QString string = assignee.property(Nepomuk::Vocabulary::NCO::fullname()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(string)));
        e->insert(QLatin1String("assignee"), v);
    }
}

void NepomukToBibTexPipe::setPriorityNumbers(Entry *e, Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::priorityNumbers()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(string)));
        e->insert(QLatin1String("prioritynumbers"), v);
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
    QList<Nepomuk::Resource> sycList = publication.property(Nepomuk::Vocabulary::SYNC::serverSyncData()).toResourceList();

    //qDebug() << "NepomukToBibTexPipe::setSyncDetails for" << publication.property(Nepomuk::Vocabulary::NIE::title()) << "found syncsettings ::" << sycList.size();

    // only add the sync details the the right storage
    foreach(const Nepomuk::Resource &r, sycList) {
        if(r.property(Nepomuk::Vocabulary::SYNC::provider()).toString() != QString("zotero")) { //TODO make this possible for others too
            //qDebug() << "r.property(Nepomuk::Vocabulary::SYNC::provider()).toString() != QString(zotero)";
            continue;
        }
        if(r.property(Nepomuk::Vocabulary::SYNC::userId()).toString() != m_syncUserId) {//TODO make this possible for others too
            //qDebug() << "r.property(Nepomuk::Vocabulary::SYNC::userId()).toString() != m_syncUserId";
            continue;
        }
        if(r.property(Nepomuk::Vocabulary::SYNC::url()).toString() != m_syncUrl) {//TODO make this possible for others too
            //qDebug() << "r.property(Nepomuk::Vocabulary::SYNC::url()).toString() != m_syncUrl";
            continue;
        }

        //now we have the right object, write down sync details
        QString etag = r.property(Nepomuk::Vocabulary::SYNC::etag()).toString();
        Value v1;
        v1.append(QSharedPointer<ValueItem>(new PlainText(etag)));
        e->insert(QLatin1String("zoteroEtag"), v1);

        QString key = r.property(Nepomuk::Vocabulary::SYNC::id()).toString();
        Value v2;
        v2.append(QSharedPointer<ValueItem>(new PlainText(key)));
        e->insert(QLatin1String("zoteroKey"), v2);

        QString updated = r.property(Nepomuk::Vocabulary::NUAO::lastModification()).toString();
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
    if(publication.hasType(Nepomuk::Vocabulary::NBIB::Article())) {
        QString articleType;
        Nepomuk::Resource collection = publication.property(Nepomuk::Vocabulary::NBIB::collection()).toResource();
        if(collection.hasType(Nepomuk::Vocabulary::NBIB::JournalIssue())) {
            articleType = QLatin1String("journal"); //article in some proceedings paper
        }
        else if(collection.hasType(Nepomuk::Vocabulary::NBIB::NewspaperIssue())) {
            articleType = QLatin1String("newspaper"); //normal article in a journal or magazine
        }
        else if(collection.hasType(Nepomuk::Vocabulary::NBIB::MagazinIssue())) {
            articleType = QLatin1String("magazine"); //normal article in a journal or magazine
        }

        if(!articleType.isEmpty()) {
            Value v;
            v.append(QSharedPointer<ValueItem>(new PlainText(articleType)));
            e->insert(QLatin1String("articletype"), v);
        }
    }
}
