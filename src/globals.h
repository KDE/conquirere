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

#ifndef GLOBALS_H
#define GLOBALS_H

#include <Qt>
#include <QString>
#include <QStringList>
#include <Nepomuk2/Resource>

#include <KLocale>

#include "nbib.h"

enum ProjectTreeRole {
    Role_LibraryType = Qt::UserRole,    /**< keeps the LibraryType */
    Role_ResourceType,                  /**< keeps one of the ResourceSelection entries */
    Role_ResourceFilter,                /**< keeps one of the BibEntryType entries */
    Role_ProjectName,                   /**< keeps the name of the conected project */
    Role_ProjectThing                   /**< keeps the pimo::thing uri conected project so we can identify it*/
};

enum LibraryType {
    Library_System,
    Library_Project
};

enum ResourceSelection {
    Resource_SearchResults,
    Resource_Library,
    Resource_Document,
    Resource_Mail,
    Resource_Media,
    Resource_Reference,
    Resource_Publication,
    Resource_Series,
    Resource_Website,
    Resource_Note,
    Resource_Event,

    Max_ResourceTypes
};

enum BibEntryType {
    BibType_Article = 0,       /**< An article from a journal or magazine. */
    BibType_Book,              /**< A book with an explicit publisher. */
    BibType_Booklet,           /**< A work that is printed and bound, but without a named publisher or sponsoring institution. */
    BibType_Collection,        /**< A collection of different publications */
    BibType_Encyclopedia,      /**< A encyclopedia publications */
    BibType_Dictionary,        /**< A dictionary publications */
    BibType_Bachelorthesis,    /**< A Bachelor thesis. */
    BibType_Mastersthesis,     /**< A Master's thesis. */
    BibType_Phdthesis,         /**< A PhD thesis. */
    BibType_Thesis,            /**< any kind of thesis */
    BibType_Report,
    BibType_Techreport,        /**< A report published by a school or other institution, usually numbered within a series. */
    BibType_Electronic ,       /**< A electronic publication */
    BibType_Forum ,            /**< A electronic publication */
    BibType_ForumPost ,        /**< A electronic publication */
    BibType_Blog ,             /**< A electronic publication */
    BibType_BlogPost ,         /**< A electronic publication */
    BibType_WebPage ,          /**< A electronic publication */
    BibType_WebSite ,          /**< A electronic publication */
    BibType_Script,
    BibType_Presentation,
    BibType_Unpublished,       /**< A document having an author and title, but not formally published. */
    BibType_Misc,              /**< Use this type when nothing else fits. */
    BibType_Manual,            /**< Technical documentation. */
    BibType_Proceedings,       /**< The proceedings of a conference. */
    BibType_JournalIssue,      /**< An issue of journal */
    BibType_NewspaperIssue,    /**< An issue of a newspaper */
    BibType_MagazinIssue,      /**< An issue of a magazine */
    BibType_Standard,
    BibType_Patent,            /**< A patent. */
    BibType_CodeOfLaw,
    BibType_CourtReporter,
    BibType_Legislation,
    BibType_Bill,
    BibType_Statute,
    BibType_LegalCaseDocument,
    BibType_Decision,
    BibType_Brief,
    BibType_Map,

    Max_BibTypes
};

static const QStringList BibEntryTypeTranslation = QStringList() << I18N_NOOP("Article")
                                                                 << I18N_NOOP("Book")
                                                                 << I18N_NOOP("Booklet")
                                                                 << I18N_NOOP("Collection")
                                                                 << I18N_NOOP("Encyclopedia")
                                                                 << I18N_NOOP("Dictionary")
                                                                 << I18N_NOOP("Bachelor thesis")
                                                                 << I18N_NOOP("Master thesis")
                                                                 << I18N_NOOP("Phd thesis")
                                                                 << I18N_NOOP("Thesis")
                                                                 << I18N_NOOP("Report")
                                                                 << I18N_NOOP("Techreport")
                                                                 << I18N_NOOP("Electronic")
                                                                 << I18N_NOOP("Forum")
                                                                 << I18N_NOOP("Forum post")
                                                                 << I18N_NOOP("Blog")
                                                                 << I18N_NOOP("Blog post")
                                                                 << I18N_NOOP("Webpage")
                                                                 << I18N_NOOP("Website")
                                                                 << I18N_NOOP("Script")
                                                                 << I18N_NOOP("Presentation")
                                                                 << I18N_NOOP("Unpublished")
                                                                 << I18N_NOOP("Misc")
                                                                 << I18N_NOOP("Manual")
                                                                 << I18N_NOOP("Proceedings")
                                                                 << I18N_NOOP("Journal Issue")
                                                                 << I18N_NOOP("Newspaper Issue")
                                                                 << I18N_NOOP("Magazin Issue")
                                                                 << I18N_NOOP("Standard")
                                                                 << I18N_NOOP("Patent")
                                                                 << I18N_NOOP("Code")
                                                                 << I18N_NOOP("CourtReporter")
                                                                 << I18N_NOOP("Legislation")
                                                                 << I18N_NOOP("Bill")
                                                                 << I18N_NOOP("Statute")
                                                                 << I18N_NOOP("LegalCaseDocument")
                                                                 << I18N_NOOP("Decision")
                                                                 << I18N_NOOP("Brief")
                                                                 << I18N_NOOP("Map");

