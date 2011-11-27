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

#include "writetostorage.h"

#include <QtNetwork/QNetworkReply>

WriteToStorage::WriteToStorage(QObject *parent)
    : QObject(parent)
    , m_requestType(Items)
    , m_reply(0)
{
    qRegisterMetaType<CollectionInfo>("CollectionInfo");
    qRegisterMetaType<QList<CollectionInfo> >("QList<CollectionInfo>");
}

WriteToStorage::~WriteToStorage()
{
    delete m_reply;
}

void WriteToStorage::setUserName(const QString & name)
{
    m_name = name;
}

QString WriteToStorage::userName() const
{
    return m_name;
}

void WriteToStorage::setPassword(const QString & pwd)
{
    m_password = pwd;
}

QString WriteToStorage::pasword() const
{
    return m_password;
}

void WriteToStorage::setRequestType(RequestType type)
{
    m_requestType = type;
}

RequestType WriteToStorage::requestType() const
{
    return m_requestType;
}

QNetworkReply *WriteToStorage::reply() const
{
    return m_reply;
}

void WriteToStorage::startRequest(const QNetworkRequest &request, const QByteArray & payload)
{
    m_reply = m_qnam.post(request, payload);
    connect(m_reply, SIGNAL(finished()),this, SLOT(requestFinished()));
    //connect(reply, SIGNAL(downloadProgress(qint64,qint64)),this, SLOT(updateDataReadProgress(qint64,qint64)));
}

void WriteToStorage::cancelUpload()
{
    m_reply->abort();
}
