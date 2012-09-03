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

#include "nepomuktovariantpipe.h"

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

using namespace Nepomuk2::Vocabulary;
using namespace Soprano::Vocabulary;

NepomukToVariantPipe::NepomukToVariantPipe(QObject *parent)
    : QObject(parent)
    , m_strict(false)
    , m_addNepomukUris(false)
{
}

void NepomukToVariantPipe::pipeExport(QList<Nepomuk2::Resource> resources)
{
    m_bibtexList.clear();

    int citeKeyNumer = 1;
    QString citeRef = QLatin1String("BibTexExport");

    m_percentPerResource = (qreal)resources.size() / 100.0;

    int i = 0;
    foreach(const Nepomuk2::Resource & resource, resources) {

        m_curEntryMap.clear();

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
        m_curEntryMap.insert(QLatin1String("bibtexcitekey"), citeKey);
        m_curEntryMap.insert(QLatin1String("bibtexentrytype"), entryType.toLower());

        // special handling for notes
        if( entryType == QLatin1String("Note")) {
            collectNoteContent(publication);

            if(m_addNepomukUris) {
                m_curEntryMap.insert(QLatin1String("nepomuk-note-uri"), publication.uri().toString());
            }
        }
        // special handling for attachments
        else if( entryType == QLatin1String("Attachment")) {
            collectAttachmentContent(publication);

            if(m_addNepomukUris) {
                m_curEntryMap.insert(QLatin1String("nepomuk-attachment-uri"), publication.uri().toString());
            }
        }
        // all other references
        else {
            collectContent(reference, publication);

            if(m_addNepomukUris) {
                m_curEntryMap.insert(QLatin1String("nepomuk-publication-uri"), publication.uri().toString());
                if(reference.isValid()) {
                    m_curEntryMap.insert(QLatin1String("nepomuk-reference-uri"), reference.uri().toString());
                }
            }
        }

        m_bibtexList.append(m_curEntryMap);
    }
}

QVariantList NepomukToVariantPipe::variantList() const
{
    return m_bibtexList;
}

void NepomukToVariantPipe::useStrictTypes(bool strict)
{
    m_strict = strict;
}

QString NepomukToVariantPipe::retrieveEntryType(Nepomuk2::Resource reference, Nepomuk2::Resource publication)
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
    else if(publication.hasType(NBIB::Booklet())) {
        type = QLatin1String("Booklet");
    }
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
    else if(publication.hasType(NBIB::Report())) {
        type = QLatin1String("Report");
    }
    else if(publication.hasType(NBIB::Manual())) {
        type = QLatin1String("Manual");
    }
    else if(publication.hasType(NBIB::PhdThesis())) {
        type = QLatin1String("Phdthesis");
    }
    else if(publication.hasType(NBIB::MastersThesis())) {
        type = QLatin1String("Mastersthesis");
    }
    else if(publication.hasType(NBIB::BachelorThesis())) {
        type = QLatin1String("Bachelorthesis");
    }
    else if(publication.hasType(NBIB::Thesis())) {
        type = QLatin1String("Thesis");
    }
    else if(publication.hasType(NBIB::Unpublished())) {
        type = QLatin1String("Unpublished");
    }
    else if(publication.hasType(NBIB::Presentation())) {
        type = QLatin1String("Presentation");
    }
    else if(publication.hasType(NBIB::Bill())) {
        type = QLatin1String("Bill");
    }
    else if(publication.hasType(NBIB::Standard())) {
        type = QLatin1String("Standard");
    }
    else if(publication.hasType(NBIB::Patent())) {
        type = QLatin1String("Patent");
    }
    else if(publication.hasType(NBIB::Statute())) {
        type = QLatin1String("Statute");
    }
    else if(publication.hasType(NBIB::Script())) {
        type = QLatin1String("Script");
    }
    else if(publication.hasType(NBIB::Map())) {
        type = QLatin1String("Map");
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
        //REFACTOR: Add function for type detection t oglobal static class
        type = QLatin1String("Misc");
    }

    // if we have strict export, transforn into standard types
    if(m_strict) {
        kDebug() << "strict export is not implemented yet";
    }

    return type;
}

