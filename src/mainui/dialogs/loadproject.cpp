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

#include "loadproject.h"
#include "ui_loadproject.h"

#include "core/library.h"

#include <Nepomuk/Resource>
#include <Nepomuk/Variant>

#include <Nepomuk/Vocabulary/PIMO>
#include <Nepomuk/Vocabulary/NIE>
#include <Soprano/Vocabulary/NAO>

#include <KDE/KConfig>
#include <KDE/KConfigGroup>
#include <KDE/KLineEdit>

#include <QtGui/QListWidgetItem>

LoadProject::LoadProject(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoadProject)
{
    ui->setupUi(this);

    fetchProjects();

    connect(ui->listWidget, SIGNAL(currentRowChanged(int)), this, SLOT(showCollection(int)));
}

LoadProject::~LoadProject()
{
    delete ui;
}

Library *LoadProject::loadedLibrary() const
{
    return m_loadLibrary;
}

void LoadProject::queryFinished()
{
    m_queryClient->close();
}

void LoadProject::showCollection(int currentRow)
{
    if(currentRow >= 0) {
        ui->listWidget->clearSelection();

        QListWidgetItem *item = ui->listWidget->item(currentRow);

        Nepomuk::Resource collection = Nepomuk::Resource(item->data(Qt::UserRole).toString());

        ui->labelName->setText(collection.property(Nepomuk::Vocabulary::NIE::title()).toString());

        QString description = collection.property(Soprano::Vocabulary::NAO::description()).toString();
        if(description.isEmpty()) {
            description = i18n("no description available");
        }

        ui->labelDescription->setText(description);
    }
}

void LoadProject::accept()
{
    m_loadLibrary = new Library();

    QListWidgetItem *curItem = ui->listWidget->currentItem();
    Nepomuk::Thing collection = Nepomuk::Thing(curItem->data(Qt::UserRole).toString());
    m_loadLibrary->loadLibrary(collection);

    QDialog::accept();
}

void LoadProject::fetchProjects()
{
    m_queryClient = new Nepomuk::Query::QueryServiceClient();
    connect(m_queryClient, SIGNAL(newEntries(QList<Nepomuk::Query::Result>)), this, SLOT(fillProjectList(QList<Nepomuk::Query::Result>)));
    connect(m_queryClient, SIGNAL(finishedListing()), this, SLOT(queryFinished()));

    QString query = "select DISTINCT ?r where { "
                     "?r a pimo:Project ."
                     "}";

     m_queryClient->sparqlQuery( query );
}

void LoadProject::fillProjectList( const QList< Nepomuk::Query::Result > &entries )
{
    foreach(const Nepomuk::Query::Result &r, entries) {

        QListWidgetItem *newItem = new QListWidgetItem;
        newItem->setText(r.resource().property(Nepomuk::Vocabulary::NIE::title()).toString());
        newItem->setData(Qt::UserRole, r.resource().resourceUri());
        ui->listWidget->addItem(newItem);
    }
}
