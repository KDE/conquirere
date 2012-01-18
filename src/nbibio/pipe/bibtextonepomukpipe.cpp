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

using namespace Nepomuk::Vocabulary;

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
    Nepomuk::Query::ResourceTypeTerm type( NCO::Contact() );
    Nepomuk::Query::Query query( type );
    QList<Nepomuk::Query::Result> queryResult = Nepomuk::Query::QueryServiceClient::syncQuery(query);
    foreach(const Nepomuk::Query::Result & nqr, queryResult) {
        QString fullname = nqr.resource().property(NCO::fullname()).toString();
        m_allContacts.insert(fullname, nqr.resource());
    }

    Nepomuk::Query::ResourceTypeTerm typeP( NBIB::Proceedings() );
    Nepomuk::Query::Query queryP( typeP );
    QList<Nepomuk::Query::Result> queryResultP = Nepomuk::Query::QueryServiceClient::syncQuery(queryP);
    foreach(const Nepomuk::Query::Result & nqr, queryResultP) {
        QString title = nqr.resource().property(NIE::title()).toString();
        m_allProceedings.insert(QString(title.toUtf8()), nqr.resource());
    }

    Nepomuk::Query::ResourceTypeTerm typeE( PIMO::Event() );
    Nepomuk::Query::Query queryE( typeE );
    QList<Nepomuk::Query::Result> queryResultE = Nepomuk::Query::QueryServiceClient::syncQuery(queryE);
    foreach(const Nepomuk::Query::Result & nqr, queryResultE) {
        QString title = nqr.resource().property(NIE::title()).toString();
        m_allPimoEvents.insert(QString(title.toUtf8()), nqr.resource());
    }

    Nepomuk::Query::ResourceTypeTerm typeS( NBIB::Series() );
    Nepomuk::Query::Query queryS( typeS );
    QList<Nepomuk::Query::Result> queryResultS = Nepomuk::Query::QueryServiceClient::syncQuery(queryS);
    foreach(const Nepomuk::Query::Result & nqr, queryResultS) {
        QString title = nqr.resource().property(NIE::title()).toString();
        m_allSeries.insert(QString(title.toUtf8()), nqr.resource());
    }

    Nepomuk::Query::ResourceTypeTerm typeC( NBIB::Collection() );
    Nepomuk::Query::Query queryC( typeC );
    QList<Nepomuk::Query::Result> queryResultC = Nepomuk::Query::QueryServiceClient::syncQuery(queryC);
    foreach(const Nepomuk::Query::Result & nqr, queryResultC) {
        QString title = nqr.resource().property(NIE::title()).toString();
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

    Nepomuk::Resource reference = Nepomuk::Resource(QUrl(), NBIB::Reference());
    reference.setProperty(NBIB::citeKey(), e->id());
    reference.setLabel(e->id()); // adds no real value, but looks nicer in the Nepomuk shell
    reference.setProperty(NBIB::publication(), publication);
    publication.addProperty(NBIB::reference(), reference);

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
            seriesURL = NBIB::Series();
            issueURL = NBIB::Encyclopedia();

            Value encyclopediaName;
            if(e->contains(QLatin1String("booktitle"))) {
                encyclopediaName = e->value(QLatin1String("booktitle"));
                e->remove(QLatin1String("booktitle"));
            }
            else if(e->contains(QLatin1String("journal"))) {
                encyclopediaName = e->value(QLatin1String("journal"));
                e->remove(QLatin1String("journal"));
            }

            addEncyclopedia(encyclopediaName,publication, issueURL);

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

    if(publication.hasType(NBIB::Collection()) ) {
        m_allProceedings.insert(publication.property(NIE::title()).toString(), publication);
    }
}

