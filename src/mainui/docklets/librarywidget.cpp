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

#include "core/library.h"
#include "core/projectsettings.h"
#include "core/models/nepomukmodel.h"

#include "qlibrarytreewidgetitem.h"
#include "projecttreedelegate.h"
#include "mainui/settings/projectsettingsdialog.h"

#include <KDE/KStandardDirs>

#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QAction>
#include <QtGui/QMenu>
#include <QtCore/QVariant>

#include <QtCore/QDebug>



#include <KPageDialog>
#include <KPageWidgetItem>
#include "mainui/settings/projectgeneralsettings.h"
#include "mainui/settings/projectsyncsettings.h"

LibraryWidget::LibraryWidget(QWidget *parent)
    : QDockWidget(parent)
    , ui(new Ui::LibraryWidget)
{
    ui->setupUi(this);
    ui->treeWidget->setItemDelegate(new ProjectTreeDelegate());

    connect(ui->treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(selectionchanged()));

    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(listContextMenu(QPoint)));

    setupUi();
}

LibraryWidget::~LibraryWidget()
{
    delete ui;
    //qDeleteAll(m_openLibraries);
    //qDeleteAll(m_items);
}

void LibraryWidget::addLibrary(Library *p)
{
    QLibraryTreeWidgetItem *root;
    root = new QLibraryTreeWidgetItem();
    root->setData(0,Role_LibraryType,p->libraryType());
    root->setData(0,Role_ResourceType,Resource_Library);
    root->setData(0,Role_ProjectName,p->settings()->name());
    root->setData(0,Role_ProjectThing, p->settings()->projectThing().uri());
    root->setIcon(0, KIcon(QLatin1String("document-multiple")));
    ui->treeWidget->addTopLevelItem(root);
    m_items.append(root);

    root->setText(0, p->settings()->name());
    root->setExpanded(true);

    setupLibraryTree(root, p);

    m_openLibraries.append(p);

    connect(this, SIGNAL(doCloseLibrary(Library*)), p, SIGNAL(closeLibrary(Library*)));
    connect(p->settings(), SIGNAL(projectDetailsChanged(Library*)), this, SLOT(renameLibrary(Library*)));
}

void LibraryWidget::closeLibrary(Library *p)
{
    int toplevelItems = ui->treeWidget->topLevelItemCount();

    for(int i=1; i <= toplevelItems; i++) {
        QTreeWidgetItem *root = ui->treeWidget->topLevelItem(i);

        if(!root)
            continue;

        QString openProject = (root->data(0,Role_ProjectThing).toString());
        if(openProject == p->settings()->projectThing().uri()) {
            ui->treeWidget->takeTopLevelItem(i);
            break;
        }
    }

    m_openLibraries.removeAll(p);
}

void LibraryWidget::renameLibrary(Library *p)
{
    int toplevelItems = ui->treeWidget->topLevelItemCount();

    for(int i=1; i <= toplevelItems; i++) {
        QTreeWidgetItem *root = ui->treeWidget->topLevelItem(i);

        QString openProject = (root->data(0,Role_ProjectThing).toString());
        if(openProject == p->settings()->projectThing().uri()) {
            root->setText(0,p->settings()->name());
            break;
        }
    }

    m_openLibraries.removeAll(p);
}

void LibraryWidget::selectionchanged()
{
    ResourceSelection rs = ResourceSelection(ui->treeWidget->currentItem()->data(0,Role_ResourceType).toInt());

    if(rs == Resource_SearchResults) {
        emit showSearchResults();
    }
    else {
        BibEntryType filter = BibEntryType(ui->treeWidget->currentItem()->data(0,Role_ResourceFilter).toInt());

        Library *selectedLibrary = 0;
        foreach(Library *p, m_openLibraries) {
            if(p->settings()->name() == ui->treeWidget->currentItem()->data(0,Role_ProjectName).toString()) {
                selectedLibrary = p;
                break;
            }
        }

        emit newSelection(rs, filter, selectedLibrary);
    }
}

