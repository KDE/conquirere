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

#ifndef BIBTEXTONEPOMUKPIPE_H
#define BIBTEXTONEPOMUKPIPE_H

#include "bibtexpipe.h"

#include <kbibtex/value.h>
#include <Nepomuk/Resource>

#include <QUrl>
#include <QList>

class Entry;

class BibTexToNepomukPipe : public BibTexPipe
{
public:
    BibTexToNepomukPipe();

    void pipeExport(File & bibEntries);

private:
    struct Name {
        QString first;
        QString last;
        QString suffix;
        QString full;
    };
    void import(Entry *e);

    QUrl typeToUrl(const QString & entryType);
    void addContent(const QString &key, const Value &value, Nepomuk::Resource publication, Nepomuk::Resource reference, const QString & originalEntryType);

    void addPublisher(const Value &publisherString, const Value &address, Nepomuk::Resource publication);
    void addJournal(const Value &journal, const Value &volume, const Value &number, Nepomuk::Resource publication);


    void addAbstract(const QString &content, Nepomuk::Resource publication);
    void addAnnote(const QString &content, Nepomuk::Resource publication);
    void addAuthor(const Value &content, Nepomuk::Resource publication, Nepomuk::Resource reference, const QString & originalEntryType);
    void addBooktitle(const QString &content, Nepomuk::Resource publication, const QString & originalEntryType);
    void addChapter(const QString &content, Nepomuk::Resource publication, Nepomuk::Resource reference);
    void addCopyrigth(const QString &content, Nepomuk::Resource publication);
    void addCrossref(const QString &content, Nepomuk::Resource publication);
    void addDoi(const QString &content, Nepomuk::Resource publication);
    void addEdition(const QString &content, Nepomuk::Resource publication);
    void addEditor(const Value &content, Nepomuk::Resource publication);
    void addEprint(const QString &content, Nepomuk::Resource publication);
    void addHowPublished(const QString &content, Nepomuk::Resource publication);
    void addInstitution(const Value &content, Nepomuk::Resource publication);
    void addIsbn(const QString &content, Nepomuk::Resource publication);
    void addIssn(const QString &content, Nepomuk::Resource publication);
    void addLanguage(const QString &content, Nepomuk::Resource publication);
    void addLccn(const QString &content, Nepomuk::Resource publication);
    void addMonth(const QString &content, Nepomuk::Resource publication);
    void addMrNumber(const QString &content, Nepomuk::Resource publication);
    void addNote(const QString &content, Nepomuk::Resource publication);
    void addNumber(const QString &content, Nepomuk::Resource publication);
    void addOrganization(const QString &content, Nepomuk::Resource publication);
    void addPages(const QString &content, Nepomuk::Resource reference);
    void addSchool(const Value &content, Nepomuk::Resource publication);
    void addSeries(const QString &content, Nepomuk::Resource publication);
    void addTitle(const QString &content, Nepomuk::Resource publication, Nepomuk::Resource reference, const QString & originalEntryType);
    void addType(const QString &content, Nepomuk::Resource publication);
    void addUrl(const QString &content, Nepomuk::Resource publication);
    void addVolume(const QString &content, Nepomuk::Resource publication);
    void addYear(const QString &content, Nepomuk::Resource publication);
    void addKewords(const QString &content, Nepomuk::Resource publication);

    QList<Nepomuk::Resource> m_allContacts;
    QList<Nepomuk::Resource> m_allPublications;
    QList<Nepomuk::Resource> m_allReferences;
};

#endif // BIBTEXTONEPOMUKPIPE_H