QUrl BibTexToNepomukPipe::typeToUrl(const QString & entryType)
{
    if(entryType == QLatin1String("article")) {
        return NBIB::Article();
    }
    else if(entryType == QLatin1String("bachelorthesis")) {
        return NBIB::BachelorThesis();
    }
    else if(entryType == QLatin1String("book")) {
        return NBIB::Book();
    }
    else if(entryType == QLatin1String("inbook")) {
        return  NBIB::Book();
    }
    else if(entryType == QLatin1String("booklet")) {
        return NBIB::Booklet();
    }
    else if(entryType == QLatin1String("collection")) {
        return NBIB::Collection();
    }
    else if(entryType == QLatin1String("incollection")) {
        return NBIB::Book();
    }
    else if(entryType == QLatin1String("electronic")) {
        return NBIB::Electronic();
    }
    else if(entryType == QLatin1String("inproceedings")) {
        return NBIB::Article();
    }
    else if(entryType == QLatin1String("manual")) {
        return NBIB::Manual();
    }
    else if(entryType == QLatin1String("mastersthesis")) {
        return NBIB::MastersThesis();
    }
    else if(entryType == QLatin1String("phdthesis")) {
        return NBIB::PhdThesis();
    }
    else if(entryType == QLatin1String("presentation")) {
        return NBIB::Presentation();
    }
    else if(entryType == QLatin1String("proceedings")) {
        return NBIB::Proceedings();
    }
    else if(entryType == QLatin1String("script")) {
        return NBIB::Script();
    }
    else if(entryType == QLatin1String("manuscript")) {
        return NBIB::Script();
    }
    else if(entryType == QLatin1String("techreport")) {
        return NBIB::Techreport();
    }
    else if(entryType == QLatin1String("report")) {
        return NBIB::Report();
    }
    else if(entryType == QLatin1String("thesis")) {
        return NBIB::Thesis();
    }
    else if(entryType == QLatin1String("unpublished")) {
        return NBIB::Unpublished();
    }
    else if(entryType == QLatin1String("patent")) {
        return NBIB::Patent();
    }
    else if(entryType == QLatin1String("standard")) {
        return NBIB::Standard();
    }
    else if(entryType == QLatin1String("statute")) {
        return NBIB::Statute();
    }
    else if(entryType == QLatin1String("case")) {
        return NBIB::LegalCaseDocument();
    }
    else if(entryType == QLatin1String("bill")) {
        return NBIB::Bill();
    }
    else if(entryType == QLatin1String("encyclopediaarticle")) {
        return NBIB::Article();
    }
    else if(entryType == QLatin1String("dictionaryentry") ||
    entryType == QLatin1String("dictionary")) {
        return NBIB::Dictionary();
    }
    else if(entryType == QLatin1String("forumpost")) {
        return NBIB::ForumPost();
    }
    else if(entryType == QLatin1String("blogpost")) {
        return NBIB::BlogPost();
    }
    else if(entryType == QLatin1String("webpage")) {
        return NBIB::WebPage();
    }
    else if(entryType == QLatin1String("misc")) {
        return NBIB::Publication();
    }
    else {
        // same as @Misc
        qWarning() << "BibTexToNepomukPipe::typeToUrl unknown type" << entryType;
        return NBIB::Publication();
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

    Nepomuk::Resource publication = syncResource.property(NBIB::publication()).toResource();
    Nepomuk::Resource reference = syncResource.property(NBIB::reference()).toResource();

    if(!publication.isValid())
        publication = Nepomuk::Resource(QUrl(), NBIB::Publication());

    if(!reference.isValid())
        reference = Nepomuk::Resource(QUrl(), NBIB::Reference());

    Entry *diffEntry = getDiff(syncResource, external, keepLocal);

    qDebug() << "created the diff" << diffEntry->size() << "changed entries";

    emit progress(0);

    //create the collection used for importing

    // we start by fetching all contacts for the conflict checking
    // this reduce the need to query nepomuk with every new author again and again
    Nepomuk::Query::ResourceTypeTerm type( NCO::Contact() );
    Nepomuk::Query::Query query( type );
    QList<Nepomuk::Query::Result> queryResult = Nepomuk::Query::QueryServiceClient::syncQuery(query);
    foreach(const Nepomuk::Query::Result & nqr, queryResult) {
        QString fullname = nqr.resource().property(NCO::fullname()).toString();
        m_allContacts.insert(fullname, nqr.resource());
    }

    Nepomuk::Query::ResourceTypeTerm typeP( NBIB::Proceedings() );
    Nepomuk::Query::Query queryP( typeP );
    QList<Nepomuk::Query::Result> queryResultP = Nepomuk::Query::QueryServiceClient::syncQuery(queryP);
    foreach(const Nepomuk::Query::Result & nqr, queryResultP) {
        QString title = nqr.resource().property(NIE::title()).toString();
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
    //############################################
    // Simple set commands

    if(key == QLatin1String("abstract")) {
        addValue(PlainTextValue::text(value), publication, NBIB::abstract() );
    }
    else if(key == QLatin1String("annote")) {
        addValue(PlainTextValue::text(value), publication, NIE::comment() );
    }
    else if(key == QLatin1String("archive")) {
        addValue(PlainTextValue::text(value), publication, NBIB::archive());
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
    else if(key == QLatin1String("note")) {
        addValue(PlainTextValue::text(value), publication, NIE::description());
    }
    else if(key == QLatin1String("number")) {
        addValue(PlainTextValue::text(value), publication, NBIB::number());
    }
    else if(key == QLatin1String("pages")) {
        addValue(PlainTextValue::text(value), reference, NBIB::pages());
    }
    else if(key == QLatin1String("numpages") || key == QLatin1String("numberofpages")) {
        addValue(PlainTextValue::text(value), publication, NBIB::numberOfPages());
    }
    else if(key == QLatin1String("numberofvolumes")) {
        addValue(PlainTextValue::text(value), publication, NBIB::numberOfVolumes());
    }
    else if(key == QLatin1String("pubmed")) {
        addValue(PlainTextValue::text(value), publication, NBIB::pubMed());
    }
    else if(key == QLatin1String("shorttitle")) {
        addValueWithLookup(PlainTextValue::text(value), publication, NBIB::shortTitle());
    }
    else if(key == QLatin1String("type")) {
        addValueWithLookup(PlainTextValue::text(value), publication, NBIB::type());
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
        addValue(PlainTextValue::text(value), publication, NUAO::lastUsage());
    }
    else if(key == QLatin1String("date")) {
        addValue(PlainTextValue::text(value), publication, NBIB::publicationDate());
    }

    //############################################
    // more advanced processing needed here

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
    else if(key == QLatin1String("editor")) {
        addEditor(value, publication);
    }
    else if(key == QLatin1String("institution")) {
        Value empty;
        addPublisher(value, empty, publication);
    }
    else if(key == QLatin1String("issn")) {
        addIssn(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("month")) {
        addMonth(PlainTextValue::text(value), publication);
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
    else if(key == QLatin1String("year")) {
        addYear(PlainTextValue::text(value), publication);
    }
    else if(key == QLatin1String("keywords")) {
        addKewords(value, publication);
    }
    else if(key == QLatin1String("assignee")) {
        addAssignee(value, publication);
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
        publication.removeProperty(NCO::publisher());
    }

    QString address = PlainTextValue::text(addressValue).toUtf8();
    address = m_macroLookup.value(address, address);

    // create the address object
    Nepomuk::Resource addr(QUrl(), NCO::PostalAddress());
    //FIXME extendedAddress is not correct, but determining which part of the @p address is the street/location and so on is nearly impossible

    if(!address.isEmpty())
        addr.setProperty(NCO::extendedAddress(), address);

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
            p = Nepomuk::Resource(QUrl(), NCO::Contact());

            p.setProperty(NCO::fullname(), publisher.full);
            if(!publisher.first.isEmpty())
                p.setProperty(NCO::nameGiven(), publisher.first);
            if(!publisher.last.isEmpty())
                p.setProperty(NCO::nameFamily(), publisher.last);
            if(!publisher.suffix.isEmpty())
                p.setProperty(NCO::nameHonorificSuffix(), publisher.suffix);

            m_allContacts.insert(publisher.full,p);
        }

        Nepomuk::Resource existingAddr = p.property(NCO::hasPostalAddress()).toResource();
        if(!existingAddr.isValid())
            p.setProperty(NCO::hasPostalAddress(), addr);

        publication.addProperty(NCO::publisher(), p);
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
        journalResource.addType(NBIB::Series()); // seems to be a bug, not the full hierachry will be set otherwise
        journalResource.addType(NIE::InformationElement());
        journalResource.setProperty(NIE::title(), journalName);
        m_allSeries.insert(journalName, journalResource);
    }

    // now check if the journalIssue exists already
    QList<Nepomuk::Resource> issues = journalResource.property(NBIB::seriesOf()).toResourceList();

    foreach(const Nepomuk::Resource & issue, issues) {
        QString checkNumber = issue.property(NBIB::number()).toString();
        QString checkVolume = issue.property(NBIB::volume()).toString();

        if( checkNumber == number && checkVolume == volume) {
            journalIssue = issue;
            break;
        }
    }

    //if we can't find an existing journal issue, create a new one
    if(!journalIssue.isValid()) {
        journalIssue = Nepomuk::Resource(QUrl(), issueUrl);
        journalIssue.addType(NBIB::Collection());
        journalIssue.addType(NBIB::Publication());
        journalIssue.addType(NIE::InformationElement());
        journalIssue.setProperty(NBIB::number(), number);
        journalIssue.setProperty(NBIB::volume(), volume);
        // duplicate title join journal and journalissue, helps to easily identify those two
        // but is more like a better way to create a prefLabel / genericLabel
        QString issueName = QString("%1 : %2 (%3)").arg(journalName).arg(volume).arg(number);
        journalIssue.setProperty(NIE::title(), issueName);

        // connect issue <-> journal
        journalIssue.setProperty(NBIB::inSeries(), journalResource);
        journalResource.addProperty(NBIB::seriesOf(), journalIssue);
    }

    // now connect the issue to the Publication/Collection
    publication.setProperty(NBIB::collection(), journalIssue);
    journalIssue.addProperty(NBIB::article(), publication);
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
        encyclopediaResource.addType(NBIB::Collection());
        encyclopediaResource.addType(NBIB::Publication());
        encyclopediaResource.addType(NIE::InformationElement());
        encyclopediaResource.setProperty(NIE::title(), encyclopediaName);
    }

    // now connect the issue to the Publication/Collection
    publication.setProperty(NBIB::collection(), encyclopediaResource);
    encyclopediaResource.addProperty(NBIB::article(), publication);
}

void BibTexToNepomukPipe::addAuthor(const Value &contentValue, Nepomuk::Resource publication, Nepomuk::Resource reference, const QString & originalEntryType)
{
    //in case of @incollection the author is used to identify who wrote the chapter not the complete book/collection
    Nepomuk::Resource authorResource;

    if(originalEntryType == QLatin1String("incollection") ) {
        Nepomuk::Resource chapter = reference.property(NBIB::referencedPart()).toResource();

        if(!chapter.isValid()) {
            chapter = Nepomuk::Resource(QUrl(), NBIB::Chapter());
            reference.setProperty(NBIB::referencedPart(), chapter);
            publication.addProperty(NBIB::documentPart(), chapter);
            chapter.setProperty(NBIB::documentPartOf(), publication);
        }
        authorResource = chapter;
    }
    else {
        authorResource = publication;
    }

    addContact(contentValue, authorResource, NCO::creator(), NCO::PersonContact());
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
                proceedingsResource = Nepomuk::Resource(QUrl(), NBIB::Proceedings());
            else if(originalEntryType == QLatin1String("encyclopediaarticle"))
                proceedingsResource = Nepomuk::Resource(QUrl(), NBIB::Encyclopedia());

            proceedingsResource.setProperty(NIE::title(), utfContent);
            m_allProceedings.insert(utfContent, proceedingsResource);
        }

        //at this point we have a valid proceedings entry connect it to the publication
        //The publication (@inproceedings) is an article while the @Proceedings is a collection
        publication.setProperty(NBIB::collection(), proceedingsResource);
        proceedingsResource.addProperty(NBIB::article(), publication);
    }
    else {
        publication.setProperty(NIE::title(), utfContent);
    }
}

