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
#include "core/tagcloud.h"
#include "core/projectsettings.h"
#include "core/models/nepomukmodel.h"

#include <QtGui/QSortFilterProxyModel>

#include <KDE/KDebug>

LibraryManager::LibraryManager(QObject *parent)
    : QObject(parent)
    , m_systemLibrary(0)
    , m_currentUsedLibrary(0)
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
    if(!m_currentUsedLibrary)
        return m_systemLibrary;
    else
        return m_currentUsedLibrary;
}

Library *LibraryManager::libFromResourceUri(const QUrl &projectThing)
{
    foreach(Library *p, m_openProjectList) {
        if(p->settings()->projectThing().uri() == projectThing) {
            return p;
        }
    }

    return m_systemLibrary;
}

void LibraryManager::addLibrary(Library *l)
{
    m_openProjectList.append(l);

    foreach (QSortFilterProxyModel *model, l->viewModels()) {
        NepomukModel *m = qobject_cast<NepomukModel *>(model->sourceModel());

        m->startFetchData();
    }

    emit libraryAdded(l);
}

void LibraryManager::addSystemLibrary(Library *l)
{
    m_systemLibrary = l;

    foreach (QSortFilterProxyModel *model, l->viewModels()) {
        NepomukModel *m = qobject_cast<NepomukModel *>(model->sourceModel());

        m->startFetchData();
    }

    emit libraryAdded(l);
}

void LibraryManager::closeLibrary(Library *l)
{
    if(l == m_currentUsedLibrary)
        m_currentUsedLibrary = m_systemLibrary;

    m_openProjectList.removeAll(l);
    QUrl thingUri = l->settings()->projectThing().uri();
    emit libraryRemoved(thingUri);

    l->deleteLater();
}

void LibraryManager::deleteLibrary(Library *l)
{
    if(l == m_currentUsedLibrary)
        m_currentUsedLibrary = m_systemLibrary;

    m_openProjectList.removeAll(l);
    QUrl thingUri = l->settings()->projectThing().uri();
    emit libraryRemoved(thingUri);

    l->deleteLibrary();
    l->deleteLater();
}

void LibraryManager::doOpenSettings(Library *l)
{
    if(l) {
        emit openSettings(l);
    }
    else {
        emit openSettings(m_systemLibrary);
    }
}

void LibraryManager::doImportFile(Library *l)
{
    if(l) {
        emit importFile(l);
    }
    else {
        emit importFile(m_systemLibrary);
    }
}

void LibraryManager::doExportFile(Library *l)
{
    if(l) {
        emit exportFile(l);
    }
    else {
        emit exportFile(m_systemLibrary);
    }
}

void LibraryManager::doSyncStorage(Library *l)
{
    if(l) {
        emit syncStorage(l);
    }
    else {
        emit syncStorage(m_systemLibrary);
    }
}

