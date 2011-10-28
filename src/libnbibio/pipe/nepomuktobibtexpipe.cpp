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
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Vocabulary/NFO>
#include <Nepomuk/Vocabulary/NCO>
#include <Nepomuk/Vocabulary/NUAO>
#include <Nepomuk/Variant>
#include <Nepomuk/Tag>

#include <QDebug>

NepomukToBibTexPipe::NepomukToBibTexPipe()
{
}

void NepomukToBibTexPipe::pipeExport(QList<Nepomuk::Resource> resources)
{
    int citeKeyNumer = 1;
    QString citeRef = QLatin1String("BibTexExport");

    foreach(Nepomuk::Resource resource, resources) {

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
            // we will extract as many information sas possible anyway
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
        m_bibtexFile.append(e);
    }
}

File NepomukToBibTexPipe::bibtexFile()
{
    return m_bibtexFile;
}

QString NepomukToBibTexPipe::retrieveEntryType(Nepomuk::Resource reference, Nepomuk::Resource publication)
{
    QString type;

    if(publication.hasType(Nepomuk::Vocabulary::NBIB::Book()) || publication.hasType(Nepomuk::Vocabulary::NBIB::Collection())) {
        QString pages = reference.property(Nepomuk::Vocabulary::NBIB::pages()).toString();
        Nepomuk::Resource chapter = reference.property(Nepomuk::Vocabulary::NBIB::referencedChapter()).toResource();
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
    else if(publication.hasType(Nepomuk::Vocabulary::NBIB::Article())) {
        Nepomuk::Resource proceedings = publication.property(Nepomuk::Vocabulary::NBIB::proceedings()).toResource();
        if(proceedings.isValid()) {
            type = QLatin1String("Inproceedings"); //article in some proceedings paper
        }
        else {
            type = QLatin1String("Article"); //normal article in a journal or magazine
        }
    }
    else {
        Nepomuk::Resource typeResource(publication.type());
        type = typeResource.genericLabel();

        Nepomuk::Resource typeResource2(Nepomuk::Vocabulary::NBIB::Publication());
        if(type == typeResource2.genericLabel()) {
            type = QLatin1String("Misc");
        }
    }

    // if we have strict export, transforn into standard types
    //    if(false) {
    //        if(type.contains(QRegExp("")))
    //    }

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

    setEditors(e, publication);
    setPublicationDate(e, publication);
    setPublisher(e, publication);
    setOrganization(e, publication);
    setUrl(e, publication);
    setSeries(e, publication);
    setEdition(e, publication);
    setJournal(e, publication);
    setVolume(e, publication);
    setNumber(e, publication);
    setPublicationMethod(e, publication);
    setType(e, publication);
    setCopyrigth(e, publication);
    setLastAccessed(e, publication);
    setEPrint(e, publication);
    setISBN(e, publication);
    setISSN(e, publication);
    setLCCN(e, publication);
    setMRNumber(e, publication);
    setDOI(e, publication);
    setAbstract(e, publication);
    setTOC(e, publication);
    setNote(e, publication);
    setAnnote(e, publication);
    setPages(e, reference);
    setKewords(e, publication);
}

void NepomukToBibTexPipe::setTitle(Entry *e, Nepomuk::Resource publication, Nepomuk::Resource reference)
{
    QString string;
    QString title;
    QString booktitle;
    Nepomuk::Resource proceedings = publication.property(Nepomuk::Vocabulary::NBIB::proceedings()).toResource();

    if(publication.hasType(Nepomuk::Vocabulary::NBIB::Article()) && proceedings.isValid()) {
        booktitle = proceedings.property(Nepomuk::Vocabulary::NIE::title()).toString();
        title = publication.property(Nepomuk::Vocabulary::NIE::title()).toString();
    }
    else{
        //ignore the case of a reference with a chapter (inbook/incollection)
        Nepomuk::Resource chapter = reference.property(Nepomuk::Vocabulary::NBIB::referencedChapter()).toResource();

        if(!chapter.isValid()) {
            title = publication.property(Nepomuk::Vocabulary::NIE::title()).toString();
        }
    }

    if(!title.isEmpty()) {
        Value v;
        v.append(new PlainText(title));
        e->insert(Entry::ftTitle, v);
    }

    if(!booktitle.isEmpty()) {
        Value v;
        v.append(new PlainText(booktitle));
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

    Nepomuk::Resource chapter = reference.property(Nepomuk::Vocabulary::NBIB::referencedChapter()).toResource();
    QString chapterTitle = chapter.property(Nepomuk::Vocabulary::NIE::title()).toString();

    Nepomuk::Resource author = chapter.property(Nepomuk::Vocabulary::NCO::creator()).toResource();
    QString chapterAuthor = author.property(Nepomuk::Vocabulary::NCO::fullname()).toString();

    Nepomuk::Resource book = reference.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();
    QString bookTitle = book.property(Nepomuk::Vocabulary::NIE::title()).toString();

    QString string;
    if(!chapterTitle.isEmpty()) {
        Value v;
        v.append(new PlainText(chapterTitle));
        e->insert(chapterEntry, v);
        Value v2;
        v2.append(new PlainText(bookTitle));
        e->insert(Entry::ftBookTitle, v2);

        if(!chapterAuthor.isEmpty()) {
            Value v;
            v.append(new PlainText(chapterAuthor));
            e->insert(Entry::ftAuthor, v);
        }
    }
}

void NepomukToBibTexPipe::setAuthors(Entry *e, Nepomuk::Resource publication)
{
    QList<Nepomuk::Resource> authors = publication.property(Nepomuk::Vocabulary::NCO::creator()).toResourceList();

    Value v;
    if(!authors.isEmpty()) {
        foreach(Nepomuk::Resource a, authors) {
            QString firstName = a.property(Nepomuk::Vocabulary::NCO::nameGiven()).toString();
            QString lastName = a.property(Nepomuk::Vocabulary::NCO::nameFamily()).toString();
            QString suffix = a.property(Nepomuk::Vocabulary::NCO::nameHonorificSuffix()).toString();
            if(firstName.isEmpty())
                firstName = a.property(Nepomuk::Vocabulary::NCO::fullname()).toString();

            Person *p = new Person(firstName, lastName, suffix);
            v.append(p);
        }

        if(!v.isEmpty()) {
            e->insert(Entry::ftAuthor, v);
        }
    }
}

void NepomukToBibTexPipe::setEditors(Entry *e, Nepomuk::Resource publication)
{
    QList<Nepomuk::Resource> authors = publication.property(Nepomuk::Vocabulary::NBIB::editor()).toResourceList();

    Value v;
    if(!authors.isEmpty()) {
        foreach(Nepomuk::Resource a, authors) {
            QString firstName = a.property(Nepomuk::Vocabulary::NCO::nameGiven()).toString();
            QString lastName = a.property(Nepomuk::Vocabulary::NCO::nameFamily()).toString();
            QString suffix = a.property(Nepomuk::Vocabulary::NCO::nameHonorificSuffix()).toString();
            if(firstName.isEmpty())
                firstName = a.property(Nepomuk::Vocabulary::NCO::fullname()).toString();

            Person *p = new Person(firstName, lastName, suffix);
            v.append(p);
        }

        if(!v.isEmpty()) {
            e->insert(Entry::ftAuthor, v);
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
            v.append(new PlainText(monthName));
            e->insert(Entry::ftMonth, v);
        }
    }

    if(!year.isEmpty()) {
        Value v;
        v.append(new PlainText(year));
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
        foreach(Nepomuk::Resource p, publisher) {
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
            v.append(new PlainText(names));
            e->insert(publisherEntry, v);
        }
        if(!address.isEmpty()) {
            Value v;
            v.append(new PlainText(address));
            e->insert(Entry::ftAddress, v);
        }
    }
}

void NepomukToBibTexPipe::setOrganization(Entry *e, Nepomuk::Resource publication)
{
    Nepomuk::Resource org;
    Nepomuk::Resource proceedings = publication.property(Nepomuk::Vocabulary::NBIB::proceedings()).toResource();
    if(publication.hasType(Nepomuk::Vocabulary::NBIB::Article()) && proceedings.isValid()) {
        org = proceedings.property(Nepomuk::Vocabulary::NBIB::organization()).toResource();
    }
    else {
        org = publication.property(Nepomuk::Vocabulary::NBIB::organization()).toResource();
    }

    QString string = org.property(Nepomuk::Vocabulary::NCO::fullname()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(new PlainText(string));
        e->insert(QLatin1String("organization"), v);
    }
}

void NepomukToBibTexPipe::setUrl(Entry *e, Nepomuk::Resource publication)
{
    qDebug() << "NepomukToBibTexPipe::setUrl /!\\ needs proper implementation /!\\ ";
}

void NepomukToBibTexPipe::setSeries(Entry *e, Nepomuk::Resource publication)
{
    Nepomuk::Resource series = publication.property(Nepomuk::Vocabulary::NBIB::inSeries()).toResource();
    QString string = series.property(Nepomuk::Vocabulary::NIE::title()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(new PlainText(string));
        e->insert(Entry::ftSeries, v);
    }
}

void NepomukToBibTexPipe::setEdition(Entry *e, Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::edition()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(new PlainText(string));
        e->insert(QLatin1String("edition"), v);
    }
}

void NepomukToBibTexPipe::setJournal(Entry *e, Nepomuk::Resource publication)
{
    Nepomuk::Resource journalIssue = publication.property(Nepomuk::Vocabulary::NBIB::journalIssue()).toResource();

    if(!journalIssue.isValid()) {
        return; // no journal available for his resource
    }

    // if we have a JournalIssue, get the journal contact and number/volume
    QString journalNumber = journalIssue.property(Nepomuk::Vocabulary::NBIB::issueNumber()).toString();
    QString journalVolume = journalIssue.property(Nepomuk::Vocabulary::NBIB::volume()).toString();

    Nepomuk::Resource journal = journalIssue.property(Nepomuk::Vocabulary::NBIB::journal()).toResource();
    QString journalName = journal.property(Nepomuk::Vocabulary::NIE::title()).toString();;

    if(!journalNumber.isEmpty()) {
        Value v;
        v.append(new PlainText(journalNumber));
        e->insert(Entry::ftNumber, v);
    }
    if(!journalVolume.isEmpty()) {
        Value v;
        v.append(new PlainText(journalVolume));
        e->insert(Entry::ftVolume, v);
    }
    if(!journalName.isEmpty()) {
        Value v;
        v.append(new PlainText(journalName));
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
        v.append(new PlainText(string));
        e->insert(Entry::ftVolume, v);
    }
}

void NepomukToBibTexPipe::setNumber(Entry *e, Nepomuk::Resource publication)
{
    // this only returns proper values if the number is not used to identify a journal
    // journal numbers are captured by the getJournal method
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::issueNumber()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(new PlainText(string));
        e->insert(Entry::ftNumber, v);
    }
}

