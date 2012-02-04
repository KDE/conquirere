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

#include "itemdeletedialog.h"
#include "ui_itemdeletedialog.h"

#include "nbib.h"
#include "sync.h"

#include <Nepomuk/Resource>
#include <Nepomuk/Variant>
#include <Nepomuk/Vocabulary/NIE>

ItemDeleteDialog::ItemDeleteDialog(Mode mode, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ItemDeleteDialog)
    , m_mode(mode)
{
    ui->setupUi(this);
    ui->detailsLabel->setVisible(false);
    connect(ui->detailButton, SIGNAL(clicked(bool)), this, SLOT(showDetails()));
}

ItemDeleteDialog::~ItemDeleteDialog()
{
    delete ui;
}

void ItemDeleteDialog::setItems(QList<SyncDetails> items)
{
    switch(m_mode) {
    case LocalDelete:
        ui->infoLabel->setText(i18n("%1 items are deleted on the server.\n\nDo you want to delete them locally too?.\nOtherwise they will be uploaded again with the next sync.", items.size()));
        break;
    case ServerDelete:
        ui->infoLabel->setText(i18n("%1 items are deleted locally.\n\nDo you want to delete them on the server too?.\nOtherwise they will be downloaded again with the next sync.", items.size()));
        break;
    case ServerGroupRemoval:
        ui->infoLabel->setText(i18n("%1 items are removed from the local project.\n\nDo you want to remove them from the server group too?.\nOtherwise they will be attached to the group again with the next sync.", items.size()));
        break;
    }

    switch(m_mode) {
    case LocalDelete:
        localDeleteDetails(items);
        break;
    case ServerDelete:
    case ServerGroupRemoval:
        serverDeleteDetails(items);
        break;
    }
}

void ItemDeleteDialog::showDetails()
{
    if(ui->detailsLabel->isVisible()) {
        ui->detailsLabel->setVisible(false);
    }
    else {
       ui->detailsLabel->setVisible(true);
    }
}

void ItemDeleteDialog::localDeleteDetails(QList<SyncDetails> items)
{
    QString details;
    foreach(const SyncDetails &sd, items) {
        Nepomuk::Resource deletedResource;
        QString type;
        QUrl syncType = sd.syncResource.property(Nepomuk::Vocabulary::SYNC::syncDataType()).toUrl();
        if(syncType == Nepomuk::Vocabulary::SYNC::Note()) {
            deletedResource = sd.syncResource.property(Nepomuk::Vocabulary::SYNC::note()).toResource();
            type = i18n("Note");
        }
        else if(syncType == Nepomuk::Vocabulary::SYNC::Attachment()) {
            deletedResource = sd.syncResource.property(Nepomuk::Vocabulary::SYNC::attachment()).toResource();
            type = i18n("Attachment");
        }
        else {
            deletedResource = sd.syncResource.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();
            type = i18n("Reference");
        }

        details.append( deletedResource.property(Nepomuk::Vocabulary::NIE::title()).toString()  + QLatin1String(" (") + type + QLatin1String(")"));
        details.append( QLatin1String(", "));
    }

    details.chop(2);

    ui->detailsLabel->setText(details);
    ui->detailsLabel->setWordWrap(true);
}

void ItemDeleteDialog::serverDeleteDetails(QList<SyncDetails> items)
{
    QString details;
    foreach(const SyncDetails &sd, items) {
        QUrl syncType = sd.syncResource.property(Nepomuk::Vocabulary::SYNC::syncDataType()).toUrl();
        QString id = sd.syncResource.property(Nepomuk::Vocabulary::SYNC::id()).toString();
        QString type;
        if(syncType == Nepomuk::Vocabulary::SYNC::Note()) {
            type = i18n("Note");
        }
        else if(syncType == Nepomuk::Vocabulary::SYNC::Attachment()) {
            type = i18n("Attachment");
        }
        else {
            type = i18n("Reference");
        }

        details.append( id + QLatin1String(" (") + type + QLatin1String(")"));
        details.append( QLatin1String(", "));
    }

    details.chop(2);

    ui->detailsLabel->setText(details);
    ui->detailsLabel->setWordWrap(true);
}

