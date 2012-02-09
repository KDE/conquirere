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

#include "nepomuktobibtexpipe.h"

#include <kbibtex/entry.h>
#include <kbibtex/macro.h>

#include "dms-copy/simpleresource.h"
#include "dms-copy/datamanagement.h"
#include "dms-copy/storeresourcesjob.h"
#include <KDE/KJob>

#include "sro/nbib/series.h"
#include "sro/nbib/collection.h"
#include "sro/nbib/documentpart.h"
#include "sro/nbib/chapter.h"
#include "sro/nbib/article.h"
#include "sro/nbib/proceedings.h"
#include "sro/nbib/codeoflaw.h"
#include "sro/nbib/legislation.h"
#include "sro/nbib/courtreporter.h"
#include "sro/nbib/legalcasedocument.h"

#include "sro/sync/serversyncdata.h"
#include "sro/pimo/note.h"
#include "sro/pimo/topic.h"
#include "sro/pimo/event.h"
#include "sro/nie/informationelement.h"
#include "sro/nao/tag.h"
#include "sro/nfo/filedataobject.h"
#include "sro/nfo/website.h"
#include "sro/nco/postaladdress.h"
#include "sro/nco/personcontact.h"
#include "sro/nco/contact.h"
#include "sro/nco/organizationcontact.h"
#include "sro/aneo/akonadidataobject.h"

#include <Soprano/Vocabulary/RDF>

#include "nbib.h"
#include "sync.h"
#include <Nepomuk/Vocabulary/PIMO>
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Vocabulary/NCO>
#include <Nepomuk/Vocabulary/NFO>
#include <Nepomuk/Vocabulary/NCAL>
#include <Nepomuk/Vocabulary/NUAO>
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk/Variant>

#include <Nepomuk/Query/QueryServiceClient>
#include <Nepomuk/Query/Result>

#include <Akonadi/Item>
#include <KABC/Addressee>
#include <Akonadi/ItemFetchJob>
#include <Akonadi/ItemCreateJob>
#include <Akonadi/CollectionCreateJob>
#include <Akonadi/CollectionFetchJob>
#include <Akonadi/CollectionFetchScope>
#include <KDE/KDebug>

#include <QtCore/QSharedPointer>
#include <QtGui/QTextDocument>
#include <QtCore/QUuid>

using namespace Nepomuk::Vocabulary;
using namespace Soprano::Vocabulary;

BibTexToNepomukPipe::BibTexToNepomukPipe()
: m_replaceMode(false)
{
}

BibTexToNepomukPipe::~BibTexToNepomukPipe()
{

}

void BibTexToNepomukPipe::pipeExport(File & bibEntries)
{
    emit progress(0);

    //create the collection used for importing

    int maxValue = bibEntries.size();
    qreal perFileProgress = (100.0/(qreal)maxValue);
    qreal currentprogress = 0.0;

    // we start by filling the lookuptable for all macros
    // in BibTeX files macros are used to create abbreviations for some fields that can be used all over again
    foreach(QSharedPointer<Element> e, bibEntries ) {
        Macro *macro = dynamic_cast<Macro *>(e.data());
        if(macro) {
            m_macroLookup.insert(macro->key(), PlainTextValue::text(macro->value()));
        }
    }

    // now do the actual import
    foreach(QSharedPointer<Element> e, bibEntries ) {
        Entry *entry = dynamic_cast<Entry *>(e.data());

        if(!entry) { continue; }

        Nepomuk::SimpleResourceGraph graph;

        // the crossref part means we fetch bibtex entries from other bibtex entries
        // so the title is only added to one and related to all others via the crossref
        if(entry->contains(Entry::ftCrossRef)) {
            entry = Entry::resolveCrossref(*entry, &bibEntries);
        }
        // thats a special case. Zotero allows to store bibtex entries of type note
        // this is not the usual publication/reference but will be created as pimo:Note instead
        if( entry->type() == QLatin1String("note") ) {
            importNote(entry, graph);
        }
        else if( entry->type() == QLatin1String("attachment") ) {
            importAttachment(entry, graph);
        }
        else {
            importBibResource(entry, graph);
        }

        Nepomuk::StoreResourcesJob *srj = Nepomuk::storeResources(graph,Nepomuk::IdentifyNew, Nepomuk::OverwriteProperties);
        connect(srj, SIGNAL(result(KJob*)), this, SLOT(slotSaveToNepomukDone(KJob*)));
        srj->exec();

        currentprogress += perFileProgress;

        emit progress(currentprogress);
    }

    emit progress(100);
}

void BibTexToNepomukPipe::setAkonadiAddressbook(Akonadi::Collection & addressbook)
{
    if(addressbook.isValid()) {
        m_addressbook = addressbook;
    }
}

void BibTexToNepomukPipe::setSyncDetails(const QString &url, const QString &userid)
{
    m_syncUrl = url;
    m_syncUserId = userid;
}

void BibTexToNepomukPipe::setProjectPimoThing(Nepomuk::Thing projectThing)
{
    kDebug() << "import bibtex into project thing ::" << projectThing.genericLabel();
    m_projectThing = projectThing;
}

void BibTexToNepomukPipe::importBibResource(Entry *entry, Nepomuk::SimpleResourceGraph &graph)
{
    Nepomuk::NBIB::Publication publication;
    addPublicationSubTypes(publication, entry);

    Nepomuk::NBIB::Reference reference;
    reference.setCiteKey( entry->id() );
    reference.addProperty( NAO::prefLabel(), entry->id()); // adds no real value, but looks nicer in the Nepomuk shell

    reference.setProperty( NBIB::publication(), publication);
    publication.setProperty( NBIB::reference(), reference);

    publication.addProperty(NAO::hasSubResource(), reference.uri() ); // remove reference when publication is deleted

    // add zotero sync details
    if(entry->contains(QLatin1String("zoterokey"))) {
        addZoteroSyncDetails(publication,reference,entry, graph);
    }

    //before we go through the whole list one by one, we take care of some special cases
    handleSpecialCases(entry,graph,publication, reference);

    //now go through the list of all remaining entries
    QMapIterator<QString, Value> i(*entry);
    while (i.hasNext()) {
        i.next();
        addContent(i.key().toLower(), i.value(), publication, reference, graph, entry->type(), entry->id());
    }

    if(m_projectThing.isValid()) {
        publication.addProperty( NAO::isRelated(), m_projectThing.uri());
        reference.addProperty( NAO::isRelated(), m_projectThing.uri());
    }

    graph << publication << reference;
}

void BibTexToNepomukPipe::importNote(Entry *e, Nepomuk::SimpleResourceGraph &graph)
{
    kDebug() << "import note " << PlainTextValue::text(e->value(QLatin1String("zoterotitle")));
    Nepomuk::PIMO::Note note;
    note.addType(NIE::InformationElement());

    note.setProperty( NAO::prefLabel(), PlainTextValue::text(e->value(QLatin1String("zoterotitle"))) );
    note.setProperty( NIE::title(), PlainTextValue::text(e->value(QLatin1String("zoterotitle"))) );

    QTextDocument content;
    content.setHtml( PlainTextValue::text(e->value(QLatin1String("note"))).simplified() );
    note.setProperty( NIE::plainTextContent(), content.toPlainText());
    note.setProperty( NIE::htmlContent(), content.toHtml());

    Value keywords = e->value("keywords");
    if(!keywords.isEmpty())
        addTopic(keywords, note, graph);

    Nepomuk::SimpleResource empty;
    if(e->contains(QLatin1String("zoterokey"))) {
        addZoteroSyncDetails(note, empty, e, graph);
    }

    graph << note;
}

void BibTexToNepomukPipe::slotSaveToNepomukDone(KJob *job)
{
    if(job->error()) {
        kDebug() << "Failed to store information in Nepomuk. " << job->errorString();
    }
}

void BibTexToNepomukPipe::importAttachment(Entry *e, Nepomuk::SimpleResourceGraph &graph)
{
    Nepomuk::NFO::FileDataObject attachment;
    attachment.addType(NFO::Document());

    Value title = e->value("title");
    addValue(PlainTextValue::text(title), attachment, NIE::title() );

    if(e->contains(QLatin1String("zoteroAttachmentFile"))) {

        Value url = e->value("localFile");
        QString localFilePath = PlainTextValue::text(url);
        localFilePath.prepend(QLatin1String("file://"));
        addValue(localFilePath, attachment, NIE::url());
        addValue(localFilePath, attachment, NAO::identifier());

        kDebug() << "create attachment with url" << localFilePath;
    }
    else {
        attachment.addType(NFO::RemoteDataObject());
        Value url = e->value("url");
        addValue(PlainTextValue::text(url), attachment, NAO::identifier());
        addValue(PlainTextValue::text(url), attachment, NIE::url());
        kDebug() << "create attachment with url" << PlainTextValue::text(url);
    }

    Value keywords = e->value("keywords");
    if(!keywords.isEmpty()) {
        addTag(keywords, attachment, graph);
    }

    QString accessdate = PlainTextValue::text(e->value("accessdate"));
    QDateTime dateTime = QDateTime::fromString(accessdate, "yyyy-MM-ddTHH:mm:ss");
    if(!dateTime.isValid()) { dateTime = QDateTime::fromString(accessdate, "yyyy-MM-dd"); }
    if(!dateTime.isValid()) { dateTime = QDateTime::fromString(accessdate, "dd-MM-yyy"); }
    if(!dateTime.isValid()) { dateTime = QDateTime::fromString(accessdate, "yyyy-MM"); }
    if(!dateTime.isValid()) { dateTime = QDateTime::fromString(accessdate, "MM-yyyy"); }
    if(!dateTime.isValid()) { dateTime = QDateTime::fromString(accessdate, "yyyy.MM.dd"); }
    if(!dateTime.isValid()) { dateTime = QDateTime::fromString(accessdate, "dd.MM.yyy"); }
    if(!dateTime.isValid()) { dateTime = QDateTime::fromString(accessdate, "MM.yyy"); }
    if(!dateTime.isValid()) { dateTime = QDateTime::fromString(accessdate, "yyyy.MM"); }

    if(dateTime.isValid()) {
        QString newDate = dateTime.toString("yyyy-MM-ddTHH:mm:ss");

        addValue(newDate, attachment, NUAO::lastUsage());
    }
    else {
        kDebug() << "could not parse accessdate" << accessdate;
    }

    Value note = e->value("note");
    addValue(PlainTextValue::text(note), attachment, NIE::comment());

    graph << attachment;

    Nepomuk::SimpleResource empty;
    if(e->contains(QLatin1String("zoterokey"))) {
        addZoteroSyncDetails(attachment, empty, e, graph);
    }
}

