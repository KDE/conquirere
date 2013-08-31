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

#include <QtCore/QObject>
#include <QtCore/QDateTime>

#include <Nepomuk2/Resource>
#include <Nepomuk2/ResourceWatcher>

/**
  * @brief Cache for one row
  *
  * Contains the nepomuk resource and the processed text + decoration data for each column of this row
  */
struct CachedRowEntry {
    QVariantList displayColums;
    QVariantList decorationColums;
    Nepomuk2::Resource resource;  //DEPRECATED: remove Nepomuk2::Resource from CachedRowEntry
    QUrl uri;
    QDateTime timestamp;
    uint resourceType; /**< saved type that can be cases to BibEntryType or SeriesEntryType */
};

Q_DECLARE_METATYPE(CachedRowEntry)
Q_DECLARE_METATYPE(QList<CachedRowEntry>)

class Library;

/**
  * @brief Abstract base class for any nepomuk query client.
  *
  * Subclasses implement the threaded nepomuk query and table model population. It is necessary
  * to do this threaded behaviour to allow large data sets to be shown. Furthermore cache entries are used
  * as  @c CachedRowEntry to speed up the display, sorting and filtering of these queries.
  *
  */
class QueryClient : public QObject
{
    Q_OBJECT
public:
    enum UpdateType {
        NEW_RESOURCE_DATA,
        UPDATE_RESOURCE_DATA
    };

    explicit QueryClient(QObject *parent = 0);
    virtual ~QueryClient();

    void setLibrary(Library *selectedLibrary);

public slots:
    virtual void startFetchData() = 0;
    virtual void stopFetchData() = 0;
    virtual void updateCacheEntry(const QUrl &uri, const QueryClient::UpdateType &updateType) = 0;

signals:
    void newCacheEntries(const QList<CachedRowEntry> &entries) const;
    void updateCacheEntries(const QList<CachedRowEntry> &entries) const;
    void removeCacheEntries(QList<QUrl> urls);

    void queryFinished() const;

private slots:
    //process resourceWatcher signals
    void propertyChanged (const Nepomuk2::Resource &resource, const Nepomuk2::Types::Property &property, const QVariantList &addedValues, const QVariantList &removedValues);
    void resourceTypeChanged (const Nepomuk2::Resource &resource, const Nepomuk2::Types::Class &type);
    void resourceRemoved(const QUrl & uri, const QList<QUrl>& types);
    void resourceCreated(const Nepomuk2::Resource & resource, const QList<QUrl>& types);

    //import sparql processed data
    void finishedInitialQuery();
    void finishedUpdateQuery();
    void finishedNewResourceQuery();

protected:
    virtual QVariantList createDisplayData(const QStringList & item) const = 0;
    virtual QVariantList createDecorationData(const QStringList & item) const = 0;

    Library *m_library;
    Nepomuk2::ResourceWatcher *m_resourceWatcher;
};

#endif // QUERYCLIENT_H
