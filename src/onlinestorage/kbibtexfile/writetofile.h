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
  * Only usefull as part of the sync process.
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
    void pushItems(const File &items, const QString &collection = QString());
    void pushNewItems(const File &items, const QString &collection = QString());

    /**
      * Not used currently, use @c pushItems() instead to update everything
      */
    void updateItem(QSharedPointer<Element> item);

    /**
      * collections in files are not supported
      */
    void addItemsToCollection(const QList<QString> &ids, const QString &collection );

    /**
      * collections in files are not supported
      */
    void removeItemsFromCollection(const QList<QString> &ids, const QString &collection );
    void deleteItems(const File &items);

    /**
      * collections in files are not supported
      */
    void createCollection(const CollectionInfo &ci);

    /**
      * collections in files are not supported
      */
    void editCollection(const CollectionInfo &ci);

    /**
      * collections in files are not supported
      */
    void deleteCollection(const CollectionInfo &ci);

protected slots:
    void requestFinished();

private:
    /**
      * @bug use Kio to export file to network space
      */
    void exportFile(const File &items);

    File emptyFile;
};

#endif // WRITETOFILE_H
