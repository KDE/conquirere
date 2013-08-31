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

#include "config/conquirere.h"
#include "config/bibglobals.h"
#include "../library.h"
#include "../projectsettings.h"
#include "../models/nepomukmodel.h"

#include "nbib.h"
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk2/Variant>
#include <QtCore/QFutureWatcher>

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
}

void QueryClient::setLibrary(Library *selectedLibrary)
{
    m_library = selectedLibrary;
}

void QueryClient::propertyChanged (const Nepomuk2::Resource &resource, const Nepomuk2::Types::Property &property, const QVariantList &addedValues, const QVariantList &removedValues)
{
    kDebug() << "propertyChanged resource" << resource.uri() << resource.genericLabel() << property << addedValues;
    Q_UNUSED(property);

    // see if we need to add / remove the changed resource from the project model
    if(property.uri() == Soprano::Vocabulary::NAO::isRelated() ) {
        if(m_library->libraryType() == BibGlobals::Library_Project) {
            if(addedValues.contains( m_library->settings()->projectThing().uri().toString() )) {
                kDebug() << resource.genericLabel() << "added to" << m_library->settings()->projectThing().genericLabel();
                updateCacheEntry(resource.uri(), QueryClient::UPDATE_RESOURCE_DATA);
            }
            else if(removedValues.contains( m_library->settings()->projectThing().uri().toString() )) {
                kDebug() << resource.genericLabel() << "removed from" << m_library->settings()->projectThing().genericLabel();
                emit removeCacheEntries(QList<QUrl>() << resource.uri());
            }
        }
        //else {...} ignore this case for system library
    }
    else {
        updateCacheEntry(resource.uri(), QueryClient::UPDATE_RESOURCE_DATA);
    }
}

void QueryClient::resourceTypeChanged (const Nepomuk2::Resource &resource, const Nepomuk2::Types::Class &type)
{
    Q_UNUSED(type);

    updateCacheEntry(resource.uri(), QueryClient::UPDATE_RESOURCE_DATA);
}

void QueryClient::resourceRemoved(const QUrl & uri, const QList<QUrl>& types)
{
    Q_UNUSED(types);
    kDebug() << "resourcewatchern resource deleted" << uri;

    emit removeCacheEntries(QList<QUrl>() << uri);
}

void QueryClient::resourceCreated(const Nepomuk2::Resource & resource, const QList<QUrl>& types)
{
    Q_UNUSED(types);
    kDebug() << "resourcewatcher new resource found" << resource.genericLabel();

    updateCacheEntry(resource.uri(), QueryClient::NEW_RESOURCE_DATA);
}

void QueryClient::finishedInitialQuery()
{
    QFutureWatcher<QList<CachedRowEntry> > *updateQueryWatcher = dynamic_cast<QFutureWatcher<QList<CachedRowEntry> > *>(sender());
    QList<CachedRowEntry> results = updateQueryWatcher->future().result();

    foreach(const CachedRowEntry &cre, results) {
        m_resourceWatcher->addResource( cre.uri );
    }

    emit newCacheEntries(results);

    //don't start the watcher if we have no resources to watch
    // will be started from the queryclient.h when updateResource inserts new items
    if( !m_resourceWatcher->resources().isEmpty()) {
        m_resourceWatcher->start();
    }

    emit queryFinished();

    delete updateQueryWatcher;
}

void QueryClient::finishedUpdateQuery()
{
    QFutureWatcher<QList<CachedRowEntry> > *updateQueryWatcher = dynamic_cast<QFutureWatcher<QList<CachedRowEntry> > *>(sender());
    QList<CachedRowEntry> results = updateQueryWatcher->future().result();

    emit updateCacheEntries(results);

    delete updateQueryWatcher;
}

void QueryClient::finishedNewResourceQuery()
{
    QFutureWatcher<QList<CachedRowEntry> > *updateQueryWatcher = dynamic_cast<QFutureWatcher<QList<CachedRowEntry> > *>(sender());
    QList<CachedRowEntry> results = updateQueryWatcher->future().result();

    m_resourceWatcher->stop(); //TODO: check if stopping resourceWatcher is necessary
    foreach(const CachedRowEntry &cre, results) {
        m_resourceWatcher->addResource( cre.uri );
    }
    m_resourceWatcher->start();

    emit newCacheEntries(results);

    delete updateQueryWatcher;
}