void NepomukToBibTexPipe::setPublicationMethod(Entry *e, Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::publicationMethod()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(new PlainText(string));
        e->insert(QLatin1String("howpublished"), v);
    }
}

void NepomukToBibTexPipe::setType(Entry *e, Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::type()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(new PlainText(string));
        e->insert(QLatin1String("type"), v);
    }
}

void NepomukToBibTexPipe::setCopyrigth(Entry *e, Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NIE::copyright()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(new PlainText(string));
        e->insert(QLatin1String("copyrigth"), v);
    }
}

void NepomukToBibTexPipe::setLastAccessed(Entry *e, Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NUAO::lastUsage()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(new PlainText(string));
        e->insert(QLatin1String("lastUsage"), v);
    }
}

void NepomukToBibTexPipe::setEPrint(Entry *e, Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::eprint()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(new PlainText(string));
        e->insert(QLatin1String("eprint"), v);
    }
}

void NepomukToBibTexPipe::setISBN(Entry *e, Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::isbn()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(new PlainText(string));
        e->insert(QLatin1String("isbn"), v);
    }
}

void NepomukToBibTexPipe::setISSN(Entry *e, Nepomuk::Resource publication)
{
    Nepomuk::Resource series = publication.property(Nepomuk::Vocabulary::NBIB::inSeries()).toResource();
    QString string = series.property(Nepomuk::Vocabulary::NBIB::issn()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(new PlainText(string));
        e->insert(QLatin1String("issn"), v);
    }
}