void BibTexToNepomukPipe::addBookAuthor(const Value &contentValue, Nepomuk::Resource publication)
{
    // bookauthor is a Zotero key for the @incollection import.
    // add author to the publication (normal author in this case is related to the chapter)
    Nepomuk::Resource authorResource = publication;

    addContact(contentValue, authorResource, NCO::creator(), NCO::PersonContact());
}

void BibTexToNepomukPipe::addSeriesEditor(const Value &contentValue, Nepomuk::Resource publication)
{
    Nepomuk::Resource seriesResource = publication.property(NBIB::inSeries()).toResource();

    if(!seriesResource.isValid()) {
        seriesResource = Nepomuk::Resource(QUrl(), NBIB::Series());
    }

    addContact(contentValue, seriesResource, NBIB::editor(), NCO::PersonContact());
}

void BibTexToNepomukPipe::addChapter(const QString &content, Nepomuk::Resource publication, Nepomuk::Resource reference)
{
    // If we import some thing we assume no reference already existied and we have a new one
    // thus referencedPart() is not valid
    // if it is valid we assume this was already a Chapter rather than an generic nbib:DocumentPart
    // if above is not true, we should throw an error message
    Nepomuk::Resource chapterResource = reference.property(NBIB::referencedPart()).toResource();

    if(!chapterResource.isValid()) {
        chapterResource = Nepomuk::Resource(QUrl(), NBIB::Chapter());
        reference.setProperty(NBIB::referencedPart(), chapterResource);
        publication.addProperty(NBIB::documentPart(), chapterResource);
        chapterResource.setProperty(NBIB::documentPartOf(), publication);
    }
    else {
        if(!chapterResource.hasType(NBIB::Chapter())) {
            qWarning() << "BibTexToNepomukPipe::addChapter tries to add a chapterNumber to a nbib:DocumentPart Resource that is not a nbib:Chapter";
        }
    }

    QString utfContent = m_macroLookup.value(QString(content.toUtf8()), QString(content.toUtf8()));
    chapterResource.setProperty( NBIB::chapterNumber(), utfContent);
}