void LibraryWidget::listContextMenu(const QPoint & pos)
{
    QTreeWidgetItem *seletedItem = 0;
    seletedItem = ui->treeWidget->itemAt(pos);

    if(!seletedItem || !seletedItem->data(0, Role_LibraryType).isValid() ) {
        return;
    }

    LibraryType type = (LibraryType)seletedItem->data(0,Role_LibraryType).toInt();

    QMenu menu;
    QList<QAction *> actionCollection; //we throw all temp actions into it and delete them again after execution

    QAction *exportToFile = new QAction(KIcon(QLatin1String("document-export")), i18n("Export to File"), this);
    exportToFile->setData( seletedItem->data(0,Role_ProjectName).toString());
    actionCollection.append(exportToFile);
    connect(exportToFile, SIGNAL(triggered(bool)),this, SLOT(exportToFile()));
    menu.addAction(exportToFile);

    QAction *importFromFile = new QAction(KIcon(QLatin1String("document-import")), i18n("Import from File"), this);
    importFromFile->setData( seletedItem->data(0,Role_ProjectName).toString());
    actionCollection.append(importFromFile);
    connect(importFromFile, SIGNAL(triggered(bool)),this, SLOT(importFromFile()));
    menu.addAction(importFromFile);

    menu.addSeparator();

    QAction *settings = new QAction(KIcon(QLatin1String("preferences-other")), i18n("Project Settings"), this);
    settings->setData( seletedItem->data(0,Role_ProjectName).toString());
    actionCollection.append(settings);
    connect(settings, SIGNAL(triggered(bool)),this, SLOT(openSettings()));
    menu.addAction(settings);

    QAction *closeProject = new QAction(KIcon(QLatin1String("document-close")), i18n("Project Close"), this);
    closeProject->setData( seletedItem->data(0,Role_ProjectName).toString());
    actionCollection.append(closeProject);
    connect(closeProject, SIGNAL(triggered(bool)),this, SLOT(closeProject()));
    menu.addAction(closeProject);

    menu.addSeparator();

    QAction *deleteProject = new QAction(KIcon(QLatin1String("document-close")), i18n("Project Delete"), this);
    deleteProject->setData( seletedItem->data(0,Role_ProjectName).toString());
    actionCollection.append(deleteProject);
    connect(deleteProject, SIGNAL(triggered(bool)),this, SLOT(deleteProject()));
    menu.addAction(deleteProject);

    if(type == Library_System) {
        closeProject->setEnabled(false);
        deleteProject->setEnabled(false);
    }

    menu.exec(QCursor::pos());

    qDeleteAll(actionCollection);
}

void LibraryWidget::exportToFile()
{

}

void LibraryWidget::importFromFile()
{

}

void LibraryWidget::openSettings()
{
    Library *selectedLibrary = libForAction();

    if(!selectedLibrary) {
        return;
    }

    if(selectedLibrary->libraryType() == Library_Project) {
        ProjectSettingsDialog settingsDialog;
        settingsDialog.setProjectSettings(selectedLibrary->settings());

        settingsDialog.exec();
    }
}

void LibraryWidget::closeProject()
{
    Library *selectedLibrary = libForAction();

    if(selectedLibrary) {
        emit doCloseLibrary(selectedLibrary);
    }
}

void LibraryWidget::deleteProject()
{
    Library *selectedLibrary = libForAction();

    if(selectedLibrary) {
        emit doCloseLibrary(selectedLibrary);
    }
}

Library *LibraryWidget::libForAction()
{
    QAction *a = qobject_cast<QAction *>(sender());

    if(!a)
        return 0;

    Library *selectedLibrary = 0;
    QString projectName = a->data().toString();
    foreach(Library *p, m_openLibraries) {
        if(p->settings()->name() == projectName) {
            selectedLibrary = p;
            break;
        }
    }

    return selectedLibrary;
}

void LibraryWidget::setupUi()
{
    QLibraryTreeWidgetItem *root;
    root = new QLibraryTreeWidgetItem();
    root->setData(0,Role_ResourceType,Resource_SearchResults);
    root->setIcon(0, KIcon(QLatin1String("system-search")));
    ui->treeWidget->addTopLevelItem(root);
    m_items.append(root);

    root->setText(0, i18n("Search Results"));
}