void NepomukToBibTexPipe::setLCCN(Entry *e, Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::lccn()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(new PlainText(string));
        e->insert(QLatin1String("lccn"), v);
    }
}

void NepomukToBibTexPipe::setMRNumber(Entry *e, Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::mrNumber()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(new PlainText(string));
        e->insert(QLatin1String("mrnumber"), v);
    }
}

void NepomukToBibTexPipe::setDOI(Entry *e, Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::doi()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(new PlainText(string));
        e->insert(QLatin1String("doi"), v);
    }
}

void NepomukToBibTexPipe::setAbstract(Entry *e, Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::abstract()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(new PlainText(string));
        e->insert(QLatin1String("abstract"), v);
    }
}

void NepomukToBibTexPipe::setTOC(Entry *e, Nepomuk::Resource publication)
{
    qDebug() << "NBibExporterBibTex::getTOC /!\ needs proper implementation /!\ ";

}

void NepomukToBibTexPipe::setNote(Entry *e, Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NIE::description()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(new PlainText(string));
        e->insert(QLatin1String("note"), v);
    }
}

void NepomukToBibTexPipe::setAnnote(Entry *e, Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NIE::comment()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(new PlainText(string));
        e->insert(QLatin1String("annote"), v);
    }
}

void NepomukToBibTexPipe::setPages(Entry *e, Nepomuk::Resource reference)
{
    QString string = reference.property(Nepomuk::Vocabulary::NBIB::pages()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(new PlainText(string));
        e->insert(QLatin1String("pages"), v);
    }
}

void NepomukToBibTexPipe::setKewords(Entry *e, Nepomuk::Resource publication)
{
    QList<Nepomuk::Tag> tags = publication.tags();

    Value v;
    foreach(Nepomuk::Tag tag, tags) {
        Keyword *p = new Keyword(tag.genericLabel());
        v.append(p);
    }

    if(!v.isEmpty()) {
        e->insert(Entry::ftKeywords, v);
    }
}

