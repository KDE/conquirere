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

#ifndef QUERYCLIENT_H
#define QUERYCLIENT_H

#include <QtCore/QThread>
#include <QtCore/QDateTime>

#include <Nepomuk2/Resource>
#include <Nepomuk2/Query/QueryServiceClient>
#include <Nepomuk2/Query/Result>
#include <Nepomuk2/ResourceWatcher>

/**
  * @brief Cache for one row
  *
  * Contains the nepomuk resource and the processed text + decoration data for each column of this row
  */
struct CachedRowEntry {
    QVariantList displayColums;
    QVariantList decorationColums;
    Nepomuk2::Resource resource;
    QDateTime timestamp;
};

Q_DECLARE_METATYPE(CachedRowEntry)
Q_DECLARE_METATYPE(QList<CachedRowEntry>)

class Library;
class NepomukModel;

/**
  * @brief Abstract base class for any nepomuk query client.
  *
  * Subclasses implement the threaded nepomuk query and table model population. It is necessary
  * to do this threaded behaviour to allow large data sets to be shown. Furthermore cache entries are used
  * as  @c CachedRowEntry to speed up the display, sorting and filtering of these queries.
  *
  */
class QueryClient : public QThread
{
    Q_OBJECT
public:
    explicit QueryClient(QObject *parent = 0);
    virtual ~QueryClient();

    void setLibrary(Library *selectedLibrary);
    void setModel(NepomukModel *nm);
    void run();

public slots:
    virtual void startFetchData() = 0;
    void stopFetchData();

    /**
      * Indicates that the resource has been changed and the cache needs an update
      *
      * @todo remove when starting to use ResourceWatcher later on
      */
    virtual void resourceChanged (const Nepomuk2::Resource &resource) = 0;

signals:
    void newCacheEntries(const QList<CachedRowEntry> &entries) const;
    void updateCacheEntries(const QList<CachedRowEntry> &entries) const;
    void removeCacheEntries(QList<QUrl> urls);

    void queryFinished() const;

private slots:
    //process resourceWatcher signals
    void propertyAdded (const Nepomuk2::Resource &resource, const Nepomuk2::Types::Property &property, const QVariant &value);
    void propertyChanged (const Nepomuk2::Resource &resource, const Nepomuk2::Types::Property &property, const QVariantList &oldValue, const QVariantList &newValue);
    void propertyRemoved (const Nepomuk2::Resource &resource, const Nepomuk2::Types::Property &property, const QVariant &value);
    void resourceTypeAdded (const Nepomuk2::Resource &res, const Nepomuk2::Types::Class &type);
    void resourceTypeRemoved (const Nepomuk2::Resource &res, const Nepomuk2::Types::Class &type);

    void addToCache( const QList< Nepomuk2::Query::Result > &entries ) const;
    void resultCount(int number) const;

    void finishedStartup();
    void initalQueryFinished();

protected:
    void updateCacheEntry(const Nepomuk2::Resource &resource);

    virtual QVariantList createDisplayData(const Nepomuk2::Resource & res) const = 0;
    virtual QVariantList createDecorationData(const Nepomuk2::Resource & res) const = 0;

    Library *m_library;
    NepomukModel *m_model;
    Nepomuk2::Query::QueryServiceClient *m_queryClient;
    Nepomuk2::ResourceWatcher *m_resourceWatcher;
    bool m_startupQuery;
};

#endif // QUERYCLIENT_H
