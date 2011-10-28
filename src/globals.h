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
    Filter_Books,
    Filter_Proceedings,
    Filter_Thesisis,
    Filter_Presentation,
    Filter_Script,
    Filter_Techreport
};

enum BibEntryType {
    BibType_Article = 0,       /**< An article from a journal or magazine. */
    BibType_Bachelorthesis,    /**< A Bachelor thesis. */
    BibType_Book,              /**< A book with an explicit publisher. */
    BibType_Booklet,           /**< A work that is printed and bound, but without a named publisher or sponsoring institution. */
    BibType_Collection,        /**< A collection of different publications */
    BibType_Manual,            /**< Technical documentation. */
    BibType_Mastersthesis,     /**< A Master's thesis. */
    BibType_Misc,              /**< Use this type when nothing else fits. */
    BibType_Phdthesis,         /**< A PhD thesis. */
    BibType_Proceedings,       /**< The proceedings of a conference. */
    BibType_Techreport,        /**< A report published by a school or other institution, usually numbered within a series. */
    BibType_Unpublished,       /**< A document having an author and title, but not formally published. */
    BibType_Patent,            /**< A patent. */
    BibType_Electronic ,       /**< A electronic publication */
    BibType_JournalIssue,      /**< An issue of a journal or magazine */
    BibType_Journal            /**< An journal or magazine */
};
								 
static const QStringList BibEntryTypeTranslation = QStringList() << I18N_NOOP("Article")
                                                                 << I18N_NOOP("Bachelor thesis")
                                                                 << I18N_NOOP("Book")
                                                                 << I18N_NOOP("Booklet")
                                                                 << I18N_NOOP("Collection")
                                                                 << I18N_NOOP("Manual")
                                                                 << I18N_NOOP("Master thesis")
                                                                 << I18N_NOOP("Misc")
                                                                 << I18N_NOOP("Phd thesis")
                                                                 << I18N_NOOP("Proceedings")
                                                                 << I18N_NOOP("Techreport")
                                                                 << I18N_NOOP("Unpublished")
                                                                 << I18N_NOOP("Patent")
                                                                 << I18N_NOOP("Electronic")
                                                                 << I18N_NOOP("Journal Issue")
                                                                 << I18N_NOOP("Journal");

static const QList<QUrl> BibEntryTypeURL = QList<QUrl>() << Nepomuk::Vocabulary::NBIB::Article()
                                                         << Nepomuk::Vocabulary::NBIB::BachelorThesis()
                                                         << Nepomuk::Vocabulary::NBIB::Book()
                                                         << Nepomuk::Vocabulary::NBIB::Booklet()
                                                         << Nepomuk::Vocabulary::NBIB::Collection()
                                                         << Nepomuk::Vocabulary::NBIB::Manual()
                                                         << Nepomuk::Vocabulary::NBIB::MastersThesis()
                                                         << Nepomuk::Vocabulary::NBIB::Publication()
                                                         << Nepomuk::Vocabulary::NBIB::PhdThesis()
                                                         << Nepomuk::Vocabulary::NBIB::Proceedings()
                                                         << Nepomuk::Vocabulary::NBIB::Techreport()
                                                         << Nepomuk::Vocabulary::NBIB::Unpublished()
                                                         << Nepomuk::Vocabulary::NBIB::Patent()
                                                         << Nepomuk::Vocabulary::NBIB::Electronic()
                                                         << Nepomuk::Vocabulary::NBIB::JournalIssue()
                                                         << Nepomuk::Vocabulary::NBIB::Journal();

static BibEntryType BibEntryTypeFromUrl(Nepomuk::Resource & resource)
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
    if(resource.hasType(Nepomuk::Vocabulary::NBIB::Collection())) {
        return BibType_Collection;
    }
    return BibType_Misc;
}

#endif // GLOBALS_H
