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

#include "syncstorage.h"

SyncStorage::SyncStorage(QObject *parent)
    : QObject(parent)
    , m_adoptBibtexTypes(true)
{
    ProviderSyncDetails psd;
    psd.providerInfo = 0;
    m_psd = psd;
}

SyncStorage::~SyncStorage()
{

}

void SyncStorage::setProviderSettings(const ProviderSyncDetails &psd)
{
    m_psd = psd;
}

void SyncStorage::setAdoptBibtexTypes(bool adopt)
{
    m_adoptBibtexTypes = adopt;
}

bool SyncStorage::adoptBibtexTypes() const
{
    return m_adoptBibtexTypes;
}