void BibTexToNepomukPipe::addPublicationSubTypes(Nepomuk::NBIB::Publication &publication, Entry *entry)
{
    QString entryType = entry->type().toLower();

    //################################################################
    //# Article and subtypes
    //################################################################
    if(entryType == QLatin1String("article") || entryType == QLatin1String("inproceedings") ||
       entryType == QLatin1String("conference") || entryType == QLatin1String("encyclopediaarticle")) {
        publication.addType(NBIB::Article());
    }
    else if(entryType == QLatin1String("blogpost")) {
        publication.addType(NBIB::Article());
        publication.addType(NBIB::BlogPost());
    }
    else if(entryType == QLatin1String("forumpost")) {
        publication.addType(NBIB::Article());
        publication.addType(NBIB::ForumPost());
    }
    else if(entryType == QLatin1String("webpage")) {
        publication.addType(NBIB::Article());
        publication.addType(NBIB::Webpage());
    }

    //################################################################
    //# Book and subtypes
    //################################################################
    else if(entryType == QLatin1String("book") || entryType == QLatin1String("inbook") || entryType == QLatin1String("incollection")) {
        publication.addType(NBIB::Book());
    }
    else if(entryType == QLatin1String("dictionaryentry") || entryType == QLatin1String("dictionary")) {
        publication.addType(NBIB::Book());
        publication.addType(NBIB::Dictionary());
    }
    else if(entryType == QLatin1String("booklet")) {
        publication.addType(NBIB::Booklet());
    }

    //################################################################
    //# Collection and subtypes
    //################################################################
    else if(entryType == QLatin1String("collection")) {
        publication.addType(NBIB::Collection());
    }
    else if(entryType == QLatin1String("proceedings")) {
        publication.addType(NBIB::Collection());
        publication.addType(NBIB::Proceedings());
    }
    else if(entryType == QLatin1String("forum")) {
        publication.addType(NBIB::Collection());
        publication.addType(NBIB::Forum());
    }
    else if(entryType == QLatin1String("blog")) {
        publication.addType(NBIB::Collection());
        publication.addType(NBIB::Blog());
    }
    else if(entryType == QLatin1String("website")) {
        publication.addType(NBIB::Collection());
        publication.addType(NBIB::Website());
    }

    //################################################################
    //# Thesis and subtypes
    //################################################################
    else if(entryType == QLatin1String("thesis")) {
        publication.addType(NBIB::Thesis());
    }
    else if(entryType == QLatin1String("bachelorthesis")) {
        publication.addType(NBIB::Thesis());
        publication.addType(NBIB::BachelorThesis());
    }
    else if(entryType == QLatin1String("mastersthesis")) {
        publication.addType(NBIB::Thesis());
        publication.addType(NBIB::MastersThesis());
    }
    else if(entryType == QLatin1String("phdthesis")) {
        publication.addType(NBIB::Thesis());
        publication.addType(NBIB::PhdThesis());
    }

    //################################################################
    //# Report and subtypes
    //################################################################
    else if(entryType == QLatin1String("report")) {
        publication.addType(NBIB::Report());
    }
    else if(entryType == QLatin1String("techreport")) {
        publication.addType(NBIB::Report());
        publication.addType(NBIB::Techreport());
    }

    //################################################################
    //# LegalDocument and subtypes
    //################################################################
    else if(entryType == QLatin1String("legaldocument")) {
        publication.addType(NBIB::LegalDocument());
    }
    else if(entryType == QLatin1String("legislation")) {
        publication.addType(NBIB::LegalDocument());
        publication.addType(NBIB::Legislation());
    }
    else if(entryType == QLatin1String("statute")) {
        publication.addType(NBIB::LegalDocument());
        publication.addType(NBIB::Legislation());
        publication.addType(NBIB::Statute());
    }
    else if(entryType == QLatin1String("bill")) {
        publication.addType(NBIB::LegalDocument());
        publication.addType(NBIB::Legislation());
        publication.addType(NBIB::Bill());
    }
    else if(entryType == QLatin1String("case")) {
        publication.addType(NBIB::LegalDocument());
        publication.addType(NBIB::LegalCaseDocument());
    }
    else if(entryType == QLatin1String("decision")) {
        publication.addType(NBIB::LegalDocument());
        publication.addType(NBIB::LegalCaseDocument());
        publication.addType(NBIB::Decision());
    }
    else if(entryType == QLatin1String("brief")) {
        publication.addType(NBIB::LegalDocument());
        publication.addType(NBIB::LegalCaseDocument());
        publication.addType(NBIB::Brief());
    }

    //################################################################
    //# All types without further subtypes
    //################################################################
    else if(entryType == QLatin1String("electronic")) {
        publication.addType(NBIB::Electronic());
    }
    else if(entryType == QLatin1String("manual")) {
        publication.addType(NBIB::Manual());
    }
    else if(entryType == QLatin1String("presentation")) {
        publication.addType(NBIB::Presentation());
    }
    else if(entryType == QLatin1String("script") || entryType == QLatin1String("manuscript")) {
        publication.addType(NBIB::Script());
    }
    else if(entryType == QLatin1String("unpublished")) {
        publication.addType(NBIB::Unpublished());
    }
    else if(entryType == QLatin1String("patent")) {
        publication.addType(NBIB::Patent());
    }
    else if(entryType == QLatin1String("standard")) {
        publication.addType(NBIB::Standard());
    }
    else if(entryType == QLatin1String("map")) {
        publication.addType(NBIB::Map());
    }
    else if(entryType == QLatin1String("misc")) {
        // do nothing, stays Publication only
    }

    // erro case
    else {
        kWarning() << "try to import unknown bibliographic resource type" << entryType;
    }
}

void BibTexToNepomukPipe::handleSpecialCases(Entry *entry, Nepomuk::SimpleResourceGraph &graph, Nepomuk::NBIB::Publication &publication, Nepomuk::NBIB::Reference &reference)
{

    // I. publisher/school/institution + address
    //    means address belongs to publisher
    if(entry->contains(QLatin1String("address"))) {
        Value publisher;
        if(entry->contains(QLatin1String("publisher"))) {
            publisher = entry->value(QLatin1String("publisher"));
            entry->remove(QLatin1String("publisher"));
        }
        else if(entry->contains(QLatin1String("school"))) {
            publisher = entry->value(QLatin1String("school"));
            entry->remove(QLatin1String("school"));
        }
        else if(entry->contains(QLatin1String("institution"))) {
            publisher = entry->value(QLatin1String("institution"));
            entry->remove(QLatin1String("institution"));
        }

        if(!publisher.isEmpty()) {
            addPublisher(publisher, entry->value(QLatin1String("address")), publication, graph);
            entry->remove(QLatin1String("address"));
        }
    }

    // II. encyclopediaarticle / blogpost / forumpost / webpage / other article type that must be in a collection
    //     as retrieved from zotero mostly
    if(entry->contains(QLatin1String("articletype"))) {
        QUrl seriesURL;
        QUrl collectionURL;
        Value emptyCollectionName;

        QString type = PlainTextValue::text(entry->value(QLatin1String("articletype")));
        if(type == QLatin1String("encyclopedia")) {
            collectionURL = NBIB::Encyclopedia();
            emptyCollectionName.append(QSharedPointer<ValueItem>(new PlainText(i18n("unknown encyclopedia"))));
        }
        else if(type == QLatin1String("blog")) {
            publication.addType(NBIB::BlogPost());
            collectionURL = NBIB::Blog();
            emptyCollectionName.append(QSharedPointer<ValueItem>(new PlainText(i18n("unknown blog"))));
        }
        else if(type == QLatin1String("webpage")) {
            publication.addType(NBIB::Webpage());
            collectionURL = NBIB::Website();
            emptyCollectionName.append(QSharedPointer<ValueItem>(new PlainText(i18n("unknown webpage"))));
        }
        else if(type == QLatin1String("forum")) {
            publication.addType(NBIB::ForumPost());
            collectionURL = NBIB::Forum();
            emptyCollectionName.append(QSharedPointer<ValueItem>(new PlainText(i18n("unknown forum"))));
        }
        if(type == QLatin1String("magazine")) {
            seriesURL = NBIB::Magazin();
            collectionURL = NBIB::MagazinIssue();
            emptyCollectionName.append(QSharedPointer<ValueItem>(new PlainText(i18n("unknown magazine"))));
        }
        else if(type == QLatin1String("newspaper")) {
            seriesURL = NBIB::Newspaper();
            collectionURL = NBIB::NewspaperIssue();
            emptyCollectionName.append(QSharedPointer<ValueItem>(new PlainText(i18n("unknown newspaper"))));
        }
        else if(type == QLatin1String("journal")) {
            seriesURL = NBIB::Journal();
            collectionURL = NBIB::JournalIssue();
            emptyCollectionName.append(QSharedPointer<ValueItem>(new PlainText(i18n("unknown journal"))));
        }

        // special series case, article in collection(Issue), in a series
        if(seriesURL.isValid()) {
            Value journalName = entry->value(QLatin1String("journal"));
            if(PlainTextValue::text(journalName).isEmpty()) {
                journalName = emptyCollectionName;
            }

            addJournal(entry->value(QLatin1String("journal")),
                       entry->value(QLatin1String("volume")),
                       entry->value(QLatin1String("number")),
                       publication, graph, seriesURL, collectionURL);

            entry->remove(QLatin1String("journal"));
            entry->remove(QLatin1String("number"));
            entry->remove(QLatin1String("volume"));
            entry->remove(QLatin1String("articletype"));
        }
        // other types like article in a blogpost, or encyclopedia
        else if(collectionURL.isValid()) {
            Value titleValue;
            if(entry->contains(QLatin1String("booktitle"))) {
                titleValue = entry->value(QLatin1String("booktitle"));
            }
            else if(entry->contains(QLatin1String("journal"))) {
                titleValue = entry->value(QLatin1String("journal"));
            }

            if(titleValue.isEmpty() ) {
                addSpecialArticle(emptyCollectionName,publication, graph, collectionURL);
            }
            else {
                addSpecialArticle(titleValue,publication, graph, collectionURL);
            }

            entry->remove(QLatin1String("journal"));
            entry->remove(QLatin1String("booktitle"));
            entry->remove(QLatin1String("articletype"));
        }
    }

    // III. journal + number + volume + zotero articletype
    //      as defined in any usual bibtex file
    if(entry->contains(QLatin1String("journal"))) {
        QUrl seriesURL;
        QUrl issueURL;

        QString type = PlainTextValue::text(entry->value(QLatin1String("articletype")));
        if(type == QLatin1String("magazine")) {
            seriesURL = NBIB::Magazin();
            issueURL = NBIB::MagazinIssue();
        }
        else if(type == QLatin1String("newspaper")) {
            seriesURL = NBIB::Newspaper();
            issueURL = NBIB::NewspaperIssue();
        }
        else {
            seriesURL = NBIB::Journal();
            issueURL = NBIB::JournalIssue();
        }

        addJournal(entry->value(QLatin1String("journal")),
                   entry->value(QLatin1String("volume")),
                   entry->value(QLatin1String("number")),
                   publication, graph, seriesURL, issueURL);

        entry->remove(QLatin1String("journal"));
        entry->remove(QLatin1String("number"));
        entry->remove(QLatin1String("volume"));
        entry->remove(QLatin1String("articletype"));
    }

    // IV. archivePrefix + eprint
    //TODO implement archivePrefix stuff


    // V. publication date
    if(entry->contains(QLatin1String("date"))) {
        addPublicationDate( PlainTextValue::text(entry->value(QLatin1String("date"))), publication );
        entry->remove(QLatin1String("date"));
        entry->remove(QLatin1String("year"));
        entry->remove(QLatin1String("month"));
        entry->remove(QLatin1String("day"));

    }
    else if(entry->contains(QLatin1String("year")) || entry->contains(QLatin1String("month"))) {
        addPublicationDate( PlainTextValue::text(entry->value(QLatin1String("year"))),
                            PlainTextValue::text(entry->value(QLatin1String("month"))),
                            PlainTextValue::text(entry->value(QLatin1String("day"))),
                            publication );

        entry->remove(QLatin1String("date"));
        entry->remove(QLatin1String("year"));
        entry->remove(QLatin1String("month"));
        entry->remove(QLatin1String("day"));
    }
}