static const QStringList BibEntryTypeIcon = QStringList()        << QString("knewsticker")
                                                                 << QString("acroread")
                                                                 << QString("acroread")
                                                                 << QString("Collection")
                                                                 << QString("accessories-dictionary")
                                                                 << QString("accessories-dictionary")
                                                                 << QString("applications-education-university")
                                                                 << QString("applications-education-university")
                                                                 << QString("applications-education-university")
                                                                 << QString("applications-education-university")
                                                                 << QString("kjournal")
                                                                 << QString("kjournal")
                                                                 << QString("applications-internet")
                                                                 << QString("applications-internet")
                                                                 << QString("applications-internet")
                                                                 << QString("applications-internet")
                                                                 << QString("applications-internet")
                                                                 << QString("applications-internet")
                                                                 << QString("applications-internet")
                                                                 << QString("kjournal")
                                                                 << QString("kpresenter")
                                                                 << QString("acroread")
                                                                 << QString("acroread")
                                                                 << QString("kjournal")
                                                                 << QString("preferences-system-time")
                                                                 << QString("kjournal")
                                                                 << QString("knewsticker")
                                                                 << QString("kjournal")
                                                                 << QString("knewsticker")
                                                                 << QString("knewsticker")
                                                                 << QString("knewsticker")
                                                                 << QString("knewsticker")
                                                                 << QString("knewsticker")
                                                                 << QString("knewsticker")
                                                                 << QString("knewsticker")
                                                                 << QString("knewsticker")
                                                                 << QString("knewsticker")
                                                                 << QString("knewsticker")
                                                                 << QString("services");

static const QList<QUrl> BibEntryTypeURL = QList<QUrl>() << Nepomuk2::Vocabulary::NBIB::Article()
                                                         << Nepomuk2::Vocabulary::NBIB::Book()
                                                         << Nepomuk2::Vocabulary::NBIB::Booklet()
                                                         << Nepomuk2::Vocabulary::NBIB::Collection()
                                                         << Nepomuk2::Vocabulary::NBIB::Encyclopedia()
                                                         << Nepomuk2::Vocabulary::NBIB::Dictionary()
                                                         << Nepomuk2::Vocabulary::NBIB::BachelorThesis()
                                                         << Nepomuk2::Vocabulary::NBIB::MastersThesis()
                                                         << Nepomuk2::Vocabulary::NBIB::PhdThesis()
                                                         << Nepomuk2::Vocabulary::NBIB::Thesis()
                                                         << Nepomuk2::Vocabulary::NBIB::Report()
                                                         << Nepomuk2::Vocabulary::NBIB::Techreport()
                                                         << Nepomuk2::Vocabulary::NBIB::Electronic()
                                                         << Nepomuk2::Vocabulary::NBIB::Forum()
                                                         << Nepomuk2::Vocabulary::NBIB::ForumPost()
                                                         << Nepomuk2::Vocabulary::NBIB::Blog()
                                                         << Nepomuk2::Vocabulary::NBIB::BlogPost()
                                                         << Nepomuk2::Vocabulary::NBIB::Webpage()
                                                         << Nepomuk2::Vocabulary::NBIB::Website()
                                                         << Nepomuk2::Vocabulary::NBIB::Script()
                                                         << Nepomuk2::Vocabulary::NBIB::Presentation()
                                                         << Nepomuk2::Vocabulary::NBIB::Unpublished()
                                                         << Nepomuk2::Vocabulary::NBIB::Publication()
                                                         << Nepomuk2::Vocabulary::NBIB::Manual()
                                                         << Nepomuk2::Vocabulary::NBIB::Proceedings()
                                                         << Nepomuk2::Vocabulary::NBIB::JournalIssue()
                                                         << Nepomuk2::Vocabulary::NBIB::NewspaperIssue()
                                                         << Nepomuk2::Vocabulary::NBIB::MagazinIssue()
                                                         << Nepomuk2::Vocabulary::NBIB::Standard()
                                                         << Nepomuk2::Vocabulary::NBIB::Patent()
                                                         << Nepomuk2::Vocabulary::NBIB::CodeOfLaw()
                                                         << Nepomuk2::Vocabulary::NBIB::CourtReporter()
                                                         << Nepomuk2::Vocabulary::NBIB::Legislation()
                                                         << Nepomuk2::Vocabulary::NBIB::Bill()
                                                         << Nepomuk2::Vocabulary::NBIB::Statute()
                                                         << Nepomuk2::Vocabulary::NBIB::LegalCaseDocument()
                                                         << Nepomuk2::Vocabulary::NBIB::Decision()
                                                         << Nepomuk2::Vocabulary::NBIB::Brief()
                                                         << Nepomuk2::Vocabulary::NBIB::Map();

