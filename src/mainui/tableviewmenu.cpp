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
#include "mainwindow.h"

#include "nbibio/pipe/bibtextoclipboardpipe.h"
#include "nbibio/pipe/bibtextonepomukpipe.h"
#include "nbibio/pipe/nepomuktobibtexpipe.h"

#include <kbibtex/entry.h>
#include <kbibtex/value.h>

#include "nbib.h"
#include <Nepomuk/Vocabulary/PIMO>
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Vocabulary/NFO>
#include <Nepomuk/Variant>

#include <KDE/KMimeType>
#include <KDE/KIcon>

#include <QtGui/QMenu>
#include <QtGui/QAction>
#include <QtGui/QDesktopServices>

#include <QtCore/QDebug>

TableViewMenu::TableViewMenu(QObject *parent)
    :QObject(parent)
    , m_parent(0)
    , m_bibtexEntry(0)
{
}

void TableViewMenu::showNepomukEntryMenu(Nepomuk::Resource resource)
{
    m_nepomukResource = resource;

    QMenu menu;
    QList<QAction *> actionCollection; //we throw all temp actions into it and delete them again after execution

    // ###########################################################
    // # add  file open menu
    QMenu openExternal;
    openExternal.setTitle(i18n("open external"));
    openExternal.setIcon(KIcon(QLatin1String("document-open")));
    menu.addMenu(&openExternal);

    if(m_nepomukResource.hasType(Nepomuk::Vocabulary::NBIB::Publication()) || m_nepomukResource.hasType(Nepomuk::Vocabulary::NBIB::Reference()) ) {
        Nepomuk::Resource queryResource;
        if(resource.hasType(Nepomuk::Vocabulary::NBIB::Reference())) {
            queryResource = m_nepomukResource.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();
        }
        else {
            queryResource = m_nepomukResource;
        }

        QList<Nepomuk::Resource> fileList = queryResource.property(Nepomuk::Vocabulary::NBIB::isPublicationOf()).toResourceList();

        bool hasDataObjects = false;
        bool hasDOI = false;
        // this adds all  DataObject links
        if(!fileList.isEmpty()) {
            hasDataObjects = true;
            foreach(const Nepomuk::Resource &r, fileList) {
                KUrl file = r.property(Nepomuk::Vocabulary::NIE::url()).toUrl();
                QString name;
                KIcon icon(KMimeType::iconNameForUrl(file));

                if(file.isLocalFile()) {
                    name = file.fileName();
                }
                else {
                    if(r.resourceType() == Nepomuk::Vocabulary::NFO::RemoteDataObject()) {
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
        QString doi = queryResource.property(Nepomuk::Vocabulary::NBIB::doi()).toString();

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
        KUrl file = m_nepomukResource.property(Nepomuk::Vocabulary::NIE::url()).toUrl();
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

    menu.addSeparator();

    // ###########################################################
    // # add to project menu
    QMenu addToProject;
    addToProject.setTitle(i18n("Add to Project"));
    addToProject.setIcon(KIcon(QLatin1String("list-add")));
    menu.addMenu(&addToProject);

    QList<Library*> openLibList = m_parent->openLibraries();

    if(openLibList.size() > 0) {
        foreach(Library *l, openLibList) {
            QAction *a = new QAction(KIcon(QLatin1String("conquirere")), l->name(), this);
            a->setData(l->pimoLibrary().resourceUri());
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
    removeFromProject.setIcon(KIcon(QLatin1String("list-remove")));
    menu.addMenu(&removeFromProject);

    // fill al list of all project this file is in
    QList<Nepomuk::Resource> projectList = m_nepomukResource.property(Nepomuk::Vocabulary::PIMO::isRelated()).toResourceList();

    if(projectList.isEmpty()) {
        removeFromProject.setEnabled(false);
    }
    else {
        removeFromProject.setEnabled(true);

        foreach(const Nepomuk::Resource &project, projectList) {
            QAction *a = new QAction(KIcon(QLatin1String("conquirere")), project.property(Nepomuk::Vocabulary::NIE::title()).toString(), this);
            a->setData(project.resourceUri());
            actionCollection.append(a);
            connect(a, SIGNAL(triggered(bool)),this, SLOT(removeSelectedFromProject()));
            removeFromProject.addAction(a);
        }
    }


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

void TableViewMenu::setMainWindow(MainWindow *mw)
{
        m_parent = mw;
}

void TableViewMenu::addSelectedToProject()
{
    // get the pimoProject we use to connect the data to
    Nepomuk::Resource pimoProject;
    QAction *a = static_cast<QAction *>(sender());
    if(a) {
        pimoProject = Nepomuk::Resource(a->data().toString());
    }

    if(pimoProject.isValid()) {
        m_nepomukResource.addProperty(Nepomuk::Vocabulary::PIMO::isRelated(), pimoProject);
        pimoProject.addProperty(Nepomuk::Vocabulary::PIMO::isRelated(), m_nepomukResource);

        // small special case, if the resource was a reference add also the publication to the project
        if(m_nepomukResource.hasType(Nepomuk::Vocabulary::NBIB::Reference())) {
            Nepomuk::Resource pub = m_nepomukResource.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();
            pub.addProperty(Nepomuk::Vocabulary::PIMO::isRelated(), pimoProject);
            pimoProject.addProperty(Nepomuk::Vocabulary::PIMO::isRelated(), pub);
        }
    }
    else {
        qDebug() << "TableViewMenu::addSelectedToProject() | could not find pimo project the data should be connected to";
    }
}
void TableViewMenu::removeSelectedFromProject()
{
    // get the pimoProject we use to remove the data from
    Nepomuk::Resource pimoProject;
    QAction *a = static_cast<QAction *>(sender());
    if(a) {
        pimoProject = Nepomuk::Resource(a->data().toString());
    }

    if(pimoProject.isValid()) {
        m_nepomukResource.removeProperty(Nepomuk::Vocabulary::PIMO::isRelated(), pimoProject);
        pimoProject.removeProperty(Nepomuk::Vocabulary::PIMO::isRelated(), m_nepomukResource);

        QList<Nepomuk::Resource> refList = m_nepomukResource.property(Nepomuk::Vocabulary::NBIB::reference()).toResourceList();

        foreach(Nepomuk::Resource r, refList) { // krazy:exclude=foreach
            r.removeProperty(Nepomuk::Vocabulary::PIMO::isRelated(), pimoProject);
            pimoProject.removeProperty(Nepomuk::Vocabulary::PIMO::isRelated(), r);
        }

        //TODO remove also all other recources which are not needed anymore from the project
    }
    else {
        qDebug() << "TableViewMenu::removeSelectedFromProject() | could not find pimo project the data should be removed from";
    }
}

void TableViewMenu::removeSelectedFromSystem()
{
    //get all connected references
    QList<Nepomuk::Resource> refList = m_nepomukResource.property(Nepomuk::Vocabulary::NBIB::reference()).toResourceList();

    foreach(Nepomuk::Resource r, refList) { // krazy:exclude=foreach
        r.remove();
    }

    // remove resource
    m_nepomukResource.remove();
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
        QList<Nepomuk::Resource> resourcelist;
        resourcelist.append(m_nepomukResource);
        ntbp.pipeExport(resourcelist);

        f = ntbp.bibtexFile();
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
        QList<Nepomuk::Resource> resourcelist;
        resourcelist.append(m_nepomukResource);
        ntbp.pipeExport(resourcelist);

        f = ntbp.bibtexFile();
    }

    BibTexToClipboardPipe btcp;
    btcp.setExportType(BibTexToClipboardPipe::Export_CITEKEY);
    btcp.pipeExport(f);
}