void BibTexToNepomukPipe::addEditor(const Value &contentValue, Nepomuk::Resource publication)
{
    //if we merge entries, we remove the old one first and add the new oney later on again
    if(m_replaceMode) {
        publication.removeProperty(NBIB::editor());
    }

    addContact(contentValue, publication, NBIB::editor(), NCO::PersonContact());
}

void BibTexToNepomukPipe::addIssn(const QString &content, Nepomuk::Resource publication)
{
    QString utfContent = m_macroLookup.value(QString(content.toUtf8()), QString(content.toUtf8()));

    //fetch already existing Series or create a new one
    Nepomuk::Resource journalIssue = publication.property(NBIB::collection()).toResource();

    if(!journalIssue.isValid()) {
        qDebug() << "BibTexToNepomukPipe::addIssn | try to set ISSN but no journalissue available";
        publication.setProperty(NBIB::issn(), utfContent);
        return;
    }

    Nepomuk::Resource series = journalIssue.property(NBIB::inSeries()).toResource();

    if(!series.isValid()) {
        series = Nepomuk::Resource(QUrl(), NBIB::Journal());
        series.addType(NBIB::Series()); // seems to be a bug, not the full hierachry will be set otherwise
        series.addType(NIE::InformationElement());
        series.addProperty(NBIB::seriesOf(), publication);
        publication.setProperty(NBIB::inSeries(), series);
    }

    series.setProperty(NBIB::issn(), utfContent);
}

