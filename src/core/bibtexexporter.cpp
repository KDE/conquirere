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

#include "bibtexexporter.h"

#include <Nepomuk/Variant>

#include <Nepomuk/Query/Term>
#include <Nepomuk/Query/ResourceTerm>
#include <Nepomuk/Query/ResourceTypeTerm>
#include <Nepomuk/Query/ComparisonTerm>
#include <Nepomuk/Query/AndTerm>
#include <Nepomuk/Query/NegationTerm>
#include <Nepomuk/Query/QueryServiceClient>
#include <Nepomuk/Query/Result>
#include <Nepomuk/Query/QueryParser>

#include <Nepomuk/Vocabulary/NFO>
#include <Nepomuk/Vocabulary/NMO>
#include <Nepomuk/Vocabulary/NCO>
#include <Nepomuk/Vocabulary/NIE>
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk/Vocabulary/PIMO>

#include "nbib.h"
#include <QFile>
#include <QTextStream>

#include <QDebug>


const QLatin1String br(",\n");

BibTexExporter::BibTexExporter(QObject *parent)
    : QObject(parent)
    , m_style(BibStyle_Strict)
{
}

void BibTexExporter::setIsRelatedTo(Nepomuk::Resource relatedResource)
{
    m_isRelatedTo = relatedResource;
}

void BibTexExporter::setResource(Nepomuk::Resource usedResource)
{
    m_resources.append(usedResource);
}

void BibTexExporter::setResource(QList <Nepomuk::Resource> usedResources)
{
    m_resources = usedResources;
}

void BibTexExporter::setStyle(BibTexStyle style)
{
    m_style = style;
}

