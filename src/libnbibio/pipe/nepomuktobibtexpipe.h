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

#ifndef NEPOMUKTOBIBTEXPIPE_H
#define NEPOMUKTOBIBTEXPIPE_H

#include "nepomukpipe.h"

#include <kbibtex/file.h>

class Entry;

class NepomukToBibTexPipe : public NepomukPipe
{
public:
    NepomukToBibTexPipe();

    void pipeExport(QList<Nepomuk::Resource> resources);

    File bibtexFile();

private:
    QString retrieveEntryType(Nepomuk::Resource reference, Nepomuk::Resource publication);
    void collectContent(Entry *e, Nepomuk::Resource reference, Nepomuk::Resource publication);

    // Helper function to get the nepomuk data
    void setTitle(Entry *e, Nepomuk::Resource publication, Nepomuk::Resource reference);
    void setAuthors(Entry *e, Nepomuk::Resource publication);
    void setEditors(Entry *e, Nepomuk::Resource publication);
    void setPublicationDate(Entry *e, Nepomuk::Resource publication);
    void setPublisher(Entry *e, Nepomuk::Resource publication);
    void setOrganization(Entry *e, Nepomuk::Resource publication);
    void setCrossref(Entry *e, Nepomuk::Resource publication);
    void setUrl(Entry *e, Nepomuk::Resource publication);
    void setSeries(Entry *e, Nepomuk::Resource publication);
    void setEdition(Entry *e, Nepomuk::Resource publication);
    void setJournal(Entry *e, Nepomuk::Resource publication);
    void setVolume(Entry *e, Nepomuk::Resource publication);
    void setNumber(Entry *e, Nepomuk::Resource publication);
    void setPublicationMethod(Entry *e, Nepomuk::Resource publication);
    void setType(Entry *e, Nepomuk::Resource publication);
    void setCopyrigth(Entry *e, Nepomuk::Resource publication);
    void setLastAccessed(Entry *e, Nepomuk::Resource publication);
    void setEPrint(Entry *e, Nepomuk::Resource publication);
    void setISBN(Entry *e, Nepomuk::Resource publication);
    void setISSN(Entry *e, Nepomuk::Resource publication);
    void setLCCN(Entry *e, Nepomuk::Resource publication);
    void setMRNumber(Entry *e, Nepomuk::Resource publication);
    void setDOI(Entry *e, Nepomuk::Resource publication);
    void setAbstract(Entry *e, Nepomuk::Resource publication);
    void setTOC(Entry *e, Nepomuk::Resource publication);
    void setNote(Entry *e, Nepomuk::Resource publication);
    void setAnnote(Entry *e, Nepomuk::Resource publication);
    void setChapter(Entry *e, Nepomuk::Resource reference);
    void setPages(Entry *e, Nepomuk::Resource reference);
    void setKewords(Entry *e, Nepomuk::Resource publication);

    File m_bibtexFile;
};

#endif // NEPOMUKTOBIBTEXPIPE_H
