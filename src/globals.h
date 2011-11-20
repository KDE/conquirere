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
#include <Nepomuk/Resource>

#include <KLocale>

#include "nbib.h"

enum ProjectTreeRole {
    Role_Library = Qt::UserRole,
    Role_ResourceType,
    Role_ResourceFilter,
    Role_Project
};

enum LibraryType {
    Library_System,
    Library_Project
};

enum ResourceSelection {
    Resource_Library,
    Resource_Document,
    Resource_Mail,
    Resource_Media,
    Resource_Reference,
    Resource_Publication,
    Resource_Website,
    Resource_Note
};

enum ResourceFilter {
    Filter_None,
    Filter_Articles,
    Filter_Journals,
    Filter_Magazine,
    Filter_Newspaper,
    Filter_Books,
    Filter_Proceedings,
    Filter_Thesisis,
    Filter_Presentation,
    Filter_Script,
    Filter_Report
};

enum BibEntryType {
    BibType_Article = 0,       /**< An article from a journal or magazine. */
    BibType_Book,              /**< A book with an explicit publisher. */
    BibType_Booklet,           /**< A work that is printed and bound, but without a named publisher or sponsoring institution. */
    BibType_Collection,        /**< A collection of different publications */
    BibType_Bachelorthesis,    /**< A Bachelor thesis. */
    BibType_Mastersthesis,     /**< A Master's thesis. */
    BibType_Phdthesis,         /**< A PhD thesis. */
    BibType_Report,
    BibType_Techreport,        /**< A report published by a school or other institution, usually numbered within a series. */
    BibType_Electronic ,       /**< A electronic publication */
    BibType_Script,
    BibType_Presentation,
    BibType_Unpublished,       /**< A document having an author and title, but not formally published. */
    BibType_Misc,              /**< Use this type when nothing else fits. */
    BibType_Manual,            /**< Technical documentation. */
    BibType_Proceedings,       /**< The proceedings of a conference. */
    BibType_JournalIssue,      /**< An issue of journal */
    BibType_Journal,           /**< An journal */
    BibType_NewspaperIssue,    /**< An issue of a newspaper */
    BibType_Newspaper,         /**< An journal or newspaper */
    BibType_MagazinIssue,      /**< An issue of a magazine */
    BibType_Magazin,           /**< An journal or magazine */
    BibType_Standard,
    BibType_Patent,            /**< A patent. */
    BibType_CodeOfLaw,
    BibType_CourtReporter,
    BibType_Legislation,
    BibType_Bill,
    BibType_Statute,
    BibType_LegalCaseDocument,
    BibType_Decision,
    BibType_Brief
};
 
static const QStringList BibEntryTypeTranslation = QStringList() << I18N_NOOP("Article")
                                                                 << I18N_NOOP("Book")
                                                                 << I18N_NOOP("Booklet")
                                                                 << I18N_NOOP("Collection")
                                                                 << I18N_NOOP("Bachelor thesis")
                                                                 << I18N_NOOP("Master thesis")
                                                                 << I18N_NOOP("Phd thesis")
                                                                 << I18N_NOOP("Report")
                                                                 << I18N_NOOP("Techreport")
                                                                 << I18N_NOOP("Electronic")
                                                                 << I18N_NOOP("Script")
                                                                 << I18N_NOOP("Presentation")
                                                                 << I18N_NOOP("Unpublished")
                                                                 << I18N_NOOP("Misc")
                                                                 << I18N_NOOP("Manual")
                                                                 << I18N_NOOP("Proceedings")
                                                                 << I18N_NOOP("Journal Issue")
                                                                 << I18N_NOOP("Journal")
                                                                 << I18N_NOOP("Newspaper Issue")
                                                                 << I18N_NOOP("Newspaper")
                                                                 << I18N_NOOP("Magazin Issue")
                                                                 << I18N_NOOP("Magazin")
                                                                 << I18N_NOOP("Standard")
                                                                 << I18N_NOOP("Patent")
                                                                 << I18N_NOOP("Code")
                                                                 << I18N_NOOP("CourtReporter")
                                                                 << I18N_NOOP("Legislation")
                                                                 << I18N_NOOP("Bill")
                                                                 << I18N_NOOP("Statute")
                                                                 << I18N_NOOP("LegalCaseDocument")
                                                                 << I18N_NOOP("Decision")
                                                                 << I18N_NOOP("Brief");