void NepomukToVariantPipe::collectContent(Nepomuk2::Resource reference, Nepomuk2::Resource publication)
{
    setTitle(publication, reference);
    setChapter(reference);

    // if the chapter has an author attached to it , don't search author of the publication
    // solves the incollection special case where the author of the chapter is not the one from the whole book
    if(!m_curEntryMap.contains(QLatin1String("author"))){
        setContact(publication, NCO::creator(), QLatin1String("author"));
    }
    else {
        setContact(publication, NCO::creator(), QLatin1String("bookauthor"));
    }

    setContact(publication, NBIB::editor(), QLatin1String("editor"));
    setContact(publication, NBIB::translator(), QLatin1String("translator"));
    setContact(publication, NBIB::contributor(), QLatin1String("contributor"));
    setContact(publication, NBIB::reviewedAuthor(), QLatin1String("reviewedauthor"));
    setContact(publication, NBIB::commenter(), QLatin1String("commenter"));
    setContact(publication, NBIB::attorneyAgent(), QLatin1String("attorneyagent"));
    setContact(publication, NBIB::counsel(), QLatin1String("counsel"));
    setContact(publication, NBIB::coSponsor(), QLatin1String("cosponsor"));

    setSeriesEditors(publication);
    setPublisher(publication);
    setOrganization(publication);

    setPublicationDate(publication);
    setUrl(publication);
    setSeries(publication);
    setISSN(publication);
    setJournal(publication);
    setKewords(publication);
    setEvent(publication);
    setCode(publication);
    setCodeNumber(publication);
    setCodeVolume(publication);
    setReporter(publication);
    setReporterVolume(publication);
    setArticleType(publication);
    setAssignee(publication);
    setNote(publication);

    setValue(publication, NBIB::edition(), QLatin1String("edition"));
    setValue(publication, NBIB::volume(), QLatin1String("volume"));
    setValue(publication, NBIB::number(), QLatin1String("number"));
    setValue(publication, NIE::copyright(), QLatin1String("copyright"));
    setValue(publication, NBIB::publicationMethod(), QLatin1String("howpublished"));
    setValue(publication, NBIB::publicationType(), QLatin1String("type"));
    setValue(publication, NUAO::lastUsage(), QLatin1String("accessdate"));
    setValue(publication, NBIB::publicationDate(), QLatin1String("date"));
    setValue(publication, NBIB::eprint(), QLatin1String("eprint"));
    setValue(publication, NBIB::isbn(), QLatin1String("isbn"));
    setValue(publication, NBIB::lccn(), QLatin1String("lccn"));
    setValue(publication, NBIB::mrNumber(), QLatin1String("mrnumber"));
    setValue(publication, NBIB::pubMed(), QLatin1String("pubmed"));
    setValue(publication, NBIB::doi(), QLatin1String("doi"));
    setValue(publication, NBIB::abstract(), QLatin1String("abstract"));
    setValue(publication, NIE::language(), QLatin1String("language"));
    setValue(reference, NBIB::pages(), QLatin1String("pages"));
    setValue(publication, NBIB::priorityNumbers(), QLatin1String("prioritynumbers"));
    setValue(publication, NBIB::applicationNumber(), QLatin1String("applicationnumber"));
    setValue(publication, NBIB::patentReferences(), QLatin1String("references"));
    setValue(publication, NBIB::legalStatus(), QLatin1String("legalstatus"));
    setValue(publication, NBIB::filingDate(), QLatin1String("filingdate"));
    setValue(publication, NBIB::archive(), QLatin1String("archive"));
    setValue(publication, NBIB::archiveLocation(), QLatin1String("archiveLocation"));
    setValue(publication, NBIB::libraryCatalog(), QLatin1String("libraryCatalog"));
    setValue(publication, NBIB::shortTitle(), QLatin1String("shorttitle"));
    setValue(publication, NBIB::numberOfPages(), QLatin1String("numpages"));
    setValue(publication, NBIB::numberOfVolumes(), QLatin1String("numberofvolumes"));
    setValue(publication, NBIB::mapScale(), QLatin1String("scale"));
    setValue(publication, NBIB::history(), QLatin1String("history"));

    // Zotero additions
    setSyncDetails(publication);
}

