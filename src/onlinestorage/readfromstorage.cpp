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

#include "readfromstorage.h"

#include <QtNetwork/QNetworkReply>

#include <QDebug>

ReadFromStorage::ReadFromStorage(QObject *parent)
    : QObject(parent)
    , m_requestType(Items)
    , m_reply(0)
{
    qRegisterMetaType<CollectionInfo>("CollectionInfo");
    qRegisterMetaType<QList<CollectionInfo> >("QList<CollectionInfo>");
}

ReadFromStorage::~ReadFromStorage()
{
    //m_reply->close();
    //delete m_reply;
}

void ReadFromStorage::setProviderSettings(const ProviderSyncDetails &psd)
{
    m_psd = psd;
}

void ReadFromStorage::setAdoptBibtexTypes(bool adopt)
{
    m_adoptBibtexTypes = adopt;
}

bool ReadFromStorage::adoptBibtexTypes() const
{
    return m_adoptBibtexTypes;
}

void ReadFromStorage::setRequestType(RequestType type)
{
    m_requestType = type;
}

RequestType ReadFromStorage::requestType() const
{
    return m_requestType;
}

QNetworkReply *ReadFromStorage::reply() const
{
    return m_reply;
}

void ReadFromStorage::startRequest(QUrl url)
{
    m_reply = m_qnam.get(QNetworkRequest(url));
    connect(m_reply, SIGNAL(finished()),this, SLOT(requestFinished()));
    //connect(reply, SIGNAL(downloadProgress(qint64,qint64)),this, SLOT(updateDataReadProgress(qint64,qint64)));
}

void ReadFromStorage::cancelDownload()
{
    m_reply->abort();
}