void BibTexToNepomukPipe::addMonth(const QString &content, Nepomuk::Resource publication)
{
    //fetch already existing publication or create a new one
    QString date = publication.property(NBIB::publicationDate()).toString();
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
    publication.setProperty(NBIB::publicationDate(), newDate);
}

void BibTexToNepomukPipe::addOrganization(const QString &content, Nepomuk::Resource publication)
{
    QString utfContent = m_macroLookup.value(QString(content.toUtf8()), QString(content.toUtf8()));

    //check if the organization already exist in the database
    Nepomuk::Resource organizationResource = m_allContacts.value(utfContent, Nepomuk::Resource());

    if(!organizationResource.isValid()) {
        qDebug() << "create a new OrganizationContact resource for " << utfContent;
        organizationResource = Nepomuk::Resource(QUrl(), NCO::OrganizationContact());
        organizationResource.setProperty(NCO::fullname(), utfContent);

        m_allContacts.insert(utfContent, organizationResource);
    }

    if(publication.hasType(NBIB::Article())) {
        Nepomuk::Resource proceedings = publication.property(NBIB::collection()).toResource();
        if(!proceedings.isValid()) {
            proceedings = Nepomuk::Resource(QUrl(), NBIB::Proceedings());
            proceedings.addProperty(NBIB::article(), publication);
            publication.setProperty(NBIB::collection(), proceedings);
        }
        proceedings.setProperty(NBIB::organization(), organizationResource);
    }
    else {
        publication.setProperty(NBIB::organization(), organizationResource);
    }
}

