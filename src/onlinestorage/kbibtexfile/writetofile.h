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

#ifndef WRITETOFILE_H
#define WRITETOFILE_H

#include "../writetostorage.h"

/**
  * @brief wrapper for the kbibtex FileExporters
  *
  * @author Jörg Ehrichs <joerg.ehrichs@gmx.de>
  */
class WriteToFile : public WriteToStorage
{
    Q_OBJECT
public:
    explicit WriteToFile(QObject *parent = 0);
    virtual ~WriteToFile();

public slots:
    void pushItems(File items, const QString &collection = QString());
    void pushNewItems(File items, const QString &collection = QString());
    void updateItem(Entry *item);
    void addItemsToCollection(QList<QString> ids, const QString &collection );
    void removeItemsFromCollection(QList<QString> ids, const QString &collection );
    void deleteItems(File items);

    void createCollection(const CollectionInfo &ci);
    void editCollection(const CollectionInfo &ci);
    void deleteCollection(const CollectionInfo &ci);

protected slots:
    void requestFinished();

private:
    /**
      * @bug use Kio to export file to network space
      */
    void exportFile(File items);
};

#endif // WRITETOFILE_H
