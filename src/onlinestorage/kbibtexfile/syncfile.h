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

#ifndef SYNCFILE_H
#define SYNCFILE_H

#include "../syncstorage.h"

class ReadFromFile;
class WriteToFile;

/**
  * @brief sync bibtex file with other file types, supported by kbibtex exporters
  *
  * no real use for KBibTeX itself, but comes in handy for conquirere
  *
  * @author Jörg Ehrichs <joerg.ehrichs@gmx.de>
  */
class SyncFile : public SyncStorage
{
    Q_OBJECT
public:
    explicit SyncFile(QObject *parent = 0);
    virtual ~SyncFile();

    /**
      * @todo merge results from server with local if server side changed
      */
    void syncWithStorage(File *bibfile, const QString &collection);

private slots:
    /**
      * Called by the ReadFromFile
      */
    void readSync(const File &serverFiles);

    /**
      * Called by WriteToFile
      */
    void writeSync(const File &serverFiles);

    /**
      * fit the 0-100 % progress from the write action into the 50-100 % for the overall progress
      */
    void writeProgress(int writeProgress);

private:
    File *m_systemFiles;
    ReadFromFile *m_rfz;
    WriteToFile *m_wtz;
    QString m_addToCollection;
};

#endif // SYNCFILE_H
