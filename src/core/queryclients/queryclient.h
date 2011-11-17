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

#include <QThread>

#include <Nepomuk/Resource>
#include <Nepomuk/Query/QueryServiceClient>
#include <Nepomuk/Query/Result>
#include "dms-copy/resourcewatcher.h"

struct CachedRowEntry {
    QVariantList displayColums;
    QVariantList decorationColums;
    Nepomuk::Resource resource;
};

Q_DECLARE_METATYPE(CachedRowEntry)
Q_DECLARE_METATYPE(QList<CachedRowEntry>)

class QueryClient : public QThread
{
    Q_OBJECT
public:
    explicit QueryClient(QObject *parent = 0);
    virtual ~QueryClient();

    void setPimoProject(const Nepomuk::Resource & project);
    void run();

public slots:
    virtual void startFetchData() = 0;
    void stopFetchData();

signals:
    void newCacheEntries(const QList<CachedRowEntry> &entries) const;
    void updateCacheEntries(const QList<CachedRowEntry> &entries) const;
    void removeCacheEntries(QList<QUrl> urls);

    void queryFinished() const;

private slots:
    void addToCache( const QList< Nepomuk::Query::Result > &entries ) const;
    void resultCount(int number) const;
    void resourceChanged (const Nepomuk::Resource &resource, const Nepomuk::Types::Property &property, const QVariant &value);

    /**
      * @todo remove when starting to use ResourceWatcher later on
      */
    void resourceChanged (const Nepomuk::Resource &resource);

    void finishedStartup();

protected:
    virtual QVariantList createDisplayData(const Nepomuk::Resource & res) const = 0;
    virtual QVariantList createDecorationData(const Nepomuk::Resource & res) const = 0;

    Nepomuk::Resource m_pimoProject;
    Nepomuk::Query::QueryServiceClient *m_queryClient;
    bool m_startupQuery;
    Nepomuk::ResourceWatcher *m_resourceWatcher;
};

#endif // QUERYCLIENT_H
