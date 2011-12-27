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
  * Solely created to allow automatic syncronizing of the bibtex entries to any kind of file
  * reusing all settings/structures that are used to sync to an online storage as well
  *
  * Possible use case is the sync of an local bibfile to a remote file of any kind. Could be  a .bib fiel too
  * or .html, .ris, .pdf etc.
  *
  * @todo add network transparency via KDE::KIO currently only localfiles via QFile are supported
  *
  * @author Jörg Ehrichs <joerg.ehrichs@gmx.de>
  */
class ReadFromFile : public ReadFromStorage
{
    Q_OBJECT
public:
    explicit ReadFromFile(QObject *parent = 0);

public slots:
    void fetchItems(const QString &collection = QString());
    void fetchItem(const QString &id, const QString &collection = QString() );
    void fetchCollections(const QString &parent = QString() );
    void fetchCollection(const QString &collection );

protected slots:
    void requestFinished();
};

#endif // READFROMFILE_H
