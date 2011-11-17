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

#include "queryclient.h"

QueryClient::QueryClient(QObject *parent)
    :QThread(parent)
    , m_queryClient(0)
    , m_startupQuery(true)
    , m_resourceWatcher(0)
{
    qRegisterMetaType<CachedRowEntry>("CachedRowEntry");
    qRegisterMetaType<QList<CachedRowEntry> >("QList<CachedRowEntry>");
    QObject::moveToThread(this);
}

QueryClient::~QueryClient()
{
}

void QueryClient::setPimoProject(const Nepomuk::Resource & project)
{
    m_pimoProject = project;
}

void QueryClient::stopFetchData()
{
    m_queryClient->close();
}

void QueryClient::run()
{
    m_queryClient = new Nepomuk::Query::QueryServiceClient();
    connect(m_queryClient, SIGNAL(newEntries(QList<Nepomuk::Query::Result>)), this, SLOT(addToCache(QList<Nepomuk::Query::Result>)));
    connect(m_queryClient, SIGNAL(entriesRemoved(QList<QUrl>)), this, SIGNAL(removeCacheEntries(QList<QUrl>)));
    connect(m_queryClient, SIGNAL(finishedListing()), this, SIGNAL(queryFinished()));
    connect(m_queryClient, SIGNAL(resultCount(int)), this, SLOT(resultCount(int)));

    m_resourceWatcher = new Nepomuk::ResourceWatcher(this);
    connect(m_resourceWatcher, SIGNAL(propertyAdded(Nepomuk::Resource, Nepomuk::Types::Property, QVariant)), this, SLOT(resourceChanged(Nepomuk::Resource,Nepomuk::Types::Property,QVariant)));
    connect(m_resourceWatcher, SIGNAL(propertyRemoved(Nepomuk::Resource, Nepomuk::Types::Property, QVariant)), this, SLOT(resourceChanged(Nepomuk::Resource,Nepomuk::Types::Property,QVariant)));

    startFetchData();
    exec();

    delete m_resourceWatcher;
    m_queryClient->close();
    delete m_queryClient;
}

void QueryClient::addToCache( const QList< Nepomuk::Query::Result > &entries ) const
{
    QList<CachedRowEntry> newCache;

    foreach(Nepomuk::Query::Result nqr, entries) {
        Nepomuk::Resource r = nqr.resource();
        m_resourceWatcher->addResource(r);
        CachedRowEntry cre;
        cre.displayColums = createDisplayData(r);
        cre.decorationColums = createDecorationData(r);
        cre.resource = r;
        newCache.append(cre);
    }

    if(m_startupQuery) {
        emit updateCacheEntries(newCache);
    }
    else {
        emit newCacheEntries(newCache);
    }
}

void QueryClient::resultCount(int number) const
{
}

void QueryClient::resourceChanged (const Nepomuk::Resource &resource, const Nepomuk::Types::Property &property, const QVariant &value)
{
    qDebug() << "QueryClient::resourceChanged";
    QList<CachedRowEntry> newCache;

    CachedRowEntry cre;
    cre.displayColums = createDisplayData(resource);
    cre.decorationColums = createDecorationData(resource);
    cre.resource = resource;
    newCache.append(cre);

    emit updateCacheEntries(newCache);
}

void QueryClient::resourceChanged (const Nepomuk::Resource &resource)
{
    //qDebug() << "QueryClient::resourceChanged without ResourceWatcher";
    QList<CachedRowEntry> newCache;

    CachedRowEntry cre;
    cre.displayColums = createDisplayData(resource);
    cre.decorationColums = createDecorationData(resource);
    cre.resource = resource;
    newCache.append(cre);

    emit updateCacheEntries(newCache);
}

void QueryClient::finishedStartup()
{
    m_startupQuery = false;
    m_resourceWatcher->start();
}
