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

#ifndef BACKGROUNDSYNC_H
#define BACKGROUNDSYNC_H

#include <QObject>

#include "core/projectsettings.h"
#include "nbibio/nbibsync.h"

class LibraryManager;
class Library;
class NBibSync;
class QThread;

/**
  * @brief Coordinates the syncronization with all availabe provider specified in the .ini file or on localrequest via a direct call to startSync()
  *
  * The sync is done in an additional QThread in the background so while all data is imported to Nepomuk or
  * exported and uploaded to the provider the gui thread will not freeze.
  *
  * @see SyncZoteroNepomuk
  * @see SyncKBibTeXFile
  */
class BackgroundSync : public QObject
{
    Q_OBJECT
public:
    explicit BackgroundSync(QObject *parent = 0);
    ~BackgroundSync();

    void setLibraryManager(LibraryManager *lm);

    void setLibraryToSyncWith(Library *l);

signals:
    /**
      * show current sync progress as 0-100% for each provider
      *
      * starts again by 0 when a new provider sync is started
      */
    void progress(int);

    /**
      * show more details about what is happening in the background
      */
    void progressStatus(QString);

    /**
      * forwards the signal to the NBibSync class
      */
    void deleteLocalFiles(bool deleteThem);
    /**
      * forwards the signal to the NBibSync class
      */
    void mergeFinished();

    /**
      * Indicates the sync is finished completely for all providers
      */
    void allSyncTargetsFinished();

public slots:
    void startSync();

private slots:
    void startSync(const ProviderSyncDetails &psd);
    void currentSyncFinished();

    void popDeletionQuestion(QList<SyncDetails> items);
    void popMergeDialog(QList<SyncDetails> items);
    void cancelSync();

    /**
      * press the 0-100 from several syncs into an overal 0-100
      */
    void calculateProgress(int value);

private:
    bool findPasswordInKWallet(ProviderSyncDetails &psd);

    LibraryManager *m_libraryManager;
    Library *m_libraryToSync;
    QThread *m_syncThread;
    NBibSync *m_syncNepomuk;

    int m_syncSteps;
    int m_curStep;

    QList<ProviderSyncDetails> m_syncList;
};

#endif // BACKGROUNDSYNC_H