Entry *BibTexToNepomukPipe::getDiff(Nepomuk::Resource local, Entry *externalEntry, bool keepLocal, QSharedPointer<Entry> &localEntry)
{
    //first we transform the nepomuk resource to a flat bibtex entry
    QList<Nepomuk::Resource> resources;
    resources.append(local);
    NepomukToBibTexPipe ntbp;
    ntbp.pipeExport(resources);

    File *localBibFile = ntbp.bibtexFile();

    localEntry = localBibFile->first().staticCast<Entry>();

    if(!localEntry)
        return 0;

    // now create a new entry containing only the differences between both Entry elements

    Entry *diffEntry = new Entry;
    diffEntry->setType(localEntry->type());

    // version 1 means we only care about any key that exist in the externalEntry but not in the localEntry
    if(keepLocal) {
        QMapIterator<QString, Value> i(*externalEntry);
        while (i.hasNext()) {
            i.next();
            if(!localEntry->contains(i.key())) {
                diffEntry->insert(i.key(), i.value());
            }
        }
        //in addition we want to update the zotero etag
        diffEntry->insert(QLatin1String("zoteroEtag"), externalEntry->value(QLatin1String("zoteroEtag")));
    }
    // version 2 means the diff contains a) all new keys + all existing changed values for existing keys
    // this will not delete any entries in the local storage which are deleted in the external storage
    else {
        QMapIterator<QString, Value> i(*externalEntry);
        while (i.hasNext()) {
            i.next();
            if(!localEntry->contains(i.key())) {
                diffEntry->insert(i.key(), i.value());
            }
            else {
                if(PlainTextValue::text(i.value()) != PlainTextValue::text(localEntry->value(i.key()))) {
                    diffEntry->insert(i.key(), i.value());
                }
            }
        }
    }

    return diffEntry;
}

void BibTexToNepomukPipe::merge(Nepomuk::Resource syncResource, Entry *external, bool keepLocal)
{
    m_replaceMode = true; // tells all functions we call from addContent() to replace the values with the new ones

    Nepomuk::Resource publication;
    Nepomuk::Resource reference;
    QUrl syncType = syncResource.property(SYNC::syncDataType()).toUrl();

    if( syncType == SYNC::Note()) {
        publication = syncResource.property(SYNC::note()).toResource();
    }
    else if ( syncType == SYNC::Attachment()) {
        publication = syncResource.property(SYNC::attachment()).toResource();
    }
    else {
        publication = syncResource.property(SYNC::publication()).toResource();
        reference = syncResource.property(SYNC::reference()).toResource();
    }

    // so we update only whats different
    // and if keep local is true, only those entries that do not exist currently
    QSharedPointer<Entry> nepomukEntry;
    Entry *diffEntry = getDiff(publication, external, keepLocal, nepomukEntry);

    mergeManual(syncResource, diffEntry);
}

void BibTexToNepomukPipe::mergeManual(Nepomuk::Resource syncResource, Entry *selectedDiff)
{
    m_replaceMode = true; // tells all functions we call from addContent() to replace the values with the new ones

    m_publicationToReplace = Nepomuk::Resource();
    m_referenceToReplace = Nepomuk::Resource();

    QUrl syncType = syncResource.property(SYNC::syncDataType()).toUrl();

    if( syncType == SYNC::Note()) {
        m_publicationToReplace = syncResource.property(SYNC::note()).toResource();
    }
    else if ( syncType == SYNC::Attachment()) {
        m_publicationToReplace = syncResource.property(SYNC::attachment()).toResource();
    }
    else {
        m_publicationToReplace = syncResource.property(SYNC::publication()).toResource();
        m_referenceToReplace = syncResource.property(SYNC::reference()).toResource();
    }

    // update zotero sync details
    if(selectedDiff->contains(QLatin1String("zoterokey"))) {
        QString etag = PlainTextValue::text(selectedDiff->value(QLatin1String("zoteroetag")));
        QString updated = PlainTextValue::text(selectedDiff->value(QLatin1String("zoteroupdated")));

        QList<QUrl> ssdUri; ssdUri << syncResource.uri();
        QVariantList value; value <<  etag;
        Nepomuk::setProperty(ssdUri, SYNC::etag(), value);
        value.clear(); value <<  updated;
        Nepomuk::setProperty(ssdUri, NUAO::lastModification(), value);

        selectedDiff->remove(QLatin1String("zoterokey"));
        selectedDiff->remove(QLatin1String("zoteroetag"));
        selectedDiff->remove(QLatin1String("zoteroupdated"));
    }

    if(selectedDiff->type() == QLatin1String("note")) {
        // here the note key is actually the content of the note
        QTextDocument content;
        content.setHtml( PlainTextValue::text(selectedDiff->value(QLatin1String("note"))).simplified() );

        QList<QUrl> noteUri; noteUri << m_publicationToReplace.uri();
        QVariantList value; value <<  content.toPlainText();
        Nepomuk::setProperty(noteUri, NIE::plainTextContent(), value);
        value.clear(); value <<  content.toHtml();
        Nepomuk::setProperty(noteUri, NIE::htmlContent(), value);

        selectedDiff->remove( QLatin1String("note") );
    }

    Nepomuk::SimpleResourceGraph graph;
    Nepomuk::SimpleResource srPublication(m_publicationToReplace.uri());
    Nepomuk::NBIB::Publication publicationResource(srPublication);
    Nepomuk::SimpleResource srReference(m_referenceToReplace.uri());
    Nepomuk::NBIB::Reference referenceResource(srReference);


    //BUG we need to set some property otherwise the DataManagement server complains the resource is invalid
    QDateTime datetime = QDateTime::currentDateTimeUtc();
    srPublication.setProperty( NUAO::lastModification(), datetime.toString("yyyy-MM-ddTHH:mm:ssZ"));
    srReference.setProperty( NUAO::lastModification(), datetime.toString("yyyy-MM-ddTHH:mm:ssZ"));


    kDebug() << "#########################################################################";
    kDebug() << "merge existing pub" << m_publicationToReplace.uri();
    kDebug() << "#########################################################################";

    handleSpecialCases(selectedDiff, graph, publicationResource, referenceResource);

    //go through the list of all remaining entries
    QMapIterator<QString, Value> i(*selectedDiff);
    while (i.hasNext()) {
        i.next();
        kDebug() << "merge key" << i.key().toLower();
        addContent(i.key().toLower(), i.value(), publicationResource,referenceResource, graph, selectedDiff->type(), selectedDiff->id());
    }

    graph << publicationResource << referenceResource;

    kDebug() << "#########################################################################";
    kDebug() << "save graph" << graph;
    kDebug() << "#########################################################################";

    Nepomuk::StoreResourcesJob *srj = Nepomuk::storeResources(graph,Nepomuk::IdentifyNew, Nepomuk::OverwriteProperties);
    connect(srj, SIGNAL(result(KJob*)), this, SLOT(slotSaveToNepomukDone(KJob*)));
    srj->exec();
}

