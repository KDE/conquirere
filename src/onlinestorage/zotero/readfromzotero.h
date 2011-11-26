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

#ifndef READFROMZOTERO_H
#define READFROMZOTERO_H

#include "../readfromstorage.h"

#include <QtCore/QObject>
#include <QXmlStreamReader>

class Element;
class Entry;

class ReadFromZotero : public ReadFromStorage
{
    Q_OBJECT
public:
    explicit ReadFromZotero(QObject *parent = 0);

public slots:
    void fetchItems();
    void fetchItem(const QString &id, const QString &collection = QString() );
    void fetchCollections(const QString &parent = QString() );
    void fetchCollection(const QString &collection );

protected slots:
    void requestFinished();

private:
    CollectionInfo readCollectionEntry(QXmlStreamReader &xmlReader);
    Element *readItemEntry(QXmlStreamReader &xmlReader);
    void readJsonContent(Entry *e, const QString &content);

    QList<CollectionInfo> m_cachedCollectionResult;
    File m_bibFile;
};

#endif // READFROMZOTERO_H