bool BibTexExporter::exportReferences(const QString &filename)
{
    // fetcha data when nothing is available
    if(m_resources.isEmpty()) {
        Nepomuk::Query::AndTerm andTerm;
        //andTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NFO::Document() ) );

        andTerm.addSubTerm( Nepomuk::Query::ComparisonTerm( Nepomuk::Vocabulary::PIMO::isRelated(), Nepomuk::Query::ResourceTerm(m_isRelatedTo) ) );

        Nepomuk::Query::Query query( andTerm );

        QList<Nepomuk::Query::Result> queryResult = Nepomuk::Query::QueryServiceClient::syncQuery(query);

        qDebug() << "queryResult" << queryResult.size();

        foreach(Nepomuk::Query::Result r, queryResult) {
            m_resources.append(r.resource());
        }
    }

    qDebug() << "export " << m_resources.size() << "to bibtex";

    QFile bibFile(filename);
    if (!bibFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&bibFile);

    int citeKeyNumer = 1;
    QString citeRef = QLatin1String("BibTexExport");

    foreach(Nepomuk::Resource document, m_resources) {

        Nepomuk::Resource reference;
        Nepomuk::Resource publication;

        // first check if we operate on a BibReference or a Publication
        if(document.hasType( Nepomuk::Vocabulary::NBIB::Publication() )) {
            // we have a BibReference
            reference = document;
            publication = reference.property(Nepomuk::Vocabulary::NBIB::usePublication()).toResource();
        }
        else {
            //we have a Document with attached BibResource
            reference = document.property(Nepomuk::Vocabulary::NBIB::publishedAs()).toResource();
        }


        Nepomuk::Resource entryType = publication.property(Nepomuk::Vocabulary::NBIB::publishedAs()).toResource();

        QString entryString;
        if(entryType.isValid()) {
            entryString = entryType.genericLabel();
        }
        // if we don't know what type to cite, ignore entry
        else {
            qDebug() << "NBIB::bibResourceType() missing";
            continue;
        }

        QString citeKey = publication.property(Nepomuk::Vocabulary::NBIB::citeKey()).toString();
        if(citeKey.isEmpty()) {
            citeKey = citeRef + QString::number(citeKeyNumer);
        }

        // now generate the content
        QString fullstring;
        QString returnString;

        returnString = getAnnote(publication);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;
        returnString = getCrossref(publication);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;
        returnString = getEdition(publication);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;
        returnString = getEditor(publication);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;
        returnString = getEprint(publication);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;
        returnString = getHasChapter(reference);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;
        returnString = getHowpublished(publication);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;
        returnString = getInstitution(publication);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;
        returnString = getInJournal(publication);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;
        returnString = getLCCN(publication);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;
        returnString = getNote(publication);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;
        returnString = getOrganization(publication);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;
        returnString = getPages(publication);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;
        returnString = getPublicationDate(publication);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;
        returnString = getPublisher(publication);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;
        returnString = getSchool(publication);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;
        returnString = getSeries(publication);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;
        returnString = getTitle(publication);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;
        returnString = getType(publication);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;
        returnString = getUrl(publication);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;
        returnString = getISBN(publication);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;
        returnString = getISSN(publication);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;
        returnString = getDOI(publication);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;
        returnString = getMrNumber(publication);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;


        fullstring.remove(QRegExp(QLatin1String("^,\n")));

        //now adopt entryString if we have a book with pages or chapter to Inbook
        if( fullstring.contains(QLatin1String("pages =")) ||
            fullstring.contains(QLatin1String("chapter =")) ) {
            entryString = QLatin1String("Inbook");
        }

        // if we have strict style, check if the necessary fields are available
        if(m_style != BibStyle_Strict) {
            if(entryType.genericLabel() == QLatin1String("Article")) {
                if(!checkArticleCiteContent(fullstring)) {
                    continue;
                }
            }
            if(entryType.genericLabel() == QLatin1String("Bachelorthesis")) {
                if(!checkBachelorthesisCiteContent(fullstring)) {
                    continue;
                }
            }
            if(entryType.genericLabel() == QLatin1String("Book")) {
                if(!checkBookCiteContent(fullstring)) {
                    continue;
                }
            }
            if(entryType.genericLabel() == QLatin1String("Booklet")) {
                if(!checkBookletCiteContent(fullstring)) {
                    continue;
                }
            }
            if(entryType.genericLabel() == QLatin1String("Conference")) {
                if(!checkConferenceCiteContent(fullstring)) {
                    continue;
                }
            }
            if(entryType.genericLabel() == QLatin1String("Inbook")) {
                if(!checkInbookCiteContent(fullstring)) {
                    continue;
                }
            }
            if(entryType.genericLabel() == QLatin1String("Inproceedings")) {
                if(!checkInproceedingsCiteContent(fullstring)) {
                    continue;
                }
            }
            if(entryType.genericLabel() == QLatin1String("Incollection")) {
                if(!checkIncollectionCiteContent(fullstring)) {
                    continue;
                }
            }
            if(entryType.genericLabel() == QLatin1String("Manual")) {
                if(!checkManualCiteContent(fullstring)) {
                    continue;
                }
            }
            if(entryType.genericLabel() == QLatin1String("Mastersthesis")) {
                if(!checkMastersthesisCiteContent(fullstring)) {
                    continue;
                }
            }
            if(entryType.genericLabel() == QLatin1String("Misc")) {
                if(!checkMiscCiteContent(fullstring)) {
                    continue;
                }
            }
            if(entryType.genericLabel() == QLatin1String("Phdthesis")) {
                if(!checkPhdthesisCiteContent(fullstring)) {
                    continue;
                }
            }
            if(entryType.genericLabel() == QLatin1String("Proceedings")) {
                if(!checkProceedingsCiteContent(fullstring)) {
                    continue;
                }
            }
            if(entryType.genericLabel() == QLatin1String("Techreport")) {
                if(!checkTechreportCiteContent(fullstring)) {
                    continue;
                }
            }
            if(entryType.genericLabel() == QLatin1String("Unpublished")) {
                if(!checkUnpublishedCiteContent(fullstring)) {
                    continue;
                }
            }
            if(entryType.genericLabel() == QLatin1String("Patent")) {
                if(!checkPatentCiteContent(fullstring)) {
                    continue;
                }
            }
            if(entryType.genericLabel() == QLatin1String("Website")) {
                if(!checkWebsiteCiteContent(fullstring)) {
                    continue;
                }
            }
        }

        if(!fullstring.isEmpty()) {
            out << "@" << entryString << "{ " << citeKey << ",\n";
            out << fullstring << "\n";
            out << "}\n\n";

            citeKeyNumer++;
        }
    }

    bibFile.close();
    return true;
}

