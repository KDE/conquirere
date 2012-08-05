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

#include "tableviewmenu.h"

#include "core/library.h"
#include "core/projectsettings.h"

#include "mainui/librarymanager.h"

#include "nbibio/pipe/bibtextoclipboardpipe.h"
#include "nbibio/pipe/bibtextonepomukpipe.h"
#include "nbibio/pipe/nepomuktobibtexpipe.h"
#include "nbibio/pipe/kilelyxpipe.h"

#include <kbibtex/value.h>

#include <Nepomuk2/DataManagement>

#include "nbib.h"
#include <Nepomuk2/Vocabulary/PIMO>
#include <Nepomuk2/Vocabulary/NIE>
#include <Nepomuk2/Vocabulary/NFO>
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk2/Variant>

#include <KDE/KMimeType>
#include <KDE/KIcon>
#include <KDE/KDebug>

#include <QtGui/QMenu>
#include <QtGui/QAction>
#include <QtGui/QDesktopServices>

using namespace Nepomuk2::Vocabulary;
using namespace Soprano::Vocabulary;

TableViewMenu::TableViewMenu(QObject *parent)
    : QObject(parent)
    , m_libraryManager(0)
    , m_bibtexEntry(0)
{
}

void TableViewMenu::showNepomukEntryMenu(Nepomuk2::Resource resource)
{
    m_nepomukResource = resource;

    QMenu menu;
    QList<QAction *> actionCollection; //we throw all temp actions into it and delete them again after execution

    // ###########################################################
    // # add  file open in tab menu
    QAction *openInTab = new QAction(KIcon(QLatin1String("tab-new-background.png")), i18n("Open in Tab"), this);
    connect(openInTab, SIGNAL(triggered(bool)),this, SLOT(openInTab()));
    menu.addAction(openInTab);
    actionCollection.append(openInTab);

    // ###########################################################
    // # add  file open menu
    QMenu openExternal;
    openExternal.setTitle(i18n("open external"));
    openExternal.setIcon(KIcon(QLatin1String("document-open")));
    menu.addMenu(&openExternal);

    if(m_nepomukResource.hasType(NBIB::Publication()) || m_nepomukResource.hasType(NBIB::Reference()) ) {
        Nepomuk2::Resource queryResource;
        if(resource.hasType(NBIB::Reference())) {
            queryResource = m_nepomukResource.property(NBIB::publication()).toResource();
        }
        else {
            queryResource = m_nepomukResource;
        }

        QList<Nepomuk2::Resource> fileList = queryResource.property(NBIB::isPublicationOf()).toResourceList();

        bool hasDataObjects = false;
        bool hasDOI = false;
        // this adds all DataObject links
        if(!fileList.isEmpty()) {
            hasDataObjects = true;
            foreach(const Nepomuk2::Resource &r, fileList) {
                KUrl file = r.property(NIE::url()).toUrl();
                QString name;
                KIcon icon(KMimeType::iconNameForUrl(file));

                if(file.isLocalFile()) {
                    name = file.fileName();
                }
                else {
                    if(r.type() == NFO::RemoteDataObject()) {
                        name = file.path();
                    }
                    else {
                        name = file.host();
                    }
                }
                QAction *a = new QAction(icon, name, this);
                a->setData(QUrl(file));
                connect(a, SIGNAL(triggered(bool)),this, SLOT(openSelected()));
                openExternal.addAction(a);
                actionCollection.append(a);
            }
        }

        // this adds the doi link
        QString doi = queryResource.property(NBIB::doi()).toString();

        if(!doi.isEmpty()) {
            hasDOI = true;
            if(!doi.startsWith(QLatin1String("http"))) {
                doi = QLatin1String("http://dx.doi.org/") + doi;
            }
            KIcon icon = KIcon("text-html");

            QAction *a = new QAction(icon, doi, this);
            a->setData(QUrl(doi));
            connect(a, SIGNAL(triggered(bool)),this, SLOT(openSelected()));
            openExternal.addAction(a);
            actionCollection.append(a);
        }

        if(!hasDOI && !hasDataObjects) {
            openExternal.setEnabled(false);
        }
    }
    else {
        KUrl file = m_nepomukResource.property(NIE::url()).toUrl();
        QString name;
        KIcon icon(KMimeType::iconNameForUrl(file));
        if(file.isLocalFile()) {
            name = file.fileName();
        }
        else {
            name = file.host();
        }

        QAction *a = new QAction(icon, name, this);
        a->setData(QUrl(file));
        actionCollection.append(a);
        connect(a, SIGNAL(triggered(bool)),this, SLOT(openSelected()));
        openExternal.addAction(a);
    }

    // ###########################################################
    // # add export menu
    QMenu openExport;
    openExport.setTitle(i18n("Export"));
    openExport.setIcon(KIcon(QLatin1String("document-export")));
    menu.addMenu(&openExport);

    QAction *exportBibTeX = new QAction(KIcon(QLatin1String("document-export")), i18n("Export BibTex to Clipboard"), this);
    connect(exportBibTeX, SIGNAL(triggered(bool)),this, SLOT(exportBibTexReference()));
    openExport.addAction(exportBibTeX);
    actionCollection.append(exportBibTeX);

    QAction *exportCiteKey = new QAction(KIcon(QLatin1String("document-export")), i18n("Export Citekey to Clipboard"), this);
    connect(exportCiteKey, SIGNAL(triggered(bool)),this, SLOT(exportCiteKey()));
    openExport.addAction(exportCiteKey);
    actionCollection.append(exportCiteKey);

    QAction *sendToKileLyX = new QAction(KIcon(QLatin1String("document-export")), i18n("Send to Kile/LyX"), this);
    connect(sendToKileLyX, SIGNAL(triggered(bool)),this, SLOT(sendToKileLyX()));
    menu.addAction(sendToKileLyX);
    actionCollection.append(sendToKileLyX);

    menu.addSeparator();

    // ###########################################################
    // # add to project menu
    QMenu addToProject;
    addToProject.setTitle(i18n("Add to Project"));
    addToProject.setIcon(KIcon(QLatin1String("insert-link")));
    menu.addMenu(&addToProject);

    QList<Library*> openLibList = m_libraryManager->openProjects();

    if(openLibList.size() > 0) {
        foreach(Library *l, openLibList) {
            QAction *a = new QAction(KIcon(QLatin1String("conquirere")), l->settings()->name(), this);
            a->setData(l->settings()->projectThing().uri());
            actionCollection.append(a);
            connect(a, SIGNAL(triggered(bool)),this, SLOT(addSelectedToProject()));
            addToProject.addAction(a);
        }
    }
    else {
        addToProject.setEnabled(false);
    }

    // ###########################################################
    // # remove from project menu
    QMenu removeFromProject;
    removeFromProject.setTitle(i18n("Remove from Project"));
    removeFromProject.setIcon(KIcon(QLatin1String("remove-link")));
    menu.addMenu(&removeFromProject);

    // fill al list of all project this file is in
    QList<Nepomuk2::Resource> projectList = m_nepomukResource.property(NAO::isRelated()).toResourceList();

    bool projectRelated = false;
    foreach(const Nepomuk2::Resource &project, projectList) {
        if(project.hasType(PIMO::Project())) {
            projectRelated = true;
            QAction *a = new QAction(KIcon(QLatin1String("conquirere")), project.property(NAO::prefLabel()).toString(), this);
            a->setData(project.uri());
            actionCollection.append(a);
            connect(a, SIGNAL(triggered(bool)),this, SLOT(removeSelectedFromProject()));
            removeFromProject.addAction(a);
        }
    }
    removeFromProject.setEnabled(projectRelated);


    // ###########################################################
    // # remove from system
    QAction *a = new QAction(KIcon(QLatin1String("edit-delete-shred")), i18n("Remove from System"), this);
    actionCollection.append(a);
    connect(a, SIGNAL(triggered(bool)),this, SLOT(removeSelectedFromSystem()));
    menu.addAction(a);

    menu.exec(QCursor::pos());

    qDeleteAll(actionCollection);
}

