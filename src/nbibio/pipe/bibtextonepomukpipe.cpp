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

#include <QtCore/QSharedPointer>
#include <QtCore/QDebug>

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

    // we start by fetching all contacts for the conflict checking
    // this reduce the need to query nepomuk with every new author again and again
    Nepomuk::Query::ResourceTypeTerm type( Nepomuk::Vocabulary::NCO::Contact() );
    Nepomuk::Query::Query query( type );
    QList<Nepomuk::Query::Result> queryResult = Nepomuk::Query::QueryServiceClient::syncQuery(query);
    foreach(const Nepomuk::Query::Result & nqr, queryResult) {
        QString fullname = nqr.resource().property(Nepomuk::Vocabulary::NCO::fullname()).toString();
        m_allContacts.insert(fullname, nqr.resource());
    }

    Nepomuk::Query::ResourceTypeTerm typeP( Nepomuk::Vocabulary::NBIB::Proceedings() );
    Nepomuk::Query::Query queryP( typeP );
    QList<Nepomuk::Query::Result> queryResultP = Nepomuk::Query::QueryServiceClient::syncQuery(queryP);
    foreach(const Nepomuk::Query::Result & nqr, queryResultP) {
        QString title = nqr.resource().property(Nepomuk::Vocabulary::NIE::title()).toString();
        m_allProceedings.insert(QString(title.toUtf8()), nqr.resource());
    }

    Nepomuk::Query::ResourceTypeTerm typeE( Nepomuk::Vocabulary::PIMO::Event() );
    Nepomuk::Query::Query queryE( typeE );
    QList<Nepomuk::Query::Result> queryResultE = Nepomuk::Query::QueryServiceClient::syncQuery(queryE);
    foreach(const Nepomuk::Query::Result & nqr, queryResultE) {
        QString title = nqr.resource().property(Nepomuk::Vocabulary::NIE::title()).toString();
        m_allPimoEvents.insert(QString(title.toUtf8()), nqr.resource());
    }

    Nepomuk::Query::ResourceTypeTerm typeS( Nepomuk::Vocabulary::NBIB::Series() );
    Nepomuk::Query::Query queryS( typeS );
    QList<Nepomuk::Query::Result> queryResultS = Nepomuk::Query::QueryServiceClient::syncQuery(queryS);
    foreach(const Nepomuk::Query::Result & nqr, queryResultS) {
        QString title = nqr.resource().property(Nepomuk::Vocabulary::NIE::title()).toString();
        m_allSeries.insert(QString(title.toUtf8()), nqr.resource());
    }

    Nepomuk::Query::ResourceTypeTerm typeC( Nepomuk::Vocabulary::NBIB::Collection() );
    Nepomuk::Query::Query queryC( typeC );
    QList<Nepomuk::Query::Result> queryResultC = Nepomuk::Query::QueryServiceClient::syncQuery(queryC);
    foreach(const Nepomuk::Query::Result & nqr, queryResultC) {
        QString title = nqr.resource().property(Nepomuk::Vocabulary::NIE::title()).toString();
        m_allCollection.insert(QString(title.toUtf8()), nqr.resource());
    }

    int maxValue = bibEntries.size();
    qreal perFileProgress = (100.0/(qreal)maxValue);

    //we start by filling the lookuptable for all macros
    foreach(QSharedPointer<Element> e, bibEntries ) {
        Macro *macro = dynamic_cast<Macro *>(e.data());
        if(macro) {
            m_macroLookup.insert(macro->key(), PlainTextValue::text(macro->value()));
        }
    }

    int i = 0;
    foreach(QSharedPointer<Element> e, bibEntries ) {
        Entry *entry = dynamic_cast<Entry *>(e.data());

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
    if(addressbook.isValid())
        m_addressbook = addressbook;
}

void BibTexToNepomukPipe::setSyncDetails(const QString &url, const QString &userid)
{
    m_syncUrl = url;
    m_syncUserId = userid;
}

