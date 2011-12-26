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

#ifndef BACKGROUNDSYNC_H
#define BACKGROUNDSYNC_H

#include <QObject>

#include "nbibio/nbibsync.h"

class BackgroundSync : public QObject
{
    Q_OBJECT
public:
    enum SyncAttentionDetails {
        SYNC_DELETION_QUESTION,
        SYNC_MERGE_QUESTION
    };

    struct SyncAttention {
        SyncAttentionDetails sad;
        NBibSync* provider;
        QList<SyncDetails> syncDetails;
    };

    explicit BackgroundSync(QObject *parent = 0);
    ~BackgroundSync();

    void addSyncProvider(NBibSync* provider);
    void removeSyncProvider(NBibSync* provider);
    QList<NBibSync*> syncProvider() const;

    void startSync();

    QList<BackgroundSync::SyncAttention> getAttentionStack() const;
    void removeAttentionEntry(BackgroundSync::SyncAttention entry);

signals:
    void inProgress(bool syncActive);
    void needsAttention();

private slots:
    //void syncFinished();
    void askForDeletion(QList<SyncDetails> details);
    void userMerge(QList<SyncDetails> details);

private:
    QList<NBibSync*> m_provider;
    QList<SyncAttention> m_attentionStack;

};

#endif // BACKGROUNDSYNC_H
