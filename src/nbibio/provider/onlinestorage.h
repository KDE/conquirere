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

#ifndef ONLINESTORAGE_H
#define ONLINESTORAGE_H

#include <QtCore/QObject>

#include "nbibio/storageglobals.h"

#include <KDE/KUrl>
#include <KDE/KIcon>
#include <QtCore/QVariantList>

class OnlineStorage : public QObject
{
    Q_OBJECT
public:
    explicit OnlineStorage(QObject *parent = 0);

    virtual QString providerId() const = 0;
    virtual QString providerName() const = 0;
    virtual KIcon providerIcon() const = 0;
    virtual bool supportCollections() const = 0;
    virtual QString helpText() const = 0;
    virtual QString defaultUrl() const = 0;
    virtual bool useUrlSelector() const = 0;

    void setProviderSettings(const ProviderSyncDetails &psd);
    ProviderSyncDetails providerSettings() const;

    virtual QVariantList data() const = 0;
    virtual QList<CollectionInfo> collectionInfo() const = 0;

signals:
    void status(const QString &curentStatus);
    void progress(int currentProgress);
    void finished();

    void error(const QString &errorMessage);

public slots:
    virtual void cancel() = 0;

    //-------------------------------------------------
    // read request
    //-------------------------------------------------
    virtual void fetchItem(const QString &itemId ) = 0;
    virtual void fetchItems(const QString &collectionId = QString()) = 0;
    virtual void fetchCollections(const QString &parent = QString() ) = 0;
    virtual void fetchCollection(const QString &collectionId ) = 0;
    virtual KUrl downloadFile(const QString &file, const QString &filename) = 0;

    //-------------------------------------------------
    // write request
    //-------------------------------------------------
    virtual void pushItems(const QVariantList &items, const QString &collection = QString()) = 0;
    virtual void pushFile(const QVariantMap &fileData) = 0;
    virtual void addItemsToCollection(const QStringList &ids, const QString &collection) = 0;
    virtual void removeItemsFromCollection(const QStringList&ids, const QString &collection) = 0;
    virtual void deleteItems(const QVariantList &items) = 0;

    virtual void createCollection(const CollectionInfo &ci) = 0;
    virtual void editCollection(const CollectionInfo &ci) = 0;
    virtual void deleteCollection(const CollectionInfo &ci) = 0;

private:
    ProviderSyncDetails m_psd;
};

#endif // ONLINESTORAGE_H
