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

#ifndef ZOTEROUPLOAD_H
#define ZOTEROUPLOAD_H

#include <QtCore/QObject>

#include "onlinestorage/storageglobals.h"
#include "../nbibsync.h"

#include <QtCore/QSharedPointer>
#include <kbibtex/element.h>
#include <kbibtex/file.h>

class Library;
class WriteToZotero;

class ZoteroUpload : public QObject
{
    Q_OBJECT
public:
    explicit ZoteroUpload(QObject *parent = 0);
    ~ZoteroUpload();

    void setProviderDetails(ProviderSyncDetails psd);
    void setSystemLibrary(Library *sl);
    void setLibraryToSyncWith(Library *l);

signals:
    void progress(int value);
    void progressStatus(const QString &status);
    void askForServerDeletion(QList<SyncDetails>);
    void askForGroupRemoval(QList<SyncDetails>);
    void finished();

public slots:
    void startUpload();
    void removeFilesFromGroup(bool removeThem);
    void removeFilesFromZotero(bool removeThem);

    void startAttachmentUpload();

    void cancel();

private slots:
    /**
      * process syncdata retrived from zotero server when new items are send to the server
      */
    void readUploadSync();

    // after we are finished uploading all new and changed items, we start to remove all entries
    // that are removed on the local side and must be removed also from the serverside
    void removeFilesFromGroup();
    void removeFilesFromZotero();
    void cleanupAfterUpload();

    /**
      * Attachment uploads are a tiny bit different as we have to take care about
      * its parent. Also as this contains large binary blogs (pdf etc) we should do this
      * one by one.
      *
      * this slot calls itself recursive and always takes the next attachment from m_attachmentsToUpload
      * until it is finished.
      */
    void uploadNextAttachment();

    /**
      * When an item was changed on the server side a new etag value is generated
      *
      * The etag valued determines if the item changed on the server and is necessary when we download or upload the file again
      * Here we search for the right @c sync:ServerSyncData for the item and update the etag and time value
      *
      * This function is called after we send the local changes to the server and when we retrieve the new etag values for these items
      */
    void updateSyncDetailsToNepomuk(const QString &id, const QString &etag, const QString &updated);

    void calculateProgress(int value);

private:
    /**
      * like updateSyncDetailsToNepomuk() but creates new sync data from new downloaded files
      *
      * @return the newly created sync:ServerSyncData object
      */
    Nepomuk::Resource writeNewSyncDetailsToNepomuk(Entry *localData, const QString &id, const QString &etag, const QString &updated);

//    QSharedPointer<Element> transformAttachmentToBibTeX(Nepomuk::Resource resource);

private:
    bool m_cancel;
    int m_syncSteps;
    int m_currentStep;
    bool m_attachmentMode;
    ProviderSyncDetails m_psd;
    Library *m_systemLibrary;
    Library *m_libraryToSyncWith;

    WriteToZotero *m_wtz;
    File *m_bibCache;
    File *m_corruptedUploads;

    QList<Nepomuk::Resource> m_syncDataToBeRemoved;
    QList<SyncDetails> m_tmpUserDeleteRequest;

};

#endif // ZOTEROUPLOAD_H
