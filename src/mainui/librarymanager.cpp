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

#include "mainui/settings/projectsettingsdialog.h"
#include "mainui/settings/conquireresettingsdialog.h"

#include "sync/bibtexexportdialog.h"
#include "sync/bibteximportwizard.h"
#include "sync/synczoterodialog.h"

#include <QtGui/QSortFilterProxyModel>

#include <KDE/KDebug>

LibraryManager::LibraryManager(QObject *parent)
    : QObject(parent)
    , m_systemLibrary(0)
    , m_currentUsedLibrary(0)
    , m_backgroundSyncManager(0)
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

void LibraryManager::openSettings()
{
    openSettings(m_systemLibrary);
}

void LibraryManager::openSettings(Library *l)
{
    if(l->libraryType() == Library_Project) {
        ProjectSettingsDialog settingsDialog;
        settingsDialog.setProjectSettings(l->settings());

        settingsDialog.exec();
    }
    else {
        ConquirereSettingsDialog csd;
        csd.setProjectSettings(l->settings());

        csd.exec();
    }
}

void LibraryManager::importData(ModeSelection mode)
{
    importData(m_systemLibrary, mode);
}

void LibraryManager::importData(Library *l, ModeSelection mode)
{
    // disable tagcloud generation during import for all libraries
    // TODO the same for the qcompleter in the property widgets
    systemLibrary()->tagCloud()->pauseUpdates(true);
    foreach(Library *p, m_openProjectList) {
        p->tagCloud()->pauseUpdates(true);
    }

    if(mode == Zotero_Sync) {
        SyncZoteroDialog szd;
//        szd.setLibraryManager(this);

//        if(l != m_systemLibrary) {
//            szd.setImportLibrary(l);
//        }
//        szd.setupUi();

        szd.exec();
    }
    else {
        BibTeXImportWizard bid;
        bid.setLibraryManager(this);

        if(l != m_systemLibrary) {
            bid.setImportLibrary(l);
        }

        bid.setupUi();
        bid.exec();
    }

//    updateListCache();

    // enable generation again
    systemLibrary()->tagCloud()->pauseUpdates(false);
    foreach(Library *p, m_openProjectList) {
        p->tagCloud()->pauseUpdates(false);
    }
}

void LibraryManager::exportData(ModeSelection mode)
{
    exportData(m_systemLibrary, mode);
}

void LibraryManager::exportData(Library *l, ModeSelection mode)
{
    if(mode == Zotero_Sync) {
        SyncZoteroDialog szd;
//        szd.setLibraryManager(this);

//        if(l != m_systemLibrary) {
//            szd.setImportLibrary(l);
//        }
//        szd.setupUi();

        szd.exec();
    }
    else {
        BibTexExportDialog bed;
        bed.setInitialFileType(NBibExporterFile::EXPORT_BIBTEX);
        bed.setLibraryManager(this);

        if(l != m_systemLibrary) {
            bed.setExportLibrary(l);
        }

        bed.exec();
    }
}

void LibraryManager::syncData(ModeSelection mode)
{
    syncData(m_systemLibrary, mode);
}

void LibraryManager::syncData(Library *l, ModeSelection mode)
{
    if(mode == Zotero_Sync) {
        SyncZoteroDialog szd;
//        szd.setLibraryManager(this);

//        if(l != m_systemLibrary) {
//            szd.setImportLibrary(l);
//        }
//        szd.setupUi();

        szd.exec();
    }
    else {

    }
}
