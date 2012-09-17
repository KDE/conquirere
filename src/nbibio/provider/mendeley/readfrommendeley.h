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

#ifndef READFROMMENDELEY_H
#define READFROMMENDELEY_H

#include "readfromstorage.h"

#include <QtCore/QStringList>
#include <QtCore/QXmlStreamReader>
#include <QtNetwork/QNetworkAccessManager>

class QNetworkReply;

class ReadFromMendeley : public ReadFromStorage
{
    Q_OBJECT
public:
    explicit ReadFromMendeley(QObject *parent = 0);

    QVariantList data() const;
    QList<CollectionInfo> getCollectionInfo() const;

public slots:
    void fetchItems(const QString &collectionId = QString());
    void fetchItem(const QString &itemId );
    void fetchCollections(const QString &parent = QString() );
    void fetchCollection(const QString &collectionId );
    
private slots:
    void itemRequestFinished();


private:
    QVariantMap readItemEntry(QXmlStreamReader &xmlReader);
    void readJsonContent(const QString &json, QVariantMap &entry);

private:
    QVariantList m_data;
    QList<CollectionInfo> m_collections;

    QNetworkAccessManager m_qnam;
    QNetworkReply *m_reply;
};

#endif // READFROMMENDELEY_H
