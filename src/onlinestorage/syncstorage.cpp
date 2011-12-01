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
    , m_askBeforeDeletion(false)
    , m_downloadOnly(false)
    , m_uploadOnly(false)
{
}

SyncStorage::~SyncStorage()
{

}

void SyncStorage::setUserName(const QString & name)
{
    m_name = name;
}

QString SyncStorage::userName() const
{
    return m_name;
}

void SyncStorage::setPassword(const QString & pwd)
{
    m_password = pwd;
}

QString SyncStorage::pasword() const
{
    return m_password;
}

void SyncStorage::setUrl(const QString & url)
{
        m_url = url;
}

QString SyncStorage::url() const
{
    return m_url;
}

void SyncStorage::setAdoptBibtexTypes(bool adopt)
{
    m_adoptBibtexTypes = adopt;
}

bool SyncStorage::adoptBibtexTypes() const
{
    return m_adoptBibtexTypes;
}

void SyncStorage::setAskBeforeDeletion(bool ask)
{
    m_askBeforeDeletion = ask;
}

bool SyncStorage::askBeforeDeletion() const
{
    return m_askBeforeDeletion;
}

void SyncStorage::setDownloadOnly(bool downloadOnly)
{
    m_downloadOnly = downloadOnly;
}

bool SyncStorage::downloadOnly() const
{
    return m_downloadOnly;
}

void SyncStorage::setUploadOnly(bool uploadOnly)
{
    m_uploadOnly = uploadOnly;
}

bool SyncStorage::uploadOnly() const
{
    return m_uploadOnly;
}
