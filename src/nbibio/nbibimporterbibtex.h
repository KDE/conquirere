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

class KJob;
class File;

class NBibImporterBibTex : public NBibImporter
{
    Q_OBJECT
public:
    explicit NBibImporterBibTex();

    void setAkonadiAddressbook(Akonadi::Collection & addressbook);
    void setImportContactToAkonadi(bool import);
    void setFindDuplicates(bool findThem);

    bool load(QIODevice *iodevice, QStringList *errorLog = NULL);

    bool readBibFile(const QString & filename);
    File *bibFile();
    bool findDuplicates();
    bool pipeToNepomuk();

private slots:
    void calculateImportProgress(int current, int max);

private:
    File *m_importedEntries;
    bool m_findDuplicates;
    bool m_contactToAkonadi;
    Akonadi::Collection m_addressbook;
};

#endif // NBIBIMPORTERBIBTEX_H
