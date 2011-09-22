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

#include "projecttreewidget.h"
#include "ui_projecttreewidget.h"

#include "core/project.h"
#include "projecttreedelegate.h"

#include <KStandardDirs>

#include <QTreeWidgetItem>
#include <QVariant>
#include <QTimer>

#include <QDebug>

ProjectTreeWidget::ProjectTreeWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ProjectTreeWidget)
    , m_project(0)
    , m_fetchingInProgress(false)
{
    ui->setupUi(this);

    ui->sourceTreeWidget->setItemDelegate(new ProjectTreeDelegate());

    QTreeWidgetItem *rootsystem = new QTreeWidgetItem();
    rootsystem->setText(0, i18n("System Library"));
    rootsystem->setData(0,Role_Library,Library_System);
    rootsystem->setData(0,Role_ResourceType,Resource_Library);
    rootsystem->setData(0,81,0);
    rootsystem->setIcon(0, KIcon(QLatin1String("document-multiple")));
    ui->sourceTreeWidget->addTopLevelItem(rootsystem);
    rootsystem->setExpanded(true);

    m_items.append(rootsystem);
    setupLibraryTree(rootsystem);

    connect(ui->sourceTreeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(selectionchanged()));

}

ProjectTreeWidget::~ProjectTreeWidget()
{
    delete ui;
}

void ProjectTreeWidget::setProject(Project *p)
{
    QTreeWidgetItem *root;
    if(m_project) {
        root = ui->sourceTreeWidget->topLevelItem(0);
    }
    else {
        root = new QTreeWidgetItem();
        root->setData(0,Role_Library,Library_Project);
        root->setData(0,Role_ResourceType,Resource_Library);
        root->setIcon(0, KIcon(QLatin1String("document-multiple")));
        ui->sourceTreeWidget->insertTopLevelItem(0,root);
        m_items.append(root);
    }

    m_project = p;

    root->setText(0, m_project->name());
    root->setExpanded(true);

    setupLibraryTree(root);
}

void ProjectTreeWidget::selectionchanged()
{
    LibraryType library = LibraryType(ui->sourceTreeWidget->currentItem()->data(0,Role_Library).toInt());

    ResourceSelection rs = ResourceSelection(ui->sourceTreeWidget->currentItem()->data(0,Role_ResourceType).toInt());
    emit newSelection(library, rs);
}

void ProjectTreeWidget::updateDataSize(int size)
{

}

void ProjectTreeWidget::fetchDataFor(LibraryType library, ResourceSelection selection, bool start)
{
    foreach(QTreeWidgetItem *twi, m_items) {
        ProjectTreeRole ptr = ProjectTreeRole(twi->data(0,Role_Library).toInt());
        ProjectTreeRole ptr2 = ProjectTreeRole(twi->data(0,Role_ResourceType).toInt());

        if(ptr == library && ptr2 == selection) {
            twi->setData(0,80, start);
        }
    }

    if(!m_fetchingInProgress) {
        m_fetchingInProgress = true;
        updateFetchAnimation();
    }
}

void ProjectTreeWidget::updateFetchAnimation()
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

void ProjectTreeWidget::setupLibraryTree(QTreeWidgetItem *root)
{
    QTreeWidgetItem *twi1 = new QTreeWidgetItem();
    twi1->setText(0, i18n("Documents"));
    twi1->setData(0,Role_Library,root->data(0,Role_Library));
    twi1->setData(0,Role_ResourceType,Resource_Document);
    twi1->setIcon(0, KIcon(QLatin1String("document-multiple")));
    root->addChild(twi1);
    m_items.append(twi1);

    QTreeWidgetItem *twi1a = new QTreeWidgetItem();
    twi1a->setText(0, i18n("Publications"));
    twi1a->setData(0,Role_Library,root->data(0,Role_Library));
    twi1a->setData(0,Role_ResourceType,Resource_Publication);
    twi1a->setIcon(0, KIcon(QLatin1String("document-open-remote")));
    root->addChild(twi1a);
    m_items.append(twi1a);

    QTreeWidgetItem *twi4 = new QTreeWidgetItem();
    twi4->setText(0, i18n("References"));
    twi4->setData(0,Role_Library,root->data(0,Role_Library));
    twi4->setData(0,Role_ResourceType,Resource_Reference);
    twi4->setIcon(0, KIcon(QLatin1String("user-identity")));
    root->addChild(twi4);
    m_items.append(twi4);

    QTreeWidgetItem *twi6 = new QTreeWidgetItem();
    twi6->setText(0, i18n("Notes"));
    twi6->setData(0,Role_Library,root->data(0,Role_Library));
    twi6->setData(0,Role_ResourceType,Resource_Note);
    twi6->setIcon(0, KIcon(QLatin1String("knotes")));
    root->addChild(twi6);
    m_items.append(twi6);

    QTreeWidgetItem *twi2 = new QTreeWidgetItem();
    twi2->setText(0, i18n("Mails"));
    twi2->setData(0,Role_Library,root->data(0,Role_Library));
    twi2->setData(0,Role_ResourceType,Resource_Mail);
    twi2->setIcon(0, KIcon(QLatin1String("mail-flag")));
    root->addChild(twi2);
    m_items.append(twi2);

    QTreeWidgetItem *twi3 = new QTreeWidgetItem();
    twi3->setText(0, i18n("Websites"));
    twi3->setData(0,Role_Library,root->data(0,Role_Library));
    twi3->setData(0,Role_ResourceType,Resource_Website);
    twi3->setIcon(0, KIcon(QLatin1String("view-web-browser-dom-tree")));
    root->addChild(twi3);
    m_items.append(twi3);

    QTreeWidgetItem *twi5 = new QTreeWidgetItem();
    twi5->setText(0, i18n("Media"));
    twi5->setData(0,Role_Library,root->data(0,Role_Library));
    twi5->setData(0,Role_ResourceType,Resource_Media);
    twi5->setIcon(0, KIcon(QLatin1String("applications-multimedia")));
    root->addChild(twi5);
}
