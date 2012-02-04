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

#ifndef ZOTERODOWNLOAD_H
#define ZOTERODOWNLOAD_H

#include <QtCore/QObject>

#include "onlinestorage/storageglobals.h"
#include "../nbibsync.h"

#include <QNetworkAccessManager>

class Library;
class ReadFromZotero;
class File;
class Entry;
class QDBusInterface;
class QNetworkReply;
class QFile;

/**
  * @brief takes care of the zotero Download, resource merging/deletion/creation
  *
  */
class ZoteroDownload : public QObject
{
    Q_OBJECT
public:
    explicit ZoteroDownload(QObject *parent = 0);
    ~ZoteroDownload();

    void setProviderDetails(ProviderSyncDetails psd);
    void setSystemLibrary(Library *sl);
    void setLibraryToSyncWith(Library *l);

    void setCorruptedUploadEntries(File *corruptedOnUpload);

signals:
    void progress(int value);
    void progressStatus(const QString &status);
    /**
      * emitted if items from the server are deleted
      * emits a signal so we can show a dialog box not possible here as we run this class in a different thread than the gui thread
      *
      * call deleteLocalFiles() when the user made his choice to proceed sync
      */
    void askForLocalDeletion(QList<SyncDetails>);

    /**
      * emitted when the user needs to decide how to merge certain items
      *
      * call mergeFinished() when the user finished merging (merging should be done directly in nepomuk)
      */
    void userMerge(QList<SyncDetails>);

    void finished();

public slots:
    void startDownload();
    void startAttachmentDownload();

    void deleteLocalFiles(bool deleteThem);
    void mergeFinished();

    void cancel();

private slots:
    /**
      * process all data retrieved from the zotero server
      */
    void readDownloadSync();
    void readDownloadSyncAfterDelete();

    void downloadNextAttachment();
    void attachmentDownloadFinished();
    void attachmentReadyRead();
    void updateDataReadProgress(qint64 bytesReceived,qint64 bytesTotal);

    void calculateProgress(int value);

private:
    void finishAndCleanUp();
    /**
      * like updateSyncDetailsToNepomuk() but creates new sync data from new downloaded files
      *
      * @return the newly created sync:ServerSyncData object
      */
    Nepomuk::Resource writeNewSyncDetailsToNepomuk(Entry *localData, const QString &id, const QString &etag, const QString &updated);

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
    void findDuplicates(QList<Nepomuk::Resource> &existingItems);

    /**
      * takes the entries from the @c m_bibCache and checks which zoterokeys tha tare available locally are missing in it
      *
      * All those keys that are available locally but not online, are deleted and have to be deleted locally too
      *
      * fills m_tmpUserDeleteRequest if the list is not empty the signal askForDeletion() is called if teh user whants that
      */
    void findRemovedEntries();

    /**
      * This function will go through all data retrieved from the server and check it again the data from @c m_corruptedUploads
      *
      * Will only solve the error when:
      * New items are uploaded to the server and successfully created
      * but the server returned the "internal server error" message instead the item info with the zoterokey + etag value
      *
      * Will remove any entry from @p zoteroData that was found so we don't have to check them again next time
      */
    void fixCorrputedUpload();

    /**
      * Goes through the list of m_tmpUserMergeRequest and fixes the problems on its own
      *
      * Means the user specified either to use always the server or always the local version
      * To solve the problem
      */
    void fixMergingAutomatically();

    /**
      * Imports all entries in @c m_newEntries and push them int othe nepomuk storage
      */
    void importNewResources();
    void importNewAttachments();

private:
    bool m_cancel;
    int m_syncSteps;
    int m_currentStep;
    bool m_attachmentMode;
    ProviderSyncDetails m_psd;
    Library *m_systemLibrary;
    Library *m_libraryToSyncWith;

    ReadFromZotero *m_rfz;

    QList<SyncDetails> m_tmpUserDeleteRequest;
    QList<SyncDetails> m_tmpUserMergeRequest;

    File *m_bibCache; // retrieved entries from zotero
    File *m_newEntries;
    File* m_corruptedUploads;

    //for the attachment download
    File *m_newEntriesToDownload;
    QDBusInterface *m_nepomukDBus;
    QNetworkAccessManager qnam;
    QNetworkReply *m_downloadReply;
    QFile *m_attachmentFile;
};

#endif // ZOTERODOWNLOAD_H
