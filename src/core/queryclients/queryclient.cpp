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
    connect(m_queryClient, SIGNAL(resultCount(int)), this, SLOT(resultCount(int)));

    startFetchData();
    exec();

    m_queryClient->close();
    delete m_queryClient;
    m_queryClient = 0;
}

void QueryClient::addToCache( const QList< Nepomuk::Query::Result > &entries ) const
{
    QList<CachedRowEntry> newCache;

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

void QueryClient::finishedStartup()
{
    m_startupQuery = false;
}
