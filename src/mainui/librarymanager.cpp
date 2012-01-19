/*
 * Copyright 2012 Jörg Ehrichs <joerg.ehrichs@gmx.de>
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

#include "librarymanager.h"

#include "core/library.h"
#include "core/projectsettings.h"
#include "core/models/nepomukmodel.h"

#include <QtGui/QSortFilterProxyModel>

#include <KDE/KDebug>

LibraryManager::LibraryManager(QObject *parent)
    : QObject(parent)
{
}

LibraryManager::~LibraryManager()
{
    qDeleteAll(m_openProjectList);
    delete m_systemLibrary;
    m_systemLibrary = 0;
    m_currentUsedLibrary = 0;
}

QList<Library *> LibraryManager::openProjects()
{
    return m_openProjectList;
}

Library *LibraryManager::systemLibrary()
{
    return m_systemLibrary;
}

void LibraryManager::setCurrentUsedLibrary(Library *l)
{
    m_currentUsedLibrary = l;
}

Library *LibraryManager::currentUsedLibrary()
{
    return m_currentUsedLibrary;
}

Library *LibraryManager::libFromResourceUri(const QUrl &projectThing)
{
    foreach(Library *p, m_openProjectList) {
        if(p->settings()->projectThing().resourceUri() == projectThing) {
            return p;
        }
    }

    return m_systemLibrary;
}

void LibraryManager::addLibrary(Library *l)
{
    m_openProjectList.append(l);
    connect(this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), l, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));

    foreach (QSortFilterProxyModel *model, l->viewModels()) {
        NepomukModel *m = qobject_cast<NepomukModel *>(model->sourceModel());

        m->startFetchData();
    }

    emit libraryAdded(l);
}

void LibraryManager::addSystemLibrary(Library *l)
{
    m_systemLibrary = l;
    connect(this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), m_systemLibrary, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));

    foreach (QSortFilterProxyModel *model, l->viewModels()) {
        NepomukModel *m = qobject_cast<NepomukModel *>(model->sourceModel());

        m->startFetchData();
    }

    emit libraryAdded(l);
}

void LibraryManager::closeLibrary(Library *l)
{
    disconnect(this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), l, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    if(l == m_currentUsedLibrary)
        m_currentUsedLibrary = m_systemLibrary;

    m_openProjectList.removeAll(l);
    emit libraryRemoved(l->settings()->projectThing().resourceUri());

    l->deleteLater();
}

void LibraryManager::deleteLibrary(Library *l)
{
    disconnect(this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), l, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    if(l == m_currentUsedLibrary)
        m_currentUsedLibrary = m_systemLibrary;

    m_openProjectList.removeAll(l);
    emit libraryRemoved(l->settings()->projectThing().resourceUri());

    l->deleteLibrary();
    l->deleteLater();
}

void LibraryManager::openSettings()
{

}

void LibraryManager::openSettings(Library *l)
{

}

void LibraryManager::importData(ImportMode mode)
{

}

void LibraryManager::importData(Library *l, ImportMode mode)
{

}

void LibraryManager::exportData(ExportMode mode)
{

}

void LibraryManager::exportData(Library *l, ExportMode mode)
{

}