static BibEntryType BibEntryTypeFromUrl(const Nepomuk2::Resource & resource)
{
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Proceedings())) {
        return BibType_Proceedings;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::MastersThesis())) {
        return BibType_Mastersthesis;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::PhdThesis())) {
        return BibType_Phdthesis;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::BachelorThesis())) {
        return BibType_Bachelorthesis;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Thesis())) {
        return BibType_Thesis;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Book())) {
        return BibType_Book;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Booklet())) {
        return BibType_Booklet;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Manual())) {
        return BibType_Manual;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Techreport())) {
        return BibType_Techreport;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Unpublished())) {
        return BibType_Unpublished;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Patent())) {
        return BibType_Patent;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Forum())) {
        return BibType_Forum;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::ForumPost())) {
        return BibType_ForumPost;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Blog())) {
        return BibType_Blog;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::BlogPost())) {
        return BibType_BlogPost;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Webpage())) {
        return BibType_WebPage;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Website())) {
        return BibType_WebSite;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::JournalIssue())) {
        return BibType_JournalIssue;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::NewspaperIssue())) {
        return BibType_NewspaperIssue;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::MagazinIssue())) {
        return BibType_MagazinIssue;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Report())) {
        return BibType_Report;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Script())) {
        return BibType_Script;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Presentation())) {
        return BibType_Presentation;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Encyclopedia())) {
        return BibType_Encyclopedia;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Dictionary())) {
        return BibType_Dictionary;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Standard())) {
        return BibType_Standard;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Bill())) {
        return BibType_Bill;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Decision())) {
        return BibType_Decision;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Brief())) {
        return BibType_Brief;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Statute())) {
        return BibType_Statute;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::CodeOfLaw())) {
        return BibType_CodeOfLaw;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::CourtReporter())) {
        return BibType_CourtReporter;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::LegalCaseDocument())) {
        return BibType_LegalCaseDocument;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Legislation())) {
        return BibType_Legislation;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Collection())) {
        return BibType_Collection;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Electronic())) {
        return BibType_Electronic;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Map())) {
        return BibType_Map;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Article())) {
        return BibType_Article;
    }
    return BibType_Misc;
}

enum SeriesType {
    SeriesType_Series = 0,
    SeriesType_BookSeries,
    SeriesType_Journal,
    SeriesType_Magazin,
    SeriesType_Newspaper,

    Max_SeriesTypes
};

static const QStringList SeriesTypeTranslation = QStringList() << I18N_NOOP("Misc Series")
                                                                 << I18N_NOOP("Book Series")
                                                                 << I18N_NOOP("Journal")
                                                                 << I18N_NOOP("Magazin")
                                                                 << I18N_NOOP("Newspaper");

static const QStringList SeriesTypeIcon = QStringList()        << QString("acroread")
                                                               << QString("acroread")
                                                               << QString("kjournal")
                                                               << QString("kjournal")
                                                               << QString("knewsticker");

static const QList<QUrl> SeriesTypeURL = QList<QUrl>() << Nepomuk2::Vocabulary::NBIB::Series()
                                                       << Nepomuk2::Vocabulary::NBIB::BookSeries()
                                                       << Nepomuk2::Vocabulary::NBIB::Journal()
                                                       << Nepomuk2::Vocabulary::NBIB::Magazin()
                                                       << Nepomuk2::Vocabulary::NBIB::Newspaper();

static SeriesType SeriesTypeFromUrl(const Nepomuk2::Resource & resource)
{
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::BookSeries())) {
        return SeriesType_BookSeries;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Journal())) {
        return SeriesType_Journal;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Magazin())) {
        return SeriesType_Magazin;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Newspaper())) {
        return SeriesType_Newspaper;
    }
    return SeriesType_Series;
}

static const QStringList keysForPersonDetection = QStringList() << QString("translator")
                                                                << QString("contributor")
                                                                << QString("reviewedauthor")
                                                                << QString("serieseditor")
                                                                << QString("bookauthor")
                                                                << QString("commenter")
                                                                << QString("counsel")
                                                                << QString("cosponsor")
                                                                << QString("attorneyagent");
#endif // GLOBALS_H