void BibTexToNepomukPipe::import(Entry *e)
{
    // conference is just another form of inproceedings...
    if(e->type() == QLatin1String("conference")) {
        e->setType(QLatin1String("inproceedings"));
    }

    //do not check duplicates, just add new resources to the system storage
    QUrl typeUrl = typeToUrl(e->type().toLower());
    Nepomuk::Resource publication = Nepomuk::Resource(QUrl(), typeUrl);

    Nepomuk::Resource reference = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::Reference());
    reference.setProperty(Nepomuk::Vocabulary::NBIB::citeKey(), e->id());
    reference.setLabel(e->id()); // adds no real value, but looks nicer in the Nepomuk shell
    reference.setProperty(Nepomuk::Vocabulary::NBIB::publication(), publication);
    publication.addProperty(Nepomuk::Vocabulary::NBIB::reference(), reference);

    // add zotero sync details / this only adds new information
    // if we updated the details, we need to change that differently
    if(e->contains(QLatin1String("zoterokey"))) {
        addZoteroSyncDetails(publication,
        reference,
        PlainTextValue::text(e->value(QLatin1String("zoterokey"))),
        PlainTextValue::text(e->value(QLatin1String("zoteroetag"))),
        PlainTextValue::text(e->value(QLatin1String("zoteroupdated"))));

        e->remove(QLatin1String("zoterokey"));
        e->remove(QLatin1String("zoteroetag"));
        e->remove(QLatin1String("zoteroupdated"));
    }

    //before we go through the whole list one by one, we take care of some special cases

    // I. publisher/school/institution + address
    //    means address belongs to publisher
    if(e->contains(QLatin1String("address"))) {
        Value publisher;
        if(e->contains(QLatin1String("publisher"))) {
            publisher = e->value(QLatin1String("publisher"));
            e->remove(QLatin1String("publisher"));
        }
        else if(e->contains(QLatin1String("school"))) {
            publisher = e->value(QLatin1String("school"));
            e->remove(QLatin1String("school"));
        }
        else if(e->contains(QLatin1String("institution"))) {
            publisher = e->value(QLatin1String("institution"));
            e->remove(QLatin1String("institution"));
        }

        if(!publisher.isEmpty()) {
            addPublisher(publisher, e->value(QLatin1String("address")),  publication);
            e->remove(QLatin1String("address"));
        }
    }

    // II. a) encyclopediaarticle
    if(e->contains(QLatin1String("articletype"))) {
        QUrl seriesURL;
        QUrl issueURL;

        QString type = PlainTextValue::text(e->value(QLatin1String("articletype")));
        if(type == QLatin1String("encyclopedia")) {
            seriesURL = Nepomuk::Vocabulary::NBIB::Series();
            issueURL = Nepomuk::Vocabulary::NBIB::Encyclopedia();

            addEncyclopedia(e->value(QLatin1String("booktitle")),
            publication, issueURL);

            e->remove(QLatin1String("journal"));
            e->remove(QLatin1String("booktitle"));
            e->remove(QLatin1String("number"));
            e->remove(QLatin1String("volume"));
            e->remove(QLatin1String("articletype"));
        }

    }

    // II b). journal + number + volume + zotero articletype
    if(e->contains(QLatin1String("journal"))) {
        QUrl seriesURL;
        QUrl issueURL;

        QString type = PlainTextValue::text(e->value(QLatin1String("articletype")));
        if(type == QLatin1String("magazine")) {
            seriesURL = Nepomuk::Vocabulary::NBIB::Magazin();
            issueURL = Nepomuk::Vocabulary::NBIB::MagazinIssue();
        }
        else if(type == QLatin1String("newspaper")) {
            seriesURL = Nepomuk::Vocabulary::NBIB::Newspaper();
            issueURL = Nepomuk::Vocabulary::NBIB::NewspaperIssue();
        }
        else {
            seriesURL = Nepomuk::Vocabulary::NBIB::Journal();
            issueURL = Nepomuk::Vocabulary::NBIB::JournalIssue();
        }

        addJournal(e->value(QLatin1String("journal")),
                   e->value(QLatin1String("volume")),
                   e->value(QLatin1String("number")),
                   publication, seriesURL, issueURL);

        e->remove(QLatin1String("journal"));
        e->remove(QLatin1String("number"));
        e->remove(QLatin1String("volume"));
        e->remove(QLatin1String("articletype"));
    }

    // III. archivePrefix + eprint
    //TODO implement archivePrefix stuff

    //now go through the list of all remaining entries
    QMapIterator<QString, Value> i(*e);
    while (i.hasNext()) {
        i.next();
        addContent(i.key().toLower(), i.value(), publication, reference, e->type());
    }

    if(publication.hasType(Nepomuk::Vocabulary::NBIB::Collection()) ) {
        m_allProceedings.insert(publication.property(Nepomuk::Vocabulary::NIE::title()).toString(), publication);
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
        return Nepomuk::Vocabulary::NBIB::Book();
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
    else if(entryType == QLatin1String("manuscript")) {
        return Nepomuk::Vocabulary::NBIB::Script();
    }
    else if(entryType == QLatin1String("techreport")) {
        return Nepomuk::Vocabulary::NBIB::Techreport();
    }
    else if(entryType == QLatin1String("report")) {
        return Nepomuk::Vocabulary::NBIB::Report();
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
    else if(entryType == QLatin1String("standard")) {
        return Nepomuk::Vocabulary::NBIB::Standard();
    }
    else if(entryType == QLatin1String("statute")) {
        return Nepomuk::Vocabulary::NBIB::Statute();
    }
    else if(entryType == QLatin1String("case")) {
        return Nepomuk::Vocabulary::NBIB::LegalCaseDocument();
    }
    else if(entryType == QLatin1String("bill")) {
        return Nepomuk::Vocabulary::NBIB::Bill();
    }
    else if(entryType == QLatin1String("encyclopediaarticle")) {
        return Nepomuk::Vocabulary::NBIB::Article();
    }
    else if(entryType == QLatin1String("dictionaryentry") ||
    entryType == QLatin1String("dictionary")) {
        return Nepomuk::Vocabulary::NBIB::Dictionary();
    }
    else if(entryType == QLatin1String("forumpost")) {
        return Nepomuk::Vocabulary::NBIB::ForumPost();
    }
    else if(entryType == QLatin1String("blogpost")) {
        return Nepomuk::Vocabulary::NBIB::BlogPost();
    }
    else if(entryType == QLatin1String("webpage")) {
        return Nepomuk::Vocabulary::NBIB::WebPage();
    }
    else if(entryType == QLatin1String("misc")) {
        return Nepomuk::Vocabulary::NBIB::Publication();
    }
    else {
        // same as @Misc
        qWarning() << "BibTexToNepomukPipe::typeToUrl unknown type" << entryType;
        return Nepomuk::Vocabulary::NBIB::Publication();
    }
}

Entry *BibTexToNepomukPipe::getDiff(Nepomuk::Resource local, Entry *externalEntry, bool keepLocal)
{
    //first we transform the nepomuk resource to a flat bibtex entry
    QList<Nepomuk::Resource> resources;
    resources.append(local);
    NepomukToBibTexPipe ntbp;
    ntbp.pipeExport(resources);

    File *localBibFile = ntbp.bibtexFile();

    Entry *localEntry = static_cast<Entry *>(localBibFile->first().data());
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
    m_replaceMode = true;

    Nepomuk::Resource publication = syncResource.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();
    Nepomuk::Resource reference = syncResource.property(Nepomuk::Vocabulary::NBIB::reference()).toResource();

    if(!publication.isValid())
        publication = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::Publication());

    if(!reference.isValid())
        reference = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::Reference());

    Entry *diffEntry = getDiff(syncResource, external, keepLocal);

    qDebug() << "created the diff" << diffEntry->size() << "changed entries";

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

    Nepomuk::Query::ResourceTypeTerm typeP( Nepomuk::Vocabulary::NBIB::Proceedings() );
    Nepomuk::Query::Query queryP( typeP );
    QList<Nepomuk::Query::Result> queryResultP = Nepomuk::Query::QueryServiceClient::syncQuery(queryP);
    foreach(const Nepomuk::Query::Result & nqr, queryResultP) {
        QString title = nqr.resource().property(Nepomuk::Vocabulary::NIE::title()).toString();
        m_allProceedings.insert(QString(title.toUtf8()), nqr.resource());
    }

    int maxValue = diffEntry->size();
    qreal perFileProgress = (100.0/(qreal)maxValue);
    qreal curProcess = 0.0;

    // update zotero sync details
    if(diffEntry->contains(QLatin1String("zoterokey"))) {
        addZoteroSyncDetails(publication,
        reference,
        PlainTextValue::text(diffEntry->value(QLatin1String("zoterokey"))),
        PlainTextValue::text(diffEntry->value(QLatin1String("zoteroetag"))),
        PlainTextValue::text(diffEntry->value(QLatin1String("zoteroupdated"))));

        diffEntry->remove(QLatin1String("zoterokey"));
        diffEntry->remove(QLatin1String("zoteroetag"));
        diffEntry->remove(QLatin1String("zoteroupdated"));
    }

    //go through the list of all remaining entries
    QMapIterator<QString, Value> i(*diffEntry);
    while (i.hasNext()) {
        i.next();
        addContent(i.key().toLower(), i.value(), publication, reference, diffEntry->type());
        curProcess += perFileProgress;
        emit progress(curProcess);
    }

    emit progress(100);
}

