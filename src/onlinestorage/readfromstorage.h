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

#ifndef READFROMSTORAGE_H
#define READFROMSTORAGE_H

#include "storageglobals.h"
#include "providersettings.h"

#include <kbibtex/file.h>

#include <QtCore/QObject>

#include <QtCore/QUrl>
#include <QtNetwork/QNetworkAccessManager>

class QNetworkReply;

/**
  * @brief Abstract base class for any kind of online publication storage
  *
  * Retrieves all available collections and the items in it from an online storage
  *
  * @author Jörg Ehrichs <joerg.ehrichs@gmx.de>
  */
class ReadFromStorage : public QObject
{
    Q_OBJECT
public:
    explicit ReadFromStorage(QObject *parent = 0);
    virtual ~ReadFromStorage();

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
      * @p collections information about all available collections
      */
    void collectionsInfo(QList<CollectionInfo> collections);

    /**
      * This signal will be emitted if the processed data is ready
      *
      * @p items the bibtex files containing all available bibtex items
      *          the result should be merged with the current entries
      */
    void itemsInfo(File items);

public slots:
    void cancelDownload();
    virtual void fetchItems(const QString &collection = QString()) = 0;
    virtual void fetchItem(const QString &id, const QString &collection = QString() ) = 0;

    virtual void fetchCollections(const QString &parent = QString() ) = 0;
    virtual void fetchCollection(const QString &collection ) = 0;

protected:
    /**
      * starts the http request to fetch the necessary data
      *
      * @p url the url to get the data from
      */
    void startRequest(QUrl url);

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
      * the reply() function holds the downloaded information about the items/collections
      */
    virtual void requestFinished() = 0;

protected:
    ProviderSyncDetails m_psd;

private:
    bool m_adoptBibtexTypes;
    RequestType m_requestType;

    QNetworkAccessManager m_qnam;
    QNetworkReply *m_reply;
};

#endif // READFROMSTORAGE_H