static const QList<QUrl> BibEntryTypeURL = QList<QUrl>() << Nepomuk::Vocabulary::NBIB::Article()
                                                         << Nepomuk::Vocabulary::NBIB::Book()
                                                         << Nepomuk::Vocabulary::NBIB::Booklet()
                                                         << Nepomuk::Vocabulary::NBIB::Collection()
                                                         << Nepomuk::Vocabulary::NBIB::BachelorThesis()
                                                         << Nepomuk::Vocabulary::NBIB::MastersThesis()
                                                         << Nepomuk::Vocabulary::NBIB::PhdThesis()
                                                         << Nepomuk::Vocabulary::NBIB::Report()
                                                         << Nepomuk::Vocabulary::NBIB::Techreport()
                                                         << Nepomuk::Vocabulary::NBIB::Electronic()
                                                         << Nepomuk::Vocabulary::NBIB::Script()
                                                         << Nepomuk::Vocabulary::NBIB::Presentation()
                                                         << Nepomuk::Vocabulary::NBIB::Unpublished()
                                                         << Nepomuk::Vocabulary::NBIB::Publication()
                                                         << Nepomuk::Vocabulary::NBIB::Manual()
                                                         << Nepomuk::Vocabulary::NBIB::Proceedings()
                                                         << Nepomuk::Vocabulary::NBIB::JournalIssue()
                                                         << Nepomuk::Vocabulary::NBIB::Journal()
                                                         << Nepomuk::Vocabulary::NBIB::NewspaperIssue()
                                                         << Nepomuk::Vocabulary::NBIB::Newspaper()
                                                         << Nepomuk::Vocabulary::NBIB::MagazinIssue()
                                                         << Nepomuk::Vocabulary::NBIB::Magazin()
                                                         << Nepomuk::Vocabulary::NBIB::Standard()
                                                         << Nepomuk::Vocabulary::NBIB::Patent()
                                                         << Nepomuk::Vocabulary::NBIB::CodeOfLaw()
                                                         << Nepomuk::Vocabulary::NBIB::CourtReporter()
                                                         << Nepomuk::Vocabulary::NBIB::Legislation()
                                                         << Nepomuk::Vocabulary::NBIB::Bill()
                                                         << Nepomuk::Vocabulary::NBIB::Statute()
                                                         << Nepomuk::Vocabulary::NBIB::LegalCaseDocument()
                                                         << Nepomuk::Vocabulary::NBIB::Decision()
                                                         << Nepomuk::Vocabulary::NBIB::Brief();

static BibEntryType BibEntryTypeFromUrl(const Nepomuk::Resource & resource)
{
    if(resource.hasType(Nepomuk::Vocabulary::NBIB::Article())) {
        return BibType_Article;
    }
    if(resource.hasType(Nepomuk::Vocabulary::NBIB::Proceedings())) {
        return BibType_Proceedings;
    }
    if(resource.hasType(Nepomuk::Vocabulary::NBIB::BachelorThesis())) {
        return BibType_Bachelorthesis;
    }
    if(resource.hasType(Nepomuk::Vocabulary::NBIB::Book())) {
        return BibType_Book;
    }
    if(resource.hasType(Nepomuk::Vocabulary::NBIB::Booklet())) {
        return BibType_Booklet;
    }
    if(resource.hasType(Nepomuk::Vocabulary::NBIB::Manual())) {
        return BibType_Manual;
    }
    if(resource.hasType(Nepomuk::Vocabulary::NBIB::MastersThesis())) {
        return BibType_Mastersthesis;
    }
    if(resource.hasType(Nepomuk::Vocabulary::NBIB::PhdThesis())) {
        return BibType_Phdthesis;
    }
    if(resource.hasType(Nepomuk::Vocabulary::NBIB::Techreport())) {
        return BibType_Techreport;
    }
    if(resource.hasType(Nepomuk::Vocabulary::NBIB::Unpublished())) {
        return BibType_Unpublished;
    }
    if(resource.hasType(Nepomuk::Vocabulary::NBIB::Patent())) {
        return BibType_Patent;
    }
    if(resource.hasType(Nepomuk::Vocabulary::NBIB::Electronic())) {
        return BibType_Electronic;
    }
    if(resource.hasType(Nepomuk::Vocabulary::NBIB::JournalIssue())) {
        return BibType_JournalIssue;
    }
    if(resource.hasType(Nepomuk::Vocabulary::NBIB::NewspaperIssue())) {
        return BibType_NewspaperIssue;
    }
    if(resource.hasType(Nepomuk::Vocabulary::NBIB::Newspaper())) {
        return BibType_Newspaper;
    }
    if(resource.hasType(Nepomuk::Vocabulary::NBIB::MagazinIssue())) {
        return BibType_MagazinIssue;
    }
    if(resource.hasType(Nepomuk::Vocabulary::NBIB::Magazin())) {
        return BibType_Magazin;
    }
    if(resource.hasType(Nepomuk::Vocabulary::NBIB::Report())) {
        return BibType_Report;
    }
    if(resource.hasType(Nepomuk::Vocabulary::NBIB::Script())) {
        return BibType_Script;
    }
    if(resource.hasType(Nepomuk::Vocabulary::NBIB::Presentation())) {
        return BibType_Presentation;
    }
    if(resource.hasType(Nepomuk::Vocabulary::NBIB::Standard())) {
        return BibType_Standard;
    }
    if(resource.hasType(Nepomuk::Vocabulary::NBIB::Bill())) {
        return BibType_Bill;
    }
    if(resource.hasType(Nepomuk::Vocabulary::NBIB::Decision())) {
        return BibType_Decision;
    }
    if(resource.hasType(Nepomuk::Vocabulary::NBIB::Brief())) {
        return BibType_Brief;
    }
    if(resource.hasType(Nepomuk::Vocabulary::NBIB::Statute())) {
        return BibType_Statute;
    }
    if(resource.hasType(Nepomuk::Vocabulary::NBIB::CodeOfLaw())) {
        return BibType_CodeOfLaw;
    }
    if(resource.hasType(Nepomuk::Vocabulary::NBIB::CourtReporter())) {
        return BibType_CourtReporter;
    }
    if(resource.hasType(Nepomuk::Vocabulary::NBIB::LegalCaseDocument())) {
        return BibType_LegalCaseDocument;
    }
    if(resource.hasType(Nepomuk::Vocabulary::NBIB::Legislation())) {
        return BibType_Legislation;
    }
    if(resource.hasType(Nepomuk::Vocabulary::NBIB::Collection())) {
        return BibType_Collection;
    }
    return BibType_Misc;
}

#endif // GLOBALS_H
