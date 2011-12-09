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
#include "qlibrarytreewidgetitem.h"
#include "projecttreedelegate.h"
#include "../../core/models/nepomukmodel.h"

#include <KDE/KStandardDirs>

#include <QtCore/QVariant>
#include <QtGui/QSortFilterProxyModel>

#include <QtCore/QDebug>

LibraryWidget::LibraryWidget(QWidget *parent)
    : QDockWidget(parent)
    , ui(new Ui::LibraryWidget)
{
    ui->setupUi(this);
    ui->treeWidget->setItemDelegate(new ProjectTreeDelegate());

    connect(ui->treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(selectionchanged()));
}

LibraryWidget::~LibraryWidget()
{
    delete ui;
    qDeleteAll(m_openLibraries);
    //qDeleteAll(m_items);
}

void LibraryWidget::addLibrary(Library *p)
{
    QLibraryTreeWidgetItem *root;
    root = new QLibraryTreeWidgetItem();
    root->setData(0,Role_Library,p->libraryType());
    root->setData(0,Role_ResourceType,Resource_Library);
    root->setData(0,Role_Project,p->name());
    root->setIcon(0, KIcon(QLatin1String("document-multiple")));
    ui->treeWidget->addTopLevelItem(root);
    m_items.append(root);

    root->setText(0, p->name());
    root->setExpanded(true);

    setupLibraryTree(root, p);

    m_openLibraries.append(p);
}

void LibraryWidget::closeLibrary(Library *p)
{
    int toplevelItems = ui->treeWidget->topLevelItemCount();

    for(int i=1; i <= toplevelItems; i++) {
        QTreeWidgetItem *root = ui->treeWidget->topLevelItem(i);

        QString openProject = (root->data(0,Role_Project).toString());
        if(openProject == p->name()) {
            ui->treeWidget->takeTopLevelItem(i);
            break;
        }
    }
    m_openLibraries.removeAll(p);
}

void LibraryWidget::selectionchanged()
{
    ResourceSelection rs = ResourceSelection(ui->treeWidget->currentItem()->data(0,Role_ResourceType).toInt());
    BibEntryType filter = BibEntryType(ui->treeWidget->currentItem()->data(0,Role_ResourceFilter).toInt());

    Library *selectedLibrary;
    selectedLibrary = 0;
    foreach(Library *p, m_openLibraries) {
        if(p->name() == ui->treeWidget->currentItem()->data(0,Role_Project).toString()) {
            selectedLibrary = p;
            break;
        }
    }

    emit newSelection(rs, filter, selectedLibrary);
}

