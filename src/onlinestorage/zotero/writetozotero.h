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

#ifndef WRITETOZOTERO_H
#define WRITETOZOTERO_H

#include "../writetostorage.h"

class WriteToZotero : public WriteToStorage
{
    Q_OBJECT
public:
    explicit WriteToZotero(QObject *parent = 0);
    virtual ~WriteToZotero();

public slots:
    void pushNewItems(File items);
    void updateItem(Entry *item);
    void addItemsToCollection(QList<QString> ids, const QString &collection );
    void removeItemsFromCollection(QList<QString> ids, const QString &collection );
    void deleteItems(QList<QString> ids);

    void createCollection(CollectionInfo ci, const QString &parent = QString());
    void editCollection(CollectionInfo ci);
    void deleteCollection(const QString &id);

protected slots:
    void requestFinished();

private:
    QByteArray writeJsonContent(File items);

    QVariantList createCreatorsJson(Entry *e);
    QVariantList createTagsJson(Entry *e);
    QVariantMap createBookJson(Entry *e);
};

#endif // WRITETOZOTERO_H