void BibTexToNepomukPipe::addContent(const QString &key, const Value &value, Nepomuk::NBIB::Publication &publication,
                                     Nepomuk::NBIB::Reference &reference, Nepomuk::SimpleResourceGraph &graph,
                                     const QString & originalEntryType, const QString & citeKey)
{
    //############################################
    // Simple set commands

    if(key == QLatin1String("abstract")) {
        addValue(PlainTextValue::text(value), publication, NBIB::abstract() );
    }
    else if(key == QLatin1String("archive")) {
        addValue(PlainTextValue::text(value), publication, NBIB::archive());
    }
    else if(key == QLatin1String("history")) {
        addValue(PlainTextValue::text(value), publication, NBIB::history());
    }
    else if(key == QLatin1String("scale")) {
        addValue(PlainTextValue::text(value), publication, NBIB::mapScale());
    }
    else if(key == QLatin1String("archivelocation")) {
        addValue(PlainTextValue::text(value), publication, NBIB::archiveLocation());
    }
    else if(key == QLatin1String("librarycatalog")) {
        addValue(PlainTextValue::text(value), publication, NBIB::libraryCatalog());
    }
    else if(key == QLatin1String("copyright")) {
        addValueWithLookup(PlainTextValue::text(value), publication, NIE::copyright());
    }
    else if(key == QLatin1String("doi")) {
        addValue(PlainTextValue::text(value), publication, NBIB::doi());
    }
    else if(key == QLatin1String("edition")) {
        addValue(PlainTextValue::text(value), publication, NBIB::edition());
    }
    else if(key == QLatin1String("eprint")) {
        addValue(PlainTextValue::text(value), publication, NBIB::eprint());
    }
    else if(key == QLatin1String("howpublished")) {
        addValue(PlainTextValue::text(value), publication, NBIB::publicationMethod());
    }
    else if(key == QLatin1String("isbn")) {
        addValue(PlainTextValue::text(value), publication, NBIB::isbn());
    }
    else if(key == QLatin1String("language")) {
        addValue(PlainTextValue::text(value), publication, NIE::language());
    }
    else if(key == QLatin1String("lccn")) {
        addValue(PlainTextValue::text(value), publication, NBIB::lccn());
    }
    else if(key == QLatin1String("mrnumber")) {
        addValue(PlainTextValue::text(value), publication, NBIB::mrNumber());
    }
    else if(key == QLatin1String("number")) {
        addValue(PlainTextValue::text(value), publication, NBIB::number());
    }
    else if(key == QLatin1String("pages")) {
        addValue(PlainTextValue::text(value), reference, NBIB::pages());
    }
    else if(key == QLatin1String("numpages") || key == QLatin1String("numberofpages")) {
        QString numpages = PlainTextValue::text(value);
        if(numpages.isEmpty()) {
            numpages = '0';
        }
        addValue(numpages, publication, NBIB::numberOfPages());
    }
    else if(key == QLatin1String("numberofvolumes")) {
        QString numvolumes = PlainTextValue::text(value);
        if(numvolumes.isEmpty()) {
            numvolumes = '0';
        }
        addValue(numvolumes, publication, NBIB::numberOfVolumes());
    }
    else if(key == QLatin1String("pubmed")) {
        addValue(PlainTextValue::text(value), publication, NBIB::pubMed());
    }
    else if(key == QLatin1String("shorttitle")) {
        addValueWithLookup(PlainTextValue::text(value), publication, NBIB::shortTitle());
    }
    else if(key == QLatin1String("type")) {
        addValueWithLookup(PlainTextValue::text(value), publication, NBIB::publicationType());
    }
    else if(key == QLatin1String("applicationnumber")) {
        addValue(PlainTextValue::text(value), publication, NBIB::applicationNumber());
    }
    else if(key == QLatin1String("prioritynumbers")) {
        addValue(PlainTextValue::text(value), publication, NBIB::priorityNumbers());
    }
    else if(key == QLatin1String("legalstatus")) {
        addValue(PlainTextValue::text(value), publication, NBIB::legalStatus());
    }
    else if(key == QLatin1String("references")) {
        addValue(PlainTextValue::text(value), publication, NBIB::patentReferences());
    }
    else if(key == QLatin1String("filingdate")) {
        addValue(PlainTextValue::text(value), publication, NBIB::filingDate());
    }
    else if(key == QLatin1String("volume")) {
        addValue(PlainTextValue::text(value), publication, NBIB::volume());
    }
    else if(key == QLatin1String("accessdate")) {
        QString accessdate = PlainTextValue::text(value);
        QDateTime dateTime = QDateTime::fromString(accessdate, "yyyy-MM-ddTHH:mm:ss");
        if(!dateTime.isValid()) { dateTime = QDateTime::fromString(accessdate, "yyyy-MM-dd"); }
        if(!dateTime.isValid()) { dateTime = QDateTime::fromString(accessdate, "dd-MM-yyy"); }
        if(!dateTime.isValid()) { dateTime = QDateTime::fromString(accessdate, "yyyy-MM"); }
        if(!dateTime.isValid()) { dateTime = QDateTime::fromString(accessdate, "MM-yyyy"); }
        if(!dateTime.isValid()) { dateTime = QDateTime::fromString(accessdate, "yyyy.MM.dd"); }
        if(!dateTime.isValid()) { dateTime = QDateTime::fromString(accessdate, "dd.MM.yyy"); }
        if(!dateTime.isValid()) { dateTime = QDateTime::fromString(accessdate, "MM.yyy"); }
        if(!dateTime.isValid()) { dateTime = QDateTime::fromString(accessdate, "yyyy.MM"); }

        if(dateTime.isValid()) {
            QString newDate = dateTime.toString("yyyy-MM-ddTHH:mm:ss");

            addValue(newDate, publication, NUAO::lastUsage());
        }
        else {
            kDebug() << "could not parse accessdate" << accessdate;
        }
    }
    else if(key == QLatin1String("date")) {
        addValue(PlainTextValue::text(value), publication, NBIB::publicationDate());
    }

    //############################################
    // more advanced processing needed here

    else if(key == QLatin1String("author")) {
        addAuthor(value, publication, reference, graph, originalEntryType);
    }
    else if(key == QLatin1String("bookauthor")) {
        addContact(value, publication, graph, NCO::creator(), NCO::PersonContact());
    }
    else if(key == QLatin1String("contributor")) {
        addContact(value, publication, graph, NBIB::contributor(), NCO::PersonContact());
    }
    else if(key == QLatin1String("translator")) {
        addContact(value, publication, graph, NBIB::translator(), NCO::PersonContact());
    }
    else if(key == QLatin1String("reviewedauthor")) {
        addContact(value, publication, graph, NBIB::reviewedAuthor(), NCO::PersonContact());
    }
    else if(key == QLatin1String("attorneyagent")) {
        addContact(value, publication, graph, NBIB::attorneyAgent(), NCO::PersonContact());
    }
    else if(key == QLatin1String("counsel")) {
        addContact(value, publication, graph, NBIB::counsel(), NCO::PersonContact());
    }
    else if(key == QLatin1String("cosponsor")) {
        addContact(value, publication, graph, NBIB::coSponsor(), NCO::PersonContact());
    }
    else if(key == QLatin1String("commenter")) {
        addContact(value, publication, graph, NBIB::commenter(), NCO::PersonContact());
    }
    else if(key == QLatin1String("serieseditor")) {
        addSeriesEditor(value, publication, graph);
    }
    else if(key == QLatin1String("booktitle")) {
        addBooktitle(PlainTextValue::text(value), publication, graph, originalEntryType);
    }
    else if(key == QLatin1String("chapter")) {
        addChapter(PlainTextValue::text(value), publication, reference, graph);
    }
    else if(key == QLatin1String("editor")) {
        addContact(value, publication, graph, NBIB::editor(), NCO::PersonContact());
    }
    else if(key == QLatin1String("institution")) {
        Value empty;
        addPublisher(value, empty, publication, graph);
    }
    else if(key == QLatin1String("issn")) {
        addIssn(PlainTextValue::text(value), publication, graph);
    }
    else if(key == QLatin1String("organization") ||
    key == QLatin1String("legislativebody") ) {
        addOrganization(PlainTextValue::text(value), publication, graph);
    }
    else if(key == QLatin1String("code")) {
        addCode(PlainTextValue::text(value), publication, graph);
    }
    else if(key == QLatin1String("codenumber")) {
        addCodeNumber(PlainTextValue::text(value), publication, graph);
    }
    else if(key == QLatin1String("codevolume")) {
        addCodeVolume(PlainTextValue::text(value), publication, graph);
    }
    else if(key == QLatin1String("reporter")) {
        addReporter(PlainTextValue::text(value), publication, graph);
    }
    else if(key == QLatin1String("reportervolume")) {
        addReporterVolume(PlainTextValue::text(value), publication, graph);
    }
    else if(key == QLatin1String("publisher")) {
        Value empty;
        addPublisher(value, empty, publication, graph);
    }
    else if(key == QLatin1String("school")) {
        Value empty;
        addPublisher(value, empty, publication, graph);
    }
    else if(key == QLatin1String("series")) {
        addSeries(PlainTextValue::text(value), publication, graph);
    }
    else if(key == QLatin1String("conferencename") ||
            key == QLatin1String("meetingname") ||
            key == QLatin1String("event")) {
        addEvent(PlainTextValue::text(value), publication, graph);
    }
    else if(key == QLatin1String("title")) {
        addTitle(PlainTextValue::text(value), publication, reference, graph, originalEntryType);
    }
    else if(key == QLatin1String("url") ||
            key == QLatin1String("localfile") ||
            key == QLatin1String("biburl") ||
            key == QLatin1String("bibsource") ||
            key == QLatin1String("ee")) {
        addUrl(PlainTextValue::text(value), publication, graph);
    }
    else if(key == QLatin1String("address")) {
        QString addressValue = PlainTextValue::text(value);
        if(addressValue.contains(QLatin1String("http:"))) {
            addUrl(addressValue, publication, graph);
        }
        //ignore else case, as the address as a postal address is handled above when a publisher is available
    }
    else if(key == QLatin1String("keywords")) {
        addTopic(value, publication, graph);
    }
    else if(key == QLatin1String("assignee")) {
        addContact(value, publication, graph, NBIB::assignee(), QUrl());
    }
    else if(key == QLatin1String("descriptor") ||
            key == QLatin1String("classification") ||
            key == QLatin1String("thesaurus") ||
            key == QLatin1String("subject")) {

        Value keywordList;
        QString keywordString = PlainTextValue::text(value);
        QStringList keywords;
        if(keywordString.contains(";")) {
            keywords = keywordString.split(';');
        }
        else {
            keywords = keywordString.split(',');
        }

        foreach(const QString &s, keywords) {
            Keyword *k = new Keyword(s.trimmed());
            keywordList.append(QSharedPointer<ValueItem>(k));
        }

        addTopic(keywordList, publication, graph);
    }
    else if(key.startsWith(QLatin1String("note")) || key.startsWith(QLatin1String("annote"))) {
        QString title = citeKey + ": " + key;
        addNote( PlainTextValue::text(value), title, publication, graph);
    }
    else {
        kDebug() << "unknown bibtex key ::" << key << PlainTextValue::text(value);
    }
}

void BibTexToNepomukPipe::addNote(const QString &contentVale, const QString &noteType, Nepomuk::NBIB::Publication &publication, Nepomuk::SimpleResourceGraph &graph)
{
    Nepomuk::PIMO::Note note;
    note.addType(NIE::InformationElement());

    note.setProperty( NAO::prefLabel(), noteType );
    note.setProperty( NIE::title(), noteType );

    QTextDocument content;
    content.setHtml( contentVale.simplified() );
    note.setProperty( NIE::plainTextContent(), content.toPlainText());
    note.setProperty( NIE::htmlContent(), content.toHtml());

    publication.addProperty(NAO::isRelated(), note);
    publication.addProperty(NAO::hasSubResource(), note);

    graph << note;
}

void BibTexToNepomukPipe::addPublisher(const Value &publisherValue, const Value &addressValue, Nepomuk::NBIB::Publication &publication, Nepomuk::SimpleResourceGraph &graph)
{
    Nepomuk::Resource publisherNR = m_publicationToReplace.property(NCO::publisher()).toResource();
    QString addressString = PlainTextValue::text(addressValue).toUtf8();
    addressString = m_macroLookup.value(addressString, addressString);

    //FIXME extendedAddress is not correct, but determining which part of the @p address is the street/location and so on is nearly impossible
    Nepomuk::NCO::PostalAddress postalAddress;
    if(m_replaceMode) {
        Nepomuk::Resource pa = publisherNR.property(NCO::hasPostalAddress()).toResource();
        if(pa.isValid()) {
            postalAddress.setUri(pa.uri());
        }
    }

    if(!addressString.isEmpty()) {
        postalAddress.setExtendedAddress( addressString );
        graph << postalAddress;
    }

    foreach(QSharedPointer<ValueItem> publisherItem, publisherValue) {
        //transform KBibTex representation of the name into my own Name
        Name publisher;
        bool personNotInstitution = false;
        Person *person = dynamic_cast<Person *>(publisherItem.data());
        if(person) {
            personNotInstitution = true;
            publisher.first = person->firstName().toUtf8();
            publisher.last = person->lastName().toUtf8();
            publisher.suffix = person->suffix().toUtf8();
            publisher.full = publisher.first + QLatin1String(" ") + publisher.last + QLatin1String(" ") + publisher.suffix;
            publisher.full = publisher.full.trimmed();
        }
        else {
            publisher.full = PlainTextValue::text(*publisherItem).toUtf8();
            publisher.full = m_macroLookup.value(publisher.full, publisher.full);
        }

        // create new publisher resource, duplicates will be merged by the DMS later on
        if( personNotInstitution ) {
            Nepomuk::NCO::PersonContact publisherResource;
            if(m_replaceMode && publisherNR.isValid()) {
                publisherResource.setUri(publisherNR.uri());
            }

            publisherResource.setFullname( publisher.full );
            publisherResource.setNameGiven( publisher.first );
            publisherResource.setNameFamily( publisher.last );
            QStringList suffixes;
            suffixes << publisher.suffix;
            publisherResource.setNameHonorificSuffixs( suffixes );
            if(!addressString.isEmpty()) {
                publisherResource.addPostalAddress( postalAddress.uri() );
                publisherResource.addProperty(NAO::hasSubResource(), postalAddress.uri() );
            }

            graph << publisherResource;
            publication.addProperty(NCO::publisher(), publisherResource);
        }
        else {
            // So we didn't get a Person as publisher but instead a single name, might be organization or something else
            Nepomuk::NCO::Contact publisherResource;

            if(m_replaceMode && publisherNR.isValid()) {
                publisherResource.setUri(publisherNR.uri());
            }

            publisherResource.setFullname( publisher.full );
            if(!addressString.isEmpty()) {
                publisherResource.addPostalAddress( postalAddress.uri() );
                publisherResource.addProperty(NAO::hasSubResource(), postalAddress.uri() );
            }

            graph << publisherResource;
            publication.addProperty(NCO::publisher(), publisherResource);
        }
    }
}