void BibTexToNepomukPipe::addCode(const QString &content, Nepomuk::Resource publication)
{
    Nepomuk::Resource codeOfLaw = Nepomuk::Resource(QUrl(), NBIB::CodeOfLaw());
    codeOfLaw.setProperty(NIE::title(), QString(content.toUtf8()));
    publication.setProperty(NBIB::codeOfLaw(), codeOfLaw);
}

void BibTexToNepomukPipe::addCodeNumber(const QString &content, Nepomuk::Resource publication)
{
    Nepomuk::Resource codeOfLaw = publication.property(NBIB::codeOfLaw()).toResource();
    codeOfLaw.setProperty(NBIB::codeNumber(), QString(content.toUtf8()));
}

void BibTexToNepomukPipe::addCodeVolume(const QString &content, Nepomuk::Resource publication)
{
    Nepomuk::Resource codeOfLaw = publication.property(NBIB::codeOfLaw()).toResource();
    codeOfLaw.setProperty(NBIB::volume(), QString(content.toUtf8()));
}

void BibTexToNepomukPipe::addReporter(const QString &content, Nepomuk::Resource publication)
{
    Nepomuk::Resource courtReporter = Nepomuk::Resource(QUrl(), NBIB::CourtReporter());
    courtReporter.setProperty(NIE::title(), QString(content.toUtf8()));
    publication.setProperty(NBIB::courtReporter(), courtReporter);
}

void BibTexToNepomukPipe::addReporterVolume(const QString &content, Nepomuk::Resource publication)
{
    Nepomuk::Resource courtReporter = publication.property(NBIB::courtReporter()).toResource();
    courtReporter.setProperty(NBIB::volume(), QString(content.toUtf8()));
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
        eventResource = Nepomuk::Resource(QUrl(), PIMO::Event());
        eventResource.setProperty(NIE::title(), QString(content.toUtf8()));
        m_allPimoEvents.insert( QString(content.toUtf8()), eventResource);
    }

    eventResource.addProperty(NBIB::eventPublication(), publication);
    publication.setProperty(NBIB::event(), eventResource);
}

void BibTexToNepomukPipe::addSeries(const QString &content, Nepomuk::Resource publication)
{
    QUrl seriesType;
    if(publication.hasType(NBIB::Book()) ||
    publication.hasType(NBIB::Booklet()) ) {
        seriesType = NBIB::BookSeries();
    }
    else {
        seriesType = NBIB::Series();
    }

    //find existing series or create a new series of them
    Nepomuk::Resource seriesResource;
    QString utfContent = m_macroLookup.value(QString(content.toUtf8()), QString(content.toUtf8()));

    // check if a series with the same name already exist

    seriesResource = m_allSeries.value(utfContent, Nepomuk::Resource());

    if(!seriesResource.isValid()) {
        qDebug() << "did not find existing series for name" << utfContent;
        seriesResource = Nepomuk::Resource(QUrl(), seriesType);
        seriesResource.addType(NBIB::Series()); // seems to be a bug, not the full hierachry will be set otherwise
        seriesResource.addType(NIE::InformationElement());
        seriesResource.setProperty(NIE::title(), utfContent);
        m_allSeries.insert(utfContent, seriesResource);
    }

    seriesResource.addProperty(NBIB::seriesOf(), publication);
    publication.setProperty(NBIB::inSeries(), seriesResource);
}