bool BibTexExporter::checkArticleCiteContent(const QString &citecontent)
{
    if(!citecontent.contains(QLatin1String("author ="))) {
        return false;
    }
    if(!citecontent.contains(QLatin1String("title ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("journal ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("year ="))) {
        return false;
    }

    return true;
}

bool BibTexExporter::checkBachelorthesisCiteContent(const QString &citecontent)
{
    if(!citecontent.contains(QLatin1String("author ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("title ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("school ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("year ="))) {
        return false;
    }

    return true;
}

bool BibTexExporter::checkBookCiteContent(const QString &citecontent)
{
    if(!citecontent.contains(QLatin1String("author ="))) {
        return false;
    }
    if(!citecontent.contains(QLatin1String("editor ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("title ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("publisher ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("year ="))) {
        return false;
    }

    return true;
}

bool BibTexExporter::checkBookletCiteContent(const QString &citecontent)
{
    if(!citecontent.contains(QLatin1String("title ="))) {
        return false;
    }

    return true;
}

bool BibTexExporter::checkConferenceCiteContent(const QString &citecontent)
{
    if(!citecontent.contains(QLatin1String("author ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("title ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("booktitle ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("year ="))) {
        return false;
    }

    return true;
}

bool BibTexExporter::checkInbookCiteContent(const QString &citecontent)
{
    if(!citecontent.contains(QLatin1String("author ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("editor ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("title ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("chapter ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("pages ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("publisher ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("year ="))) {
        return false;
    }

    return true;
}

bool BibTexExporter::checkIncollectionCiteContent(const QString &citecontent)
{
    if(!citecontent.contains(QLatin1String("author ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("editor ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("title ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("booktitle ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("year ="))) {
        return false;
    }

    return true;
}

bool BibTexExporter::checkManualCiteContent(const QString &citecontent)
{
    if(!citecontent.contains(QLatin1String("title ="))) {
        return false;
    }

    return true;
}

bool BibTexExporter::checkInproceedingsCiteContent(const QString &citecontent)
{
    if(!citecontent.contains(QLatin1String("author ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("title ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("booktitle ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("year ="))) {
        return false;
    }

    return true;
}

bool BibTexExporter::checkMastersthesisCiteContent(const QString &citecontent)
{
    if(!citecontent.contains(QLatin1String("author ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("title ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("year ="))) {
        return false;
    }

    return true;
}

bool BibTexExporter::checkMiscCiteContent(const QString &citecontent)
{
    return true;
}

bool BibTexExporter::checkPatentCiteContent(const QString &citecontent)
{
    if(!citecontent.contains(QLatin1String("author ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("title ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("year ="))) {
        return false;
    }

    return true;
}

bool BibTexExporter::checkPhdthesisCiteContent(const QString &citecontent)
{
    if(!citecontent.contains(QLatin1String("author ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("title ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("school ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("year ="))) {
        return false;
    }

    return true;
}

bool BibTexExporter::checkProceedingsCiteContent(const QString &citecontent)
{
    if(!citecontent.contains(QLatin1String("title ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("year ="))) {
        return false;
    }

    return true;
}

bool BibTexExporter::checkTechreportCiteContent(const QString &citecontent)
{
    if(!citecontent.contains(QLatin1String("author ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("title ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("institution ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("year ="))) {
        return false;
    }

    return true;
}

bool BibTexExporter::checkUnpublishedCiteContent(const QString &citecontent)
{
    if(!citecontent.contains(QLatin1String("author ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("title ="))) {
        return false;
    }

    return true;
}

bool BibTexExporter::checkWebsiteCiteContent(const QString &citecontent)
{
    if(!citecontent.contains(QLatin1String("url ="))) {
        return false;
    }

    return true;
}

//########################################################################

QString BibTexExporter::getAnnote(const Nepomuk::Resource &document)
{
    QString annoteString = document.property(Nepomuk::Vocabulary::NBIB::annote()).toString();

    if(!annoteString.isEmpty()) {
        annoteString.prepend(QLatin1String("\tannote = \""));
        annoteString.append(QLatin1String("\""));
    }

    return annoteString;
}

QString BibTexExporter::getCrossref(const Nepomuk::Resource &document)
{
    Nepomuk::Resource crossRef = document.property(Nepomuk::Vocabulary::NBIB::crossref()).toResource();

    QString crossRefString;
    if(crossRef.isValid()) {
        crossRefString = crossRef.property(Nepomuk::Vocabulary::NBIB::citeKey()).toString();
    }

    if(!crossRefString.isEmpty()) {
        crossRefString.prepend(QLatin1String("\tcrossref = \""));
        crossRefString.append(QLatin1String("\""));
    }

    return crossRefString;
}

QString BibTexExporter::getDOI(const Nepomuk::Resource &document)
{
    QString doiString = document.property(Nepomuk::Vocabulary::NBIB::doi()).toString();

    if(!doiString.isEmpty()) {
        doiString.prepend(QLatin1String("\tdoi = \""));
        doiString.append(QLatin1String("\""));
    }

    return doiString;
}

QString BibTexExporter::getEdition(const Nepomuk::Resource &document)
{
    QString editionString = document.property(Nepomuk::Vocabulary::NBIB::edition()).toString();

    if(!editionString.isEmpty()) {
        editionString.prepend(QLatin1String("\tedition = \""));
        editionString.append(QLatin1String("\""));
    }

    return editionString;
}

QString BibTexExporter::getEditor(const Nepomuk::Resource &document)
{
    QString authorString;
    QList<Nepomuk::Resource> authors = document.property(Nepomuk::Vocabulary::NBIB::editor()).toResourceList();
    if(!authors.isEmpty()) {
        foreach(Nepomuk::Resource a, authors) {
            //TODO don't rely only on fullname of NC::Contact
            authorString.append(a.property(Nepomuk::Vocabulary::NCO::fullname()).toString());
            authorString.append(QLatin1String(" and "));
        }
        authorString.chop(5);
        if(!authorString.isEmpty()) {
            authorString.prepend(QLatin1String("\tauthor = \""));
            authorString.append(QLatin1String("\""));
        }
    }

    return authorString;
}

QString BibTexExporter::getEprint(const Nepomuk::Resource &document)
{
    QString epString = document.property(Nepomuk::Vocabulary::NBIB::eprint()).toString();

    if(!epString.isEmpty()) {
        epString.prepend(QLatin1String("\teprint = \""));
        epString.append(QLatin1String("\""));
    }

    return epString;
}

QString BibTexExporter::getHasChapter(const Nepomuk::Resource &document)
{
    Nepomuk::Resource chapter = document.property(Nepomuk::Vocabulary::NBIB::hasChapter()).toResource();

    QString chapterTitle = chapter.property(Nepomuk::Vocabulary::NIE::title()).toString();
    Nepomuk::Resource book = chapter.property(Nepomuk::Vocabulary::NIE::hasLogicalPart()).toResource();
    QString bookTitle = book.property(Nepomuk::Vocabulary::NIE::title()).toString();

    if(!chapterTitle.isEmpty()) {
        chapterTitle.prepend(QLatin1String("\tchapter = \""));
        chapterTitle.append(QLatin1String("\""));
        chapterTitle.append(QLatin1String("\tbooktitle = \""));
        chapterTitle.append(bookTitle);
        chapterTitle.append(QLatin1String("\""));
    }

    return chapterTitle;
}

QString BibTexExporter::getHowpublished(const Nepomuk::Resource &document)
{
    QString hpString = document.property(Nepomuk::Vocabulary::NBIB::howPublished()).toString();

    if(!hpString.isEmpty()) {
        hpString.prepend(QLatin1String("\thowpublished = \""));
        hpString.append(QLatin1String("\""));
    }

    return hpString;
}

QString BibTexExporter::getInstitution(const Nepomuk::Resource &document)
{
    QString institutionString = document.property(Nepomuk::Vocabulary::NBIB::institution()).toString();

    if(!institutionString.isEmpty()) {
        institutionString.prepend(QLatin1String("\tinstitution = \""));
        institutionString.append(QLatin1String("\""));
    }

    return institutionString;
}

QString BibTexExporter::getISBN(const Nepomuk::Resource &document)
{
    QString isbnString = document.property(Nepomuk::Vocabulary::NBIB::isbn()).toString();

    if(!isbnString.isEmpty()) {
        isbnString.prepend(QLatin1String("\tisbn = \""));
        isbnString.append(QLatin1String("\""));
    }

    return isbnString;
}

QString BibTexExporter::getISSN(const Nepomuk::Resource &document)
{
    QString issnString = document.property(Nepomuk::Vocabulary::NBIB::issn()).toString();

    if(!issnString.isEmpty()) {
        issnString.prepend(QLatin1String("\tissn = \""));
        issnString.append(QLatin1String("\""));
    }

    return issnString;
}

QString BibTexExporter::getInJournal(const Nepomuk::Resource &document)
{
    Nepomuk::Resource journalIssue = document.property(Nepomuk::Vocabulary::NBIB::inJournalIssue()).toResource();

    if(!journalIssue.isValid()) {
        return QString(); // no journal available for his resource
    }

    // if we have a JournalIssue, get the journal contact and number/volume
    QString journalNumber = journalIssue.property(Nepomuk::Vocabulary::NBIB::number()).toString();
    QString journalVolume = journalIssue.property(Nepomuk::Vocabulary::NBIB::volume()).toString();

    Nepomuk::Resource journal = document.property(Nepomuk::Vocabulary::NBIB::journal()).toResource();
    QString journalName = journal.property(Nepomuk::Vocabulary::NCO::fullname()).toString();;

    if(!journalNumber.isEmpty()) {
        journalNumber.prepend(QLatin1String("\tnumber = \""));
        journalNumber.append(QLatin1String("\""));
    }
    if(!journalVolume.isEmpty()) {
        journalVolume.prepend(QLatin1String("\tvolume = \""));
        journalVolume.append(QLatin1String("\""));
    }
    if(!journalName.isEmpty()) {
        journalName.prepend(QLatin1String("\tjournal = \""));
        journalName.append(QLatin1String("\""));
    }

    QString returnString = journalNumber + br + journalVolume + br + journalName;

    return journalName;
}

QString BibTexExporter::getLCCN(const Nepomuk::Resource &document)
{
    QString issnString = document.property(Nepomuk::Vocabulary::NBIB::lccn()).toString();

    if(!issnString.isEmpty()) {
        issnString.prepend(QLatin1String("\tlccn = \""));
        issnString.append(QLatin1String("\""));
    }

    return issnString;
}

QString BibTexExporter::getMrNumber(const Nepomuk::Resource &document)
{
    QString mrnumberString = document.property(Nepomuk::Vocabulary::NBIB::mrNumber()).toString();

    if(!mrnumberString.isEmpty()) {
        mrnumberString.prepend(QLatin1String("\tmrnumber = \""));
        mrnumberString.append(QLatin1String("\""));
    }

    return mrnumberString;
}

QString BibTexExporter::getNote(const Nepomuk::Resource &document)
{
    QString noteString = document.property(Nepomuk::Vocabulary::NBIB::note()).toString();

    if(!noteString.isEmpty()) {
        noteString.prepend(QLatin1String("\tnote = \""));
        noteString.append(QLatin1String("\""));
    }

    return noteString;
}

QString BibTexExporter::getOrganization(const Nepomuk::Resource &document)
{
    //TODO get organization
    //We would have one nco:OrganizationContact for the organization. This
    //would be related via nco:org from an nco:Affiliation which in turn would
    //be the nco:affiliation of the publisher nco:[Person]Contact.
    //QString orgString = document.property(Nepomuk::Vocabulary::NBIB::organization()).toString();

//    if(!orgString.isEmpty()) {
//        orgString.prepend(QLatin1String("\torganization = \""));
//        orgString.append(QLatin1String("\""));
//    }

//    return orgString;
    return QString();
}

QString BibTexExporter::getPages(const Nepomuk::Resource &document)
{
    QString pagesString = document.property(Nepomuk::Vocabulary::NBIB::pages()).toString();

    if(!pagesString.isEmpty()) {
        pagesString.prepend(QLatin1String("\tpages = \""));
        pagesString.append(QLatin1String("\""));
    }

    return pagesString;
}

QString BibTexExporter::getPublisher(const Nepomuk::Resource &document)
{

    Nepomuk::Resource publisher = document.property(Nepomuk::Vocabulary::NCO::creator()).toResource();

    //TODO don't rely only on fullname of NC::Contact
   QString publisherString = publisher.property(Nepomuk::Vocabulary::NCO::fullname()).toString();

    if(!publisherString.isEmpty()) {
        publisherString.prepend(QLatin1String("\tpublisher = \""));
        publisherString.append(QLatin1String("\""));
    }

    return publisherString;
}

QString BibTexExporter::getPublicationDate(const Nepomuk::Resource &document)
{
    QString pdString = document.property(Nepomuk::Vocabulary::NBIB::publicationDate()).toString();

    QRegExp rx(QLatin1String("(\\d{4})-(\\d{2})-(\\d{2})*"));
    QString year;
    QString month;
    QString day;
      if (rx.indexIn(pdString) != -1) {
          year = rx.cap(1);
          month = rx.cap(2);
          day = rx.cap(3);
      }

    QString publicationDate;
    if(!month.isEmpty()) {
        int mInt = month.toInt();

        publicationDate.prepend(QLatin1String("\tmonth = \""));
        switch(mInt) {
        case 1:
            publicationDate.append(QLatin1String("jan"));
            break;
        case 2:
            publicationDate.append(QLatin1String("feb"));
            break;
        case 3:
            publicationDate.append(QLatin1String("mar"));
            break;
        case 4:
            publicationDate.append(QLatin1String("apr"));
            break;
        case 5:
            publicationDate.append(QLatin1String("may"));
            break;
        case 6:
            publicationDate.append(QLatin1String("jun"));
            break;
        case 7:
            publicationDate.append(QLatin1String("jul"));
            break;
        case 8:
            publicationDate.append(QLatin1String("aug"));
            break;
        case 9:
            publicationDate.append(QLatin1String("sep"));
            break;
        case 10:
            publicationDate.append(QLatin1String("oct"));
            break;
        case 11:
            publicationDate.append(QLatin1String("nov"));
            break;
        case 12:
            publicationDate.append(QLatin1String("dec"));
            break;
        default:
            return month; //DEBUG
        }

        publicationDate.append(QLatin1String("\""));
    }

    if(!year.isEmpty()) {
        publicationDate.append(br);
        publicationDate.append(QLatin1String("\tmonth = \""));
        publicationDate.append(year);
        publicationDate.append(QLatin1String("\""));

    }

    return publicationDate;
}

QString BibTexExporter::getSchool(const Nepomuk::Resource &document)
{

    Nepomuk::Resource school = document.property(Nepomuk::Vocabulary::NBIB::school()).toResource();

    //TODO don't rely only on fullname of NC::Contact
   QString schoolString = school.property(Nepomuk::Vocabulary::NCO::fullname()).toString();

    if(!schoolString.isEmpty()) {
        schoolString.prepend(QLatin1String("\tschool = \""));
        schoolString.append(QLatin1String("\""));
    }

    return schoolString;
}

QString BibTexExporter::getSeries(const Nepomuk::Resource &document)
{
    Nepomuk::Resource series = document.property(Nepomuk::Vocabulary::NBIB::inSeries()).toResource();
    QString seriesString = series.property(Nepomuk::Vocabulary::NIE::title()).toString();

    if(!seriesString.isEmpty()) {
        seriesString.prepend(QLatin1String("\tseries = \""));
        seriesString.append(QLatin1String("\""));
    }

    return seriesString;
}

QString BibTexExporter::getTitle(const Nepomuk::Resource &document)
{
    QString titleString = document.property(Nepomuk::Vocabulary::NIE::title()).toString();

    if(!titleString.isEmpty()) {
        titleString.prepend(QLatin1String("\ttitle = \""));
        titleString.append(QLatin1String("\""));
    }

    return titleString;
}

QString BibTexExporter::getType(const Nepomuk::Resource &document)
{
    QString typeString = document.property(Nepomuk::Vocabulary::NBIB::type()).toString();

    if(!typeString.isEmpty()) {
        typeString.prepend(QLatin1String("\ttype = \""));
        typeString.append(QLatin1String("\""));
    }

    return typeString;
}

QString BibTexExporter::getUrl(const Nepomuk::Resource &document)
{
    QString urlString = document.property(Nepomuk::Vocabulary::NBIB::url()).toString();

    if(!urlString.isEmpty()) {
        urlString.prepend(QLatin1String("\turl = \""));
        urlString.append(QLatin1String("\""));
    }

    return urlString;
}

