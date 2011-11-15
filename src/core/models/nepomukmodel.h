/*
 * Copyright 2011 Jörg Ehrichs <joerg.ehichs@gmx.de>
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

#ifndef NEPOMUKMODEL_H
#define NEPOMUKMODEL_H

#include "../../globals.h"
#include "../queryclients/queryclient.h"

#include <Nepomuk/Resource>

#include <QtCore/QAbstractTableModel>

class Library;
class QModelIndex;
class QueryClient;

class NepomukModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit NepomukModel(QObject *parent = 0);
    virtual ~NepomukModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;

    void setLibrary(Library *library);
    Nepomuk::Resource documentResource(const QModelIndex &selection);

signals:
    // connects to the library treeview
    void dataSizeChaged(int size);
    void queryStarted();
    void queryFinished();

    // for the connected tag cloud widget to change the internas
    void resourceAdded(const Nepomuk::Resource &resource);
    void resourceUpdated(const Nepomuk::Resource & resource);
    void resourceRemoved(const QUrl &resourceUrl);

public slots:
    void startFetchData();
    void stopFetchData();

private slots:
    void addCacheData(const QList<CachedRowEntry> &entries);
    void removeCacheData(QList<QUrl> urls);
    void updateCacheData(const QList<CachedRowEntry> &entries);

protected:
    Library *m_library;

    // instead of creating the data to display everytime it is requested, we cache the values for the
    // display and decoration entries in here this speeds up the model for huge data sets
    // the cache is updated via updateCacheEntry() which will be triggered by the property widgets when they
    // update the data. Later this will be replaced by the ResourceWatcher so that external changes will
    // trigger chache changes too
    QueryClient *m_queryClient;
    QList<CachedRowEntry> m_modelCacheData;
};
#endif // NEPOMUKMODEL_H
