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

#include "globals.h"
#include "core/queryclients/queryclient.h"

#include <Nepomuk/Resource>

#include <QtCore/QAbstractTableModel>

class Library;
class QModelIndex;

/**
  * @brief The Nepomuk model is a tablemodel that fetches and caches all nepomuk data automatically.
  *
  * In order to list and sort large sets of nepomuk data in a table model it is necessary to create a string cache for
  * the displayed data. This Model and the necessary subcalsses realize these via the @c CachedRowEntry
  *
  * The data is internally fetched with the corresponding @c QueryClient that retrieves the nepomuk data and updates the cache.
  * The @c QueryClient is realized as QThread to allow nonblocking polution of the TableModel.
  *
  * To alter the content of the table change the necessary header data in the @c headerData() function of the subclasses or
  * the @c createDisplayData() and @c createDecorationData() of the cofrresponding @c QueryClient
  *
  * @todo implement cache update via Nepomuk::ResourceWatcher
  */
class NepomukModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit NepomukModel(QObject *parent = 0);
    virtual ~NepomukModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;

    virtual int defaultSectionSize(int i) const = 0;
    virtual QList<int> fixedWithSections() const;

    void setLibrary(Library *library);
    Nepomuk::Resource documentResource(const QModelIndex &selection);

    virtual QString id();

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

    /**
      * saves the content of the table to disk
      *
      * Can be used to speed up the startup time of the program when very large
      * data sets are used. Currently disabled
      *
      * @see loadCache
      */
    void saveCache();

    /**
      * load the saved content of the table from disk
      *
      * Can be used to speed up the startup time of the program when very large
      * data sets are used. Currently disabled
      *
      * @see saveCache
      */
    void loadCache();

    /**
      * Updates all cached list data
      */
    void updateCacheData();

private slots:
    void addCacheData(const QList<CachedRowEntry> &entries);
    void removeCacheData(QList<QUrl> urls);
    void updateCacheData(const QList<CachedRowEntry> &entries);

protected:
    Library *m_library;

    // instead of creating the data to display every time it is requested, we cache the values for the
    // display and decoration entries in here this speeds up the model for huge data sets
    // the cache is updated via updateCacheEntry() which will be triggered by the property widgets when they
    // update the data. Later this will be replaced by the ResourceWatcher so that external changes will
    // trigger chache changes too
    QueryClient *m_queryClient;
    QList<CachedRowEntry> m_modelCacheData;
    QMap<QString, int> m_lookupCache;
};

#endif // NEPOMUKMODEL_H
