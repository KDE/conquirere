/*
 * Copyright 2011 Jörg Ehrichs <joerg.ehichs@gmx.de>
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

#include "librarywidget.h"
#include "ui_librarywidget.h"

#include "core/librarymanager.h"
#include "mainui/settings/projectsettingsdialog.h"
#include "mainui/docklets/searchwidget.h"

#include "core/library.h"
#include "core/projectsettings.h"
#include "core/models/nepomukmodel.h"

#include "qlibrarytreewidgetitem.h"
#include "projecttreedelegate.h"

#include "config/conquirere.h"

#include <KDE/KStandardDirs>

#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QAction>
#include <QtGui/QMenu>
#include <QtCore/QVariant>

LibraryWidget::LibraryWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LibraryWidget)
{
    ui->setupUi(this);

    setupUi();
}

LibraryWidget::~LibraryWidget()
{
    delete ui;
}

void LibraryWidget::setLibraryManager(LibraryManager *lm)
{
    m_libraryManager = lm;

    connect(lm, SIGNAL(libraryAdded(Library*)), this, SLOT(addLibrary(Library*)));
    connect(lm, SIGNAL(libraryRemoved(QUrl)), this, SLOT(closeLibrary(QUrl)));
}

SearchResultModel* LibraryWidget::searchResultModel()
{
    return ui->searchWidget->searchResultModel();
}

void LibraryWidget::addLibrary(Library *p)
{
    QLibraryTreeWidgetItem *root = new QLibraryTreeWidgetItem();
    root->setData(0,BibGlobals::Role_LibraryType,p->libraryType());
    root->setData(0,BibGlobals::Role_ResourceType,BibGlobals::Resource_Library);
    root->setData(0,BibGlobals::Role_ProjectThing, p->settings()->projectThing().uri());
    root->setIcon(0, KIcon(QLatin1String("document-multiple")));
    ui->treeWidget->addTopLevelItem(root);

    root->setText(0, p->settings()->name());
    root->setExpanded(true);

    setupLibraryTree(root, p);

    connect(p->settings(), SIGNAL(projectDetailsChanged(Library*)), this, SLOT(renameLibrary(Library*)));

    if(p->libraryType() == BibGlobals::Library_System)
        m_systemRoot = root;
}

void LibraryWidget::closeLibrary(const QUrl &projectThingUrl)
{
    int toplevelItems = ui->treeWidget->topLevelItemCount();

    for(int i=1; i <= toplevelItems; i++) {
        QTreeWidgetItem *root = ui->treeWidget->topLevelItem(i);

        QUrl openProject = (root->data(0,BibGlobals::Role_ProjectThing).toUrl());
        if(openProject == projectThingUrl) {
            QTreeWidgetItem *closedItem = ui->treeWidget->takeTopLevelItem(i);
            delete closedItem;
            break;
        }
    }
}

void LibraryWidget::renameLibrary(Library *p)
{
    int toplevelItems = ui->treeWidget->topLevelItemCount();

    for(int i=1; i <= toplevelItems; i++) {
        QTreeWidgetItem *root = ui->treeWidget->topLevelItem(i);

        QUrl openProject = (root->data(0,BibGlobals::Role_ProjectThing).toUrl());
        if(openProject == p->settings()->projectThing().uri()) {
            root->setText(0,p->settings()->name());
            break;
        }
    }
}

void LibraryWidget::selectionchanged()
{
    BibGlobals::ResourceSelection rs = BibGlobals::ResourceSelection(ui->treeWidget->currentItem()->data(0,BibGlobals::Role_ResourceType).toInt());

    if(rs == BibGlobals::Resource_SearchResults) {
        emit showSearchResults();
    }
    else {
        BibGlobals::BibEntryType filter = BibGlobals::BibEntryType(ui->treeWidget->currentItem()->data(0,BibGlobals::Role_ResourceFilter).toInt());

        // get the right Library for the resourceuri saved in the tree
        QUrl selectedProjectThing = ui->treeWidget->currentItem()->data(0,BibGlobals::Role_ProjectThing).toUrl();

        Library *selectedLibrary = 0;
        if(!selectedProjectThing.isValid()) {
            selectedLibrary = m_libraryManager->systemLibrary();
        }
        else
            selectedLibrary = m_libraryManager->libFromResourceUri(selectedProjectThing);

        emit newSelection(rs, filter, selectedLibrary);
    }
}

void LibraryWidget::listContextMenu(const QPoint & pos)
{
    QTreeWidgetItem *seletedItem = 0;
    seletedItem = ui->treeWidget->itemAt(pos);

    if(!seletedItem || !seletedItem->data(0, BibGlobals::Role_LibraryType).isValid() ) {
        return;
    }

    BibGlobals::LibraryType type = (BibGlobals::LibraryType)seletedItem->data(0,BibGlobals::Role_LibraryType).toInt();

    QMenu menu;
    QList<QAction *> actionCollection; //we throw all temp actions into it and delete them again after execution

    QAction *importFromFile = new QAction(KIcon(QLatin1String("document-import")), i18n("Import from File"), this);
    importFromFile->setData( seletedItem->data(0,BibGlobals::Role_ProjectThing));
    actionCollection.append(importFromFile);
    connect(importFromFile, SIGNAL(triggered(bool)),this, SLOT(importFromFile()));
    menu.addAction(importFromFile);

    QAction *exportToFile = new QAction(KIcon(QLatin1String("document-export")), i18n("Export to File"), this);
    exportToFile->setData( seletedItem->data(0,BibGlobals::Role_ProjectThing));
    actionCollection.append(exportToFile);
    connect(exportToFile, SIGNAL(triggered(bool)),this, SLOT(exportToFile()));
    menu.addAction(exportToFile);

    menu.addSeparator();

    QAction *settings = new QAction(KIcon(QLatin1String("preferences-other")), i18n("Project Settings"), this);
    settings->setData( seletedItem->data(0,BibGlobals::Role_ProjectThing));
    actionCollection.append(settings);
    connect(settings, SIGNAL(triggered(bool)),this, SLOT(openSettings()));
    menu.addAction(settings);

    QAction *closeProject = new QAction(KIcon(QLatin1String("document-close")), i18n("Project Close"), this);
    closeProject->setData( seletedItem->data(0,BibGlobals::Role_ProjectThing));
    actionCollection.append(closeProject);
    connect(closeProject, SIGNAL(triggered(bool)),this, SLOT(closeProject()));
    menu.addAction(closeProject);

    menu.addSeparator();

    QAction *deleteProject = new QAction(KIcon(QLatin1String("document-close")), i18n("Project Delete"), this);
    deleteProject->setData( seletedItem->data(0,BibGlobals::Role_ProjectThing));
    actionCollection.append(deleteProject);
    connect(deleteProject, SIGNAL(triggered(bool)),this, SLOT(deleteProject()));
    menu.addAction(deleteProject);

    if(type == BibGlobals::Library_System) {
        closeProject->setEnabled(false);
        deleteProject->setEnabled(false);
    }

    menu.exec(QCursor::pos());

    qDeleteAll(actionCollection);
}

void LibraryWidget::exportToFile()
{
    Library *selectedLibrary = libForAction();

    if(!selectedLibrary) {
        return;
    }

    m_libraryManager->doExportFile(selectedLibrary);
}

void LibraryWidget::importFromFile()
{
    Library *selectedLibrary = libForAction();

    if(!selectedLibrary) {
        return;
    }

    m_libraryManager->doImportFile(selectedLibrary);
}

void LibraryWidget::openSettings()
{
    Library *selectedLibrary = libForAction();

    if(!selectedLibrary) {
        return;
    }
    m_libraryManager->doOpenSettings(selectedLibrary);
}

void LibraryWidget::closeProject()
{
    ui->treeWidget->setCurrentItem(m_systemRoot);

    Library *selectedLibrary = libForAction();

    m_libraryManager->closeLibrary(selectedLibrary);
}

void LibraryWidget::deleteProject()
{
    ui->treeWidget->setCurrentItem(m_systemRoot);

    Library *selectedLibrary = libForAction();

    m_libraryManager->deleteLibrary(selectedLibrary);
}

Library *LibraryWidget::libForAction()
{
    QAction *a = qobject_cast<QAction *>(sender());

    if(!a)
        return 0;

    QUrl selectedProjectThing = a->data().toUrl();

    Library *selectedLibrary = 0;
    if(!selectedProjectThing.isValid())
        selectedLibrary = m_libraryManager->systemLibrary();
    else
        selectedLibrary = m_libraryManager->libFromResourceUri(selectedProjectThing);

    return selectedLibrary;
}

void LibraryWidget::setupUi()
{
    ui->treeWidget->setItemDelegate(new ProjectTreeDelegate());
    connect(ui->treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(selectionchanged()));

    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(listContextMenu(QPoint)));

    QLibraryTreeWidgetItem *root = new QLibraryTreeWidgetItem();
    root->setData(0,BibGlobals::Role_ResourceType,BibGlobals::Resource_SearchResults);
    root->setIcon(0, KIcon(QLatin1String("system-search")));
    ui->treeWidget->addTopLevelItem(root);

    root->setText(0, i18n("Search Results"));

    connect(ui->searchWidget, SIGNAL(newSearchStarted()), this, SIGNAL(showSearchResults()));
}

void LibraryWidget::setupLibraryTree(QLibraryTreeWidgetItem *root, Library *p)
{
    QLibraryTreeWidgetItem *twi1 = new QLibraryTreeWidgetItem();
    twi1->setText(0, i18n("Documents"));
    twi1->setData(0,BibGlobals::Role_ProjectThing, p->settings()->projectThing().uri());
    twi1->setData(0,BibGlobals::Role_ResourceType,BibGlobals::Resource_Document);
    twi1->setData(0,BibGlobals::Role_LibraryType,p->libraryType());
    twi1->setIcon(0, KIcon(QLatin1String("document-multiple")));
    twi1->startQueryFetch();
    root->addChild(twi1);
    connectModelSignals(twi1, p, BibGlobals::Resource_Document);

    QLibraryTreeWidgetItem *twi5 = new QLibraryTreeWidgetItem();
    twi5->setText(0, i18n("Series"));
    twi5->setData(0,BibGlobals::Role_ResourceType,BibGlobals::Resource_Series);
    twi5->setData(0,BibGlobals::Role_ResourceFilter,BibGlobals::Max_SeriesTypes);
    twi5->setData(0,BibGlobals::Role_LibraryType,p->libraryType());
    twi5->setData(0,BibGlobals::Role_ProjectThing, p->settings()->projectThing().uri());
    twi5->setIcon(0, KIcon(BibGlobals::SeriesTypeIcon((BibGlobals::SeriesType)0)));
    twi5->startQueryFetch();
    root->addChild(twi5);
    connectModelSignals(twi5, p, BibGlobals::Resource_Series);

    for(int i=0; i < BibGlobals::Max_SeriesTypes; i++) {
        if(ConqSettings::hiddenNbibSeries().contains(i)) {
            continue;
        }

        QLibraryTreeWidgetItem *refSub = new QLibraryTreeWidgetItem();
        refSub->setText(0, BibGlobals::SeriesTypeTranslation((BibGlobals::SeriesType)i));
        refSub->setData(0,BibGlobals::Role_ResourceType,BibGlobals::Resource_Series);
        refSub->setData(0,BibGlobals::Role_ResourceFilter,BibGlobals::SeriesType(i));
        refSub->setData(0,BibGlobals::Role_LibraryType,p->libraryType());
        refSub->setData(0,BibGlobals::Role_ProjectThing, p->settings()->projectThing().uri());
        refSub->setIcon(0, KIcon(BibGlobals::SeriesTypeIcon((BibGlobals::SeriesType)i)));
        twi5->addChild(refSub);
        connectModelSignals(refSub, p, BibGlobals::Resource_Series);
    }

    QLibraryTreeWidgetItem *twi1a = new QLibraryTreeWidgetItem();
    twi1a->setText(0, i18n("Publications"));
    twi1a->setData(0,BibGlobals::Role_ResourceType,BibGlobals::Resource_Publication);
    twi1a->setData(0,BibGlobals::Role_LibraryType,p->libraryType());
    twi1a->setData(0,BibGlobals::Role_ProjectThing, p->settings()->projectThing().uri());
    twi1a->setData(0,BibGlobals::Role_ResourceFilter,BibGlobals::Max_BibTypes);
    twi1a->setIcon(0, KIcon(QLatin1String("document-open-remote")));
    twi1a->startQueryFetch();
    root->addChild(twi1a);
    connectModelSignals(twi1a, p, BibGlobals::Resource_Publication);

    for(int i=0; i < BibGlobals::Max_BibTypes; i++) {
        if(ConqSettings::hiddenNbibPublications().contains(i)) {
            continue;
        }

        QLibraryTreeWidgetItem *pubSub = new QLibraryTreeWidgetItem();
        pubSub->setText(0, BibGlobals::BibEntryTypeTranslation((BibGlobals::BibEntryType)i));
        pubSub->setData(0,BibGlobals::Role_ResourceType,BibGlobals::Resource_Publication);
        pubSub->setData(0,BibGlobals::Role_ResourceFilter,BibGlobals::BibEntryType(i));
        pubSub->setData(0,BibGlobals::Role_LibraryType,p->libraryType());
        pubSub->setData(0,BibGlobals::Role_ProjectThing, p->settings()->projectThing().uri());
        pubSub->setIcon(0, KIcon(BibGlobals::BibEntryTypeIcon((BibGlobals::BibEntryType)i)));
        twi1a->addChild(pubSub);
        connectModelSignals(pubSub, p, BibGlobals::Resource_Publication);
    }

    QLibraryTreeWidgetItem *twi4 = new QLibraryTreeWidgetItem();
    twi4->setText(0, i18n("References"));
    twi4->setData(0,BibGlobals::Role_ResourceType,BibGlobals::Resource_Reference);
    twi4->setData(0,BibGlobals::Role_ResourceFilter,BibGlobals::Max_BibTypes);
    twi4->setData(0,BibGlobals::Role_LibraryType,p->libraryType());
    twi4->setData(0,BibGlobals::Role_ProjectThing, p->settings()->projectThing().uri());
    twi4->setIcon(0, KIcon(QLatin1String("user-identity")));
    twi4->startQueryFetch();
    root->addChild(twi4);
    connectModelSignals(twi4, p, BibGlobals::Resource_Reference);

    for(int i=0; i < BibGlobals::Max_BibTypes; i++) {
        if(ConqSettings::hiddenNbibPublications().contains(i)) {
            continue;
        }

        QLibraryTreeWidgetItem *refSub = new QLibraryTreeWidgetItem();
        refSub->setText(0, BibGlobals::BibEntryTypeTranslation((BibGlobals::BibEntryType)i));
        refSub->setData(0,BibGlobals::Role_ResourceType,BibGlobals::Resource_Reference);
        refSub->setData(0,BibGlobals::Role_ResourceFilter,BibGlobals::BibEntryType(i));
        refSub->setData(0,BibGlobals::Role_LibraryType,p->libraryType());
        refSub->setData(0,BibGlobals::Role_ProjectThing, p->settings()->projectThing().uri());
        refSub->setIcon(0, KIcon(BibGlobals::BibEntryTypeIcon((BibGlobals::BibEntryType)i)));
        twi4->addChild(refSub);
        connectModelSignals(refSub, p, BibGlobals::Resource_Publication);
    }

    QLibraryTreeWidgetItem *twi8 = new QLibraryTreeWidgetItem();
    twi8->setText(0, i18n("Events"));
    twi8->setData(0,BibGlobals::Role_ResourceType,BibGlobals::Resource_Event);
    twi8->setData(0,BibGlobals::Role_LibraryType,p->libraryType());
    twi8->setData(0,BibGlobals::Role_ProjectThing, p->settings()->projectThing().uri());
    twi8->setIcon(0, KIcon(QLatin1String("view-pim-calendar")));
    twi8->startQueryFetch();
    root->addChild(twi8);
    connectModelSignals(twi8, p, BibGlobals::Resource_Event);

    QLibraryTreeWidgetItem *twi6 = new QLibraryTreeWidgetItem();
    twi6->setText(0, i18n("Notes"));
    twi6->setData(0,BibGlobals::Role_ResourceType,BibGlobals::Resource_Note);
    twi6->setData(0,BibGlobals::Role_LibraryType,p->libraryType());
    twi6->setData(0,BibGlobals::Role_ProjectThing, p->settings()->projectThing().uri());
    twi6->setIcon(0, KIcon(QLatin1String("knotes")));
    twi6->startQueryFetch();
    root->addChild(twi6);
    connectModelSignals(twi6, p, BibGlobals::Resource_Note);

    /*
    // Ignored for now, the nfo::Website is not working anyway or implemented someweher currently
    QLibraryTreeWidgetItem *twi3 = new QLibraryTreeWidgetItem(this);
    twi3->setText(0, i18n("Bookmarks"));
    twi3->setData(0,Role_ResourceType,Resource_Website);
    twi3->setData(0,Role_Library,p->libraryType());
    twi3->setData(0,Role_ProjectThing, p->settings()->projectThing().uri());
    twi3->setIcon(0, KIcon(QLatin1String("view-web-browser-dom-tree")));
    root->addChild(twi3);
    connectModelSignals(twi3, p, Resource_Website);
    */
