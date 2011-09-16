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

#include <QTreeWidgetItem>
#include <QVariant>
#include <QDebug>

ProjectTreeWidget::ProjectTreeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProjectTreeWidget)
{
    ui->setupUi(this);

    setupWidget();

}

ProjectTreeWidget::~ProjectTreeWidget()
{
    delete ui;
}

void ProjectTreeWidget::selectionchanged()
{
    ResourceSelection ss = ResourceSelection(ui->sourceTreeWidget->currentItem()->data(0,Qt::UserRole).toInt());

    emit newSelection(ss);
}

void ProjectTreeWidget::setupWidget()
{
    QTreeWidgetItem *twi1 = new QTreeWidgetItem();
    twi1->setText(0, tr("Documents"));
    twi1->setData(0,Qt::UserRole,Resource_Document);
    twi1->setIcon(0, KIcon(QLatin1String("document-multiple")));
    ui->sourceTreeWidget->addTopLevelItem(twi1);

    QTreeWidgetItem *twi2 = new QTreeWidgetItem();
    twi2->setText(0, tr("Mails"));
    twi2->setData(0,Qt::UserRole,Resource_Mail);
    twi2->setIcon(0, KIcon(QLatin1String("mail-flag")));
    ui->sourceTreeWidget->addTopLevelItem(twi2);

    QTreeWidgetItem *twi3 = new QTreeWidgetItem();
    twi3->setText(0, tr("Websites"));
    twi3->setData(0,Qt::UserRole,Resource_Website);
    twi3->setIcon(0, KIcon(QLatin1String("view-web-browser-dom-tree")));
    ui->sourceTreeWidget->addTopLevelItem(twi3);

    QTreeWidgetItem *twi4 = new QTreeWidgetItem();
    twi4->setText(0, tr("References"));
    twi4->setData(0,Qt::UserRole,Resource_Reference);
    twi4->setIcon(0, KIcon(QLatin1String("user-identity")));
    ui->sourceTreeWidget->addTopLevelItem(twi4);

    QTreeWidgetItem *twi5 = new QTreeWidgetItem();
    twi5->setText(0, tr("Media"));
    twi5->setData(0,Qt::UserRole,Resource_Media);
    twi5->setIcon(0, KIcon(QLatin1String("applications-multimedia")));
    ui->sourceTreeWidget->addTopLevelItem(twi5);

    QTreeWidgetItem *twi6 = new QTreeWidgetItem();
    twi6->setText(0, tr("Notes"));
    twi6->setData(0,Qt::UserRole,Resource_Note);
    twi6->setIcon(0, KIcon(QLatin1String("knotes")));
    ui->sourceTreeWidget->addTopLevelItem(twi6);

    connect(ui->sourceTreeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(selectionchanged()));
}