void LibraryWidget::setupLibraryTree(QLibraryTreeWidgetItem *root, Library *p)
{
    QString projectName = p->settings()->name();
    QLibraryTreeWidgetItem *twi1 = new QLibraryTreeWidgetItem();
    twi1->setText(0, i18n("Documents"));
    twi1->setData(0,Role_ProjectName, projectName);
    twi1->setData(0,Role_ProjectThing, p->settings()->projectThing().uri());
    twi1->setData(0,Role_ResourceType,Resource_Document);
    twi1->setData(0,Role_LibraryType,p->libraryType());
    twi1->setIcon(0, KIcon(QLatin1String("document-multiple")));
    root->addChild(twi1);
    connectModelSignals(twi1, p, Resource_Document);
    m_items.append(twi1);

    QLibraryTreeWidgetItem *twi5 = new QLibraryTreeWidgetItem();
    twi5->setText(0, i18n("Series"));
    twi5->setData(0,Role_ResourceType,Resource_Series);
    twi5->setData(0,Role_ResourceFilter,Max_SeriesTypes);
    twi5->setData(0,Role_LibraryType,p->libraryType());
    twi5->setData(0,Role_ProjectName, projectName);
    twi5->setData(0,Role_ProjectThing, p->settings()->projectThing().uri());
    twi5->setIcon(0, KIcon(SeriesTypeIcon.at(0)));
    root->addChild(twi5);
    connectModelSignals(twi5, p, Resource_Series);
    m_items.append(twi5);

    for(int i=0; i < Max_SeriesTypes; i++) {
        QLibraryTreeWidgetItem *refSub = new QLibraryTreeWidgetItem();
        refSub->setText(0, SeriesTypeTranslation.at(i));
        refSub->setData(0,Role_ResourceType,Resource_Series);
        refSub->setData(0,Role_ResourceFilter,SeriesType(i));
        refSub->setData(0,Role_LibraryType,p->libraryType());
        refSub->setData(0,Role_ProjectName, projectName);
        refSub->setData(0,Role_ProjectThing, p->settings()->projectThing().uri());
        refSub->setIcon(0, KIcon(SeriesTypeIcon.at(i)));
        twi5->addChild(refSub);
        connectModelSignals(refSub, p, Resource_Series);
        m_items.append(refSub);
    }

    QLibraryTreeWidgetItem *twi1a = new QLibraryTreeWidgetItem();
    twi1a->setText(0, i18n("Publications"));
    twi1a->setData(0,Role_ResourceType,Resource_Publication);
    twi1a->setData(0,Role_LibraryType,p->libraryType());
    twi1a->setData(0,Role_ProjectName, projectName);
    twi1a->setData(0,Role_ProjectThing, p->settings()->projectThing().uri());
    twi1a->setData(0,Role_ResourceFilter,Max_BibTypes);
    twi1a->setIcon(0, KIcon(QLatin1String("document-open-remote")));
    root->addChild(twi1a);
    connectModelSignals(twi1a, p, Resource_Publication);
    m_items.append(twi1a);

    for(int i=0; i < Max_BibTypes; i++) {
        QLibraryTreeWidgetItem *pubSub = new QLibraryTreeWidgetItem();
        pubSub->setText(0, BibEntryTypeTranslation.at(i));
        pubSub->setData(0,Role_ResourceType,Resource_Publication);
        pubSub->setData(0,Role_ResourceFilter,BibEntryType(i));
        pubSub->setData(0,Role_LibraryType,p->libraryType());
        pubSub->setData(0,Role_ProjectName, projectName);
        pubSub->setData(0,Role_ProjectThing, p->settings()->projectThing().uri());
        pubSub->setIcon(0, KIcon(BibEntryTypeIcon.at(i)));
        twi1a->addChild(pubSub);
        connectModelSignals(pubSub, p, Resource_Publication);
        m_items.append(pubSub);
    }

    QLibraryTreeWidgetItem *twi4 = new QLibraryTreeWidgetItem();
    twi4->setText(0, i18n("References"));
    twi4->setData(0,Role_ResourceType,Resource_Reference);
    twi4->setData(0,Role_ResourceFilter,Max_BibTypes);
    twi4->setData(0,Role_LibraryType,p->libraryType());
    twi4->setData(0,Role_ProjectName, projectName);
    twi4->setData(0,Role_ProjectThing, p->settings()->projectThing().uri());
    twi4->setIcon(0, KIcon(QLatin1String("user-identity")));
    root->addChild(twi4);
    connectModelSignals(twi4, p, Resource_Reference);
    m_items.append(twi4);

    for(int i=0; i < Max_BibTypes; i++) {
        QLibraryTreeWidgetItem *refSub = new QLibraryTreeWidgetItem();
        refSub->setText(0, BibEntryTypeTranslation.at(i));
        refSub->setData(0,Role_ResourceType,Resource_Reference);
        refSub->setData(0,Role_ResourceFilter,BibEntryType(i));
        refSub->setData(0,Role_LibraryType,p->libraryType());
        refSub->setData(0,Role_ProjectName, projectName);
        refSub->setData(0,Role_ProjectThing, p->settings()->projectThing().uri());
        refSub->setIcon(0, KIcon(BibEntryTypeIcon.at(i)));
        twi4->addChild(refSub);
        connectModelSignals(refSub, p, Resource_Publication);
        m_items.append(refSub);
    }

    QLibraryTreeWidgetItem *twi8 = new QLibraryTreeWidgetItem();
    twi8->setText(0, i18n("Events"));
    twi8->setData(0,Role_ResourceType,Resource_Event);
    twi8->setData(0,Role_LibraryType,p->libraryType());
    twi8->setData(0,Role_ProjectName, projectName);
    twi8->setData(0,Role_ProjectThing, p->settings()->projectThing().uri());
    twi8->setIcon(0, KIcon(QLatin1String("view-pim-calendar")));
    root->addChild(twi8);
    connectModelSignals(twi8, p, Resource_Event);
    m_items.append(twi8);

    QLibraryTreeWidgetItem *twi6 = new QLibraryTreeWidgetItem();
    twi6->setText(0, i18n("Notes"));
    twi6->setData(0,Role_ResourceType,Resource_Note);
    twi6->setData(0,Role_LibraryType,p->libraryType());
    twi6->setData(0,Role_ProjectName, projectName);
    twi6->setData(0,Role_ProjectThing, p->settings()->projectThing().uri());
    twi6->setIcon(0, KIcon(QLatin1String("knotes")));
    root->addChild(twi6);
    connectModelSignals(twi6, p, Resource_Note);
    m_items.append(twi6);

    /*
    // Ignored for now, the nfo::Website is not working anyway or implemented someweher currently
    QLibraryTreeWidgetItem *twi3 = new QLibraryTreeWidgetItem();
    twi3->setText(0, i18n("Bookmarks"));
    twi3->setData(0,Role_ResourceType,Resource_Website);
    twi3->setData(0,Role_Library,p->libraryType());
    twi3->setData(0,Role_ProjectName, projectName);
    twi3->setData(0,Role_ProjectThing, p->settings()->projectThing().uri());
    twi3->setIcon(0, KIcon(QLatin1String("view-web-browser-dom-tree")));
    root->addChild(twi3);
    connectModelSignals(twi3, p, Resource_Website);
    m_items.append(twi3);
    */

    if(root->data(0,Role_LibraryType).toInt() == Library_Project) {
        QLibraryTreeWidgetItem *twi2 = new QLibraryTreeWidgetItem();
        twi2->setText(0, i18n("Mails"));
        twi2->setData(0,Role_ResourceType,Resource_Mail);
        twi2->setData(0,Role_LibraryType,p->libraryType());
        twi2->setData(0,Role_ProjectName, projectName);
        twi2->setData(0,Role_ProjectThing, p->settings()->projectThing().uri());
        twi2->setIcon(0, KIcon(QLatin1String("mail-flag")));
        root->addChild(twi2);
        connectModelSignals(twi2, p, Resource_Mail);
        m_items.append(twi2);
    }
}

void LibraryWidget::connectModelSignals(QLibraryTreeWidgetItem *root, Library *p, ResourceSelection resourceType)
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
