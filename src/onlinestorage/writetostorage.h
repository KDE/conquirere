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

#ifndef WRITETOSTORAGE_H
#define WRITETOSTORAGE_H

#include "storageglobals.h"
#include <kbibtex/file.h>

#include <QtCore/QObject>

#include <QtCore/QUrl>
#include <QtCore/QByteArray>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>

class QIODevice;
class QNetworkReply;
class Entry;

class WriteToStorage : public QObject
{
    Q_OBJECT
public:
    explicit WriteToStorage(QObject *parent = 0);
    virtual ~WriteToStorage();

    void setUserName(const QString & name);
    QString userName() const;
    void setPassword(const QString & pwd);
    QString pasword() const;

signals:
    /**
      * Emits a string telling whats going on right now
      */
    void status(const QString &curentStatus);

    /**
      * Emits the progress of the api reading/processing from 0-100
      */
    void progress(int currentProgress);

    /**
      * This signal will be emitted if the processed data is ready
      *
      * @p collections informations about the changed collections
      */
    void collectionsInfo(QList<CollectionInfo> collections);

    /**
      * This signal will be emitted if the processed data is ready
      *
      * @p items the bibtex file containing the changed made to the server
      */
    void itemsInfo(File items);

public slots:
    void cancelUpload();

    virtual void pushNewItems(File items) = 0;
    virtual void updateItem(Entry *item) = 0;
    virtual void addItemsToCollection(QList<QString> ids, const QString &collection) = 0;
    virtual void removeItemsFromCollection(QList<QString> ids, const QString &collection) = 0;
    virtual void deleteItems(QList<QString> ids) = 0;

    virtual void createCollection(CollectionInfo ci, const QString &parent = QString()) = 0;
    virtual void editCollection(CollectionInfo ci) = 0;
    virtual void deleteCollection(const QString &id) = 0;

protected:
    /**
      * starts the http post request to push the necessary data
      *
      * @p url the url to get the data from
      */
    void startRequest(const QNetworkRequest &request, const QByteArray & payload);

    /**
      * sets the current request type
      *
      * Helps when parsing the requestFinished() data
      */
    void setRequestType(RequestType type);

    /**
      * @return the type of the request made
      */
    RequestType requestType() const;

    /**
      * @return the QNetworkReply instance with the data from the request
      */
    QNetworkReply *reply() const;

protected slots:
    /**
      * Called when the network request is finished
      *
      * the reply() function holds the server response for the write request
      */
    virtual void requestFinished() = 0;

private:
    QString m_name;
    QString m_password;
    RequestType m_requestType;

    QNetworkAccessManager m_qnam;
    QNetworkReply *m_reply;
};

#endif // WRITETOSTORAGE_H
