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

#include "nepomuksyncclient.h"

NepomukSyncClient::NepomukSyncClient(QObject *parent) :
    QObject(parent)
{
}

void NepomukSyncClient::setProviderSettings(const ProviderSyncDetails &psd)
{
    m_psd = psd;
}

ProviderSyncDetails NepomukSyncClient::providerSettings() const
{
    return m_psd;
}

void NepomukSyncClient::setProject(const Nepomuk2::Resource &project)
{
    m_project = project;
}

void NepomukSyncClient::importData()
{

}

void NepomukSyncClient::exportData()
{

}

void NepomukSyncClient::syncData()
{

}

void NepomukSyncClient::status(const QString &message)
{

}

void NepomukSyncClient::progress(int currentProgress)
{

}

void NepomukSyncClient::error(const QString &message)
{

}

void NepomukSyncClient::finished()
{

}

void NepomukSyncClient::cancel()
{

}
