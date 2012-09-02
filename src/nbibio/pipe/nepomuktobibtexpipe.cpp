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
#include <Nepomuk2/Vocabulary/NIE>
#include <Nepomuk2/Vocabulary/NFO>
#include <Nepomuk2/Vocabulary/NCO>
#include <Nepomuk2/Vocabulary/NUAO>
#include <Nepomuk2/Vocabulary/PIMO>
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk2/Variant>
#include <Nepomuk2/Tag>

#include <KDE/KDebug>

#include <QtCore/QSharedPointer>

using namespace Nepomuk2::Vocabulary;
using namespace Soprano::Vocabulary;

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

void NepomukToBibTexPipe::pipeExport(QList<Nepomuk2::Resource> resources)
{
    delete m_bibtexFile;
    m_bibtexFile = new File;

    int citeKeyNumer = 1;
    QString citeRef = QLatin1String("BibTexExport");

    m_percentPerResource = (qreal)resources.size() / 100.0;

    int i = 0;
    foreach(const Nepomuk2::Resource & resource, resources) {
        int curProgress = i * m_percentPerResource;
        emit progress(curProgress);
        i++;

        Nepomuk2::Resource reference;
        Nepomuk2::Resource publication;

        //BUG: not all types are fetched correctly, fixed in 4.9.1
        resource.types();

        // first check if we operate on a Reference or a Publication
        if(resource.hasType( NBIB::Reference() )) {
            // we have a Reference
            reference = resource;
            publication = reference.property(NBIB::publication()).toResource();
        }
        else {
            //we have a publication and no idea what reference to use with it
            // we will extract as many information as possible anyway
            // or we try to export pimo:Note or some attachment information
            publication = resource;
        }

        //BUG: not all types are fetched correctly, fixed in 4.9.1
        publication.types();
        reference.types();

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

        // special handling for notes
        if( entryType == QLatin1String("Note")) {
            collectNoteContent(e, publication);

            if(m_addNepomukUris) {
                Value v1;
                v1.append(QSharedPointer<ValueItem>(new PlainText(publication.uri().toString())));
                e->insert(QLatin1String("nepomuk-note-uri"), v1);
            }
        }
        // special handling for attachments
        else if( entryType == QLatin1String("Attachment")) {
            collectAttachmentContent(e, publication);

            if(m_addNepomukUris) {
                Value v1;
                v1.append(QSharedPointer<ValueItem>(new PlainText(publication.uri().toString())));
                e->insert(QLatin1String("nepomuk-attachment-uri"), v1);
            }
        }
        // all other references
        else {
            collectContent(e, reference, publication);

            if(m_addNepomukUris) {
                Value v1;
                v1.append(QSharedPointer<ValueItem>(new PlainText(publication.uri().toString())));
                e->insert(QLatin1String("nepomuk-publication-uri"), v1);
                if(reference.isValid()) {
                    Value v2;
                    v2.append(QSharedPointer<ValueItem>(new PlainText(reference.uri().toString())));
                    e->insert(QLatin1String("nepomuk-reference-uri"), v2);
                }
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

QString NepomukToBibTexPipe::retrieveEntryType(Nepomuk2::Resource reference, Nepomuk2::Resource publication)
{
    QString type;
    //FIXME: simplyfy this via globals.h refactoring, not all types are recocnized currently
    if(publication.hasType(NBIB::Dictionary())) {
        QString pages = reference.property(NBIB::pages()).toString();
        Nepomuk2::Resource chapter = reference.property(NBIB::referencedPart()).toResource();
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
        Nepomuk2::Resource chapter = reference.property(NBIB::referencedPart()).toResource();
        Nepomuk2::Resource chapterAuthor = chapter.property(NCO::creator()).toResource();

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
            // check for publisher, its book with and booklet without
            QList<Nepomuk2::Resource> publisher = publication.property( NCO::publisher() ).toResourceList();
            if(publisher.isEmpty())
                type = QLatin1String("Booklet");
            else
                type = QLatin1String("Book");
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
    else if(publication.hasType(NBIB::Techreport())) {
        type = QLatin1String("Techreport");
    }
    else if(publication.hasType(NBIB::Manual())) {
        type = QLatin1String("Manual");
    }
    // handle special articles
    else if(publication.hasType(NBIB::Article())) {
        Nepomuk2::Resource collection = publication.property(NBIB::collection()).toResource();
        //BUG:: Not all types are fetched, fixed in 4.9.1
        collection.types();
        if(collection.hasType(NBIB::Proceedings())) {
            type = QLatin1String("Inproceedings"); //article in some proceedings paper
        }
        else {
            type = QLatin1String("Article"); //normal article in a journal, magazine pr newspaper
        }
    }
    else if(publication.hasType( PIMO::Note() )) {
        type = QLatin1String("Note");
    }
    else if( publication.hasType( NFO::Document() ) || publication.hasType( NFO::FileDataObject() )
             || publication.hasType( NFO::RemoteDataObject() )) {
        type = QLatin1String("Attachment");
    }
    // all other cases
    else {
        //FIXME: not everything here is misc type, use global function to get the correct type
        type = QLatin1String("Misc");
    }

    // if we have strict export, transforn into standard types
    if(m_strict) {
        kDebug() << "strict export is not implemented yet";
    }

    return type;
}

void NepomukToBibTexPipe::collectContent(Entry *e, Nepomuk2::Resource reference, Nepomuk2::Resource publication)
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
    setNote(e,publication);

    setValue(e, publication, NBIB::edition(), QLatin1String("edition"));
    setValue(e, publication, NBIB::volume(), Entry::ftVolume);
    setValue(e, publication, NBIB::number(), Entry::ftNumber);
    setValue(e, publication, NIE::copyright(), QLatin1String("copyright"));
    setValue(e, publication, NBIB::publicationMethod(), QLatin1String("howpublished"));
    setValue(e, publication, NBIB::publicationType(), QLatin1String("type"));
    setValue(e, publication, NUAO::lastUsage(), QLatin1String("accessdate"));
    setValue(e, publication, NBIB::publicationDate(), QLatin1String("date"));
    setValue(e, publication, NBIB::eprint(), QLatin1String("eprint"));
    setValue(e, publication, NBIB::isbn(), QLatin1String("isbn"));
    setValue(e, publication, NBIB::lccn(), QLatin1String("lccn"));
    setValue(e, publication, NBIB::mrNumber(), QLatin1String("mrnumber"));
    setValue(e, publication, NBIB::pubMed(), QLatin1String("pubmed"));
    setValue(e, publication, NBIB::doi(), QLatin1String("doi"));
    setValue(e, publication, NBIB::abstract(), QLatin1String("abstract"));
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

void NepomukToBibTexPipe::collectNoteContent(Entry *e, Nepomuk2::Resource note)
{
    setValue(e, note, NIE::plainTextContent(), QLatin1String("note"));
    setKewords(e, note);

    // Zotero additions
    setSyncDetails(e, note);

    // if we don't have a zotero key we might need to add a new item and need to know the parent of it first
    if(e->contains("zoteroKey")) {
        return;
    }

    QList<Nepomuk2::Resource> parents = note.property(NAO::isRelated()).toResourceList();

    foreach( const Nepomuk2::Resource &r, parents) {
        QList<Nepomuk2::Resource> sycList = r.property(SYNC::serverSyncData()).toResourceList();

        // only add the sync details from the right storage
        foreach(const Nepomuk2::Resource &parentSync, sycList) {
            if(parentSync.property(SYNC::provider()).toString() != QString("zotero")) { //TODO make this possible for others too
                continue;
            }
            if(parentSync.property(SYNC::userId()).toString() != m_syncUserId) {
                continue;
            }
            if(parentSync.property(SYNC::url()).toString() != m_syncUrl) {
                continue;
            }

            //now we have the right object, write down parent sync details

            QString updated = parentSync.property(SYNC::id()).toString();
            Value v3;
            v3.append(QSharedPointer<ValueItem>(new PlainText(updated)));
            e->insert(QLatin1String("zoteroParent"), v3);

            return; // stop here
        }
    }
}

void NepomukToBibTexPipe::collectAttachmentContent(Entry *e, Nepomuk2::Resource attachment)
{
    kDebug() << "collectAttachmentContent";
    setValue(e, attachment, NIE::title(), QLatin1String("title"));
    if(!e->contains( QLatin1String("title") )) {
        setValue(e, attachment, NIE::url(), QLatin1String("title"));
    }

    setValue(e, attachment, NIE::url(), QLatin1String("url"));
    setValue(e, attachment, NUAO::lastUsage(), QLatin1String("accessDate"));

    QString linkMode;
    if(attachment.hasType(NFO::RemoteDataObject())) {
        linkMode = '1';
    }
    else {
        linkMode = '0';
    }

    Value v;
    v.append(QSharedPointer<ValueItem>(new PlainText(linkMode)));
    e->insert(QLatin1String("linkMode"), v);


    //DEBUG
    Value v1;
    v1.append(QSharedPointer<ValueItem>(new PlainText(QString("text/html"))));
    e->insert(QLatin1String("mimeType"), v1);
    Value v2;
    v2.append(QSharedPointer<ValueItem>(new PlainText(QString("utf-8"))));
    e->insert(QLatin1String("charset"), v2);


    //DEBUG END

    setValue(e, attachment, NIE::comment(), QLatin1String("note"));

    setKewords(e, attachment);

    // Zotero additions
    setSyncDetails(e, attachment);

    // if we don't have a zotero key we might need to add a new item and need to know the parent of it first
    // because attachments are always a child of an existing parent item
    if(e->contains("zoteroKey")) {
        return;
    }

    QList<Nepomuk2::Resource> parents = attachment.property(NBIB::publishedAs()).toResourceList();

    foreach( const Nepomuk2::Resource &r, parents) {
        QList<Nepomuk2::Resource> sycList = r.property(SYNC::serverSyncData()).toResourceList();

        // only add the sync details from the right storage
        foreach(const Nepomuk2::Resource &parentSync, sycList) {
            if(parentSync.property(SYNC::provider()).toString() != QString("zotero")) { //TODO make this possible for others too
                continue;
            }
            else if(parentSync.property(SYNC::userId()).toString() != m_syncUserId) {
                continue;
            }
            else if(parentSync.property(SYNC::url()).toString() != m_syncUrl) {
                continue;
            }

            //now we have the right object, write down parent sync details

            QString updated = parentSync.property(SYNC::id()).toString();
            Value v3;
            v3.append(QSharedPointer<ValueItem>(new PlainText(updated)));
            e->insert(QLatin1String("zoteroParent"), v3);

            return; // stop here
        }
    }
}

void NepomukToBibTexPipe::setTitle(Entry *e, Nepomuk2::Resource publication, Nepomuk2::Resource reference)
{
    QString title;
    QString booktitle;

    // handle special case where "title=" is name of the chapter and "booktitle=" is the name of the book
    if(e->type() == QLatin1String("Incollection") || e->type() == QLatin1String("DictionaryEntry")) {
        booktitle = publication.property(NIE::title()).toString();

        Nepomuk2::Resource chapter = reference.property(NBIB::referencedPart()).toResource();
        title = chapter.property(NIE::title()).toString();
    }
    else {
        title = publication.property(NIE::title()).toString();

        if(publication.hasType(NBIB::Article())) {
            Nepomuk2::Resource collection = publication.property(NBIB::collection()).toResource();
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

void NepomukToBibTexPipe::setChapter(Entry *e, Nepomuk2::Resource reference)
{
    Nepomuk2::Resource chapter = reference.property(NBIB::referencedPart()).toResource();

    QString chapterNumber = chapter.property(NBIB::chapterNumber()).toString();
    QString chapterName = chapter.property(NIE::title()).toString();

    if( chapterNumber.isEmpty() ) {
        chapterNumber = chapterName;
    }

    if(!chapterNumber.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(chapterNumber)));
        e->insert(QLatin1String("chapter"), v);
    }
    if(!chapterName.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(chapterName)));
        e->insert(QLatin1String("chaptername"), v);
    }

    // now if the chapter has authors attached, add then as "author=" instead of the
    // authors from the publication. This is the case for InCollection
    // the publication authors will be added as "bookauthor=" later on

    QList<Nepomuk2::Resource> authors = chapter.property(NCO::creator()).toResourceList();

    Value v;
    if(!authors.isEmpty()) {
        foreach(const Nepomuk2::Resource & a, authors) {
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

void NepomukToBibTexPipe::setSeriesEditors(Entry *e, Nepomuk2::Resource publication)
{
    Nepomuk2::Resource series =  publication.property(NBIB::inSeries()).toResource();
    if(!series.isValid()) {
        Nepomuk2::Resource collection =  publication.property(NBIB::collection()).toResource();
        series =  collection.property(NBIB::inSeries()).toResource();
    }

    setContact(e, series, NBIB::editor(), QLatin1String("seriesEditor"));
}

void NepomukToBibTexPipe::setPublicationDate(Entry *e, Nepomuk2::Resource publication)
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

void NepomukToBibTexPipe::setPublisher(Entry *e, Nepomuk2::Resource publication)
{
    QString publisherEntry = QLatin1String("publisher");
    QList<Nepomuk2::Resource> publisher = publication.property(NCO::publisher()).toResourceList();

    if(publication.hasType(NBIB::Thesis())) {
        publisherEntry = QLatin1String("school");
    }
    else if(publication.hasType(NBIB::Techreport())) {
        publisherEntry = QLatin1String("institution");
    }

    QString names;
    QString address;
    if(!publisher.isEmpty()) {
        foreach(const Nepomuk2::Resource & p, publisher) {
            //TODO don't rely only on fullname of NCO::Contact
            names.append(p.property(NCO::fullname()).toString());
            names.append(QLatin1String(" and "));


            Nepomuk2::Resource existingAddr = p.property(NCO::hasPostalAddress()).toResource();
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

void NepomukToBibTexPipe::setOrganization(Entry *e, Nepomuk2::Resource publication)
{
    Nepomuk2::Resource org;
    if(publication.hasType(NBIB::Article())) {
        Nepomuk2::Resource collectionResource = publication.property(NBIB::collection()).toResource();
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

void NepomukToBibTexPipe::setUrl(Entry *e, Nepomuk2::Resource publication)
{
    QList<Nepomuk2::Resource> linksList = publication.property(NIE::links()).toResourceList();

    int i=0;
    foreach(const Nepomuk2::Resource &links, linksList) {
        QString url = links.property(NIE::url()).toString();
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(url)));

        QString key = QLatin1String("url");
        if(i!=0) {
            key.append( QLatin1String("-") + QString::number(i));
        }
        e->insert(key, v);
        i++;
    }

    QList<Nepomuk2::Resource> fileList = publication.property(NBIB::isPublicationOf()).toResourceList();

    int l=0;
    int r=0;
    foreach(const Nepomuk2::Resource &dataObjects, fileList) {

        QString url = dataObjects.property(NIE::url()).toString();
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(url)));

        QString key;

        if(dataObjects.hasType(NFO::RemoteDataObject()) ) {
            key = QLatin1String("remotefile");
            if(r!=0) {
                key.append( QLatin1String("-") + QString::number(r));
            }
            r++;
        }
        else {
            key = QLatin1String("localfile");
            if(l!=0) {
                key.append( QLatin1String("-") + QString::number(r));
            }
            l++;
        }

        e->insert(key, v);
    }
}

void NepomukToBibTexPipe::setSeries(Entry *e, Nepomuk2::Resource publication)
{
    Nepomuk2::Resource series = publication.property(NBIB::inSeries()).toResource();
    QString string = series.property(NIE::title()).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(string)));
        e->insert(Entry::ftSeries, v);
    }
}

void NepomukToBibTexPipe::setJournal(Entry *e, Nepomuk2::Resource publication)
{
    Nepomuk2::Resource journalIssue = publication.property(NBIB::collection()).toResource();

    if(!journalIssue.isValid()) {
        return; // no journal available for his resource
    }

    // if we have a JournalIssue, get the journal contact and number/volume
    QString journalNumber = journalIssue.property(NBIB::number()).toString(); //Issue number
    QString journalVolume = journalIssue.property(NBIB::volume()).toString();

    Nepomuk2::Resource journal = journalIssue.property(NBIB::inSeries()).toResource();
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

void NepomukToBibTexPipe::setISSN(Entry *e, Nepomuk2::Resource publication)
{
    Nepomuk2::Resource series = publication.property(NBIB::inSeries()).toResource();
    QString issnString = series.property(NBIB::issn()).toString();

    // handles article of a collection in a series which has the issn attached
    if(issnString.isEmpty()) {
        Nepomuk2::Resource collection = publication.property(NBIB::collection()).toResource();
        Nepomuk2::Resource series2 = collection.property(NBIB::inSeries()).toResource();
        issnString = series2.property(NBIB::issn()).toString();
    }

    if(!issnString.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(issnString)));
        e->insert(QLatin1String("issn"), v);
    }
}

void NepomukToBibTexPipe::setEvent(Entry *e, Nepomuk2::Resource publication)
{
    Nepomuk2::Resource event = publication.property(NBIB::event()).toResource();

    if(!event.isValid()) {
        Nepomuk2::Resource collection = publication.property(NBIB::collection()).toResource();
        event = collection.property(NBIB::event()).toResource();
    }

    QString string = event.property(NIE::title()).toString();
    if(string.isEmpty()) {
        string = event.property(NAO::prefLabel()).toString();
    }

    if(!string.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(string)));
        e->insert(QLatin1String("event"), v);
    }
}

void NepomukToBibTexPipe::setCode(Entry *e, Nepomuk2::Resource publication)
{
    Nepomuk2::Resource codeOfLaw = publication.property(NBIB::codeOfLaw()).toResource();

    setValue(e, codeOfLaw, NIE::title(), QLatin1String("code"));
}

void NepomukToBibTexPipe::setCodeNumber(Entry *e, Nepomuk2::Resource publication)
{
    Nepomuk2::Resource codeOfLaw = publication.property(NBIB::codeOfLaw()).toResource();

    setValue(e, codeOfLaw, NBIB::codeNumber(), QLatin1String("codenumber"));
}

void NepomukToBibTexPipe::setCodeVolume(Entry *e, Nepomuk2::Resource publication)
{
    Nepomuk2::Resource codeOfLaw = publication.property(NBIB::codeOfLaw()).toResource();

    setValue(e, codeOfLaw, NBIB::volume(), QLatin1String("codevolume"));
}

void NepomukToBibTexPipe::setReporter(Entry *e, Nepomuk2::Resource publication)
{
    Nepomuk2::Resource courtReporter = publication.property(NBIB::courtReporter()).toResource();

    setValue(e, courtReporter, NIE::title(), QLatin1String("reporter"));
}

void NepomukToBibTexPipe::setReporterVolume(Entry *e, Nepomuk2::Resource publication)
{
    Nepomuk2::Resource courtReporter = publication.property(NBIB::courtReporter()).toResource();

    setValue(e, courtReporter, NBIB::volume(), QLatin1String("reportervolume"));
}

void NepomukToBibTexPipe::setAssignee(Entry *e, Nepomuk2::Resource publication)
{
    Nepomuk2::Resource assignee = publication.property(NBIB::assignee()).toResource();

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

void NepomukToBibTexPipe::setNote(Entry *e, Nepomuk2::Resource publication)
{
    QList<Nepomuk2::Resource> resourceList = publication.property(NAO::isRelated()).toResourceList();

    int i=0;
    foreach(const Nepomuk2::Resource & r, resourceList) {
        if( !r.hasType( PIMO::Note() ) ) { continue; }

        if(i == 0) {
            setValue(e, r, NIE::plainTextContent(), QLatin1String("note"));
        }
        else {
            QString noteKey = QLatin1String("note-") + QString::number(i);
            setValue(e, r, NIE::plainTextContent(), noteKey);
        }
        i++;
    }
}

void NepomukToBibTexPipe::setKewords(Entry *e, Nepomuk2::Resource publication)
{
    QList<Nepomuk2::Resource> tags = publication.property(NAO::hasTopic()).toResourceList();

    Value v;
    foreach(const Nepomuk2::Resource & tag, tags) {
        Keyword *p = new Keyword(tag.genericLabel());
        v.append(QSharedPointer<ValueItem>(p));
    }

    if(!v.isEmpty()) {
        e->insert(Entry::ftKeywords, v);
    }
}

void NepomukToBibTexPipe::setSyncDetails(Entry *e, Nepomuk2::Resource publication)
{
    kDebug() << "search for sync details" << publication.genericLabel();
    QList<Nepomuk2::Resource> sycList = publication.property(SYNC::serverSyncData()).toResourceList();

    QUrl syncDataType;
    if(publication.hasType(PIMO::Note())) {
        syncDataType = SYNC::Note();
    }
    else if(publication.hasType(NBIB::Reference()) || publication.hasType(NBIB::Publication())) {
        syncDataType = SYNC::BibResource();
    }
    else{
        syncDataType = SYNC::Attachment();
    }

    // only add the sync details the the right storage
    foreach(const Nepomuk2::Resource &r, sycList) {
        if(r.property(SYNC::provider()).toString() != QString("zotero")) { //TODO make this possible for others too
            continue;
        }
        if(r.property(SYNC::userId()).toString() != m_syncUserId) {
            continue;
        }
        if(r.property(SYNC::url()).toString() != m_syncUrl) {
            continue;
        }
        // this step is necessary to find the right sync detail resource when we double type.
        // As we might have Attachment and bibresource on the same nepomukresource (double typed)
        if(r.property(SYNC::syncDataType()).toUrl() != syncDataType) {
            kDebug() << "wrong data type";
            continue;
        }


        kDebug() << "found right sync details" << publication.genericLabel();

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

void NepomukToBibTexPipe::setArticleType(Entry *e, Nepomuk2::Resource publication)
{
    if(publication.hasType(NBIB::Article())) {
        QString articleType;
        Nepomuk2::Resource collection = publication.property(NBIB::collection()).toResource();
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

void NepomukToBibTexPipe::setValue(Entry *e, Nepomuk2::Resource publication, QUrl property, const QString &bibkey)
{
    QString string = publication.property(property).toString();

    if(!string.isEmpty()) {
        Value v;
        v.append(QSharedPointer<ValueItem>(new PlainText(string)));
        e->insert(bibkey, v);
    }
}

void NepomukToBibTexPipe::setContact(Entry *e, Nepomuk2::Resource publication, QUrl property, const QString &bibkey)
{
    QList<Nepomuk2::Resource> authors = publication.property(property).toResourceList();

    Value v;
    if(!authors.isEmpty()) {
        foreach(const Nepomuk2::Resource & a, authors) {
            QString firstName = a.property(NCO::nameGiven()).toString();
            QString lastName = a.property(NCO::nameFamily()).toString();
            QString suffix = a.property(NCO::nameHonorificSuffix()).toString();

            if(firstName.isEmpty() || lastName.isEmpty()) {
                kDebug() << "could not split firstname / lastname component, try to find it automatically from the fullname";
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
