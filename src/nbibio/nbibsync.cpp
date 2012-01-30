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

#include "nbibsync.h"

#include <KDE/KDebug>

NBibSync::NBibSync(QObject *parent)
    : QObject(parent)
    , m_systemLibrary(0)
    , m_libraryToSyncWith(0)
    , m_syncSteps(0)
    , m_curStep(0)
{
    qRegisterMetaType<SyncDetails>("SyncDetails");
    qRegisterMetaType<QList<SyncDetails> >("QList<SyncDetails>");
    qRegisterMetaType<SyncDetails>("File");
}

void NBibSync::setUniqueProviderID(const QString &id)
{
    m_uniqueId = id;
}

QString NBibSync::uniqueProviderID() const
{
    return m_uniqueId;
}

void NBibSync::setProviderDetails(ProviderSyncDetails psd)
{
    m_psd = psd;
}

ProviderSyncDetails NBibSync::providerSyncDetails() const
{
    return m_psd;
}

void NBibSync::setSystemLibrary(Library *sl)
{
    m_systemLibrary = sl;
}

void NBibSync::setLibraryToSyncWith(Library *l)
{
    m_libraryToSyncWith = l;
}

void NBibSync::calculateProgress(int value)
{
    qreal curProgress = ((qreal)value * 1.0/m_syncSteps);

    curProgress += (qreal)(100.0/m_syncSteps) * m_curStep;

//    kDebug() << curProgress << m_syncSteps << m_curStep;

    emit progress(curProgress);
}