void BibTexToNepomukPipe::addTitle(const QString &content, Nepomuk::Resource publication, Nepomuk::Resource reference, const QString & originalEntryType)
{
    QString utfContent = m_macroLookup.value(QString(content.toUtf8()), QString(content.toUtf8()));

    // in the case of @InCollection title means title of the article in the book
    // while booktitle is the actual title of the book
    if(originalEntryType == QLatin1String("incollection")) {

        Nepomuk::Resource chapterResource = reference.property(NBIB::referencedPart()).toResource();

        if(!chapterResource.isValid()) {
            chapterResource = Nepomuk::Resource(QUrl(), NBIB::Chapter());
            reference.setProperty(NBIB::referencedPart(), chapterResource);
            publication.addProperty(NBIB::documentPart(), chapterResource);
            chapterResource.setProperty(NBIB::documentPartOf(), publication);
        }

        chapterResource.setProperty( NIE::title(), utfContent);
    }
    // for a dictionary entry the title is the name of the chapter
    else if(originalEntryType == QLatin1String("dictionaryentry")) {

        Nepomuk::Resource chapterResource = reference.property(NBIB::referencedPart()).toResource();

        if(!chapterResource.isValid()) {
            chapterResource = Nepomuk::Resource(QUrl(), NBIB::Chapter());
            reference.setProperty(NBIB::referencedPart(), chapterResource);
            publication.addProperty(NBIB::documentPart(), chapterResource);
            chapterResource.setProperty(NBIB::documentPartOf(), publication);
        }

        chapterResource.setProperty( NIE::title(), utfContent);
    }
    else {
        publication.setProperty(NIE::title(), utfContent);
    }
}

void BibTexToNepomukPipe::addAssignee(const Value &contentValue, Nepomuk::Resource publication)
{
    addContact(contentValue, publication, NBIB::assignee(), QUrl()); // let addContact decide is it should be PersonContact or plain contact
}

void BibTexToNepomukPipe::addContributor(const Value &contentValue, Nepomuk::Resource publication)
{
    addContact(contentValue, publication, NBIB::contributor(), NCO::PersonContact());
}

void BibTexToNepomukPipe::addTranslator(const Value &contentValue, Nepomuk::Resource publication)
{
    addContact(contentValue, publication, NBIB::translator(), NCO::PersonContact());
}

void BibTexToNepomukPipe::addReviewedAuthor(const Value &contentValue, Nepomuk::Resource publication)
{
    addContact(contentValue, publication, NBIB::reviewedAuthor(), NCO::PersonContact());
}

void BibTexToNepomukPipe::addUrl(const QString &content, Nepomuk::Resource publication)
{
    // in merge mode we remove all urls and replace them by whatever we get in the current content
    if(m_replaceMode) {
        publication.removeProperty(NBIB::isPublicationOf());
    }

    // first check if the url is already attached to the publication
    QList<Nepomuk::Resource> dataObjectList = publication.property(NBIB::isPublicationOf()).toResourceList();
    foreach(const Nepomuk::Resource &r, dataObjectList) {
        if(r.property(NIE::url()).toString() == QString(content.toUtf8())) {
            qDebug() << "BibTexToNepomukPipe::addUrl || url already connected to publication" << QString(content.toUtf8());
            return;
        }
    }

    QUrl urlType;

    // first check if the given url points to a local file
    KUrl url = KUrl(content);
    if(url.isLocalFile()) {
        urlType = NFO::FileDataObject();
    }
    else {
        // simple check, either the url ends with ".xxx" like .pdf
        // than it is a RemoteDataObject
        // otherwise a Website
        // this will produce wrong results when the webpage is a php script that returns files
        QString path = QUrl(content).path();

        if(path.contains(QRegExp(".*\\.\\D{3}$"))) {
            urlType = NFO::RemoteDataObject();
        }
        else {
            urlType = NFO::Website();
        }
    }

    Nepomuk::Resource dataObject(QString(content.toUtf8()), urlType);
    dataObject.setProperty(NIE::url(), QString(content.toUtf8()));
    QStringList identifier;
    identifier.append(QString(content.toUtf8()));
    dataObject.setIdentifiers(identifier);

    publication.addProperty(NBIB::isPublicationOf(), dataObject);
}

