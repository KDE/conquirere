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
#include <QDebug>

WriteToStorage::WriteToStorage(QObject *parent)
    : QObject(parent)
    , m_adoptBibtexTypes(true)
    , m_requestType(Items)
{
    qRegisterMetaType<CollectionInfo>("CollectionInfo");
    qRegisterMetaType<QList<CollectionInfo> >("QList<CollectionInfo>");
}

WriteToStorage::~WriteToStorage()
{
    QMapIterator<QNetworkReply *, Entry *> i(m_replies);
    while (i.hasNext()) {
        i.next();
        i.key()->close();
        //i.key()->deleteLater();
    }
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

void WriteToStorage::setUrl(const QString & url)
{
    m_url = url;
}

QString WriteToStorage::url() const
{
    return m_url;
}

void WriteToStorage::setAdoptBibtexTypes(bool adopt)
{
    m_adoptBibtexTypes = adopt;
}

bool WriteToStorage::adoptBibtexTypes() const
{
    return m_adoptBibtexTypes;
}

void WriteToStorage::setRequestType(RequestType type)
{
    m_requestType = type;
}

RequestType WriteToStorage::requestType() const
{
    return m_requestType;
}

void WriteToStorage::serverReplyFinished(QNetworkReply *reply)
{
    m_replies.remove(reply);

    //reply->deleteLater();
}

Entry * WriteToStorage::serverReplyEntry(QNetworkReply *reply)
{
    return m_replies.value(reply);
}

int WriteToStorage::openReplies() const
{
    return m_replies.size();
}

void WriteToStorage::startRequest(const QNetworkRequest &request, const QByteArray & payload, QNetworkAccessManager::Operation mode, Entry *item)
{
    QNetworkReply *reply;
    switch(mode) {
    case QNetworkAccessManager::PutOperation:
        reply = m_qnam.put(request, payload);
        break;
    case QNetworkAccessManager::PostOperation:
        reply = m_qnam.post(request, payload);
        break;
    case QNetworkAccessManager::DeleteOperation:
        reply = m_qnam.deleteResource(request);
        break;
    default:
        reply = m_qnam.post(request, payload);
    }

    connect(reply, SIGNAL(finished()),this, SLOT(requestFinished()));
    //connect(reply, SIGNAL(downloadProgress(qint64,qint64)),this, SLOT(updateDataReadProgress(qint64,qint64)));

    m_replies.insert( reply, item );
}

void WriteToStorage::cancelUpload()
{
    QMapIterator<QNetworkReply *, Entry *> i(m_replies);
    while (i.hasNext()) {
        i.next();
        i.key()->abort();
    }
}