void BibTexToNepomukPipe::addContent(const QString &key, const Value &value, Nepomuk::Resource publication, Nepomuk::Resource reference, const QString & originalEntryType)
{
    if(key == QLatin1String("abstract")) {
        addAbstract(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("annote")) {
        addAnnote(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("archive")) {
        addArchive(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("archivelocation")) {
        addArchiveLocation(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("librarycatalog")) {
        addLibraryCatalog(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("author")) {
        addAuthor(value, publication, reference, originalEntryType);
    }
    else if(key == QLatin1String("bookauthor")) {
        addBookAuthor(value, publication);
    }
    else if(key == QLatin1String("contributor")) {
        addContributor(value, publication);
    }
    else if(key == QLatin1String("translator")) {
        addTranslator(value, publication);
    }
    else if(key == QLatin1String("reviewedauthor")) {
        addReviewedAuthor(value, publication);
    }
    else if(key == QLatin1String("serieseditor")) {
        addSeriesEditor(value, publication);
    }
    else if(key == QLatin1String("booktitle")) {
        addBooktitle(PlainTextValue::text(value), publication, originalEntryType);
    }
    else if(key == QLatin1String("chapter")) {
        addChapter(PlainTextValue::text(value), publication, reference);
    }
    else if(key == QLatin1String("copyright")) {
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
        Value empty;
        addPublisher(value, empty, publication);
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
    else if(key == QLatin1String("organization") ||
    key == QLatin1String("legislativebody") ) {
        addOrganization(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("code")) {
        addCode(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("codenumber")) {
        addCodeNumber(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("codevolume")) {
        addCodeVolume(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("reporter")) {
        addReporter(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("reportervolume")) {
        addReporterVolume(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("pages")) {
        addPages(PlainTextValue::text(value), reference);
    }
    else if(key == QLatin1String("numpages") || key == QLatin1String("numberofpages")) {
        addNumberOfPages(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("numberofvolumes")) {
        addNumberOfVolumes(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("pubmed")) {
        addPubMed(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("publisher")) {
        Value empty;
        addPublisher(value, empty, publication);
    }
    else if(key == QLatin1String("school")) {
        Value empty;
        addPublisher(value, empty, publication);
    }
    else if(key == QLatin1String("series")) {
        addSeries(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("conferencename") ||
    key == QLatin1String("meetingname") ||
    key == QLatin1String("event")) {
        addEvent(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("title")) {
        addTitle(PlainTextValue::text(value), publication, reference, originalEntryType);
    }
    else if(key == QLatin1String("shorttitle")) {
        addShortTitle(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("type")) {
        addType(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("url") ||
    key == QLatin1String("localfile") ||
    key == QLatin1String("biburl") ||
    key == QLatin1String("bibsource") ||
    key == QLatin1String("ee")) {
        addUrl(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("address")) {
        QString addressValue = PlainTextValue::text(value);
        if(addressValue.contains(QLatin1String("http:"))) {
            addUrl(addressValue, publication);
        }
        //ignore else case, as the address as a postal address is handled above when a publisher is available
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
    else if(key == QLatin1String("accessdate")) {
        addLastUsage(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("applicationnumber")) {
        addApplicationNumber(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("prioritynumbers")) {
        addPriorityNumbers(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("legalstatus")) {
        addLegalStatus(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("references")) {
        addReferences(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("filingdate")) {
        addFilingDate(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("assignee")) {
        addAssignee(value, publication);
    }
    else if(key == QLatin1String("date")) {
        addDate(PlainTextValue::text(value), publication);
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

        addKewords(keywordList, publication);
    }
    else {
        qDebug() << "BibTexToNepomukPipe::addContent | unknown key ::" << key << PlainTextValue::text(value);
    }
}

void BibTexToNepomukPipe::addPublisher(const Value &publisherValue, const Value &addressValue, Nepomuk::Resource publication)
{
    if(m_replaceMode) {
        publication.removeProperty(Nepomuk::Vocabulary::NCO::publisher());
    }

    QString address = PlainTextValue::text(addressValue).toUtf8();
    address = m_macroLookup.value(address, address);

    // create the address object
    Nepomuk::Resource addr(QUrl(), Nepomuk::Vocabulary::NCO::PostalAddress());
    //FIXME extendedAddress is not correct, but determining which part of the @p address is the street/location and so on is nearly impossible

    if(!address.isEmpty())
        addr.setProperty(Nepomuk::Vocabulary::NCO::extendedAddress(), address);

    foreach(QSharedPointer<ValueItem> publisherItem, publisherValue) {
        //transform KBibTex representation of the name into my own Name
        Name publisher;
        Person *person = dynamic_cast<Person *>(publisherItem.data());
        if(person) {
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

        //check if the publisher already exist in the database
        Nepomuk::Resource p = m_allContacts.value(publisher.full, Nepomuk::Resource());

        if(!p.isValid()) {
            qDebug() << "create a new Publisher resource for " << publisher.full;
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

void BibTexToNepomukPipe::addJournal(const Value &journalValue, const Value &volumeValue, const Value &numberValue, Nepomuk::Resource publication, QUrl seriesUrl, QUrl issueUrl)
{
    QString journalName = PlainTextValue::text(journalValue).toUtf8();
    journalName = m_macroLookup.value(journalName, journalName);
    QString volume = PlainTextValue::text(volumeValue).toUtf8();
    QString number = PlainTextValue::text(numberValue).toUtf8();

    //find existing journal or create a new series of them
    Nepomuk::Resource journalResource;
    Nepomuk::Resource journalIssue;

    //check if the publisher already exist in the database
    journalResource = m_allSeries.value(journalName, Nepomuk::Resource());

    if(!journalResource.isValid()) {
        qDebug() << "no existing journal for" << journalName << "with type" << seriesUrl;
        journalResource = Nepomuk::Resource(QUrl(), seriesUrl);
        journalResource.addType(Nepomuk::Vocabulary::NBIB::Series()); // seems to be a bug, not the full hierachry will be set otherwise
        journalResource.addType(Nepomuk::Vocabulary::NIE::InformationElement());
        journalResource.setProperty(Nepomuk::Vocabulary::NIE::title(), journalName);
        m_allSeries.insert(journalName, journalResource);
    }

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
        journalIssue = Nepomuk::Resource(QUrl(), issueUrl);
        journalIssue.addType(Nepomuk::Vocabulary::NBIB::Collection());
        journalIssue.addType(Nepomuk::Vocabulary::NBIB::Publication());
        journalIssue.addType(Nepomuk::Vocabulary::NIE::InformationElement());
        journalIssue.setProperty(Nepomuk::Vocabulary::NBIB::number(), number);
        journalIssue.setProperty(Nepomuk::Vocabulary::NBIB::volume(), volume);
        // duplicate title join journal and journalissue, helps to easily identify those two
        // but is more like a better way to create a prefLabel / genericLabel
        QString issueName = QString("%1 : %2 (%3)").arg(journalName).arg(volume).arg(number);
        journalIssue.setProperty(Nepomuk::Vocabulary::NIE::title(), issueName);

        // connect issue <-> journal
        journalIssue.setProperty(Nepomuk::Vocabulary::NBIB::inSeries(), journalResource);
        journalResource.addProperty(Nepomuk::Vocabulary::NBIB::seriesOf(), journalIssue);
    }

    // now connect the issue to the Publication/Collection
    publication.setProperty(Nepomuk::Vocabulary::NBIB::collection(), journalIssue);
    journalIssue.addProperty(Nepomuk::Vocabulary::NBIB::article(), publication);
}

void BibTexToNepomukPipe::addEncyclopedia(const Value &journalValue, Nepomuk::Resource publication,QUrl seriesUrl,QUrl issueUrl)
{
    QString encyclopediaName = PlainTextValue::text(journalValue).toUtf8();
    encyclopediaName = m_macroLookup.value(encyclopediaName, encyclopediaName);

    Nepomuk::Resource encyclopediaResource;

    //check if the publisher already exist in the database
    encyclopediaResource = m_allCollection.value(encyclopediaName, Nepomuk::Resource());

    if(!encyclopediaResource.isValid()) {
        qDebug() << "no existing encyclopedia for" << encyclopediaName << "with type" << seriesUrl;
        encyclopediaResource = Nepomuk::Resource(QUrl(), issueUrl);
        encyclopediaResource.addType(Nepomuk::Vocabulary::NBIB::Collection());
        encyclopediaResource.addType(Nepomuk::Vocabulary::NBIB::Publication());
        encyclopediaResource.addType(Nepomuk::Vocabulary::NIE::InformationElement());
        encyclopediaResource.setProperty(Nepomuk::Vocabulary::NIE::title(), encyclopediaName);
    }

    // now connect the issue to the Publication/Collection
    publication.setProperty(Nepomuk::Vocabulary::NBIB::collection(), encyclopediaResource);
    encyclopediaResource.addProperty(Nepomuk::Vocabulary::NBIB::article(), publication);
}

void BibTexToNepomukPipe::addAbstract(const QString &content, Nepomuk::Resource publication)
{
    QString utfContent = m_macroLookup.value(QString(content.toUtf8()), QString(content.toUtf8()));
    publication.setProperty(Nepomuk::Vocabulary::NBIB::abstract(), utfContent);
}

void BibTexToNepomukPipe::addAnnote(const QString &content, Nepomuk::Resource publication)
{
    QString utfContent = m_macroLookup.value(QString(content.toUtf8()), QString(content.toUtf8()));
    publication.setProperty(Nepomuk::Vocabulary::NIE::comment(), utfContent);
}

void BibTexToNepomukPipe::addAuthor(const Value &contentValue, Nepomuk::Resource publication, Nepomuk::Resource reference, const QString & originalEntryType)
{
    //in case of @incollection the author is used to identify who wrote the chapter not the complete book/collection
    Nepomuk::Resource authorResource;

    if(originalEntryType == QLatin1String("incollection") ) {
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

    addContact(contentValue, authorResource, Nepomuk::Vocabulary::NCO::creator(), Nepomuk::Vocabulary::NCO::PersonContact());
}

void BibTexToNepomukPipe::addBooktitle(const QString &content, Nepomuk::Resource publication, const QString & originalEntryType)
{
    QString utfContent = m_macroLookup.value(QString(content.toUtf8()), QString(content.toUtf8()));

    //two specialities occur here
    // I. "booktitle" means the title of a book, where "title" than means the title of the article in the book where the author fits to
    // this is valid for any publication other than @InProceedings
    // II. "booktitle" marks the title of the @proceedings from an @InProceedings or @Conference

    if(originalEntryType == QLatin1String("inproceedings")) {
        //check if a resource @Proceedings with the name of content exist or create a new one

        Nepomuk::Resource proceedingsResource = m_allProceedings.value(utfContent, Nepomuk::Resource());

        if(!proceedingsResource.isValid()) {
            qDebug() << "found no existing proceedings with the name " << utfContent << "create new one";
            if(originalEntryType == QLatin1String("inproceedings"))
                proceedingsResource = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::Proceedings());
            else if(originalEntryType == QLatin1String("encyclopediaarticle"))
                proceedingsResource = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::Encyclopedia());

            proceedingsResource.setProperty(Nepomuk::Vocabulary::NIE::title(), utfContent);
            m_allProceedings.insert(utfContent, proceedingsResource);
        }

        //at this point we have a valid proceedings entry connect it to the publication
        //The publication (@inproceedings) is an article while the @Proceedings is a collection
        publication.setProperty(Nepomuk::Vocabulary::NBIB::collection(), proceedingsResource);
        proceedingsResource.addProperty(Nepomuk::Vocabulary::NBIB::article(), publication);
    }
    else {
        publication.setProperty(Nepomuk::Vocabulary::NIE::title(), utfContent);
    }
}

void BibTexToNepomukPipe::addArchive(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NBIB::archive(), QString(content.toUtf8()));
}

void BibTexToNepomukPipe::addArchiveLocation(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NBIB::archiveLocation(), QString(content.toUtf8()));
}

void BibTexToNepomukPipe::addLibraryCatalog(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NBIB::libraryCatalog(), QString(content.toUtf8()));
}

void BibTexToNepomukPipe::addBookAuthor(const Value &contentValue, Nepomuk::Resource publication)
{
    // bookauthor is a Zotero key for the @incollection import.
    // add author to the publication (normal author in this case is related to the chapter)
    Nepomuk::Resource authorResource = publication;

    addContact(contentValue, authorResource, Nepomuk::Vocabulary::NCO::creator(), Nepomuk::Vocabulary::NCO::PersonContact());
}

void BibTexToNepomukPipe::addSeriesEditor(const Value &contentValue, Nepomuk::Resource publication)
{
    Nepomuk::Resource seriesResource = publication.property(Nepomuk::Vocabulary::NBIB::inSeries()).toResource();

    if(!seriesResource.isValid()) {
        seriesResource = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::Series());
    }

    addContact(contentValue, seriesResource, Nepomuk::Vocabulary::NBIB::editor(), Nepomuk::Vocabulary::NCO::PersonContact());
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

    QString utfContent = m_macroLookup.value(QString(content.toUtf8()), QString(content.toUtf8()));
    chapterResource.setProperty( Nepomuk::Vocabulary::NBIB::chapterNumber(), utfContent);
}

void BibTexToNepomukPipe::addCopyrigth(const QString &content, Nepomuk::Resource publication)
{
    QString utfContent = m_macroLookup.value(QString(content.toUtf8()), QString(content.toUtf8()));
    publication.setProperty(Nepomuk::Vocabulary::NIE::copyright(), utfContent);
}

void BibTexToNepomukPipe::addDoi(const QString &content, Nepomuk::Resource publication)
{
    QString utfContent = m_macroLookup.value(QString(content.toUtf8()), QString(content.toUtf8()));
    publication.setProperty(Nepomuk::Vocabulary::NBIB::doi(), utfContent);
}

void BibTexToNepomukPipe::addEdition(const QString &content, Nepomuk::Resource publication)
{
    QString utfContent = m_macroLookup.value(QString(content.toUtf8()), QString(content.toUtf8()));
    publication.setProperty(Nepomuk::Vocabulary::NBIB::edition(), utfContent);
}

void BibTexToNepomukPipe::addEditor(const Value &contentValue, Nepomuk::Resource publication)
{
    //if we merge entries, we remove the old one first and add the new oney later on again
    if(m_replaceMode) {
        publication.removeProperty(Nepomuk::Vocabulary::NBIB::editor());
    }

    addContact(contentValue, publication, Nepomuk::Vocabulary::NBIB::editor(), Nepomuk::Vocabulary::NCO::PersonContact());
}

void BibTexToNepomukPipe::addEprint(const QString &content, Nepomuk::Resource publication)
{
    QString utfContent = m_macroLookup.value(QString(content.toUtf8()), QString(content.toUtf8()));
    publication.setProperty(Nepomuk::Vocabulary::NBIB::eprint(), utfContent);
}

void BibTexToNepomukPipe::addHowPublished(const QString &content, Nepomuk::Resource publication)
{
    QString utfContent = m_macroLookup.value(QString(content.toUtf8()), QString(content.toUtf8()));
    publication.setProperty(Nepomuk::Vocabulary::NBIB::publicationMethod(), utfContent);
}

void BibTexToNepomukPipe::addIsbn(const QString &content, Nepomuk::Resource publication)
{
    QString utfContent = m_macroLookup.value(QString(content.toUtf8()), QString(content.toUtf8()));
    publication.setProperty(Nepomuk::Vocabulary::NBIB::isbn(), utfContent);
}

void BibTexToNepomukPipe::addIssn(const QString &content, Nepomuk::Resource publication)
{
    QString utfContent = m_macroLookup.value(QString(content.toUtf8()), QString(content.toUtf8()));

    //fetch already existing Series or create a new one
    Nepomuk::Resource journalIssue = publication.property(Nepomuk::Vocabulary::NBIB::collection()).toResource();

    if(!journalIssue.isValid()) {
        qDebug() << "BibTexToNepomukPipe::addIssn | try to set ISSN but no journalissue available";
        publication.setProperty(Nepomuk::Vocabulary::NBIB::issn(), utfContent);
        return;
    }

    Nepomuk::Resource series = journalIssue.property(Nepomuk::Vocabulary::NBIB::inSeries()).toResource();

    if(!series.isValid()) {
        series = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::Journal());
        series.addType(Nepomuk::Vocabulary::NBIB::Series()); // seems to be a bug, not the full hierachry will be set otherwise
        series.addType(Nepomuk::Vocabulary::NIE::InformationElement());
        series.addProperty(Nepomuk::Vocabulary::NBIB::seriesOf(), publication);
        publication.setProperty(Nepomuk::Vocabulary::NBIB::inSeries(), series);
    }

    series.setProperty(Nepomuk::Vocabulary::NBIB::issn(), utfContent);
}

void BibTexToNepomukPipe::addLanguage(const QString &content, Nepomuk::Resource publication)
{
    QString utfContent = m_macroLookup.value(QString(content.toUtf8()), QString(content.toUtf8()));
    publication.setProperty(Nepomuk::Vocabulary::NIE::language(), utfContent);
}

void BibTexToNepomukPipe::addLccn(const QString &content, Nepomuk::Resource publication)
{
    QString utfContent = m_macroLookup.value(QString(content.toUtf8()), QString(content.toUtf8()));
    publication.setProperty(Nepomuk::Vocabulary::NBIB::lccn(), utfContent);
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

    QString contentMonth = content.toLower().toUtf8();
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
    else {
        // assume the month was already a number
        month = contentMonth;
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
    publication.setProperty(Nepomuk::Vocabulary::NBIB::mrNumber(), QString(content.toUtf8()));
}

void BibTexToNepomukPipe::addNote(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NIE::description(), QString(content.toUtf8()));
}

void BibTexToNepomukPipe::addNumber(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NBIB::number(), QString(content.toUtf8()));
}

void BibTexToNepomukPipe::addOrganization(const QString &content, Nepomuk::Resource publication)
{
    QString utfContent = m_macroLookup.value(QString(content.toUtf8()), QString(content.toUtf8()));

    //check if the organization already exist in the database
    Nepomuk::Resource organizationResource = m_allContacts.value(utfContent, Nepomuk::Resource());

    if(!organizationResource.isValid()) {
        qDebug() << "create a new OrganizationContact resource for " << utfContent;
        organizationResource = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NCO::OrganizationContact());
        organizationResource.setProperty(Nepomuk::Vocabulary::NCO::fullname(), utfContent);

        m_allContacts.insert(utfContent, organizationResource);
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

void BibTexToNepomukPipe::addCode(const QString &content, Nepomuk::Resource publication)
{
    Nepomuk::Resource codeOfLaw = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::CodeOfLaw());
    codeOfLaw.setProperty(Nepomuk::Vocabulary::NIE::title(), QString(content.toUtf8()));
    publication.setProperty(Nepomuk::Vocabulary::NBIB::codeOfLaw(), codeOfLaw);
}

void BibTexToNepomukPipe::addCodeNumber(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NBIB::codeNumber(), QString(content.toUtf8()));
}

void BibTexToNepomukPipe::addCodeVolume(const QString &content, Nepomuk::Resource publication)
{
    Nepomuk::Resource codeOfLaw = publication.property(Nepomuk::Vocabulary::NBIB::codeOfLaw()).toResource();
    codeOfLaw.setProperty(Nepomuk::Vocabulary::NBIB::volume(), QString(content.toUtf8()));
}

void BibTexToNepomukPipe::addReporter(const QString &content, Nepomuk::Resource publication)
{
    Nepomuk::Resource courtReporter = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::CourtReporter());
    courtReporter.setProperty(Nepomuk::Vocabulary::NIE::title(), QString(content.toUtf8()));
    publication.setProperty(Nepomuk::Vocabulary::NBIB::courtReporter(), courtReporter);
}

void BibTexToNepomukPipe::addReporterVolume(const QString &content, Nepomuk::Resource publication)
{
    Nepomuk::Resource courtReporter = publication.property(Nepomuk::Vocabulary::NBIB::courtReporter()).toResource();
    courtReporter.setProperty(Nepomuk::Vocabulary::NBIB::volume(), QString(content.toUtf8()));
}

void BibTexToNepomukPipe::addPages(const QString &content, Nepomuk::Resource reference)
{
    reference.setProperty(Nepomuk::Vocabulary::NBIB::pages(), QString(content.toUtf8()));
}

void BibTexToNepomukPipe::addNumberOfPages(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NBIB::numberOfPages(), QString(content.toUtf8()));
}

void BibTexToNepomukPipe::addNumberOfVolumes(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NBIB::numberOfVolumes(), QString(content.toUtf8()));
}

void BibTexToNepomukPipe::addPubMed(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NBIB::pubMed(), QString(content.toUtf8()));
}

void BibTexToNepomukPipe::addEvent(const QString &content, Nepomuk::Resource publication)
{
    //find existing pimo::Event with the same name
    Nepomuk::Resource eventResource;
    Nepomuk::Resource pimoEventResource = m_allPimoEvents.value(QString(content.toUtf8()), Nepomuk::Resource());
    if(pimoEventResource.isValid()) {
        eventResource = pimoEventResource;
    }
    else {
        eventResource = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::PIMO::Event());
        eventResource.setProperty(Nepomuk::Vocabulary::NIE::title(), QString(content.toUtf8()));
        m_allPimoEvents.insert( QString(content.toUtf8()), eventResource);
    }

    eventResource.addProperty(Nepomuk::Vocabulary::NBIB::eventPublication(), publication);
    publication.setProperty(Nepomuk::Vocabulary::NBIB::event(), eventResource);
}

void BibTexToNepomukPipe::addSeries(const QString &content, Nepomuk::Resource publication)
{
    QUrl seriesType;
    if(publication.hasType(Nepomuk::Vocabulary::NBIB::Book()) ||
    publication.hasType(Nepomuk::Vocabulary::NBIB::Booklet()) ) {
        seriesType = Nepomuk::Vocabulary::NBIB::BookSeries();
    }
    else {
        seriesType = Nepomuk::Vocabulary::NBIB::Series();
    }

    //find existing series or create a new series of them
    Nepomuk::Resource seriesResource;
    QString utfContent = m_macroLookup.value(QString(content.toUtf8()), QString(content.toUtf8()));

    // check if a series with the same name already exist

    seriesResource = m_allSeries.value(utfContent, Nepomuk::Resource());

    if(!seriesResource.isValid()) {
        qDebug() << "did not find existing series for name" << utfContent;
        seriesResource = Nepomuk::Resource(QUrl(), seriesType);
        seriesResource.addType(Nepomuk::Vocabulary::NBIB::Series()); // seems to be a bug, not the full hierachry will be set otherwise
        seriesResource.addType(Nepomuk::Vocabulary::NIE::InformationElement());
        seriesResource.setProperty(Nepomuk::Vocabulary::NIE::title(), utfContent);
        m_allSeries.insert(utfContent, seriesResource);
    }

    seriesResource.addProperty(Nepomuk::Vocabulary::NBIB::seriesOf(), publication);
    publication.setProperty(Nepomuk::Vocabulary::NBIB::inSeries(), seriesResource);
}

void BibTexToNepomukPipe::addTitle(const QString &content, Nepomuk::Resource publication, Nepomuk::Resource reference, const QString & originalEntryType)
{
    QString utfContent = m_macroLookup.value(QString(content.toUtf8()), QString(content.toUtf8()));

    // in the case of @InCollection title means title of the article in the book
    // while booktitle is the actual title of the book
    if(originalEntryType == QLatin1String("incollection")) {

        Nepomuk::Resource chapterResource = reference.property(Nepomuk::Vocabulary::NBIB::referencedPart()).toResource();

        if(!chapterResource.isValid()) {
            chapterResource = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::Chapter());
            reference.setProperty(Nepomuk::Vocabulary::NBIB::referencedPart(), chapterResource);
            publication.addProperty(Nepomuk::Vocabulary::NBIB::documentPart(), chapterResource);
            chapterResource.setProperty(Nepomuk::Vocabulary::NBIB::documentPartOf(), publication);
        }

        chapterResource.setProperty( Nepomuk::Vocabulary::NIE::title(), utfContent);
    }
    // for a dictionary entry the title is the name of the chapter
    else if(originalEntryType == QLatin1String("dictionaryentry")) {

        Nepomuk::Resource chapterResource = reference.property(Nepomuk::Vocabulary::NBIB::referencedPart()).toResource();

        if(!chapterResource.isValid()) {
            chapterResource = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::Chapter());
            reference.setProperty(Nepomuk::Vocabulary::NBIB::referencedPart(), chapterResource);
            publication.addProperty(Nepomuk::Vocabulary::NBIB::documentPart(), chapterResource);
            chapterResource.setProperty(Nepomuk::Vocabulary::NBIB::documentPartOf(), publication);
        }

        chapterResource.setProperty( Nepomuk::Vocabulary::NIE::title(), utfContent);
    }
    else {
        publication.setProperty(Nepomuk::Vocabulary::NIE::title(), utfContent);
    }
}

void BibTexToNepomukPipe::addShortTitle(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NBIB::shortTitle(), QString(content.toUtf8()));
}

void BibTexToNepomukPipe::addType(const QString &content, Nepomuk::Resource publication)
{
    QString utfContent = m_macroLookup.value(QString(content.toUtf8()), QString(content.toUtf8()));
    publication.setProperty(Nepomuk::Vocabulary::NBIB::type(), utfContent);
}

void BibTexToNepomukPipe::addApplicationNumber(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NBIB::applicationNumber(), QString(content.toUtf8()));
}

void BibTexToNepomukPipe::addPriorityNumbers(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NBIB::priorityNumbers(), QString(content.toUtf8()));
}

void BibTexToNepomukPipe::addAssignee(const Value &contentValue, Nepomuk::Resource publication)
{
    addContact(contentValue, publication, Nepomuk::Vocabulary::NBIB::assignee(), Nepomuk::Vocabulary::NCO::Contact());
}

void BibTexToNepomukPipe::addContributor(const Value &contentValue, Nepomuk::Resource publication)
{
    addContact(contentValue, publication, Nepomuk::Vocabulary::NBIB::contributor(), Nepomuk::Vocabulary::NCO::PersonContact());
}

void BibTexToNepomukPipe::addTranslator(const Value &contentValue, Nepomuk::Resource publication)
{
    addContact(contentValue, publication, Nepomuk::Vocabulary::NBIB::translator(), Nepomuk::Vocabulary::NCO::PersonContact());
}

void BibTexToNepomukPipe::addReviewedAuthor(const Value &contentValue, Nepomuk::Resource publication)
{
    addContact(contentValue, publication, Nepomuk::Vocabulary::NBIB::reviewedAuthor(), Nepomuk::Vocabulary::NCO::PersonContact());
}

void BibTexToNepomukPipe::addReferences(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NBIB::patentReferences(), QString(content.toUtf8()));
}

void BibTexToNepomukPipe::addLegalStatus(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NBIB::legalStatus(), QString(content.toUtf8()));
}

void BibTexToNepomukPipe::addFilingDate(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NBIB::filingDate(), QString(content.toUtf8()));
}

void BibTexToNepomukPipe::addUrl(const QString &content, Nepomuk::Resource publication)
{
    // in merge mode we remove all urls and replace them by whatever we get in the current content
    if(m_replaceMode) {
        publication.removeProperty(Nepomuk::Vocabulary::NBIB::isPublicationOf());
    }

    // first check if the url is already attached to the publication
    QList<Nepomuk::Resource> dataObjectList = publication.property(Nepomuk::Vocabulary::NBIB::isPublicationOf()).toResourceList();
    foreach(const Nepomuk::Resource &r, dataObjectList) {
        if(r.property(Nepomuk::Vocabulary::NIE::url()).toString() == QString(content.toUtf8())) {
            qDebug() << "BibTexToNepomukPipe::addUrl || url already connected to publication" << QString(content.toUtf8());
            return;
        }
    }

    QUrl urlType;

    // first check if the given url points to a local file
    KUrl url = KUrl(content);
    if(url.isLocalFile()) {
        urlType = Nepomuk::Vocabulary::NFO::FileDataObject();
    }
    else {
        // simple check, either the url ends with ".xxx" like .pdf
        // than it is a RemoteDataObject
        // otherwise a Website
        // this will produce wrong results when the webpage is a php script that returns files
        QString path = QUrl(content).path();

        if(path.contains(QRegExp(".*\\.\\D{3}$"))) {
            urlType = Nepomuk::Vocabulary::NFO::RemoteDataObject();
        }
        else {
            urlType = Nepomuk::Vocabulary::NFO::Website();
        }
    }

    Nepomuk::Resource dataObject(QString(content.toUtf8()), urlType);
    dataObject.setProperty(Nepomuk::Vocabulary::NIE::url(), QString(content.toUtf8()));
    QStringList identifier;
    identifier.append(QString(content.toUtf8()));
    dataObject.setIdentifiers(identifier);

    publication.addProperty(Nepomuk::Vocabulary::NBIB::isPublicationOf(), dataObject);
}

void BibTexToNepomukPipe::addVolume(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NBIB::volume(), QString(content.toUtf8()));
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

    QString newDate = content.toUtf8().trimmed() + QLatin1String("-") + month + QLatin1String("-") + day + QLatin1String("T00:00:00");
    publication.setProperty(Nepomuk::Vocabulary::NBIB::publicationDate(), newDate);
}

void BibTexToNepomukPipe::addKewords(const Value &content, Nepomuk::Resource publication)
{
    // in case we merge, we remove all old keys and add only the new ones
    if(m_replaceMode) {
        publication.removeProperty(Soprano::Vocabulary::NAO::hasTag());
    }

    foreach(QSharedPointer<ValueItem> vi, content) {
        Keyword *k = dynamic_cast<Keyword *>(vi.data());
        Nepomuk::Tag tag(k->text().toUtf8());
        tag.setLabel(k->text().toUtf8());
        publication.addTag(tag);
    }
}

void BibTexToNepomukPipe::addLastUsage(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NUAO::lastUsage(), QString(content.toUtf8()));
}

void BibTexToNepomukPipe::addDate(const QString &content, Nepomuk::Resource publication)
{
    publication.setProperty(Nepomuk::Vocabulary::NBIB::publicationDate(), QString(content.toUtf8()));
}

void BibTexToNepomukPipe::addZoteroSyncDetails(Nepomuk::Resource publication, Nepomuk::Resource reference, const QString &id,
const QString &etag, const QString &updated)
{
    Nepomuk::Resource syncDetails;

    // if we merge, we try to find existing zotero details we can update first
    if(m_replaceMode) {
        QList<Nepomuk::Resource> syncList = publication.property(Nepomuk::Vocabulary::SYNC::serverSyncData()).toResourceList();

        foreach(const Nepomuk::Resource &r, syncList) {
            if(r.property(Nepomuk::Vocabulary::SYNC::provider()).toString() != QString("zotero"))
                continue;
            if(r.property(Nepomuk::Vocabulary::SYNC::userId()).toString() != m_syncUserId)
                continue;
            if(r.property(Nepomuk::Vocabulary::SYNC::id()).toString() != id)
                continue;

            syncDetails = r;
            break;
        }
    }

    if(!syncDetails.isValid()) {
        qDebug() << "BibTexToNepomukPipe::addZoteroSyncDetails >> syncDetails is not valid!";
        syncDetails = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::SYNC::ServerSyncData());
    }

    syncDetails.setProperty(Nepomuk::Vocabulary::SYNC::provider(), QString("zotero"));
    syncDetails.setProperty(Nepomuk::Vocabulary::SYNC::url(), m_syncUrl);
    syncDetails.setProperty(Nepomuk::Vocabulary::SYNC::userId(), m_syncUserId);
    syncDetails.setProperty(Nepomuk::Vocabulary::SYNC::id(), id);
    syncDetails.setProperty(Nepomuk::Vocabulary::SYNC::etag(), etag);
    syncDetails.setProperty(Nepomuk::Vocabulary::NUAO::lastModification(), updated);
    syncDetails.setProperty(Nepomuk::Vocabulary::NBIB::publication(), publication);
    syncDetails.setProperty(Nepomuk::Vocabulary::NBIB::reference(), reference);

    publication.setProperty(Nepomuk::Vocabulary::SYNC::serverSyncData(), syncDetails);
    reference.setProperty(Nepomuk::Vocabulary::SYNC::serverSyncData(), syncDetails);
}

void BibTexToNepomukPipe::addContact(const Value &contentValue, Nepomuk::Resource res, QUrl property, QUrl contactType )
{
    //now if we update the nepomuk details, we remove all existing authors first and add only the authors from the new entry again
    if(m_replaceMode) {
        res.removeProperty(property);
    }

    foreach(QSharedPointer<ValueItem> authorItem, contentValue) {
        //transform KBibTex representation of the name into my own Name
        Name author;
        Person *person = dynamic_cast<Person *>(authorItem.data());
        if(person) {
            author.first = person->firstName().toUtf8();
            author.last = person->lastName().toUtf8();
            author.suffix = person->suffix().toUtf8();
            author.full = author.first + QLatin1String(" ") + author.last + QLatin1String(" ") + author.suffix;
            author.full = author.full.trimmed();
        }
        else {
            author.full = PlainTextValue::text(*authorItem).toUtf8();
            author.full = m_macroLookup.value(author.full, author.full);
        }

        //check if the author already exist in the database
        Nepomuk::Resource contact = m_allContacts.value(author.full, Nepomuk::Resource());

        if(!contact.isValid()) {
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
                contact = Nepomuk::Resource(job->item().url(), contactType);
                contact.setProperty(Nepomuk::Vocabulary::NIE::url(), job->item().url());

                contact.setProperty(Nepomuk::Vocabulary::NCO::fullname(), author.full);

                if(!author.first.isEmpty())
                    contact.setProperty(Nepomuk::Vocabulary::NCO::nameGiven(), author.first);
                if(!author.last.isEmpty())
                    contact.setProperty(Nepomuk::Vocabulary::NCO::nameFamily(), author.last);
                if(!author.suffix.isEmpty())
                    contact.setProperty(Nepomuk::Vocabulary::NCO::nameAdditional(), author.suffix);

                qDebug() << "akonadi/nepomuk id" << job->item().url() << contact.isValid() << contact.resourceUri();
            }
            else {
                contact = Nepomuk::Resource(QUrl(), contactType);

                contact.setProperty(Nepomuk::Vocabulary::NCO::fullname(), author.full);

                if(!author.first.isEmpty())
                    contact.setProperty(Nepomuk::Vocabulary::NCO::nameGiven(), author.first);
                if(!author.last.isEmpty())
                    contact.setProperty(Nepomuk::Vocabulary::NCO::nameFamily(), author.last);
                if(!author.suffix.isEmpty())
                    contact.setProperty(Nepomuk::Vocabulary::NCO::nameAdditional(), author.suffix);
            }

            m_allContacts.insert(author.full,contact);
        }

        res.addProperty(property, contact);
    }
}