void TableViewMenu::showBibTeXEntryMenu(QSharedPointer<Entry> entry)
{
    m_bibtexEntry = entry;

    QMenu menu;
    QList<QAction *> actionCollection; //we throw all temp actions into it and delete them again after execution

    // ###########################################################
    // # add  file open menu
    QMenu openExternal;
    openExternal.setTitle(i18n("open external"));
    openExternal.setIcon(KIcon(QLatin1String("document-open")));
    menu.addMenu(&openExternal);

    bool hasUrl = false;

    QString url = PlainTextValue::text(entry->value(QLatin1String("url")));
    if(!url.isEmpty()) {
        hasUrl = true;
        KIcon icon = KIcon("text-html");

        QAction *a = new QAction(icon, url, this);
        a->setData(QUrl(url));
        connect(a, SIGNAL(triggered(bool)),this, SLOT(openSelected()));
        openExternal.addAction(a);
        actionCollection.append(a);
    }

    // this adds the doi link
    QString doi = PlainTextValue::text(entry->value(QLatin1String("doi")));

    if(!doi.isEmpty()) {
        hasUrl = true;
        if(!doi.startsWith(QLatin1String("http"))) {
            doi = QLatin1String("http://dx.doi.org/") + doi;
        }
        KIcon icon = KIcon("text-html");

        QAction *a = new QAction(icon, doi, this);
        a->setData(QUrl(doi));
        connect(a, SIGNAL(triggered(bool)),this, SLOT(openSelected()));
        openExternal.addAction(a);
        actionCollection.append(a);
    }

    if(!hasUrl) {
        openExternal.setEnabled(false);
    }

    menu.addSeparator();

    // ###########################################################
    // # add import to nepomuk action
    QAction *import = new QAction(KIcon(QLatin1String("document-import")), i18n("Import Entry"), this);
    connect(import, SIGNAL(triggered(bool)),this, SLOT(importSearchResult()));
    menu.addAction(import);
    actionCollection.append(import);

    // ###########################################################
    // # add export menu
    QMenu openExport;
    openExport.setTitle(i18n("Export"));
    openExport.setIcon(KIcon(QLatin1String("document-export")));
    menu.addMenu(&openExport);

    QAction *exportBibTeX = new QAction(KIcon(QLatin1String("document-export")), i18n("Export BibTex to Clipboard"), this);
    connect(exportBibTeX, SIGNAL(triggered(bool)),this, SLOT(exportBibTexReference()));
    openExport.addAction(exportBibTeX);
    actionCollection.append(exportBibTeX);

    QAction *exportCiteKey = new QAction(KIcon(QLatin1String("document-export")), i18n("Export Citekey to Clipboard"), this);
    connect(exportCiteKey, SIGNAL(triggered(bool)),this, SLOT(exportCiteKey()));
    openExport.addAction(exportCiteKey);
    actionCollection.append(exportCiteKey);


    menu.exec(QCursor::pos());

    qDeleteAll(actionCollection);

}

