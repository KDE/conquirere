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

#include "writetofile.h"
#include "readfromfile.h"

#include <kbibtex/entry.h>
#include <kbibtex/fileimporterbibtex.h>

#include <qjson/serializer.h>
#include <QtNetwork/QNetworkReply>
#include <QtCore/QXmlStreamReader>

#include <QDebug>

WriteToFile::WriteToFile(QObject *parent)
    : WriteToStorage(parent)
    , m_allRequestsSend(false)
{
}

WriteToFile::~WriteToFile()
{

}

void WriteToFile::pushItems(File items, const QString &collection)
{

}

void WriteToFile::pushNewItems(File items, const QString &collection)
{

}

void WriteToFile::updateItem(Entry *item)
{

}

void WriteToFile::addItemsToCollection(QList<QString> ids, const QString &collection )
{

}

void WriteToFile::removeItemsFromCollection(QList<QString> ids, const QString &collection )
{

}

void WriteToFile::deleteItems(File items)
{

}

void WriteToFile::createCollection(const CollectionInfo &ci)
{

}

void WriteToFile::editCollection(const CollectionInfo &ci)
{

}

void WriteToFile::deleteCollection(const CollectionInfo &ci)
{

}

void WriteToFile::requestFinished()
{

}
