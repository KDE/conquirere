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

#include "nbibio/conquirere.h"
#include "globals.h"

#include "nbib.h"
#include <Nepomuk/Vocabulary/NCAL>
#include <Nepomuk/Vocabulary/PIMO>
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Variant>
#include <Nepomuk/Thing>

QueryClient::QueryClient(QObject *parent)
    :QThread(parent)
    , m_queryClient(0)
    , m_startupQuery(true)
{
    qRegisterMetaType<CachedRowEntry>("CachedRowEntry");
    qRegisterMetaType<QList<CachedRowEntry> >("QList<CachedRowEntry>");
    QObject::moveToThread(this);
}

QueryClient::~QueryClient()
{
    m_queryClient->close();
    delete m_queryClient;
}

void QueryClient::setLibrary(Library *selectedLibrary)
{
    m_library = selectedLibrary;
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
    connect(m_queryClient, SIGNAL(finishedListing()), this, SLOT(initalQueryFinished()));
    connect(m_queryClient, SIGNAL(resultCount(int)), this, SLOT(resultCount(int)));

    m_resourceWatcher = new Nepomuk::ResourceWatcher(this);
    m_resourceWatcher->addProperty(Nepomuk::Vocabulary::NIE::title());
//    m_resourceWatcher->addProperty(Nepomuk::Vocabulary::NBIB::publishedAs());
//    m_resourceWatcher->addProperty(Nepomuk::Vocabulary::NBIB::isPublicationOf());
    //    m_resourceWatcher->addProperty(Soprano::Vocabulary::NAO::isRelated());

    connect(m_resourceWatcher, SIGNAL(propertyAdded(Nepomuk::Resource,Nepomuk::Types::Property,QVariant)), this, SLOT(propertyAdded(Nepomuk::Resource,Nepomuk::Types::Property,QVariant)));
    connect(m_resourceWatcher, SIGNAL(propertyChanged(Nepomuk::Resource,Nepomuk::Types::Property,QVariantList,QVariantList)), this, SLOT(propertyChanged(Nepomuk::Resource,Nepomuk::Types::Property,QVariantList,QVariantList)));
    connect(m_resourceWatcher, SIGNAL(propertyRemoved(Nepomuk::Resource,Nepomuk::Types::Property,QVariant)), this, SLOT(propertyRemoved(Nepomuk::Resource,Nepomuk::Types::Property,QVariant)));
    connect(m_resourceWatcher, SIGNAL(resourceTypeAdded(Nepomuk::Resource,Nepomuk::Types::Class)), this, SLOT(resourceTypeAdded(Nepomuk::Resource,Nepomuk::Types::Class)));
    connect(m_resourceWatcher, SIGNAL(resourceTypeRemoved(Nepomuk::Resource,Nepomuk::Types::Class)), this, SLOT(resourceTypeRemoved(Nepomuk::Resource,Nepomuk::Types::Class)));


    startFetchData();
    exec();

    m_resourceWatcher->stop();
    delete m_resourceWatcher;
    m_resourceWatcher = 0;
    m_queryClient->close();
    delete m_queryClient;
    m_queryClient = 0;
}

void QueryClient::propertyAdded (const Nepomuk::Resource &resource, const Nepomuk::Types::Property &property, const QVariant &value)
{
    updateCacheEntry(resource);
}

void QueryClient::propertyChanged (const Nepomuk::Resource &resource, const Nepomuk::Types::Property &property, const QVariantList &oldValue, const QVariantList &newValue)
{
    updateCacheEntry(resource);
}

void QueryClient::propertyRemoved (const Nepomuk::Resource &resource, const Nepomuk::Types::Property &property, const QVariant &value)
{
    updateCacheEntry(resource);
}

void QueryClient::resourceTypeAdded (const Nepomuk::Resource &res, const Nepomuk::Types::Class &type)
{
    updateCacheEntry(res);
}

void QueryClient::resourceTypeRemoved (const Nepomuk::Resource &res, const Nepomuk::Types::Class &type)
{
    updateCacheEntry(res);
}

void QueryClient::updateCacheEntry(const Nepomuk::Resource &resource)
{
    kDebug() << "resourcewatcher found change by" << resource.genericLabel();

    QList<CachedRowEntry> newCache;

    CachedRowEntry cre;
    cre.displayColums = createDisplayData(resource);
    cre.decorationColums = createDecorationData(resource);
    cre.resource = resource;
    newCache.append(cre);

    emit updateCacheEntries(newCache);
}

void QueryClient::addToCache( const QList< Nepomuk::Query::Result > &entries ) const
{
//    QList<CachedRowEntry> newCache;

    foreach(const Nepomuk::Query::Result &nqr, entries) {
        Nepomuk::Resource r = nqr.resource();

        //TODO Event workaround as we search for pimo:Event and NCAl:Event we end up
        // with duplicates. here we ignore all ncal:Events when a pimo:event exist for it
        if(r.hasType(Nepomuk::Vocabulary::NCAL::Event())) {
            Nepomuk::Thing t = r.pimoThing();
            if(t.isValid())
                continue;
        }

        // workaround as creating a query regarding this leads to a very bad behaviour
        // @see referencequery.cpp
        if(r.hasType(Nepomuk::Vocabulary::NBIB::Reference())) {
            Nepomuk::Resource pub = r.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();
            if(ConqSettings::hiddenNbibPublications().contains( (int)BibEntryTypeFromUrl(pub))) {
                continue;
            }
        }

        m_resourceWatcher->addResource(r);

        QList<CachedRowEntry> newCache;
        CachedRowEntry cre;
        cre.displayColums = createDisplayData(r);
        cre.decorationColums = createDecorationData(r);
        cre.resource = r;
        newCache.append(cre);

        // we do not send all found cache results at once (like commented below)
        // because the queryclient seems to cach simmilar queries and does not return each single query
        // as soon as found but all found entries
        // this leads to a slot table insert and very slow startup of the program
        if(m_startupQuery) {
            emit updateCacheEntries(newCache);
        }
        else {
            emit newCacheEntries(newCache);
        }
    }

//    if(m_startupQuery) {
//        emit updateCacheEntries(newCache);
//    }
//    else {
//        emit newCacheEntries(newCache);
//    }
}

void QueryClient::resultCount(int number) const
{
}

void QueryClient::finishedStartup()
{
    m_startupQuery = false;
}

void QueryClient::initalQueryFinished()
{
//    kDebug() << "startResourceWatcher" << m_resourceWatcher->resources().size();

//    if( m_resourceWatcher->resources().size() > 0)
//        m_resourceWatcher->start();
}