void BibTexToNepomukPipe::addYear(const QString &content, Nepomuk::Resource publication)
{
    //fetch already existing publication or create a new one
    QString date = publication.property(NBIB::publicationDate()).toString();
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
    publication.setProperty(NBIB::publicationDate(), newDate);
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

void BibTexToNepomukPipe::addZoteroSyncDetails(Nepomuk::Resource publication, Nepomuk::Resource reference, const QString &id,
const QString &etag, const QString &updated)
{
    Nepomuk::Resource syncDetails;

    // if we merge, we try to find existing zotero details we can update first
    if(m_replaceMode) {
        QList<Nepomuk::Resource> syncList = publication.property(SYNC::serverSyncData()).toResourceList();

        foreach(const Nepomuk::Resource &r, syncList) {
            if(r.property(SYNC::provider()).toString() != QString("zotero"))
                continue;
            if(r.property(SYNC::userId()).toString() != m_syncUserId)
                continue;
            if(r.property(SYNC::id()).toString() != id)
                continue;

            syncDetails = r;
            break;
        }
    }

    if(!syncDetails.isValid()) {
        qDebug() << "BibTexToNepomukPipe::addZoteroSyncDetails >> syncDetails is not valid!";
        syncDetails = Nepomuk::Resource(QUrl(), SYNC::ServerSyncData());
    }

    syncDetails.setProperty(SYNC::provider(), QString("zotero"));
    syncDetails.setProperty(SYNC::url(), m_syncUrl);
    syncDetails.setProperty(SYNC::userId(), m_syncUserId);
    syncDetails.setProperty(SYNC::id(), id);
    syncDetails.setProperty(SYNC::etag(), etag);
    syncDetails.setProperty(NUAO::lastModification(), updated);
    syncDetails.setProperty(NBIB::publication(), publication);
    syncDetails.setProperty(NBIB::reference(), reference);

    publication.setProperty(SYNC::serverSyncData(), syncDetails);
    reference.setProperty(SYNC::serverSyncData(), syncDetails);
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

            if(!contactType.isValid())
                contactType = NCO::PersonContact();
        }
        else {
            author.full = PlainTextValue::text(*authorItem).toUtf8();
            author.full = m_macroLookup.value(author.full, author.full);
            qDebug() << "BibTexToNepomukPipe::addContact No Person Contact available!" << author.full;

            if(!contactType.isValid())
                contactType = NCO::Contact();
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
                addr.setSuffix( author.suffix );
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
                contact.setProperty(NIE::url(), job->item().url());

                contact.setProperty(NCO::fullname(), author.full);

                if(!author.first.isEmpty())
                    contact.setProperty(NCO::nameGiven(), author.first);
                if(!author.last.isEmpty())
                    contact.setProperty(NCO::nameFamily(), author.last);
                if(!author.suffix.isEmpty())
                    contact.setProperty(NCO::nameHonorificSuffix(), author.suffix);

                qDebug() << "akonadi/nepomuk id" << job->item().url() << contact.isValid() << contact.resourceUri();
            }
            else {
                contact = Nepomuk::Resource(QUrl(), contactType);

                contact.setProperty(NCO::fullname(), author.full);

                if(!author.first.isEmpty())
                    contact.setProperty(NCO::nameGiven(), author.first);
                if(!author.last.isEmpty())
                    contact.setProperty(NCO::nameFamily(), author.last);
                if(!author.suffix.isEmpty())
                    contact.setProperty(NCO::nameHonorificSuffix(), author.suffix);
            }

            m_allContacts.insert(author.full,contact);
        }

        res.addProperty(property, contact);
    }
}

void BibTexToNepomukPipe::addValue(const QString &content, Nepomuk::Resource publication, QUrl property)
{
    publication.setProperty(property, QString(content.toUtf8()));
}

void BibTexToNepomukPipe::addValueWithLookup(const QString &content, Nepomuk::Resource publication, QUrl property)
{
    QString utfContent = m_macroLookup.value(QString(content.toUtf8()), QString(content.toUtf8()));
    publication.setProperty(property, utfContent);
}
