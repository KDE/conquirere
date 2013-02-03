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

#include "core/queryclients/queryclient.h"

#include <Nepomuk2/Resource>

#include <QtCore/QAbstractTableModel>
#include <QtCore/QThread>

class Library;
class QModelIndex;

/**
  * @brief The Nepomuk model is a tablemodel that fetches and caches all nepomuk data automatically.
  *
  * In order to list and sort large sets of nepomuk data in a table model it is necessary to create a string cache for
  * the displayed data. This Model and the necessary subclasses realize these via the @c CachedRowEntry
  *
  * The data is internally fetched with the corresponding @c QueryClient that retrieves the nepomuk data and updates the cache via
  * the @c Nepomuk2::ResourceWatcher.
  * The @c QueryClient is running inside a QThread to allow nonblocking polution of the TableModel.
  *
  * To alter the content of the table change the necessary header data in the @c headerData() function of the subclasses or
  * the @c createDisplayData() and @c createDecorationData() of the corresponding @c QueryClient.
  */
class NepomukModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit NepomukModel(QObject *parent = 0);
    virtual ~NepomukModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;

    /**
      * @return the default width for a table section if the user didn't changed it.
      */
    virtual int defaultSectionSize(int i) const = 0;

    /**
      * @return The numbers of all sections width a fixed with the user can't change (width will be default width).
      *
      * @see defaultSectionSize
      */
    virtual QList<int> fixedWidthSections() const;

    void setLibrary(Library *library);

    /**
     * @brief Returns the nepomuk resource for the model selection
     *
     * @param selection selected row of the table
     * @return the nepomuk resource for the selected row
     */
    Nepomuk2::Resource documentResource(const QModelIndex &selection);

    /**
     * @brief moves the @c QueryClient into a QThread and starts it
     */
    void startFetchData();

signals:
    // connects to the library treeview
    void dataSizeChaged(int size);
    void queryStarted();
    void queryFinished();

private slots:
    void addCacheData(const QList<CachedRowEntry> &entries);
    void removeCacheData(QList<QUrl> urls);
    void updateCacheData(const QList<CachedRowEntry> &entries);

protected:
    Library *m_library;

    // instead of creating the data to display every time it is requested, we cache the values for the
    // display and decoration entries in here this speeds up the model for huge data sets
    // the cache is updated via updateCacheEntry() which will be triggered by ResourceWatcher
    QueryClient *m_queryClient;
    QList<CachedRowEntry> m_modelCacheData;
    QMap<QString, int> m_lookupCache;
};

#endif // NEPOMUKMODEL_H