void LibraryWidget::setupLibraryTree(QLibraryTreeWidgetItem *root, Library *p)
{
    QLibraryTreeWidgetItem *twi1 = new QLibraryTreeWidgetItem();
    twi1->setText(0, i18n("Documents"));
    twi1->setData(0,Role_Library,root->data(0,Role_Library));
    twi1->setData(0,Role_Project,root->data(0,Role_Project));
    twi1->setData(0,Role_ResourceType,Resource_Document);
    twi1->setIcon(0, KIcon(QLatin1String("document-multiple")));
    root->addChild(twi1);
    connectModelSignals(twi1, p, Resource_Document);
    m_items.append(twi1);

    QLibraryTreeWidgetItem *twi1a = new QLibraryTreeWidgetItem();
    twi1a->setText(0, i18n("Publications"));
    twi1a->setData(0,Role_Library,root->data(0,Role_Library));
    twi1a->setData(0,Role_ResourceType,Resource_Publication);
    twi1a->setData(0,Role_Project,root->data(0,Role_Project));
    twi1a->setData(0,Role_ResourceFilter,Max_BibTypes);
    twi1a->setIcon(0, KIcon(QLatin1String("document-open-remote")));
    root->addChild(twi1a);
    connectModelSignals(twi1a, p, Resource_Publication);
    m_items.append(twi1a);

    for(int i=0; i < Max_BibTypes; i++) {
        QLibraryTreeWidgetItem *pubSub = new QLibraryTreeWidgetItem();
        pubSub->setText(0, BibEntryTypeTranslation.at(i));
        pubSub->setData(0,Role_Library,root->data(0,Role_Library));
        pubSub->setData(0,Role_ResourceType,Resource_Publication);
        pubSub->setData(0,Role_ResourceFilter,BibEntryType(i));
        pubSub->setData(0,Role_Project,root->data(0,Role_Project));
        pubSub->setIcon(0, KIcon(BibEntryTypeIcon.at(i)));
        twi1a->addChild(pubSub);
        connectModelSignals(pubSub, p, Resource_Publication);
        m_items.append(pubSub);
    }

    QLibraryTreeWidgetItem *twi4 = new QLibraryTreeWidgetItem();
    twi4->setText(0, i18n("References"));
    twi4->setData(0,Role_Library,root->data(0,Role_Library));
    twi4->setData(0,Role_ResourceType,Resource_Reference);
    twi4->setData(0,Role_ResourceFilter,Max_BibTypes);
    twi4->setData(0,Role_Project,root->data(0,Role_Project));
    twi4->setIcon(0, KIcon(QLatin1String("user-identity")));
    root->addChild(twi4);
    connectModelSignals(twi4, p, Resource_Reference);
    m_items.append(twi4);

    for(int i=0; i < Max_BibTypes; i++) {
        QLibraryTreeWidgetItem *refSub = new QLibraryTreeWidgetItem();
        refSub->setText(0, BibEntryTypeTranslation.at(i));
        refSub->setData(0,Role_Library,root->data(0,Role_Library));
        refSub->setData(0,Role_ResourceType,Resource_Reference);
        refSub->setData(0,Role_ResourceFilter,BibEntryType(i));
        refSub->setData(0,Role_Project,root->data(0,Role_Project));
        refSub->setIcon(0, KIcon(BibEntryTypeIcon.at(i)));
        twi4->addChild(refSub);
        connectModelSignals(refSub, p, Resource_Publication);
        m_items.append(refSub);
    }

    QLibraryTreeWidgetItem *twi5 = new QLibraryTreeWidgetItem();
    twi5->setText(0, i18n("Series"));
    twi5->setData(0,Role_Library,root->data(0,Role_Library));
    twi5->setData(0,Role_ResourceType,Resource_Series);
    twi5->setData(0,Role_ResourceFilter,Max_SeriesTypes);
    twi5->setData(0,Role_Project,root->data(0,Role_Project));
    twi5->setIcon(0, KIcon(SeriesTypeIcon.at(0)));
    root->addChild(twi5);
    connectModelSignals(twi5, p, Resource_Series);
    m_items.append(twi5);

    for(int i=0; i < Max_SeriesTypes; i++) {
        QLibraryTreeWidgetItem *refSub = new QLibraryTreeWidgetItem();
        refSub->setText(0, SeriesTypeTranslation.at(i));
        refSub->setData(0,Role_Library,root->data(0,Role_Library));
        refSub->setData(0,Role_ResourceType,Resource_Series);
        refSub->setData(0,Role_ResourceFilter,SeriesType(i));
        refSub->setData(0,Role_Project,root->data(0,Role_Project));
        refSub->setIcon(0, KIcon(SeriesTypeIcon.at(i)));
        twi5->addChild(refSub);
        connectModelSignals(refSub, p, Resource_Series);
        m_items.append(refSub);
    }

    QLibraryTreeWidgetItem *twi6 = new QLibraryTreeWidgetItem();
    twi6->setText(0, i18n("Notes"));
    twi6->setData(0,Role_Library,root->data(0,Role_Library));
    twi6->setData(0,Role_ResourceType,Resource_Note);
    twi6->setData(0,Role_Project,root->data(0,Role_Project));
    twi6->setIcon(0, KIcon(QLatin1String("knotes")));
    root->addChild(twi6);
    connectModelSignals(twi6, p, Resource_Note);
    m_items.append(twi6);

    QLibraryTreeWidgetItem *twi3 = new QLibraryTreeWidgetItem();
    twi3->setText(0, i18n("Bookmarks"));
    twi3->setData(0,Role_Library,root->data(0,Role_Library));
    twi3->setData(0,Role_ResourceType,Resource_Website);
    twi3->setData(0,Role_Project,root->data(0,Role_Project));
    twi3->setIcon(0, KIcon(QLatin1String("view-web-browser-dom-tree")));
    root->addChild(twi3);
    connectModelSignals(twi3, p, Resource_Website);
    m_items.append(twi3);

    if(root->data(0,Role_Library).toInt() == Library_Project) {
        QLibraryTreeWidgetItem *twi2 = new QLibraryTreeWidgetItem();
        twi2->setText(0, i18n("Mails"));
        twi2->setData(0,Role_Library,root->data(0,Role_Library));
        twi2->setData(0,Role_ResourceType,Resource_Mail);
        twi2->setData(0,Role_Project,root->data(0,Role_Project));
        twi2->setIcon(0, KIcon(QLatin1String("mail-flag")));
        root->addChild(twi2);
        connectModelSignals(twi2, p, Resource_Mail);
        m_items.append(twi2);
    }
}

void LibraryWidget::connectModelSignals(QLibraryTreeWidgetItem *root, Library *p, ResourceSelection resourceType)
{
    QSortFilterProxyModel *viewModel = p->viewModel(resourceType);

    NepomukModel *nm = qobject_cast<NepomukModel *>(viewModel->sourceModel());

    connect(nm, SIGNAL(queryStarted()), root, SLOT(startQueryFetch()));
    connect(nm, SIGNAL(queryFinished()), root, SLOT(stopQueryFetch()));
    connect(nm, SIGNAL(dataSizeChaged(int)), root, SLOT(updateItemCount(int)));
}