void BibTexToNepomukPipe::addJournal(const Value &journalValue, const Value &volumeValue, const Value &numberValue,
                                     Nepomuk::NBIB::Publication &publication, Nepomuk::SimpleResourceGraph &graph, QUrl seriesUrl, QUrl issueUrl)
{
    QString journalString = PlainTextValue::text(journalValue).toUtf8();
    journalString = m_macroLookup.value(journalString, journalString);
    QString volumeString = PlainTextValue::text(volumeValue).toUtf8();
    QString numberString = PlainTextValue::text(numberValue).toUtf8();

    //find existing journal or create a new series of them
    Nepomuk::NBIB::Collection collection;
    collection.addType( issueUrl );
    Nepomuk::NBIB::Series series;
    series.addType( seriesUrl );

    if(m_replaceMode) {
        Nepomuk::Resource collectionNR = m_publicationToReplace.property(NBIB::collection()).toResource();
        if(collectionNR.isValid()) {
            collection.setUri( collectionNR.uri() );
        }
        Nepomuk::Resource seriesNR = collectionNR.property(NBIB::inSeries()).toResource();
        if(!seriesNR.isValid()) {
            seriesNR = m_publicationToReplace.property(NBIB::inSeries()).toResource();
        }
        if( seriesNR.isValid() ) {
            series.setUri( seriesNR.uri() );
        }
    }

    // create the resources, the DMS will merge them later on together again
    series.setProperty( NIE::title(), journalString );

    collection.setNumber( numberString );
    collection.setProperty( NBIB::volume(), volumeString );

    // the collection name should be created by the series name + collection number/volume
    // we do this here so we get nicer results when we use genericLabel/prefLabel
    QString issueName = journalString + ' ' + volumeString;
    if(!numberString.isEmpty()) {
        issueName += ' ' + i18n("Vol. %1",numberString);
    }
    collection.setProperty( NIE::title(), issueName.trimmed() );

    // connect issue <-> journal
    collection.setInSeries( series.uri() );
    series.addSeriesOf( collection.uri() );

    // connect article <-> collection
    publication.setProperty(NBIB::collection(), collection );
    collection.addArticle( publication.uri() );
    collection.addProperty(NAO::hasSubResource(), publication.uri() ); // delete article when collection is removed

    if(m_projectThing.isValid()) {
        collection.addProperty( NAO::isRelated() , m_projectThing.uri());
        series.addProperty( NAO::isRelated() , m_projectThing.uri());
    }

    graph << collection << series;
}

void BibTexToNepomukPipe::addSpecialArticle(const Value &titleValue, Nepomuk::NBIB::Publication &article, Nepomuk::SimpleResourceGraph &graph, QUrl collectionUrl)
{
    QString collectionString = PlainTextValue::text(titleValue).toUtf8();
    collectionString = m_macroLookup.value(collectionString, collectionString);

    Nepomuk::NBIB::Collection collection;
    collection.addType( collectionUrl );

    if(m_replaceMode) {
        Nepomuk::Resource collectionNR = m_publicationToReplace.property(NBIB::collection()).toResource();
        if(collectionNR.isValid()) {
            collection.setUri( collectionNR.uri() );
        }
    }

    // create the resources, the DMS will merge them later on together again
    collection.setProperty( NIE::title(), collectionString );

    // connect article <-> collection
    article.setProperty(NBIB::collection(), collection.uri() );
    collection.addArticle( article.uri() );
    collection.addProperty(NAO::hasSubResource(), article.uri() ); // delete article when collection is removed

    if(m_projectThing.isValid()) {
        collection.addProperty( NAO::isRelated() , m_projectThing.uri());
    }

    graph << collection;
}

void BibTexToNepomukPipe::addAuthor(const Value &contentValue, Nepomuk::NBIB::Publication &publication, Nepomuk::NBIB::Reference &reference, Nepomuk::SimpleResourceGraph &graph, const QString & originalEntryType)
{
    //in case of @incollection the author is used to identify who wrote the chapter not the complete book/collection
    if(originalEntryType == QLatin1String("incollection") ) {

        if(m_replaceMode) {
            Nepomuk::Resource refChaper = m_referenceToReplace.property(NBIB::referencedPart()).toResource();
            if(refChaper.exists()) {
                Nepomuk::NBIB::Chapter chapterResource(refChaper.uri());
                addContact(contentValue, chapterResource, graph, NCO::creator(), NCO::PersonContact());
                graph << chapterResource;
                return;
            }
        }

        QUrl chapterUrl = reference.referencedPart();

        // if no chapter resources existed up to now, create one and use it
        if(!chapterUrl.isValid()) {
            Nepomuk::NBIB::Chapter chapterResource;
            chapterResource.addType(NIE::DataObject());
            chapterResource.setProperty(NIE::title(), i18n("unknown chapter") );
            chapterResource.setChapterNumber( 0 );
            // the chapter needs a "unique identifier, otherwise we merge them with other chapters together";
            chapterResource.setProperty(NAO::identifier(), QUuid::createUuid().toString());
            chapterResource.setProperty(NIE::url(), QUuid::createUuid().toString());

            // connect refrence <-> chapter <-> publication
            chapterResource.setDocumentPartOf( publication.uri() );
            publication.addDocumentPart( chapterResource.uri() );
            publication.addProperty(NAO::hasSubResource(), chapterResource.uri() ); //delete chapter when publication is removed
            reference.setReferencedPart( chapterResource.uri() );

            graph << chapterResource;

            addContact(contentValue, chapterResource, graph, NCO::creator(), NCO::PersonContact());
        }
        else {
            addContact(contentValue, graph[chapterUrl], graph, NCO::creator(), NCO::PersonContact());
        }
    }
    else {
        addContact(contentValue, publication, graph, NCO::creator(), NCO::PersonContact());
    }
}

void BibTexToNepomukPipe::addBooktitle(const QString &content, Nepomuk::NBIB::Publication &publication, Nepomuk::SimpleResourceGraph &graph, const QString & originalEntryType)
{
    QString utfContent = m_macroLookup.value(QString(content.toUtf8()), QString(content.toUtf8()));

    //two specialities occur here
    // I. "booktitle" means the title of a book, where "title" than means the title of the article in the book where the author fits to
    // this is valid for any publication other than @InProceedings/@encyclopediaarticle
    //
    // II. "booktitle" marks the title of the @Proceedings/@Encyclopedia (Collection) from an @InProceedings/@encyclopediaarticle (Article)

    if(originalEntryType == QLatin1String("inproceedings")) {

        //create a new collection resource, the DMS will merge if necessary
        Nepomuk::NBIB::Collection collection;

        if(m_replaceMode) {
            Nepomuk::Resource colRes = m_publicationToReplace.property(NBIB::collection()).toResource();
            if(colRes.exists()) {
                collection.setUri(colRes.uri());
                collection.setProperty(NIE::title(), utfContent );
                graph << collection;
                return;
            }
        }

        if(originalEntryType == QLatin1String("inproceedings")) {
            collection.addType( NBIB::Proceedings() );
        }
        else if(originalEntryType == QLatin1String("encyclopediaarticle")) {
            collection.addType( NBIB::Encyclopedia() );
        }

        collection.setProperty(NIE::title(), utfContent );

        graph << collection;

        // connect collection <-> article publication
        publication.setProperty(NBIB::collection(), collection.uri() );
        collection.addArticle( publication.uri() );
        collection.addProperty(NAO::hasSubResource(), publication.uri() ); // delete article when collection is removed

        if(m_projectThing.isValid()) {
            collection.addProperty( NAO::isRelated() , m_projectThing.uri() );
        }
    }
    else {
        // put booktitle as the main title of the publication
        publication.setProperty(NIE::title(), utfContent);
    }
}

void BibTexToNepomukPipe::addSeriesEditor(const Value &contentValue, Nepomuk::NBIB::Publication &publication, Nepomuk::SimpleResourceGraph &graph)
{
    if(m_replaceMode) {
        Nepomuk::Resource seriesRes = m_publicationToReplace.property(NBIB::inSeries()).toResource();
        if(seriesRes.exists()) {
            Nepomuk::NBIB::Series newSeries;
            newSeries.setUri(seriesRes.uri());

            graph << newSeries;
            addContact(contentValue, newSeries, graph, NBIB::editor(), NCO::PersonContact());
            return;
        }
    }


    QUrl seriesUrl = publication.inSeries();

    if(!seriesUrl.isValid()) {
        Nepomuk::NBIB::Series newSeries;

        newSeries.addSeriesOf( publication.uri() );
        publication.setInSeries( newSeries.uri() );

        if(m_projectThing.isValid()) {
            newSeries.addProperty( NAO::isRelated() , m_projectThing.uri() );
        }

        graph << newSeries;

        addContact(contentValue, newSeries, graph, NBIB::editor(), NCO::PersonContact());
    }
    else {
        addContact(contentValue, graph[seriesUrl], graph, NBIB::editor(), NCO::PersonContact());
    }
}

void BibTexToNepomukPipe::addChapter(const QString &content, Nepomuk::NBIB::Publication &publication, Nepomuk::NBIB::Reference &reference, Nepomuk::SimpleResourceGraph &graph)
{
    QString utfContent = m_macroLookup.value(QString(content.toUtf8()), QString(content.toUtf8()));

    // If we import some thing we assume no reference already existed and we have a new one
    // thus referencedPart() is not valid
    // if it is valid we assume this was already a nbib:Chapter rather than an generic nbib:DocumentPart

    QUrl chapterUrl = reference.referencedPart();

    if(!chapterUrl.isValid()) {
        Nepomuk::NBIB::Chapter chapterResource;
        chapterResource.addType(NIE::DataObject());
        chapterResource.setProperty( NIE::title(), i18n("unknown chapter") );
        chapterResource.setChapterNumber( utfContent );
        // the chapter needs a "unique identifier, otherwise we merge them with other chapters together";
        chapterResource.setProperty(NAO::identifier(), QUuid::createUuid().toString());
        chapterResource.setProperty(NIE::url(), QUuid::createUuid().toString());

        // connect refrence <-> chapter <-> publication
        chapterResource.setDocumentPartOf( publication.uri() );
        publication.addDocumentPart( chapterResource.uri() );
        publication.addProperty(NAO::hasSubResource(), chapterResource.uri() ); //delete chapter when publication is removed
        reference.setReferencedPart( chapterResource.uri() );

        graph << chapterResource;
    }
    else {
        graph[chapterUrl].setProperty(NBIB::chapterNumber(), utfContent);
    }
}

