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

#ifndef READFROMZOTERO_H
#define READFROMZOTERO_H

#include "../readfromstorage.h"

#include <QtCore/QObject>
#include <QtCore/QXmlStreamReader>

class Element;
class Entry;

/**
  * @brief Fetch all items/collections from an Zotero storage for a specific user
  *
  * The items that will be fetched will be emitted via the itemsInfo() signal.
  * The collections are emitted via collectionsInfo().
  *
  * Some entry types and key names are changed to respect the BibTeX format.
  * To help synchronize the local items with the fetched items from the zotero storage some additinal
  * keys are added to the BibTeX format.
  *
  * @li @c zoteroKey The unique id to identify an item in the online storage
  * @li @c zoteroChildren The number of child items (notes/attachments) in the zotero storage
  * @li @c zoteroUpdated The time this item was updated the last time in the zotero storage
  * @li @c zoteroEtag An additinal key to help identify if the item has changed on the server
  *                   since the last pull request, necessary for the writeback to the server
  *
  * @see https://www.zotero.org/support/dev/server_api/read_api
  *
  * @author Jörg Ehrichs <joerg.ehrichs@gmx.de>
  */
class ReadFromZotero : public ReadFromStorage
{
    Q_OBJECT
public:
    explicit ReadFromZotero(QObject *parent = 0);

    CollectionInfo readCollectionEntry(QXmlStreamReader &xmlReader);
    Entry * readItemEntry(QXmlStreamReader &xmlReader);
    void readJsonContent(Entry *e, const QString &content);
    File *getFile();
    QList<CollectionInfo> getCollectionInfo();

    /**
      * Set search filter as specified in the read api
      *
      * For exempel to get all Note attachments: &itemType=note
      * Or for all file attachments : &itemType=attachment
      *
      * After we did the usual fetchItems() th egetFile returns a bibfile with
      * the necessary information for the notes/files and most importantly the parent it belongs to.
      */
    void setSearchFilter(const QString &filter);

public slots:
    void fetchItems(const QString &collection = QString());
    void fetchItems(int limit, int start, const QString &collection = QString());
    void fetchItem(const QString &id, const QString &collection = QString() );
    void fetchCollections(const QString &parent = QString() );
    void fetchCollections(int limit, int start, const QString &parent = QString());
    void fetchCollection(const QString &collection );

protected slots:
    void requestFinished();

private:
    /**
      * Modifies the Entry @p e to hold all zotero keys/values as returned form zotero
      */
    void readJsonContentOriginal(Entry *e, const QString &content);

    /**
      * Modifies the Entry @p e to hold all zotero keys/values adopted to the bibtex standard
      *
      * @see m_zoteroToBibTeX;
      */
    void readJsonContentBibTeX(Entry *e, const QString &content);

    QList<CollectionInfo> m_cachedCollectionResult;
    File *m_bibFile;
    QString m_collectionToFetchFrom;
    bool m_fetchIncomplete;
    QMap<QString, QString> m_zoteroToBibTeX;

    QString m_searchFilter;
};

#endif // READFROMZOTERO_H
