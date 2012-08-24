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
#include "../models/nepomukmodel.h"

#include "nbib.h"
#include <Nepomuk2/Vocabulary/NCAL>
#include <Nepomuk2/Vocabulary/PIMO>
#include <Nepomuk2/Vocabulary/NIE>
#include <Nepomuk2/Variant>


QueryClient::QueryClient(QObject *parent)
    :QObject(parent)
{
    qRegisterMetaType<CachedRowEntry>("CachedRowEntry");
    qRegisterMetaType<QList<CachedRowEntry> >("QList<CachedRowEntry>");
}

QueryClient::~QueryClient()
{
    m_resourceWatcher->stop();
}

void QueryClient::setLibrary(Library *selectedLibrary)
{
    m_library = selectedLibrary;
}

void QueryClient::propertyChanged (const Nepomuk2::Resource &resource, const Nepomuk2::Types::Property &property, const QVariantList &oldValue, const QVariantList &newValue)
{
    Q_UNUSED(property);
    Q_UNUSED(oldValue);
    Q_UNUSED(newValue);

    updateCacheEntry(resource);
}

void QueryClient::resourceTypeChanged (const Nepomuk2::Resource &resource, const Nepomuk2::Types::Class &type)
{
    Q_UNUSED(type);

    updateCacheEntry(resource);
}

void QueryClient::resourceRemoved(const QUrl & uri, const QList<QUrl>& types)
{
    Q_UNUSED(types);

    emit removeCacheEntries(QList<QUrl>() << uri);
}

void QueryClient::resourceCreated(const Nepomuk2::Resource & resource, const QList<QUrl>& types)
{
    Q_UNUSED(types);
    kDebug() << "resourcewatcher new resource found" << resource.genericLabel();

    QList<CachedRowEntry> newCache;

    CachedRowEntry cre;
    cre.displayColums = createDisplayData(resource);
    cre.decorationColums = createDecorationData(resource);
    cre.resource = resource;
    newCache.append(cre);
    m_resourceWatcher->addResource(resource);

    emit newCacheEntries(newCache);
}

void QueryClient::updateCacheEntry(const Nepomuk2::Resource &resource)
{
    kDebug() << "resourcewatcher found change for" << resource.genericLabel();

    QList<CachedRowEntry> newCache;

    CachedRowEntry cre;
    cre.displayColums = createDisplayData(resource);
    cre.decorationColums = createDecorationData(resource);
    cre.resource = resource;
    newCache.append(cre);

    emit updateCacheEntries(newCache);
}


