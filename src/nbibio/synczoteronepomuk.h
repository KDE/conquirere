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

#ifndef SYNCZOTERONEPOMUK_H
#define SYNCZOTERONEPOMUK_H

#include "nbibsync.h"

#include <kbibtex/file.h>

class ReadFromZotero;
class WriteToZotero;
class BibTexToNepomukPipe;
class NepomukToBibTexPipe;
class Element;
class Entry;
class EntryClique;

/**
  * @brief Base class to sync nepomuk data with the Zotero online service
  *
  * This class combines the ReadFromZotero and WriteToZotero and organize the merging between these steps.
  * When some data on the server side changes and the mergemode is set to Manual the signal userMerge() is emitted
  * Any gui that calls this class need to show a gui and resolve the issues before this class can continue
  * When the merge is solved the slot mergeFinished() must be called to continue the process
  *
  * Same applies for deleting items, when the user interaction is requested the signal askForDeletion() is emitted
  * and the slot deleteLocalFiles() must be called when the selection was made.
  *
  * The current progress can be shown via progress() and progressStatus()
  *
  * Two different sync cases must be handeled in here
  * @li syncing with the base zotero collection (all items regardless of their groups)
  * @li syncing with a specific group
  *
  * The 2nd case together with the snc of a project Library instead the full system Library changes the way
  * the sync merging/removing/addition is done.
  *
  * Here we add the pimo:isRelated to any new item retrieved from the server and
  * remove this relation when an item should be deleted (while the item will still exist in the system library)
  *
  * This means in return, that a local project represents 1 group on the Zotero server.
  *
  * Another special case:
  * We removed the publication/reference from a library project and need to remove them on the server side too.
  *
  * They way to find such a case follows the following concept:
  * Any reference/publication that is synced with a storage has it @c sync:ServerSyncData when the reference/publication is deleted
  * by conquirere or any other program the @c sync:ServerSyncData is still there but without a valid reference/publication
  *
  * Now when we download data from te hserver and check for teh duplicates, we find the @c sync:ServerSyncData and see that the ref/pub was deleted
  * We know now teh user wanted to delete it (remove it from teh group) and we do not import this entyr again. Instead we
  * will delete it from the server side as soon as the next upload happens.
  *
  */
class SyncZoteroNepomuk : public NBibSync
{
    Q_OBJECT
public:
    explicit SyncZoteroNepomuk(QObject *parent = 0);
    virtual ~SyncZoteroNepomuk();

public slots:
    void startUpload();
    void startDownload();
    void startSync();
    void cancel();

    void deleteLocalFiles(bool deleteThem);
    void mergeFinished();

private slots:
    /**
      * process all data retrieved from the zotero server
      */
    void readDownloadSync(const File &zoteroData);
    void readDownloadSyncAfterDelete(const File &zoteroData);

    /**
      * process syncdata retrived from zotero server when new items are send to the server
      */
    void readUploadSync(const File &zoteroData);

    // after we are finsihed uploading all new and changed items, we start to remove all entries
    // that are removed on the local side and mus tbe removed also from the serverside
    void removeFilesFromGroup();
    void removeFilesFromZotero();
    void cleanupAfterUpload();

    /**
      * When an item was changed on the server side a new etag value is generated
      *
      * The etag valued determines if the item changed on the server and is necessary when we download or upload the file again
      * Here we search for the right @c sync:ServerSyncData for the item and update the etag and time value
      *
      * This function is called after we send the local changes to the server and when we retrieve the new etag values for these items
      */
    void updateSyncDetailsToNepomuk(const QString &id, const QString &etag, const QString &updated);

private:
    /**
      * Does what is say. It find the entry in the Nepomuk storage that is used to sync data with the zotero storage
      *
      * As each synced item has a @c sync::ServerSyncData attached to it, we can very easily search nepomuk for these resources
      * that match the current provider(zotero) the given userName, url and the unique ID from zotero for this zotero storage.
      *
      * @li When no @c sync::ServerSyncData the @p newEntries list will be filled with the new item to process later on
      * @li If we found the duplicate we check if the item was changed on the serverside since the last sync. This will be done by comparing the
      *     etag value for this item wit hthe one stored in the @c sync::ServerSyncData object.
      *     If the etag values do not match we need to merge the online and local item and store them for this purpose in @p mergeRequest
      *
      * @li Another feature is the detection of all resources in the storage that are in use in the @p existingItems list.
      *     This helps to add the pimo:isRelated property to items already existing in the storage but not already related to
      *     a project.
      */
    void findDuplicates(const File &zoteroData, File &newEntries, QList<SyncDetails> &mergeRequest, QList<Nepomuk::Resource> &existingItems);
    void findRemovedEntries(const File &zoteroData, QList<SyncDetails> &userDeleteRequest);

    /**
      * like updateSyncDetailsToNepomuk() but creates new sync data from new downloaded files
      */
    void writeNewSyncDetailsToNepomuk(Entry *localData, const QString &id, const QString &etag, const QString &updated);

private:
    ReadFromZotero *m_rfz;
    WriteToZotero *m_wtz;
    BibTexToNepomukPipe *m_btnp;
    NepomukToBibTexPipe *m_ntbp;
    File m_bibCache;
    bool m_syncMode;
    bool m_cancel;
    QList<SyncDetails> m_tmpUserDeleteRequest;
    QList<SyncDetails> m_tmpUserMergeRequest;
};

#endif // SYNCZOTERONEPOMUK_H