void BibTexToNepomukPipe::addIssn(const QString &content, Nepomuk::NBIB::Publication &publication, Nepomuk::SimpleResourceGraph &graph)
{
    QString utfContent = m_macroLookup.value(QString(content.toUtf8()), QString(content.toUtf8()));

    if(m_replaceMode) {
        Nepomuk::Resource seriesResource ;
        if(m_publicationToReplace.hasType(NBIB::Article())) {
            Nepomuk::Resource collectionResource = m_publicationToReplace.property(NBIB::collection()).toResource();
            seriesResource = collectionResource.property(NBIB::inSeries()).toResource();
        }
        else {
            seriesResource = m_publicationToReplace.property(NBIB::inSeries()).toResource();
        }
        if(seriesResource.exists()) {
            Nepomuk::NBIB::Series series(seriesResource.uri());
            series.setIssn( utfContent );
            graph << series;
            return;
        }
    }

    QUrl seriesUrl;
    if(publication.contains(RDF::type(), NBIB::Article())) {
        if( publication.property(NBIB::collection()).isEmpty() ) {
            kWarning() << "we added an article without any collection and try to set the ISSN";
            Nepomuk::NBIB::Collection collection;
            collection.setProperty(NIE::title(), i18n("unknown collection"));

            Nepomuk::NBIB::Series series;
            series.setProperty(NIE::title(), i18n("unknown series"));

            // connect series <-> collection
            collection.setInSeries( series.uri() );
            series.addSeriesOf( collection.uri() );

            // connect article <-> collection
            publication.setProperty(NBIB::collection(), collection );
            collection.addArticle( publication.uri() );

            seriesUrl = series.uri();
            graph << collection << series;
        }
        else {
            QUrl collectionUrl = publication.property(NBIB::collection()).first().toUrl();

            if( !graph[collectionUrl].property(NBIB::inSeries()).isEmpty() ) {
                seriesUrl = graph[collectionUrl].property(NBIB::inSeries()).first().toUrl();
            }
        }
    }
    else {
        // for anything else than an article, we attach the ISSN to a series directly to the publication
        seriesUrl = publication.inSeries();
    }

    if(!seriesUrl.isValid()) {
        Nepomuk::NBIB::Series newSeries;
        newSeries.addSeriesOf( publication.uri() );
        publication.setInSeries( newSeries.uri() );
        newSeries.setIssn( utfContent );

        if(m_projectThing.isValid()) {
            newSeries.addProperty( NAO::isRelated() , m_projectThing.uri() );
        }

        graph << newSeries;
    }
    else {
        graph[seriesUrl].setProperty(NBIB::issn(), utfContent);
    }
}

void BibTexToNepomukPipe::addOrganization(const QString &content, Nepomuk::NBIB::Publication &publication, Nepomuk::SimpleResourceGraph &graph)
{
    QString utfContent = m_macroLookup.value(QString(content.toUtf8()), QString(content.toUtf8()));

    if(m_replaceMode) {
        Nepomuk::Resource orgResource ;
        if(m_publicationToReplace.hasType(NBIB::Article())) {
            Nepomuk::Resource collectionResource = m_publicationToReplace.property(NBIB::collection()).toResource();
            orgResource = collectionResource.property(NBIB::organization()).toResource();
        }
        else {
            orgResource = m_publicationToReplace.property(NBIB::organization()).toResource();
        }
        if(orgResource.exists()) {
            Nepomuk::NCO::OrganizationContact organization(orgResource.uri());
            organization.setFullname( utfContent );
            graph << organization;
            return;
        }
    }

    Nepomuk::NCO::OrganizationContact organization;
    organization.setFullname( utfContent );

    if(publication.contains(RDF::type(), NBIB::Article())) {
        // special case if the publication is an article, we assume the organization was responsible for the full collection
        // organization of a proceedings/conference or respnsible for teh blog/forum etc

        QUrl collectionUrl;
        if( !publication.property(NBIB::collection()).isEmpty()) {
            collectionUrl = publication.property(NBIB::collection()).first().toUrl();
        }

        if(!collectionUrl.isValid()) {
            // create new proceedings resource, as non existed yet
            Nepomuk::NBIB::Proceedings newProceedings;
            newProceedings.addArticle( publication.uri() );
            publication.setProperty(NBIB::collection(), newProceedings.uri() );

            newProceedings.setOrganization( organization.uri() );

            if(m_projectThing.isValid()) {
                newProceedings.addProperty( NAO::isRelated() , m_projectThing.uri() );
            }

            graph << newProceedings;
        }
        else {
            // add organization to previous created proceedings
            graph[collectionUrl].setProperty(NBIB::organization(), organization );
        }
    }
    else {
        // publication seems to be proceedings already
        publication.setProperty(NBIB::organization(), organization );
    }

    graph << organization;
}

void BibTexToNepomukPipe::addCode(const QString &content, Nepomuk::NBIB::Publication &publication, Nepomuk::SimpleResourceGraph &graph)
{
    Nepomuk::NBIB::CodeOfLaw codeOfLaw;
    if(m_replaceMode) {
        Nepomuk::Resource cResource = m_publicationToReplace.property(NBIB::codeOfLaw()).toResource();
        if(cResource.exists())
            codeOfLaw.setUri(cResource.uri());
    }

    codeOfLaw.setProperty(NIE::title(), QString(content.toUtf8()));

    publication.setProperty(NBIB::codeOfLaw(), codeOfLaw.uri()  );

    if(m_projectThing.isValid()) {
        codeOfLaw.addProperty( NAO::isRelated() , m_projectThing.uri() );
    }

    graph << codeOfLaw;
}

void BibTexToNepomukPipe::addCodeNumber(const QString &content, Nepomuk::NBIB::Publication &publication, Nepomuk::SimpleResourceGraph &graph)
{
    if(m_replaceMode) {
        Nepomuk::Resource cResource = m_publicationToReplace.property(NBIB::codeOfLaw()).toResource();
        if(cResource.exists()) {
            Nepomuk::NBIB::CodeOfLaw codeOfLaw(cResource.uri());
            codeOfLaw.setProperty(NBIB::codeNumber(), QString(content.toUtf8()) );
            graph << codeOfLaw;
            return;
        }
    }

    QUrl codeOfLawUrl = Nepomuk::NBIB::Legislation(publication).codeOfLaw();

    if(!codeOfLawUrl.isValid()) {
        // create new code of law, seems none existed up to now
        Nepomuk::NBIB::CodeOfLaw codeOfLaw;
        codeOfLaw.setProperty(NIE::title(), QString(content.toUtf8()));

        publication.setProperty(NBIB::codeOfLaw(), codeOfLaw.uri()  );
        codeOfLaw.setProperty(NBIB::codeNumber(), QString(content.toUtf8()) );

        if(m_projectThing.isValid()) {
            codeOfLaw.addProperty( NAO::isRelated() , m_projectThing.uri() );
        }

        graph << codeOfLaw;
    }
    else {
        // add to existing code
        graph[codeOfLawUrl].setProperty(NBIB::codeNumber(), QString(content.toUtf8()) );
    }
}

void BibTexToNepomukPipe::addCodeVolume(const QString &content, Nepomuk::NBIB::Publication &publication, Nepomuk::SimpleResourceGraph &graph)
{
    if(m_replaceMode) {
        Nepomuk::Resource cResource = m_publicationToReplace.property(NBIB::codeOfLaw()).toResource();
        if(cResource.exists()) {
            Nepomuk::NBIB::CodeOfLaw codeOfLaw(cResource.uri());
            codeOfLaw.setProperty(NBIB::volume(), QString(content.toUtf8()) );
            graph << codeOfLaw;
            return;
        }
    }

    QUrl codeOfLawUrl = Nepomuk::NBIB::Legislation(publication).codeOfLaw();

    if(!codeOfLawUrl.isValid()) {
        // create new code of law, seems none existed up to now
        Nepomuk::NBIB::CodeOfLaw codeOfLaw;
        codeOfLaw.setProperty(NIE::title(), QString(content.toUtf8()));

        publication.setProperty(NBIB::codeOfLaw(), codeOfLaw.uri()  );
        codeOfLaw.setProperty(NBIB::volume(), QString(content.toUtf8()) );

        if(m_projectThing.isValid()) {
            codeOfLaw.addProperty( NAO::isRelated() , m_projectThing.uri() );
        }

        graph << codeOfLaw;
    }
    else {
        // add to existing code
        Nepomuk::NBIB::Publication(graph[codeOfLawUrl]).setVolume( QString(content.toUtf8()) );
    }
}

void BibTexToNepomukPipe::addReporter(const QString &content, Nepomuk::NBIB::Publication &publication, Nepomuk::SimpleResourceGraph &graph)
{
    if(m_replaceMode) {
        Nepomuk::Resource cResource = m_publicationToReplace.property(NBIB::courtReporter()).toResource();
        if(cResource.exists()) {
            Nepomuk::NBIB::CourtReporter courtReporter(cResource.uri());
            courtReporter.setProperty(NIE::title(), QString(content.toUtf8()) );
            graph << courtReporter;
            return;
        }
    }

    Nepomuk::NBIB::CourtReporter courtReporter;
    courtReporter.setProperty(NIE::title(), QString(content.toUtf8()));
    publication.setProperty(NBIB::courtReporter(), courtReporter.uri()  );

    graph << courtReporter;

    if(m_projectThing.isValid()) {
        courtReporter.addProperty( NAO::isRelated() , m_projectThing.uri() );
    }
}

void BibTexToNepomukPipe::addReporterVolume(const QString &content, Nepomuk::NBIB::Publication &publication, Nepomuk::SimpleResourceGraph &graph)
{
    if(m_replaceMode) {
        Nepomuk::Resource cResource = m_publicationToReplace.property(NBIB::courtReporter()).toResource();
        if(cResource.exists()) {
            Nepomuk::NBIB::CourtReporter courtReporter(cResource.uri());
            courtReporter.setProperty(NBIB::volume(), QString(content.toUtf8()) );
            graph << courtReporter;
            return;
        }
    }

    QUrl courtReporterUrl;
    if(!publication.property(NBIB::courtReporter()).isEmpty()) {
        courtReporterUrl = publication.property(NBIB::courtReporter()).first().toUrl();
    }

    if(!courtReporterUrl.isValid()) {
        // create new ccourt reporter, seems none existed up to now
        Nepomuk::NBIB::CourtReporter courtReporter;
        publication.setProperty(NBIB::courtReporter(), courtReporter.uri() );
        courtReporter.setProperty(NBIB::volume(), QString(content.toUtf8()) );

        if(m_projectThing.isValid()) {
            courtReporter.addProperty( NAO::isRelated() , m_projectThing.uri() );
        }

        graph << courtReporter;
    }
    else {
        // add to existing court reporter
        graph[courtReporterUrl].setProperty(NBIB::volume(), QString(content.toUtf8()) );
    }
}

void BibTexToNepomukPipe::addEvent(const QString &content, Nepomuk::NBIB::Publication &publication, Nepomuk::SimpleResourceGraph &graph)
{
    if(m_replaceMode) {
        Nepomuk::Resource eResource = m_publicationToReplace.property(NBIB::event()).toResource();
        if(eResource.exists()) {
            Nepomuk::PIMO::Event event(eResource.uri());
            event.setProperty(NAO::prefLabel(), QString(content.toUtf8()) );
            graph << event;
            return;
        }
    }

    Nepomuk::PIMO::Event event;

    event.setProperty(NAO::prefLabel(), QString(content.toUtf8()));

    event.addProperty(NBIB::eventPublication(), publication);
    publication.addEvent( event.uri() );

    if(m_projectThing.isValid()) {
        event.addProperty( NAO::isRelated() , m_projectThing.uri() );
    }

    graph << event;
}

void BibTexToNepomukPipe::addSeries(const QString &content, Nepomuk::NBIB::Publication &publication, Nepomuk::SimpleResourceGraph &graph)
{
    if(m_replaceMode) {
        Nepomuk::Resource sResource = m_publicationToReplace.property(NBIB::inSeries()).toResource();
        if(sResource.exists()) {
            Nepomuk::NBIB::Series series(sResource.uri());
            series.setProperty(NIE::title(), QString(content.toUtf8()) );
            graph << series;
            return;
        }
    }

    Nepomuk::NBIB::Series series;

    if( publication.contains(RDF::type(), NBIB::Book()) || publication.contains(RDF::type(), NBIB::Booklet()) ) {
        series.addType( NBIB::BookSeries() );
    }

    QString utfContent = m_macroLookup.value(QString(content.toUtf8()), QString(content.toUtf8()));
    series.setProperty( NIE::title(), utfContent );

    series.addSeriesOf( publication.uri() );
    publication.setInSeries( series.uri()  );

    if(m_projectThing.isValid()) {
        series.addProperty( NAO::isRelated() , m_projectThing.uri() );
    }

    graph << series;
}

