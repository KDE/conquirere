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

#include "readfrommendeley.h"

#include <qjson/parser.h>

#include <KDE/KConfig>
#include <KDE/KConfigGroup>
#include <KDE/KDebug>

#include <QtNetwork/QNetworkReply>
#include <QtCore/QUrl>
#include <QtCore/QRegExp>

ReadFromMendeley::ReadFromMendeley(QObject *parent)
    : ReadFromStorage(parent)
    , m_reply(0)
{
}

QVariantList ReadFromMendeley::data() const
{
    return m_data;
}

QList<CollectionInfo> ReadFromMendeley::getCollectionInfo() const
{
    return m_collections;
}

void ReadFromMendeley::fetchItems(const QString &collectionId)
{

}

void ReadFromMendeley::fetchItem(const QString &itemId )
{

}

void ReadFromMendeley::fetchCollections(const QString &parent )
{

}

void ReadFromMendeley::fetchCollection(const QString &collectionId )
{

}

void ReadFromMendeley::itemRequestFinished()
{
}

QVariantMap ReadFromMendeley::readItemEntry(QXmlStreamReader &xmlReader)
{
    QVariantMap entry;

    return entry;
}