void NepomukToVariantPipe::collectNoteContent(Nepomuk2::Resource note)
{
    setValue(note, NIE::plainTextContent(), QLatin1String("note"));
    setKewords(note);

    // Zotero additions
    setSyncDetails(note);

    // if we don't have a zotero key we might need to add a new item and need to know the parent of it first
    if(m_curEntryMap.contains("zoteroKey")) {
        return;
    }

    QList<Nepomuk2::Resource> parents = note.property(NAO::isRelated()).toResourceList();

    foreach( const Nepomuk2::Resource &r, parents) {
        QList<Nepomuk2::Resource> sycList = r.property(SYNC::serverSyncData()).toResourceList();

        // only add the sync details from the right storage
        foreach(const Nepomuk2::Resource &parentSync, sycList) {
            if(parentSync.property(SYNC::provider()).toString() != QString("zotero")) { //TODO: make syncobject possible for others too
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
            m_curEntryMap.insert(QLatin1String("zoteroParent"), updated);

            return; // stop here
        }
    }
}

void NepomukToVariantPipe::collectAttachmentContent(Nepomuk2::Resource attachment)
{
    setValue(attachment, NIE::title(), QLatin1String("title"));
    if(!m_curEntryMap.contains( QLatin1String("title") )) {
        setValue(attachment, NIE::url(), QLatin1String("title"));
    }

    setValue(attachment, NIE::url(), QLatin1String("url"));
    setValue(attachment, NUAO::lastUsage(), QLatin1String("accessDate"));

    QString linkMode;
    if(attachment.hasType(NFO::RemoteDataObject())) {
        linkMode = '1';
    }
    else {
        linkMode = '0';
    }

    m_curEntryMap.insert(QLatin1String("linkMode"), linkMode);

    setValue(attachment, NIE::comment(), QLatin1String("note"));

    setKewords(attachment);

    // Zotero additions
    setSyncDetails(attachment);

    // if we don't have a zotero key we might need to add a new item and need to know the parent of it first
    // because attachments are always a child of an existing parent item
    if(m_curEntryMap.contains("zoteroKey")) {
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
            m_curEntryMap.insert(QLatin1String("zoteroParent"), updated);

            return; // stop here
        }
    }
}

void NepomukToVariantPipe::setTitle(Nepomuk2::Resource publication, Nepomuk2::Resource reference)
{
    QString title;
    QString booktitle;

    QString curType = m_curEntryMap.value(QLatin1String("bibtexentrytype")).toString().toLower();

    // handle special case where "title=" is name of the chapter and "booktitle=" is the name of the book
    if(curType == QLatin1String("incollection") || curType == QLatin1String("dictionaryentry")) {
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
        m_curEntryMap.insert(QLatin1String("title"), title);
    }

    if(!booktitle.isEmpty()) {
        m_curEntryMap.insert(QLatin1String("booktitle"), booktitle);
    }
}

void NepomukToVariantPipe::setChapter(Nepomuk2::Resource reference)
{
    Nepomuk2::Resource chapter = reference.property(NBIB::referencedPart()).toResource();

    QString chapterNumber = chapter.property(NBIB::chapterNumber()).toString();
    QString chapterName = chapter.property(NIE::title()).toString();

    if( chapterNumber.isEmpty() ) {
        chapterNumber = chapterName;
    }

    if(!chapterNumber.isEmpty()) {
        m_curEntryMap.insert(QLatin1String("chapter"), chapterNumber);
    }
    if(!chapterName.isEmpty()) {
        m_curEntryMap.insert(QLatin1String("chaptername"), chapterName);
    }

    // now if the chapter has authors attached, add then as "author=" instead of the
    // authors from the publication. This is the case for InCollection
    // the publication authors will be added as "bookauthor=" later on

    QList<Nepomuk2::Resource> authors = chapter.property(NCO::creator()).toResourceList();

    if(!authors.isEmpty()) {
        QString personList;
        foreach(const Nepomuk2::Resource & a, authors) {
            // all we ever save is the fullname
            personList.append(a.property(NCO::fullname()).toString() );
            personList.append(QLatin1String("; "));
        }

        personList.chop(2); // remove last "; "

        if(!personList.isEmpty()) {
            m_curEntryMap.insert(QLatin1String("author"), personList);
        }
    }
}

