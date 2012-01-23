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

class BackgroundSync : public QObject
{
    Q_OBJECT
public:
    explicit BackgroundSync(QObject *parent = 0);

    void setLibraryManager(LibraryManager *lm);

    void setLibraryToSyncWith(Library *l);

signals:
    void progress(int);
    void progressStatus(QString);

    void deleteLocalFiles(bool deleteThem);
    void mergedResults(QList<SyncDetails> items);
    void mergeFinished();

    void allSyncTargetsFinished();

public slots:
    void startSync();

private slots:
    void startSync(const ProviderSyncDetails &psd);
    void currentSyncFinished();

    void popDeletionQuestion(QList<SyncDetails> items);
    void popMergeDialog(QList<SyncDetails> items);

private:
    bool findPasswordInKWallet(ProviderSyncDetails &psd);

    LibraryManager *m_libraryManager;
    Library *m_libraryToSync;

    QList<ProviderSyncDetails> m_syncList;
};

#endif // BACKGROUNDSYNC_H