/*
    if(root->data(0,Role_LibraryType).toInt() == Library_Project) {
        QLibraryTreeWidgetItem *twi2 = new QLibraryTreeWidgetItem();
        twi2->setText(0, i18n("Mails"));
        twi2->setData(0,Role_ResourceType,Resource_Mail);
        twi2->setData(0,Role_LibraryType,p->libraryType());
        twi2->setData(0,Role_ProjectThing, p->settings()->projectThing().uri());
        twi2->setIcon(0, KIcon(QLatin1String("mail-flag")));
        twi2->startQueryFetch();
        root->addChild(twi2);
        connectModelSignals(twi2, p, Resource_Mail);
    }
    */
}

void LibraryWidget::connectModelSignals(QLibraryTreeWidgetItem *root, Library *p, BibGlobals::ResourceSelection resourceType)
{
    QSortFilterProxyModel *viewModel = p->viewModel(resourceType);
    if(!viewModel)
        return;

    NepomukModel *nm = qobject_cast<NepomukModel *>(viewModel->sourceModel());
    if(!nm)
        return;

    connect(nm, SIGNAL(queryStarted()), root, SLOT(startQueryFetch()));
    connect(nm, SIGNAL(queryFinished()), root, SLOT(stopQueryFetch()));
    connect(nm, SIGNAL(dataSizeChaged(int)), root, SLOT(updateItemCount(int)));
}
