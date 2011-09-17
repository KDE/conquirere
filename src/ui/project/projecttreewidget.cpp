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

#include "../../core/project.h"

#include <KStandardDirs>

#include <QTreeWidgetItem>
#include <QVariant>
#include <QDebug>

ProjectTreeWidget::ProjectTreeWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ProjectTreeWidget)
    , m_project(0)
{
    ui->setupUi(this);

    QTreeWidgetItem *root = new QTreeWidgetItem();
    root->setText(0, i18n("System Library"));
    root->setData(0,Role_Library,Library_System);
    root->setData(0,Role_ResourceType,Resource_Library);
    root->setIcon(0, KIcon(QLatin1String("document-multiple")));
    ui->sourceTreeWidget->addTopLevelItem(root);

    setupLibraryTree(root);

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

void ProjectTreeWidget::setupLibraryTree(QTreeWidgetItem *root)
{
    QTreeWidgetItem *twi1 = new QTreeWidgetItem();
    twi1->setText(0, i18n("Documents"));
    twi1->setData(0,Role_Library,root->data(0,Role_Library));
    twi1->setData(0,Role_ResourceType,Resource_Document);
    twi1->setIcon(0, KIcon(QLatin1String("document-multiple")));
    root->addChild(twi1);

    QTreeWidgetItem *twi2 = new QTreeWidgetItem();
    twi2->setText(0, i18n("Mails"));
    twi2->setData(0,Role_Library,root->data(0,Role_Library));
    twi2->setData(0,Role_ResourceType,Resource_Mail);
    twi2->setIcon(0, KIcon(QLatin1String("mail-flag")));
    root->addChild(twi2);

    QTreeWidgetItem *twi3 = new QTreeWidgetItem();
    twi3->setText(0, i18n("Websites"));
    twi3->setData(0,Role_Library,root->data(0,Role_Library));
    twi3->setData(0,Role_ResourceType,Resource_Website);
    twi3->setIcon(0, KIcon(QLatin1String("view-web-browser-dom-tree")));
    root->addChild(twi3);

    QTreeWidgetItem *twi4 = new QTreeWidgetItem();
    twi4->setText(0, i18n("References"));
    twi4->setData(0,Role_Library,root->data(0,Role_Library));
    twi4->setData(0,Role_ResourceType,Resource_Reference);
    twi4->setIcon(0, KIcon(QLatin1String("user-identity")));
    root->addChild(twi4);

    QTreeWidgetItem *twi5 = new QTreeWidgetItem();
    twi5->setText(0, i18n("Media"));
    twi5->setData(0,Role_Library,root->data(0,Role_Library));
    twi5->setData(0,Role_ResourceType,Resource_Media);
    twi5->setIcon(0, KIcon(QLatin1String("applications-multimedia")));
    root->addChild(twi5);

    QTreeWidgetItem *twi6 = new QTreeWidgetItem();
    twi6->setText(0, i18n("Notes"));
    twi6->setData(0,Role_Library,root->data(0,Role_Library));
    twi6->setData(0,Role_ResourceType,Resource_Note);
    twi6->setIcon(0, KIcon(QLatin1String("knotes")));
    root->addChild(twi6);
}
