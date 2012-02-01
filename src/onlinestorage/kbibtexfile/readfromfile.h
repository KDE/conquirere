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

#ifndef READFROMFILE_H
#define READFROMFILE_H

#include "../readfromstorage.h"

#include <QtCore/QObject>

/**
  * @brief Wrapper around kbibtex normal file import, adds no additional functionality but allows to reuse sync plugin structure
  *
  * Solely created to allow automatic synchronizing of the bibtex entries to any kind of file
  * reusing all settings/structures that are used to sync to an online storage as well
  *
  * Possible use case is the sync of an local bibfile to a remote file of any kind. Could be  a .bib file too
  * or .html, .ris, .pdf etc.
  *
  * @author Jörg Ehrichs <joerg.ehrichs@gmx.de>
  */
class ReadFromFile : public ReadFromStorage
{
    Q_OBJECT
public:
    explicit ReadFromFile(QObject *parent = 0);

    File *getFile();
    QList<CollectionInfo> getCollectionInfo();

public slots:
    /**
      * Opens a file via KIO::NetAccess from any space
      *
      * Support based on file extension, only .bib, .pdf, .ris supported currently
      *
      * @todo import based on .html and other formats exported by KBibTeX FileExporters ( blg, ps, rtf, xslt, xml)
      */
    void fetchItems(const QString &collection = QString());

    /**
      * @todo not implemented right now. should do fetchItems() and filter the id as citekey and emit only this one
      */
    void fetchItem(const QString &id, const QString &collection = QString() );

    /**
      * not supported for files
      */
    void fetchCollections(const QString &parent = QString() );

    /**
      * not supported for files
      */
    void fetchCollection(const QString &collection );

protected slots:
    void requestFinished();

private:
    File *m_importedFile;
};

#endif // READFROMFILE_H