void BibTexToNepomukPipe::addTitle(const QString &content, Nepomuk::NBIB::Publication &publication, Nepomuk::NBIB::Reference &reference, Nepomuk::SimpleResourceGraph &graph, const QString & originalEntryType)
{
    QString utfContent = m_macroLookup.value(QString(content.toUtf8()), QString(content.toUtf8()));

    // in the case of @InCollection title means title of the article in the book
    // while booktitle is the actual title of the book
    if(originalEntryType == QLatin1String("incollection") || originalEntryType == QLatin1String("dictionaryentry")) {

        if(m_replaceMode) {
            Nepomuk::Resource refChaper = m_referenceToReplace.property(NBIB::referencedPart()).toResource();
            if(refChaper.exists()) {
                Nepomuk::NBIB::Chapter chapterResource(refChaper.uri());
                chapterResource.setProperty( NIE::title(), utfContent);
                graph << chapterResource;
                return;
            }
        }

        QUrl chapterUrl = reference.referencedPart();

        if(!chapterUrl.isValid()) {
            Nepomuk::NBIB::Chapter chapter;
            chapter.addType(NIE::DataObject());
            chapter.setProperty( NIE::title(), utfContent);
            // the chapter needs a "unique identifier, otherwise we merge them with other chapters together";
            chapter.setProperty(NAO::identifier(), QUuid::createUuid().toString());
            chapter.setProperty(NIE::url(), QUuid::createUuid().toString());

            // connect reference <-> chapter <-> document
            reference.setReferencedPart( chapter.uri() );
            publication.addDocumentPart( chapter.uri() );
            chapter.setDocumentPartOf( publication.uri() );
            publication.addProperty(NAO::hasSubResource(), chapter.uri() ); // delete chapter when publication is removed

            graph << chapter;
        }
        else {
            graph[chapterUrl].setProperty( NIE::title(), utfContent);
        }
    }
    else {
        publication.setProperty(NIE::title(), utfContent);
    }
}

void BibTexToNepomukPipe::addUrl(const QString &content, Nepomuk::NBIB::Publication &publication, Nepomuk::SimpleResourceGraph &graph)
{
    if(m_replaceMode) {
        kDebug() << "remove all urls from" << m_publicationToReplace;
        QList<QUrl> resourceUris; resourceUris << m_publicationToReplace.uri();
        QList<QUrl> value; value << NIE::links();
        KJob *job = Nepomuk::removeProperties(resourceUris, value);
        job->exec();
    }

    //TODO differentiate between webpage and webseite
    // TODO split webpages if necessary
    QUrl url( QString(content.toUtf8()) );

    QString protocol = url.scheme();

    if(protocol.isEmpty()) {
        kDebug() << "tried to add invalid url without protocol add http://";
        url.setScheme(QLatin1String("http"));
    }
    else if(!protocol.contains(QLatin1String("http")) && !protocol.contains(QLatin1String("https")) &&
            !protocol.contains(QLatin1String("ftp"))) {
        kDebug() << "tried to add invalid url with unknown protocol" << protocol <<  "add http://";
        url.setScheme(QLatin1String("http://") + protocol);
    }

    Nepomuk::NFO::Website website( url );
    website.addType(NFO::WebDataObject());

    publication.addProperty(NIE::links(), website.uri() );

    graph << website;
}

void BibTexToNepomukPipe::addPublicationDate(const QString &fullDate, Nepomuk::NBIB::Publication &publication)
{
    // try to find out what format was used to specify the full date
    QDateTime dateTime = QDateTime::fromString(fullDate, "yyyy-MM-ddTHH:mm:ssZ");

    if(!dateTime.isValid()) { dateTime = QDateTime::fromString(fullDate, "yyyy-MM-ddTHH:mm:ss"); }
    if(!dateTime.isValid()) { dateTime = QDateTime::fromString(fullDate, "yyyy-MM-dd"); }
    if(!dateTime.isValid()) { dateTime = QDateTime::fromString(fullDate, "dd-MM-yyy"); }
    if(!dateTime.isValid()) { dateTime = QDateTime::fromString(fullDate, "yyyy-MM"); }
    if(!dateTime.isValid()) { dateTime = QDateTime::fromString(fullDate, "MM-yyyy"); }
    if(!dateTime.isValid()) { dateTime = QDateTime::fromString(fullDate, "yyyy.MM.dd"); }
    if(!dateTime.isValid()) { dateTime = QDateTime::fromString(fullDate, "dd.MM.yyy"); }
    if(!dateTime.isValid()) { dateTime = QDateTime::fromString(fullDate, "MM.yyy"); }
    if(!dateTime.isValid()) { dateTime = QDateTime::fromString(fullDate, "yyyy.MM"); }
    if(!dateTime.isValid()) { dateTime = QDateTime::fromString(fullDate, "yyyy"); }
    if(!dateTime.isValid()) { dateTime = QDateTime::fromString(fullDate, "yy"); }

    if(dateTime.isValid()) {
        QString newDate = dateTime.toString("yyyy-MM-ddTHH:mm:ss");

        publication.setProperty( NBIB::publicationDate(), newDate);
    }
    else {
        kDebug() << "could not parse publication date " << fullDate;
    }
}

void BibTexToNepomukPipe::addPublicationDate(const QString &year, const QString &month, const QString &day, Nepomuk::NBIB::Publication &publication)
{
    QString finalYear = year;
    QString finalMonth = month;
    QString finalDay = day;

    if(finalYear.size() != 4) {
        finalYear.prepend(QLatin1String("20")); // transforms '11 into 2011
    }

    if(finalMonth.isEmpty()) {
        finalMonth = QLatin1String("01");
    }
    else {
        bool monthIsInt;
        finalMonth.toInt(&monthIsInt);
        if(monthIsInt) {
            finalMonth = month;
        }
        else {
            QString contentMonth = finalMonth.toLower().toUtf8();
            //transform bibtex month to numbers
            if(contentMonth.contains(QLatin1String("jan"))) {
                contentMonth = QString::number(1);
            }
            else if(contentMonth.contains(QLatin1String("feb"))) {
                contentMonth = QString::number(2);
            }
            else if(contentMonth.contains(QLatin1String("mar"))) {
                contentMonth = QString::number(3);
            }
            else if(contentMonth.contains(QLatin1String("apr"))) {
                contentMonth = QString::number(4);
            }
            else if(contentMonth.contains(QLatin1String("may"))) {
                contentMonth = QString::number(5);
            }
            else if(contentMonth.contains(QLatin1String("jun"))) {
                contentMonth = QString::number(6);
            }
            else if(contentMonth.contains(QLatin1String("jul"))) {
                contentMonth = QString::number(7);
            }
            else if(contentMonth.contains(QLatin1String("aug"))) {
                contentMonth = QString::number(8);
            }
            else if(contentMonth.contains(QLatin1String("sep"))) {
                contentMonth = QString::number(9);
            }
            else if(contentMonth.contains(QLatin1String("oct"))) {
                contentMonth = QString::number(10);
            }
            else if(contentMonth.contains(QLatin1String("nov"))) {
                contentMonth = QString::number(11);
            }
            else if(contentMonth.contains(QLatin1String("dec"))) {
                contentMonth = QString::number(12);
            }
            else {
                contentMonth = QLatin1String("01");
            }

            finalMonth = contentMonth;
        }

    }

    if(finalMonth.size() != 2) {
        finalMonth = finalMonth.prepend(QLatin1String("0")); // transforms '1 into 01
    }

    if(day.isEmpty()) {
        finalDay = QLatin1String("01");
    }
    else if(day.size() == 1) {
        finalDay.prepend(QLatin1String("0")); // transforms '1 into 01
    }

    QString newDate = finalYear + QLatin1String("-") + finalMonth + QLatin1String("-") + finalDay + QLatin1String("T00:00:00");

    publication.setProperty( NBIB::publicationDate(), newDate);
}

void BibTexToNepomukPipe::addTag(const Value &content, Nepomuk::SimpleResource &resource, Nepomuk::SimpleResourceGraph &graph)
{
    if(m_replaceMode) {
        kDebug() << "remove all tags from" << m_publicationToReplace;
        QList<QUrl> resourceUris; resourceUris << m_publicationToReplace.uri();
        QList<QUrl> value; value << NAO::hasTag();
        KJob *job = Nepomuk::removeProperties(resourceUris, value);
        job->exec();
    }

    foreach(QSharedPointer<ValueItem> vi, content) {
        Keyword *k = dynamic_cast<Keyword *>(vi.data());
        if(!k) { continue; }

        //TODO does this need a change? works differently than pimo:Topic creation, still works though...
        // there exist 2 apis for the ontology class, one that uses pointers the other that uses references ...
        Nepomuk::SimpleResource tagResource;
        Nepomuk::NAO::Tag tag (&tagResource);

        kDebug() << "add tag" << k->text();

        tagResource.addProperty( NAO::identifier(), QUrl::fromEncoded(k->text().toUtf8()));
        tag.addPrefLabel( k->text() );

        graph << tagResource;
        resource.addProperty( NAO::hasTag(), tagResource.uri());
    }
}

void BibTexToNepomukPipe::addTopic(const Value &content, Nepomuk::SimpleResource &resource, Nepomuk::SimpleResourceGraph &graph)
{
    if(m_replaceMode) {
        kDebug() << "remove all topics from" << m_publicationToReplace;
        QList<QUrl> resourceUris; resourceUris << m_publicationToReplace.uri();
        QList<QUrl> value; value << NAO::hasTopic();
        KJob *job = Nepomuk::removeProperties(resourceUris, value);
        job->exec();
    }

    foreach(QSharedPointer<ValueItem> vi, content) {
        Keyword *k = dynamic_cast<Keyword *>(vi.data());
        if(!k) { continue; }

        kDebug() << "add topic" << k->text();

        Nepomuk::PIMO::Topic topic;

        topic.addProperty( NAO::identifier(), QUrl::fromEncoded(k->text().toUtf8()) );
        topic.setTagLabel(k->text());

        graph << topic;
        resource.addProperty( NAO::hasTopic(), topic.uri());
    }
}

