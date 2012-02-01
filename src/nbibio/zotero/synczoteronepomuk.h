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

#include "../nbibsync.h"

class ZoteroDownload;
class ZoteroUpload;

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
    void startDownload();
    void startAttachmentDownload();
    void startUpload();
    void startAttachmentUpload();
    void startSync();
    void cancel();

    void deleteLocalFiles(bool deleteThem);
    void mergeFinished();

private slots:
    void downloadFinished();
    void uploadFinished();

    /**
      * Attachment uploads are a tiny bit different as we have to take care about
      * its parent. Also as this contains large binary blogs (pdf etc) we should do this
      * one by one.
      *
      * this slot calls itself recursive and always takes the next attachment from m_attachmentsToUpload
      * until it is finished.
      */
    void uploadNextAttachment();

private:
//    QSharedPointer<Element> transformAttachmentToBibTeX(Nepomuk::Resource resource);

private:
    bool m_syncMode;
    bool m_cancel;
    ZoteroDownload *m_zoteroDownload;
    ZoteroUpload *m_zoteroUpload;

//    QList<Nepomuk::Resource> m_syncDataToBeRemoved; // sync data that tells us to delete items on the server side
                                                    // will be removed when we succeed on the server side
};

#endif // SYNCZOTERONEPOMUK_H