void TableViewMenu::setLibraryManager(LibraryManager *lm)
{
    m_libraryManager = lm;
}

void TableViewMenu::addSelectedToProject()
{
    // get the pimoProject we use to connect the data to
    Nepomuk2::Resource pimoProject;
    QAction *a = static_cast<QAction *>(sender());
    if(a) {
        pimoProject = Nepomuk2::Resource(a->data().toString());
    }

    if(pimoProject.isValid()) {
        Library *l = m_libraryManager->libFromResourceUri(pimoProject.uri());
        l->addResource(m_nepomukResource);
    }
    else {
        kDebug() << "TableViewMenu::addSelectedToProject() | could not find pimo project the data should be connected to";
    }
}
void TableViewMenu::removeSelectedFromProject()
{
    // get the pimoProject we use to remove the data from
    Nepomuk2::Resource pimoProject;
    QAction *a = static_cast<QAction *>(sender());
    if(a) {
        pimoProject = Nepomuk2::Resource(a->data().toString());
    }

    if(pimoProject.isValid()) {
        // find the library that is related to this prject
        Library *l = m_libraryManager->libFromResourceUri(pimoProject.uri());
        l->removeResource(m_nepomukResource);
    }
    else {
        kDebug() << "TableViewMenu::removeSelectedFromProject() | could not find pimo project the data should be removed from";
    }
}

void TableViewMenu::removeSelectedFromSystem()
{
    m_libraryManager->systemLibrary()->deleteResource(m_nepomukResource);
}

void TableViewMenu::openSelected()
{
    QAction *a = static_cast<QAction *>(sender());

    if(a) {
        QDesktopServices::openUrl(a->data().toUrl());
    }
}

void TableViewMenu::importSearchResult()
{
    File f;

    f.append(m_bibtexEntry);

    BibTexToNepomukPipe btnp;
    btnp.pipeExport(f);
}

void TableViewMenu::exportBibTexReference()
{
    File f;
    NepomukToBibTexPipe ntbp;

    if(m_bibtexEntry) {
        f.append(m_bibtexEntry);
    }
    else {
        QList<Nepomuk2::Resource> resourcelist;
        resourcelist.append(m_nepomukResource);
        ntbp.pipeExport(resourcelist);

        f = *ntbp.bibtexFile();
    }

    BibTexToClipboardPipe btcp;
    btcp.setExportType(BibTexToClipboardPipe::Export_SOURCE);
    btcp.pipeExport(f);
}

void TableViewMenu::exportCiteKey()
{
    File f;
    NepomukToBibTexPipe ntbp;

    if(m_bibtexEntry) {
        f.append(m_bibtexEntry);
    }
    else {
        QList<Nepomuk2::Resource> resourcelist;
        resourcelist.append(m_nepomukResource);
        ntbp.pipeExport(resourcelist);

        f = *ntbp.bibtexFile();
    }

    BibTexToClipboardPipe btcp;
    btcp.setExportType(BibTexToClipboardPipe::Export_CITEKEY);
    btcp.pipeExport(f);
}

void TableViewMenu::sendToKileLyX()
{
    File f;
    KileLyxPipe klp;

    if(m_bibtexEntry) {
        f.append(m_bibtexEntry);
        klp.pipeExport(f);
    }
    else {
        QList<Nepomuk2::Resource> resourcelist;
        resourcelist.append(m_nepomukResource);
        klp.pipeExport(resourcelist);
    }
}

void TableViewMenu::openInTab()
{
    emit openResourceInTab(m_nepomukResource, true);
}
