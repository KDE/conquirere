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
#include "../library.h"
#include "../projectsettings.h"
#include "../models/nepomukmodel.h"

#include "nbib.h"
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk2/Variant>


QueryClient::QueryClient(QObject *parent)
    :QObject(parent)
    , m_library(0)
    , m_resourceWatcher(0)
{
    qRegisterMetaType<CachedRowEntry>("CachedRowEntry");
    qRegisterMetaType<QList<CachedRowEntry> >("QList<CachedRowEntry>");
}

QueryClient::~QueryClient()
{
    m_resourceWatcher->stop();
    delete m_resourceWatcher;
}

void QueryClient::setLibrary(Library *selectedLibrary)
{
    m_library = selectedLibrary;
}

void QueryClient::propertyChanged (const Nepomuk2::Resource &resource, const Nepomuk2::Types::Property &property, const QVariantList &addedValues, const QVariantList &removedValues)
{
    Q_UNUSED(property);

    // see if we need to add / remove the changed resource from the project model
    if(property.uri() == Soprano::Vocabulary::NAO::isRelated() ) {
        if(m_library->libraryType() == Library_Project) {
            if(addedValues.contains( m_library->settings()->projectThing().uri().toString() )) {
                kDebug() << resource.genericLabel() << "added to" << m_library->settings()->projectThing().genericLabel();
                updateCacheEntry(resource);
            }
            else if(removedValues.contains( m_library->settings()->projectThing().uri().toString() )) {
                kDebug() << resource.genericLabel() << "removed from" << m_library->settings()->projectThing().genericLabel();
                emit removeCacheEntries(QList<QUrl>() << resource.uri());
            }
        }
        //else {...} ignore this case for system library
    }
    else {
        updateCacheEntry(resource);
    }
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
    m_resourceWatcher->stop(); //TODO: check if stopping resourceWatcher is necessary
    m_resourceWatcher->addResource(resource);
    m_resourceWatcher->start();

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