void BibTexToNepomukPipe::addZoteroSyncDetails(Nepomuk::SimpleResource &mainResource, Nepomuk::SimpleResource &referenceResource,
                                               Entry *e, Nepomuk::SimpleResourceGraph &graph)
{
    QString id = PlainTextValue::text(e->value(QLatin1String("zoterokey")));
    QString etag = PlainTextValue::text(e->value(QLatin1String("zoteroetag")));
    QString updated = PlainTextValue::text(e->value(QLatin1String("zoteroupdated")));
    QString parentId = PlainTextValue::text(e->value(QLatin1String("zoteroparent")));

    e->remove(QLatin1String("zoterokey"));
    e->remove(QLatin1String("zoteroetag"));
    e->remove(QLatin1String("zoteroupdated"));
    e->remove(QLatin1String("zoteroparent"));

    Nepomuk::SYNC::ServerSyncData serverSyncData;
    serverSyncData.addType(NIE::DataObject());

    // first set an identifier, when the object already exist we merge them together
    QString identifier = QLatin1String("zotero") + m_syncUserId + m_syncUrl + id;
    serverSyncData.addProperty( NAO::identifier(),QUrl::fromEncoded( identifier.toUtf8()) );
    serverSyncData.setProperty(NIE::url(), QUrl::fromEncoded(identifier.toUtf8())); // we need the url to make this unique and not merge it wth something else

    // now we set the new values
    serverSyncData.setProvider( QLatin1String("zotero") );
    serverSyncData.setUrl( m_syncUrl );
    serverSyncData.setUserId( m_syncUserId );
    serverSyncData.setId( id );
    serverSyncData.setEtag( etag );
    serverSyncData.setProperty( NUAO::lastModification(), updated);

    // now depending on what kind of mainResource we have, we add another TypeClass
    // helps to find the right data later on again and create the right links between Resource and syncData
    if(e->type() == QLatin1String("note")) {
        serverSyncData.setSyncDataType( SYNC::Note() );
        serverSyncData.setNote( mainResource.uri() );
        mainResource.setProperty(SYNC::serverSyncData(), serverSyncData.uri() );
    }
    else if(e->type() == QLatin1String("attachment")) {
        serverSyncData.setSyncDataType( SYNC::Attachment() );
        serverSyncData.setAttachment( mainResource.uri() );
        mainResource.setProperty(SYNC::serverSyncData(), serverSyncData );
    }
    else {
        serverSyncData.setSyncDataType( SYNC::BibResource() );
        serverSyncData.setPublication( mainResource.uri() );
        mainResource.setProperty(SYNC::serverSyncData(), serverSyncData );

        if(referenceResource.isValid()) {
            serverSyncData.setReference( referenceResource.uri() );
            referenceResource.setProperty(SYNC::serverSyncData(), serverSyncData );
        }
    }

    graph << serverSyncData;

    // check if the current item we added is a child item and need to be added to a parent too
    if( parentId.isEmpty() ) {
        return;
    }

    //now we do have a parent id, so find the parent and make the important isRealated relation
    QString query = "select DISTINCT ?r where {  "
                     "?r a sync:ServerSyncData . "
                     "?r sync:provider ?provider . FILTER regex(?provider, \"zotero\") "
                     "?r sync:userId ?userId . FILTER regex(?userId, \"" + m_syncUserId + "\") "
                     "?r sync:url ?url . FILTER regex(?url, \"" + m_syncUrl + "\") "
                     "?r sync:id ?id . FILTER regex(?id, \""+ parentId + "\") "
                     "}";

    QList<Nepomuk::Query::Result> results = Nepomuk::Query::QueryServiceClient::syncSparqlQuery(query);

    if(results.size() > 1 || results.isEmpty()) {
        kDebug() << "could not find the right sync details for the current parent item query" << "zotero" << m_syncUserId << m_syncUrl << parentId;
        return;
    }

    Nepomuk::Resource parentSyncResourceNepomuk = results.first().resource();
    Nepomuk::SimpleResource parentSyncResource(results.first().resource().uri());

    QUrl syncDataType = parentSyncResourceNepomuk.property(SYNC::syncDataType()).toUrl();

    if(syncDataType == SYNC::Attachment()) { //ignore
    }
    else if(syncDataType == SYNC::Note()) { //ignore
    }
    else if(syncDataType == SYNC::BibResource()) {
        // here we add the information that a note as related(a child) of the reference and the publication.
        // and that the attachment (nfo:FileDataObject) is the publicationOf/publishedAs of the nbib:Publication

        if( mainResource.contains(RDF::type(), PIMO::Note())) {
            Nepomuk::Resource parentPublicationxx = parentSyncResourceNepomuk.property(SYNC::publication()).toResource();
            Nepomuk::SimpleResource parentPublication(parentPublicationxx.uri());
            parentPublication.addProperty( NAO::isRelated(), mainResource.uri());
            mainResource.addProperty( NAO::isRelated(), parentPublication.uri());

            parentPublication.addProperty( NAO::hasSubResource(), mainResource.uri()); //delete note when publication is deleted

            Nepomuk::Resource parentReferencexx = parentSyncResourceNepomuk.property(SYNC::reference()).toResource();
            Nepomuk::SimpleResource parentReference(parentReferencexx.uri());
            parentReference.addProperty( NAO::isRelated(), mainResource.uri());
            mainResource.addProperty( NAO::isRelated(), parentReference.uri());
            graph << parentReference << parentPublication;
        }
        else if( mainResource.contains(RDF::type(), NFO::FileDataObject()) || mainResource.contains(RDF::type(), NFO::RemoteDataObject())) {
            Nepomuk::Resource parentPublicationxx = parentSyncResourceNepomuk.property(SYNC::publication()).toResource();
            Nepomuk::SimpleResource parentPublication(parentPublicationxx.uri());
            parentPublication.addProperty( NBIB::isPublicationOf(), mainResource.uri());
            mainResource.addProperty( NBIB::publishedAs(), parentPublication.uri());

            parentPublication.addProperty( NAO::hasSubResource(), mainResource.uri()); // delete file when publication is deleted
            graph << parentPublication;
        }
    }

    // this creates the link for the syncResources so we know how they are connected
    // connect child syncDetails to its parent syncDetails
    serverSyncData.setProperty(NAO::isRelated(), parentSyncResource );
    parentSyncResource.setProperty( SYNC::provider(), QLatin1String("zotero")); // we need to add some kind of property, or the resource is invalid

    graph << serverSyncData << parentSyncResource;

}

void BibTexToNepomukPipe::addContact(const Value &contentValue, Nepomuk::SimpleResource &resource, Nepomuk::SimpleResourceGraph &graph, QUrl contactProperty, QUrl contactType )
{
    if(m_replaceMode) {
        kDebug() << "remove all contacts from" << contactProperty;
        QList<QUrl> resourceUris; resourceUris << m_publicationToReplace.uri();
        QList<QUrl> value; value << contactProperty;
        KJob *job = Nepomuk::removeProperties(resourceUris, value);
        job->exec();
    }

    foreach(QSharedPointer<ValueItem> authorItem, contentValue) {
        //transform KBibTex representation of the name into my own Name
        Name author;
        Person *person = dynamic_cast<Person *>(authorItem.data());

        bool personNotInstitution = false;
        if(person) {
            author.first = person->firstName().toUtf8();
            author.last = person->lastName().toUtf8();
            author.suffix = person->suffix().toUtf8();
            author.full = author.first + QLatin1String(" ") + author.last + QLatin1String(" ") + author.suffix;
            author.full = author.full.trimmed();

            if(!contactType.isValid()) {
                personNotInstitution = true;
            }
        }
        else {
            author.full = PlainTextValue::text(*authorItem).toUtf8();
            author.full = m_macroLookup.value(author.full, author.full);

            if(!contactType.isValid()) {
                personNotInstitution = false;
            }
        }

        kDebug() << "add new contact" << author.full;

        // if we want to push the data into akonadi let akonadi handle everything
        if(m_addressbook.isValid()) {
            // first we search if there exist a contact with the same fullname in the nepomuk storage
            // that is handlet by akonadi
            QString query = "select DISTINCT ?r ?fullname where {"
                            "?r a nco:Contact ."
                            "?r a aneo:AkonadiDataObject ."
                            "?r nco:fullname ?fullname . FILTER ( regex (?fullname, \""+ author.full + "\") )"
                            "}";

            QList<Nepomuk::Query::Result> queryResult = Nepomuk::Query::QueryServiceClient::syncSparqlQuery(query);

            if(!queryResult.isEmpty()) {
                // take the already available object
                Nepomuk::SimpleResource contact;
                contact.setProperty(NIE::url(), queryResult.first().resource().property(NIE::url()).toString());
                contact.setProperty(NCO::fullname(), queryResult.first().resource().property(NCO::fullname()).toString());
                resource.addProperty(contactProperty, contact);
                graph << contact;
                return;
            }

            // if no contact with the same fullname exist that is managed by akonadi (aneo:AkonadiDataObject)
            // we create a new contact

            kDebug() << "add author" << author.full << "to akonadi collection" << m_addressbook.name();
            KABC::Addressee addr;
            addr.setFamilyName( author.last );
            addr.setGivenName( author.first );
            addr.setSuffix( author.suffix );
            addr.setName( author.full );
            addr.setFormattedName( author.full );

            Akonadi::Item item;
            item.setMimeType( KABC::Addressee::mimeType() );
            item.setPayload<KABC::Addressee>( addr );

            Akonadi::ItemCreateJob *job = new Akonadi::ItemCreateJob( item, m_addressbook );

            if ( !job->exec() ) {
                kDebug() << "Error:" << job->errorString();
            }

            // akonadi saves its contacts with a specific nepomuk url, we create another one with the same url
            // this the DMS will megre these resources. Akonadi will fill all necessary parts of the contact itself, as
            // from now on changes are made via Akonadi not nepomuk
            Nepomuk::NCO::Contact contact;
            contact.addType(NIE::InformationElement());
            contact.addType(NIE::DataObject());
            contact.setProperty(NIE::url(), job->item().url());

            graph << contact;
            resource.addProperty(contactProperty, contact);
        }
        // ok we did not want to push the resource into akonadi
        // create it the usual way
        else {
            // create new contact resource, duplicates will be merged by the DMS later on
            if( personNotInstitution ) {
                Nepomuk::NCO::PersonContact authorResource;
                authorResource.setFullname( author.full );
                authorResource.setNameGiven( author.first );
                authorResource.setNameFamily( author.last );
                QStringList suffixes;
                suffixes << author.suffix;
                authorResource.setNameHonorificSuffixs( suffixes );

                resource.addProperty(contactProperty, authorResource);
                graph << authorResource;
            }
            else {
                // So we didn't get a Person as contact but instead a single name, might be organization or something else
                Nepomuk::NCO::Contact authorResource;
                authorResource.setFullname( author.full );

                resource.addProperty(contactProperty, authorResource);
                graph << authorResource;
            }
        }
    } //end foreach
}

void BibTexToNepomukPipe::addValue(const QString &content, Nepomuk::SimpleResource &resource, QUrl property)
{
//    if(m_replaceMode) {
//        QList<QUrl> res;
//        if(resource.contains(RDF::type(), NBIB::Publication())) {
//             res << m_publicationToReplace.uri();
//        }
//        else {
//            res << m_referenceToReplace.uri();
//        }
//        QVariantList value; value << content;
//        Nepomuk::setProperty(res, property, value);
//    }
//    else {
        resource.setProperty(property, QString(content.toUtf8()));
//    }
}

void BibTexToNepomukPipe::addValueWithLookup(const QString &content, Nepomuk::SimpleResource &resource, QUrl property)
{
    QString utfContent = m_macroLookup.value(QString(content.toUtf8()), QString(content.toUtf8()));

//    if(m_replaceMode) {
//        QList<QUrl> res;
//        if(resource.contains(RDF::type(), NBIB::Publication())) {
//             res << m_publicationToReplace.uri();
//        }
//        else {
//            res << m_referenceToReplace.uri();
//        }
//        QVariantList value; value << utfContent;
//        Nepomuk::setProperty(res, property, value);
//    }
//    else {
        resource.setProperty(property, utfContent);
//    }
}
