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

#include "backgroundsync.h"

#include "nbibio/nbibsync.h"
BackgroundSync::BackgroundSync(QObject *parent)
    : QObject(parent)
{
}

BackgroundSync::~BackgroundSync()
{
    qDeleteAll(m_provider);
}

void BackgroundSync::addSyncProvider(NBibSync* provider)
{
    m_provider.append(provider);

    connect(provider, SIGNAL(askForDeletion(QList<SyncDetails>)), this, SLOT(askForDeletion(QList<SyncDetails>)));
    connect(provider, SIGNAL(userMerge(QList<SyncDetails>)), this, SLOT(userMerge(QList<SyncDetails>)));
}

void BackgroundSync::removeSyncProvider(NBibSync* provider)
{
    disconnect(provider, SIGNAL(askForDeletion(QList<SyncDetails>)), this, SLOT(askForDeletion(QList<SyncDetails>)));
    disconnect(provider, SIGNAL(userMerge(QList<SyncDetails>)), this, SLOT(userMerge(QList<SyncDetails>)));

    m_provider.removeAll(provider);
    delete provider;
    provider = 0;
}

QList<NBibSync*> BackgroundSync::syncProvider() const
{
    return m_provider;
}

void BackgroundSync::startSync()
{
    emit inProgress(true);
    foreach(NBibSync* provider, m_provider) {
        provider->startSync();
    }
}

QList<BackgroundSync::SyncAttention> BackgroundSync::getAttentionStack() const
{
    return m_attentionStack;
}

void BackgroundSync::removeAttentionEntry(BackgroundSync::SyncAttention entry)
{
    int i=0;
    foreach(const SyncAttention &sa, m_attentionStack) {
        if(entry.provider == sa.provider && sa.sad == entry.sad) {
            break;
        }
        i++;
    }

    m_attentionStack.removeAt(i);
}

void syncFinished()
{

}

void BackgroundSync::askForDeletion(QList<SyncDetails> details)
{
    qDebug() << "BackgroundSync::askForDeletion";
    NBibSync* nbs = qobject_cast<NBibSync*>(sender());
    SyncAttention newEntry;
    newEntry.sad = SYNC_DELETION_QUESTION;
    newEntry.provider = nbs;
    newEntry.syncDetails = details;

    m_attentionStack.append(newEntry);

    emit needsAttention();
}

void BackgroundSync::userMerge(QList<SyncDetails> details)
{
    qDebug() << "BackgroundSync::userMerge";

    NBibSync* nbs = qobject_cast<NBibSync*>(sender());
    SyncAttention newEntry;
    newEntry.sad = SYNC_MERGE_QUESTION;
    newEntry.provider = nbs;
    newEntry.syncDetails = details;

    m_attentionStack.append(newEntry);

    emit needsAttention();
}
