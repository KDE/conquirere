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

#include "mergeresourceswidget.h"
#include "ui_mergeresourceswidget.h"

#include <Nepomuk/ResourceManager>
#include <KDE/KIcon>

#include <QtCore/QDebug>

MergeResourcesWidget::MergeResourcesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MergeResourcesWidget)
{
    ui->setupUi(this);

    ui->mergeButton->setIcon(KIcon("merge"));
    connect(ui->mergeButton, SIGNAL(clicked()), this, SLOT(merge()));
}

MergeResourcesWidget::~MergeResourcesWidget()
{
    delete ui;
}

void MergeResourcesWidget::setResources(QList<Nepomuk::Resource> resourcelist)
{
    m_resourceList = resourcelist;
}

void MergeResourcesWidget::merge()
{
    qDebug() << "wait for kde 4.8 to arrive in ubuntu...";
}