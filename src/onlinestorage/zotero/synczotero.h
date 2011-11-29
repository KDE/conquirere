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

#ifndef SYNCZOTERO_H
#define SYNCZOTERO_H

#include "../syncstorage.h"

class ReadFromZotero;
class WriteToZotero;

/**
  * @brief sync bibtex file with zotero storage
  *
  * @author Jörg Ehrichs <joerg.ehrichs@gmx.de>
  */
class SyncZotero : public SyncStorage
{
    Q_OBJECT
public:
    explicit SyncZotero(QObject *parent = 0);
    virtual ~SyncZotero();

    /**
      * @todo merge results from server with local if server side changed
      */
    void syncWithStorage(File *bibfile);

private slots:
    /**
      * Called by the ReadFromZotero
      */
    void readSync(File serverFiles);

    /**
      * @todo merge newly created zotero items with local ones, all fields are the same but server version adds
      *       zoteroKey, zoteroetag, zoteroupdated fields and has no citekey
      */
    void writeSync(File serverFiles);

    /**
      * fit the 0-100 % progress from the write action into the 50-100 % for the overall progress
      */
    void writeProgress(int writeProgress);

private:
    File *m_systemFiles;
    ReadFromZotero *m_rfz;
    WriteToZotero *m_wtz;
};

#endif // SYNCZOTERO_H
