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

#ifndef NEPOMUKSYNCCLIENT_H
#define NEPOMUKSYNCCLIENT_H

#include <QtCore/QObject>

#include "storageglobals.h"

#include <Nepomuk2/Resource>

class OnlineStorage;

struct SyncMergeDetails {
    Nepomuk2::Resource syncResource;
    QVariantMap        externalResource;
};

class NepomukSyncClient : public QObject
{
    Q_OBJECT
public:
    explicit NepomukSyncClient(QObject *parent = 0);

    void setProviderSettings(const ProviderSyncDetails &psd);
    ProviderSyncDetails providerSettings() const;

    void setProject(const Nepomuk2::Resource &project);

signals:
    void status(const QString &message);
    void progress(int currentProgress);
    void error(const QString &message);

    /**
      * emitted if items from the server are deleted
      * emits a signal so we can show a dialog box not possible here as we run this class in a different thread than the gui thread
      *
      * call deleteLocalFiles() when the user made his choice to proceed sync
      */
    void askForLocalDeletion(QList<Nepomuk2::Resource> &listOfRemovedResources);

    /**
      * emitted when the user needs to decide how to merge certain items
      *
      * call mergeFinished() when the user finished merging (merging should be done directly in nepomuk)
      */
    void userMerge(QList<SyncMergeDetails> &itemsThatNeedMerge);

    void finished();

    void pushNextItem();
    void deleteNextItem();

public slots:
    void cancel();

    /**
     * @brief importData from onlinestorage to nepomuk
     */
    void importData();

    /**
     * @brief exportData from nepomuk to onlinestorage
     */
    void exportData();
    void syncData();

    void deleteLocalFiles(bool deleteThem);
    void mergeFinished();

private slots:
    void dataDownloadFinished();

    void pushNewItemCache();
    void newItemUploadFinished();
    void errorDuringUpload(const QString &msg);
    void itemNeedMerge(const QVariantMap & item);

    void deleteItemsCache();
    void deleteItemFinished();
    void errorDuringDelete(const QString &msg);

    void calculateProgress(int value);

private:
    void findRemovedEntries();
    void readDownloadSyncAfterDelete();
    void findDuplicates(QList<Nepomuk2::Resource> &existingItems);
    void fixMergingItems();
    void importNewResources();

    void endSyncStep();
    void clearInternalData();

private:
    ProviderSyncDetails m_psd;
    Nepomuk2::Resource m_project;
    OnlineStorage *m_storage;

    QVariantList m_cacheDownloaded;
    QVariantList m_newEntries;
    QList<Nepomuk2::Resource> m_tmpUserDeleteRequest;
    QList<SyncMergeDetails> m_tmpUserMergeRequest;

    QVariantList m_tmpPushItemList;
    QVariantMap  m_tmpCurPushedItem;

    QVariantList m_tmpPushNotesItemList;
    QVariantList m_tmpPushFilesItemList;
    QVariantMap m_tmpCurPushedAttachmentItem;

    QStringList m_pushRemoveFromCollection;
    QVariantList m_pushDeleteItems;

    int m_syncSteps;
    int m_currentStep;
    bool m_cancel;

    bool m_mergeFinished;
    bool m_downloadFinished;
    bool m_uploadFinished;

    qreal m_curProgressPerItem;
    qreal m_curProgress;
};

#endif // NEPOMUKSYNCCLIENT_H