void NepomukToVariantPipe::setSeriesEditors(Nepomuk2::Resource publication)
{
    Nepomuk2::Resource series =  publication.property(NBIB::inSeries()).toResource();
    if(!series.isValid()) {
        Nepomuk2::Resource collection =  publication.property(NBIB::collection()).toResource();
        series =  collection.property(NBIB::inSeries()).toResource();
    }

    setContact(series, NBIB::editor(), QLatin1String("serieseditor"));
}

void NepomukToVariantPipe::setPublicationDate(Nepomuk2::Resource publication)
{
    QString pdString = publication.property(NBIB::publicationDate()).toString();

    QRegExp rx(QLatin1String("(\\d*)-(\\d*)-(\\d*)*"));
    QString year;
    QString month;
    if (rx.indexIn(pdString) != -1) {
        year = rx.cap(1);
        month = rx.cap(2);
    }
    else {
        return;
    }

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
            m_curEntryMap.insert(QLatin1String("month"), monthName);
        }
    }

    if(!year.isEmpty()) {
        m_curEntryMap.insert(QLatin1String("year"), year);
    }
}

void NepomukToVariantPipe::setPublisher(Nepomuk2::Resource publication)
{
    QString publisherEntry = QLatin1String("publisher");
    QList<Nepomuk2::Resource> publisher = publication.property(NCO::publisher()).toResourceList();

    //BUG: not all types are fetched. Fixed in 4.9.1
    publication.types();

    if(publication.hasType(NBIB::Thesis())) {
        publisherEntry = QLatin1String("school");
    }
    else if(publication.hasType(NBIB::Report())) {
        publisherEntry = QLatin1String("institution");
    }

    if(!publisher.isEmpty()) {
        QString names;
        QString address;

        foreach(const Nepomuk2::Resource & p, publisher) {
            names.append(p.property(NCO::fullname()).toString());
            names.append(QLatin1String("; "));

            Nepomuk2::Resource existingAddr = p.property(NCO::hasPostalAddress()).toResource();
            if(existingAddr.isValid())
                address.append(existingAddr.property(NCO::extendedAddress()).toString());
        }

        names.chop(2); // remove last "; "

        if(!names.isEmpty()) {
            m_curEntryMap.insert(publisherEntry, names);
        }
        if(!address.isEmpty()) {
            m_curEntryMap.insert(QLatin1String("address"), address);
        }
    }
}

void NepomukToVariantPipe::setOrganization(Nepomuk2::Resource publication)
{
    Nepomuk2::Resource org;
    org = publication.property(NBIB::organization()).toResource();

    if(!org.isValid() || !org.exists()) {
        Nepomuk2::Resource collectionResource = publication.property(NBIB::collection()).toResource();
        org = collectionResource.property(NBIB::organization()).toResource();
    }

    QString string = org.property(NCO::fullname()).toString();

    if(!string.isEmpty()) {
        m_curEntryMap.insert(QLatin1String("organization"), string);
    }
}

