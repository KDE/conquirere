/*
 * Copyright 2012 Jörg Ehrichs <joerg.ehrichs@gmx.de>
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

#ifndef VARIANTTONEPOMUKPIPE_H
#define VARIANTTONEPOMUKPIPE_H

#include <QtCore/QObject>
#include <QtCore/QVariantList>
#include <QtCore/QVariantMap>

#include <Nepomuk2/Resource>
#include <Nepomuk2/SimpleResource>
#include <Nepomuk2/SimpleResourceGraph>

#include "../storageglobals.h"

class OnlineStorage;

class VariantToNepomukPipe : public QObject
{
    Q_OBJECT
public:
    explicit VariantToNepomukPipe(QObject *parent = 0);

    /**
      * Does the piping action of new tems
      *
      * @p resources list of publications
      */
    void pipeExport(QVariantList &publicationList);

    void setSyncStorageProvider(OnlineStorage *storage);

    /**
      * if the @p projectThing is valid all imported data will be related via @c pimo:isRelated to the project
      */
    void setProjectPimoThing(Nepomuk2::Resource projectThing);

signals:
    void progress(int value);

private:
    void importNote(const QVariantMap &noteEntry);
    /**
     * @brief import a new file attachment and downloads its content
     * @param attachmentEntry
     */
    void importAttachment(const QVariantMap &attachmentEntry);

    /**
     * @brief Adds Zotero Sync details to the newly created nbib:Reference and nbib:Publication
     * @param publicationResource the created nbib:Publication Resources
     * @param referenceResource the created nbib:Reference Resources
     * @param item the original QVariantMap item that was used to create the publication/reference
     *
     * @todo check how this step can be simplified. Currently a blocking save is added to first create the ServerSyncData object and than connect it.
     *       there seems to be a bug that prevents doing this in one step via SimpleResourceGraph
     */
    void addStorageSyncDetails(Nepomuk2::Resource &publicationResource, Nepomuk2::Resource &referenceResource, const QVariantMap &item);
    void addStorageSyncDetails(Nepomuk2::Resource attachment, const QVariantMap &item);

private:
    OnlineStorage *m_storage;
    Nepomuk2::Resource m_projectThing;

};

#endif // VARIANTTONEPOMUKPIPE_H
