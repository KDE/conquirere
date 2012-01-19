/*
 * Copyright 2012 Jörg Ehrichs <joerg.ehrichs@gmx.de>
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

#include "libraryinfowidget.h"
#include "../build/src/mainui/ui_libraryinfowidget.h"

#include "core/library.h"
#include "core/projectsettings.h"

LibraryInfoWidget::LibraryInfoWidget(QWidget *parent) :
    SidebarComponent(parent),
    ui(new Ui::LibraryInfoWidget)
{
    ui->setupUi(this);

    setupUI();
}

LibraryInfoWidget::~LibraryInfoWidget()
{
    delete ui;
}

void LibraryInfoWidget::setLibrary(Library *p)
{
    SidebarComponent::setLibrary(p);

    if(!p)
        return;

    ui->nameLabel->setText(p->settings()->name());
    ui->descriptionLabel->setText(p->settings()->description());

    if(p->libraryType() == Library_System) {
        ui->deleteButton->setEnabled(false);
        ui->closeButton->setEnabled(false);
    }
    else {
        ui->deleteButton->setEnabled(true);
        ui->closeButton->setEnabled(true);
    }
}

void LibraryInfoWidget::setResource(Nepomuk::Resource & resource)
{
    Q_UNUSED(resource);
}

void LibraryInfoWidget::newButtonClicked()
{

}

void LibraryInfoWidget::deleteButtonClicked()
{

}

void LibraryInfoWidget::setupUI()
{
    ui->editButton->setIcon(KIcon("preferences-other"));
    ui->importButton->setIcon(KIcon("document-import"));
    ui->exportButton->setIcon(KIcon("document-export"));
    ui->syncButton->setIcon(KIcon("view-refresh"));
    ui->closeButton->setIcon(KIcon("document-close"));
    ui->deleteButton->setIcon(KIcon("document-close"));
}