void NepomukToVariantPipe::setUrl(Nepomuk2::Resource publication)
{
    //TODO: Better url handling for Nepomuk to variant
    QList<Nepomuk2::Resource> linksList = publication.property(NIE::links()).toResourceList();

    int i=0;
    foreach(const Nepomuk2::Resource &links, linksList) {
        QString url = links.property(NIE::url()).toString();

        QString key = QLatin1String("url");
        if(i!=0) {
            key.append( QLatin1String("-") + QString::number(i));
        }
        m_curEntryMap.insert(key, url);
        i++;
    }

    QList<Nepomuk2::Resource> fileList = publication.property(NBIB::isPublicationOf()).toResourceList();

    int l=0;
    int r=0;
    foreach(const Nepomuk2::Resource &dataObjects, fileList) {

        QString url = dataObjects.property(NIE::url()).toString();

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

        m_curEntryMap.insert(key, url);
    }
}

void NepomukToVariantPipe::setSeries(Nepomuk2::Resource publication)
{
    Nepomuk2::Resource series = publication.property(NBIB::inSeries()).toResource();
    QString string = series.property(NIE::title()).toString();

    if(!string.isEmpty()) {
        m_curEntryMap.insert(QLatin1String("series"), string);
    }
}

void NepomukToVariantPipe::setJournal(Nepomuk2::Resource publication)
{
    Nepomuk2::Resource journalIssue = publication.property(NBIB::collection()).toResource();

    if(!journalIssue.isValid()) {
        return; // no journal available for his resource
    }

    // if we have a JournalIssue, get the journal contact and number/volume
    QString journalNumber = journalIssue.property(NBIB::number()).toString(); //Issue number
    QString journalVolume = journalIssue.property(NBIB::volume()).toString();

    Nepomuk2::Resource journal = journalIssue.property(NBIB::inSeries()).toResource();
    QString journalName = journal.property(NIE::title()).toString();

    // in case if encyclopedia etc, where the series makes no sense, and the actual title is in the collection
    if(journalName.isEmpty()) {
        journalName = journalIssue.property(NIE::title()).toString();
    }

    if(!journalNumber.isEmpty()) {
        m_curEntryMap.insert(QLatin1String("number"), journalNumber);
    }
    if(!journalVolume.isEmpty()) {
        m_curEntryMap.insert(QLatin1String("volume"), journalVolume);
    }
    if(!journalName.isEmpty()) {
        m_curEntryMap.insert(QLatin1String("journal"), journalName);
    }
}

void NepomukToVariantPipe::setISSN(Nepomuk2::Resource publication)
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
        m_curEntryMap.insert(QLatin1String("issn"), issnString);
    }
}

void NepomukToVariantPipe::setEvent( Nepomuk2::Resource publication)
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
        m_curEntryMap.insert(QLatin1String("event"), string);
    }
}

void NepomukToVariantPipe::setCode(Nepomuk2::Resource publication)
{
    Nepomuk2::Resource codeOfLaw = publication.property(NBIB::codeOfLaw()).toResource();

    setValue(codeOfLaw, NIE::title(), QLatin1String("code"));
}

void NepomukToVariantPipe::setCodeNumber(Nepomuk2::Resource publication)
{
    Nepomuk2::Resource codeOfLaw = publication.property(NBIB::codeOfLaw()).toResource();

    setValue(codeOfLaw, NBIB::codeNumber(), QLatin1String("codenumber"));
}

void NepomukToVariantPipe::setCodeVolume(Nepomuk2::Resource publication)
{
    Nepomuk2::Resource codeOfLaw = publication.property(NBIB::codeOfLaw()).toResource();

    setValue(codeOfLaw, NBIB::volume(), QLatin1String("codevolume"));
}

void NepomukToVariantPipe::setReporter(Nepomuk2::Resource publication)
{
    Nepomuk2::Resource courtReporter = publication.property(NBIB::courtReporter()).toResource();

    setValue(courtReporter, NIE::title(), QLatin1String("reporter"));
}

void NepomukToVariantPipe::setReporterVolume(Nepomuk2::Resource publication)
{
    Nepomuk2::Resource courtReporter = publication.property(NBIB::courtReporter()).toResource();

    setValue(courtReporter, NBIB::volume(), QLatin1String("reportervolume"));
}

