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
#include "projecttreedelegate.h"

#include <KStandardDirs>

#include <QTreeWidgetItem>
#include <QVariant>
#include <QTimer>

#include <QDebug>

LibraryWidget::LibraryWidget(QWidget *parent)
    : QDockWidget(parent)
    , ui(new Ui::LibraryWidget)
    , m_fetchingInProgress(false)
{
    ui->setupUi(this);
    ui->treeWidget->setItemDelegate(new ProjectTreeDelegate());

    connect(ui->treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(selectionchanged()));
}

LibraryWidget::~LibraryWidget()
{
    delete ui;
}

void LibraryWidget::addLibrary(Library *p)
{ QTreeWidgetItem *root;
    root = new QTreeWidgetItem();
    root->setData(0,Role_Library,p->libraryType());
    root->setData(0,Role_ResourceType,Resource_Library);
    root->setData(0,Role_Project,p->name());
    root->setIcon(0, KIcon(QLatin1String("document-multiple")));
    ui->treeWidget->addTopLevelItem(root);
    m_items.append(root);

    root->setText(0, p->name());
    root->setExpanded(true);

    setupLibraryTree(root);

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

    Library *selectedLibrary;
    selectedLibrary = 0;
    foreach(Library *p, m_openLibraries) {
        if(p->name() == ui->treeWidget->currentItem()->data(0,Role_Project).toString()) {
            selectedLibrary = p;
            break;
        }
    }

    emit newSelection(rs, selectedLibrary);
}

void LibraryWidget::fetchDataFor(ResourceSelection selection, bool start, Library *p)
{
    foreach(QTreeWidgetItem *twi, m_items) {
        ProjectTreeRole ptr = ProjectTreeRole(twi->data(0,Role_ResourceType).toInt());

        QString name = twi->data(0,Role_Project).toString();

        if(p->name() == name && ptr == selection) {
            twi->setData(0,80, start);
        }
    }

    if(!m_fetchingInProgress) {
        m_fetchingInProgress = true;
        updateFetchAnimation();
    }
}

void LibraryWidget::updateFetchAnimation()
{
    int fetching = 0;
    foreach(QTreeWidgetItem *twi, m_items) {
        if(twi->data(0,80).toBool()) {
            int angle = twi->data(0,81).toInt();
            angle = (angle+30)%360;
            twi->setData(0,81,angle);
            fetching++;
        }
    }

    if(fetching > 0)
        QTimer::singleShot(250,this,SLOT(updateFetchAnimation()));
    else
        m_fetchingInProgress = false;
}

void LibraryWidget::setupLibraryTree(QTreeWidgetItem *root)
{
    QTreeWidgetItem *twi1 = new QTreeWidgetItem();
    twi1->setText(0, i18n("Documents"));
    twi1->setData(0,Role_Library,root->data(0,Role_Library));
    twi1->setData(0,Role_Project,root->data(0,Role_Project));
    twi1->setData(0,Role_ResourceType,Resource_Document);
    twi1->setIcon(0, KIcon(QLatin1String("document-multiple")));
    root->addChild(twi1);
    m_items.append(twi1);

    QTreeWidgetItem *twi1a = new QTreeWidgetItem();
    twi1a->setText(0, i18n("Publications"));
    twi1a->setData(0,Role_Library,root->data(0,Role_Library));
    twi1a->setData(0,Role_ResourceType,Resource_Publication);
    twi1a->setData(0,Role_Project,root->data(0,Role_Project));
    twi1a->setIcon(0, KIcon(QLatin1String("document-open-remote")));
    root->addChild(twi1a);
    m_items.append(twi1a);

    QTreeWidgetItem *twi4 = new QTreeWidgetItem();
    twi4->setText(0, i18n("References"));
    twi4->setData(0,Role_Library,root->data(0,Role_Library));
    twi4->setData(0,Role_ResourceType,Resource_Reference);
    twi4->setData(0,Role_Project,root->data(0,Role_Project));
    twi4->setIcon(0, KIcon(QLatin1String("user-identity")));
    root->addChild(twi4);
    m_items.append(twi4);

    QTreeWidgetItem *twi6 = new QTreeWidgetItem();
    twi6->setText(0, i18n("Notes"));
    twi6->setData(0,Role_Library,root->data(0,Role_Library));
    twi6->setData(0,Role_ResourceType,Resource_Note);
    twi6->setData(0,Role_Project,root->data(0,Role_Project));
    twi6->setIcon(0, KIcon(QLatin1String("knotes")));
    root->addChild(twi6);
    m_items.append(twi6);

    QTreeWidgetItem *twi3 = new QTreeWidgetItem();
    twi3->setText(0, i18n("Bookmarks"));
    twi3->setData(0,Role_Library,root->data(0,Role_Library));
    twi3->setData(0,Role_ResourceType,Resource_Website);
    twi3->setData(0,Role_Project,root->data(0,Role_Project));
    twi3->setIcon(0, KIcon(QLatin1String("view-web-browser-dom-tree")));
    root->addChild(twi3);
    m_items.append(twi3);

    if(root->data(0,Role_Library).toInt() == Library_Project) {
        QTreeWidgetItem *twi2 = new QTreeWidgetItem();
        twi2->setText(0, i18n("Mails"));
        twi2->setData(0,Role_Library,root->data(0,Role_Library));
        twi2->setData(0,Role_ResourceType,Resource_Mail);
        twi2->setData(0,Role_Project,root->data(0,Role_Project));
        twi2->setIcon(0, KIcon(QLatin1String("mail-flag")));
        root->addChild(twi2);
        m_items.append(twi2);

        QTreeWidgetItem *twi5 = new QTreeWidgetItem();
        twi5->setText(0, i18n("Media"));
        twi5->setData(0,Role_Library,root->data(0,Role_Library));
        twi5->setData(0,Role_ResourceType,Resource_Media);
        twi5->setData(0,Role_Project,root->data(0,Role_Project));
        twi5->setIcon(0, KIcon(QLatin1String("applications-multimedia")));
        root->addChild(twi5);
        m_items.append(twi5);
    }
}
