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

#ifndef NBIBIMPORTERBIBTEX_H
#define NBIBIMPORTERBIBTEX_H

#include "nbibimporter.h"

#include <Akonadi/Collection>
#include <Nepomuk/Thing>

class File;
class EntryClique;

/**
  * @brief Imports a bibtex file into the nepomuk storage.
  *
  * Makes use of the KBibTeX::FileImporterBibTeX to read a bibtex file, and optionally finds any duplicates
  * in this file via KBibTeX::FindDuplicates.
  *
  * Afterwards the imported KBibTeX::File is imported via the BibTexToNepomukPipe int othe Nepomuk storage.
  * An Akonadi addressbook can be specified which will be used by the BibTexToNepomukPipe to add all authors
  * also into Akonadi and not only into Nepomuk.
  *
  */
class NBibImporterBibTex : public NBibImporter
{
    Q_OBJECT
public:
    enum FileType {
        EXPORT_BIBTEX,
//        EXPORT_COPAC,
//        EXPORT_ENDNOTE,
//        EXPORT_ISI,
//        EXPORT_MEDLINE,
//        EXPORT_MODS,
        EXPORT_PDF,
        EXPORT_RIS
    };

    explicit NBibImporterBibTex();
    virtual ~NBibImporterBibTex();

    /**
      * loads the bibtex file directly into the nepomuk staorage without using Akonadi or the possibility
      * to find duplicates first.
      *
      * @p iodevice the device to read from via KBibTeX::FileImporterBibTeX
      * @p errorLog pointer to the errors
      *
      * Better use the single chains
      * @li setAkonadiAddressbook()
      * @li setFindDuplicates()
      * @li readBibFile() which calls findDuplicates() if specified
      * @li pipeToNepomuk()
      * and handle user interaction before the pipe to nepomuk.
      */
    bool load(QIODevice *iodevice, QStringList *errorLog = NULL);

    /**
      * Sets the Akonadi addressbook where all contacts (authors, editors etc) are imported to beside the Nepomuk storage.
      *
      * @p addressbook a valid Akonadi::Collection representing a addressbook
      */
    void setAkonadiAddressbook(Akonadi::Collection & addressbook);

    /**
      * Specify if duplicate entries in the imported bibtex file should be searched.
      *
      * The duplicates can be retrieved via duplicates() and changed via KBibTeX::FindDuplicatesUi
      */
    void setFindDuplicates(bool findThem);

    void setFileType(NBibImporterBibTex::FileType selectedFileType);

    /**
      * Calls KBibTeX::FileImporterBibTeX to read the bibtex file @p filename into the system.
      *
      * Also calls findDuplicates() if setFindDuplicates() is true.
      *
      * @p filename the path and name of the file to read from via KBibTeX::FileImporterBibTeX
      * @p errorLog pointer to the errors
      */
    bool readBibFile(const QString & filename, QStringList *errorLog = NULL);

    /**
      * @return the bibtex file as returned from KBibTeX::FileImporterBibTeX
      */
    File *bibFile();

    /**
      * Uses KBibTeX::FindDuplicates to find all duplicate entries in the imported KBibTeX::File
      *
      * the results can be retrieved via duplicates()
      */
    bool findDuplicates();

    void setProjectPimoThing(Nepomuk::Thing projectThing);

    /**
      * @return the list of duplicated entries that can be used by KBibTeX::FindDuplicatesUi
      */
    QList<EntryClique*> duplicates();

    /**
      * The main importer that takes the read input from readBibFile() and puts it into the nepomuk storage
      * @p importFile is the file from which bibliography is imported (used to resolve relative paths)
      * @p errorLog pointer to the errors
      */
    bool pipeToNepomuk(QStringList *errorLog = NULL);

private slots:
    /**
      * Transforms the process signals from KBibTeX into a 0-100% range
      *
      * Used to transform readBibFile() signal to 0-50 and findDuplicates() to 50-100.
      *
      * @see progress();
      */
    void calculateImportProgress(int current, int max);

private:
    File *m_importedEntries;
    KUrl m_importFile;
    bool m_findDuplicates;
    NBibImporterBibTex::FileType m_selectedFileType;
    QList<EntryClique*> m_cliques;
    Akonadi::Collection m_addressbook;
    Nepomuk::Thing m_projectThing;
};

#endif // NBIBIMPORTERBIBTEX_H