void NepomukToVariantPipe::setAssignee(Nepomuk2::Resource publication)
{
    Nepomuk2::Resource assignee = publication.property(NBIB::assignee()).toResource();

    QString name = assignee.property(NCO::fullname()).toString();

    if(!name.isEmpty()) {
        m_curEntryMap.insert(QLatin1String("assignee"), name);
    }
}

void NepomukToVariantPipe::setNote(Nepomuk2::Resource publication)
{
    QList<Nepomuk2::Resource> resourceList = publication.property(NAO::isRelated()).toResourceList();

    int i=0;
    foreach(const Nepomuk2::Resource & r, resourceList) {
        //BUG: not all types are fetched correctly fixed in 4.9.1
        r.types();
        if( !r.hasType( PIMO::Note() ) ) { continue; }

        if(i == 0) {
            setValue(r, NIE::plainTextContent(), QLatin1String("note"));
        }
        else {
            QString noteKey = QLatin1String("note-") + QString::number(i);
            setValue(r, NIE::plainTextContent(), noteKey);
        }
        i++;
    }
}

void NepomukToVariantPipe::setKewords( Nepomuk2::Resource publication)
{
    QList<Nepomuk2::Resource> tags = publication.property(NAO::hasTopic()).toResourceList();

    QString keywords;
    foreach(const Nepomuk2::Resource & tag, tags) {
        keywords.append( tag.genericLabel() );
        keywords.append( QLatin1String("; ") );
    }

    keywords.chop(2); //remove last "; "

    if(!keywords.isEmpty()) {
        m_curEntryMap.insert(QLatin1String("keywords"), keywords);
    }
}

void NepomukToVariantPipe::setSyncDetails(Nepomuk2::Resource publication)
{
    //FIXME: reenable syncdetail search
    return;
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
        if(r.property(SYNC::provider()).toString() != QString("zotero")) { //TODO: make this possible for others too
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
        m_curEntryMap.insert(QLatin1String("zoteroEtag"), etag);

        QString key = r.property(SYNC::id()).toString();
        m_curEntryMap.insert(QLatin1String("zoteroKey"), key);

        QString updated = r.property(NUAO::lastModification()).toString();
        m_curEntryMap.insert(QLatin1String("zoteroUpdated"), updated);

        break;
    }
}

void NepomukToVariantPipe::setSyncDetails(const QString &url, const QString &userid)
{
    m_syncUrl = url;
    m_syncUserId = userid;
}

void NepomukToVariantPipe::addNepomukUries(bool addThem)
{
    m_addNepomukUris = addThem;
}

void NepomukToVariantPipe::setArticleType(Nepomuk2::Resource publication)
{
    //BUG: not all types are fetched correctly. Fixed in 4.9.1
    publication.types();

    if(publication.hasType(NBIB::Article())) {
        QString articleType;
        Nepomuk2::Resource collection = publication.property(NBIB::collection()).toResource();

        //BUG: not all types are fetched correctly. Fixed in 4.9.1
        collection.types();

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
        else if(collection.hasType(NBIB::Website())) {
            articleType = QLatin1String("webpage");
        }

        if(!articleType.isEmpty()) {
            m_curEntryMap.insert(QLatin1String("articletype"), articleType);
        }
    }
}

void NepomukToVariantPipe::setValue(Nepomuk2::Resource publication, QUrl property, const QString &bibkey)
{
    QString string = publication.property(property).toString();

    if(!string.isEmpty()) {
        m_curEntryMap.insert( bibkey, string);
    }
}

void NepomukToVariantPipe::setContact(Nepomuk2::Resource publication, QUrl property, const QString &bibkey)
{
    QList<Nepomuk2::Resource> authors = publication.property(property).toResourceList();

    if(!authors.isEmpty()) {
        QString personList;
        foreach(const Nepomuk2::Resource & a, authors) {
            personList.append( a.property(NCO::fullname()).toString() );
            personList.append( QLatin1String("; "));
        }

        personList.chop(2); // remove last "; "

        if(!personList.isEmpty()) {
            m_curEntryMap.insert( bibkey, personList);
        }
    }
}
