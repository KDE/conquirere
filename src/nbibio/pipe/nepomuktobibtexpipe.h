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

/**
  * @brief Transforms a list of Nepomuk resources into a KBibTex equivalent File
  *
  * If this pipe operates on NBIB:Publication resources rather than NBIB:Reference resources
  * a custom reference key will be generated in the form of "BibTexExportXX" where XX represents the
  * number in the list
  *
  * After the execution of pipeExport() the File can be retrieved via bibtexFile()
  */
class NepomukToBibTexPipe : public NepomukPipe
{
public:
    NepomukToBibTexPipe();

    virtual ~NepomukToBibTexPipe();

    /**
      * Does the piping action
      *
      * @p resources list of publication or reference resources
      */
    void pipeExport(QList<Nepomuk2::Resource> resources);

    /**
      * @return the transformed KBibTeX File with all BibTeX entries
      */
    File *bibtexFile() const;

    /**
      * If @p strict is true the nepomuk resource types will be transformed into standard bibtex types
      *
      * For example @BachelorThesis becomes Masterthesis
      */
    void useStrictTypes(bool strict);
    void setSyncDetails(const QString &url, const QString &userid);

    /**
      * Includes the nepomuk resource uri for the publication and reference to the bibtex output
      */
    void addNepomukUries(bool addThem);

private:
    /**
      * @todo implement strict bibtex entry type export
      */
    QString retrieveEntryType(Nepomuk2::Resource reference, Nepomuk2::Resource publication);
    void collectContent(Entry *e, Nepomuk2::Resource reference, Nepomuk2::Resource publication);
    void collectNoteContent(Entry *e, Nepomuk2::Resource note);
    void collectAttachmentContent(Entry *e, Nepomuk2::Resource attachment);

    // Helper function to get the nepomuk data
    // title + booktitle
    void setTitle(Entry *e, Nepomuk2::Resource publication, Nepomuk2::Resource reference);
    void setSeriesEditors(Entry *e, Nepomuk2::Resource publication);
    void setPublicationDate(Entry *e, Nepomuk2::Resource publication);
    void setPublisher(Entry *e, Nepomuk2::Resource publication);
    void setOrganization(Entry *e, Nepomuk2::Resource publication);
    void setCrossref(Entry *e, Nepomuk2::Resource publication);
    void setUrl(Entry *e, Nepomuk2::Resource publication);
    void setSeries(Entry *e, Nepomuk2::Resource publication);
    void setJournal(Entry *e, Nepomuk2::Resource publication);
    void setISSN(Entry *e, Nepomuk2::Resource publication);
    void setChapter(Entry *e, Nepomuk2::Resource reference);
    void setEvent(Entry *e, Nepomuk2::Resource publication);
    void setCode(Entry *e, Nepomuk2::Resource publication);
    void setCodeNumber(Entry *e, Nepomuk2::Resource publication);
    void setCodeVolume(Entry *e, Nepomuk2::Resource publication);
    void setReporter(Entry *e, Nepomuk2::Resource publication);
    void setReporterVolume(Entry *e, Nepomuk2::Resource publication);
    void setAssignee(Entry *e, Nepomuk2::Resource publication);
    void setNote(Entry *e, Nepomuk2::Resource publication);
    void setKewords(Entry *e, Nepomuk2::Resource publication);
    void setArticleType(Entry *e, Nepomuk2::Resource publication);

    /**
      * Reuseable function that simply transforms a nepomuk value as QString to bibtex without further processing
      */
    void setValue(Entry *e, Nepomuk2::Resource publication, QUrl property, const QString &bibkey);
    void setContact(Entry *e, Nepomuk2::Resource publication, QUrl property, const QString &bibkey);

    void setSyncDetails(Entry *e, Nepomuk2::Resource publication);

    File *m_bibtexFile;
    bool m_strict;
    bool m_addNepomukUris;
    QString m_syncUrl;
    QString m_syncUserId;

    qreal m_percentPerResource;
};

#endif // NEPOMUKTOBIBTEXPIPE_H
