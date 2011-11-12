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

#include "../globals.h"
#include <Nepomuk/Resource>
#include <Nepomuk/Query/QueryServiceClient>
#include <Nepomuk/Query/Result>

#include <QtCore/QAbstractTableModel>

class Library;
class QModelIndex;

struct CachedRowEntry {
    QVariantList displayColums;
    QVariantList decorationColums;
    Nepomuk::Resource resource;
    QString lastModified;
};

class NepomukModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit NepomukModel(QObject *parent = 0);
    virtual ~NepomukModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;

    virtual void setLibrary(Library *library);
    virtual void setResourceType(ResourceSelection selection);
    virtual Nepomuk::Resource documentResource(const QModelIndex &selection);

    virtual void removeSelectedFromProject(const QModelIndex & index, Library *l);
    virtual void removeSelectedFromSystem(const QModelIndex & index);

signals:
    void dataSizeChaged(int size);
    void updateFetchDataFor(ResourceSelection selection, bool start, Library *library);
    void updateEntry(int row);
    void hasTag(const QString & tag);

public slots:
    virtual void startFetchData() = 0;
    virtual void stopFetchData();

protected:
    QList<CachedRowEntry> addToCache( const QList< Nepomuk::Query::Result > &entries );
    virtual QVariantList createDisplayData(const Nepomuk::Resource & res) = 0;
    virtual QVariantList createDecorationData(const Nepomuk::Resource & res) = 0;

private slots:
    void addData(const QList< Nepomuk::Query::Result > &entries);
    void removeData( const QList< QUrl > &entries );
    void updateCacheEntry(Nepomuk::Resource resource);
    void dataCacheProcessed();

    void resultCount(int number);
    void listingsFinished();
    void listingsError(const QString & 	errorMessage);

protected:
    Library *m_library;
    ResourceSelection m_selection;
    Nepomuk::Query::QueryServiceClient *m_queryClient;
    bool m_queryFinished;
    int m_activeFutureWatchers;
    QList< Nepomuk::Query::Result > m_queryCache;

    // instead of creating the data to display everytime it is requested, we cache the values for the
    // display and decoration entries in here this speeds up the model for huge data sets
    // the cache is updated via updateCacheEntry() which will be triggered by the property widgets when they
    // update the data. Later this will be replaced by the ResourceWatcher so that external changes will
    // trigger chache changes too
    QList<CachedRowEntry> m_modelCacheData;
};
#endif // NEPOMUKMODEL_H
