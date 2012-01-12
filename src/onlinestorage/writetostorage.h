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
#include "providersettings.h"

#include <kbibtex/file.h>

#include <QtCore/QObject>

#include <QtCore/QUrl>
#include <QtCore/QByteArray>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>

class QIODevice;
class QNetworkReply;
class Entry;

/**
  * @brief Abstract Base class to write bibtex entries to an online storage
  *
  * @author Jörg Ehrichs <joerg.ehrichs@gmx.de>
  */
class WriteToStorage : public QObject
{
    Q_OBJECT
public:
    explicit WriteToStorage(QObject *parent = 0);
    virtual ~WriteToStorage();

    void setProviderSettings(const ProviderSyncDetails &psd);

    void setAdoptBibtexTypes(bool adopt);
    bool adoptBibtexTypes() const;

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
      * @p collections information about the changed collections
      */
    void collectionsInfo(const QList<CollectionInfo> &collections);

    /**
      * This signal will be emitted if the processed data is ready
      *
      * @p items the bibtex file containing the changes made to the server
      */
    void itemsInfo(const File &items);

public slots:
    void cancelUpload();

    virtual void pushItems(const File &items, const QString &collection = QString()) = 0;
    virtual void pushNewItems(const File &items, const QString &collection = QString()) = 0;
    virtual void updateItem(QSharedPointer<Element> item) = 0;
    virtual void addItemsToCollection(const QList<QString> &ids, const QString &collection) = 0;
    virtual void removeItemsFromCollection(const QList<QString> &ids, const QString &collection) = 0;
    virtual void deleteItems(const File &items) = 0;

    virtual void createCollection(const CollectionInfo &ci) = 0;
    virtual void editCollection(const CollectionInfo &ci) = 0;
    virtual void deleteCollection(const CollectionInfo &ci) = 0;

protected:
    /**
      * starts the http post request to push the necessary data
      *
      * @p url the url to get the data from
      */
    void startRequest(const QNetworkRequest &request, const QByteArray & payload, QNetworkAccessManager::Operation mode, QSharedPointer<Entry> item = QSharedPointer<Entry>());

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
      * removes the reply from the m_replies map and call deleteLater() on them
      */
    void serverReplyFinished(QNetworkReply *reply);

    /**
      * returns the entry that was used to call the reply
      *
      * Useful when updating items one by one, as we cann directly and easily update the entry with the response
      */
    QSharedPointer<Entry> serverReplyEntry(QNetworkReply *reply);

    /**
      * Tells how many open reply we still have
      *
      * if the value is 0 and we are not ading new requests we are definitly finished
      */
    int openReplies() const;

protected slots:
    /**
      * Called when the network request is finished
      *
      * If changes to the bibtex entry need to be made based on the result of the reply you can get the information via
      * QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
      *
      * To get the Entry related to that request a call to serverReplyEntry() is necessary
      * it retiurns a pointer to the right entry or 0
      */
    virtual void requestFinished() = 0;

protected:
    ProviderSyncDetails m_psd;

private:
    bool m_adoptBibtexTypes;
    RequestType m_requestType;

    QNetworkAccessManager m_qnam;
    QMap<QNetworkReply *, QSharedPointer<Entry> > m_replies;
};

#endif // WRITETOSTORAGE_H
