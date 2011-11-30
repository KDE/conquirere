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

#include "syncstorageui.h"
#include "ui_syncstorageui.h"

#include "zotero/synczotero.h"
#include "zotero/readfromzotero.h"

#include <KDE/KIcon>

#include <QtDebug>

SyncStorageUi::SyncStorageUi(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SyncStorageUi)
    , m_syncInProgress(false)
{
    ui->setupUi(this);

    connect(ui->cancelCloseButton, SIGNAL(clicked()), this, SLOT(cancelClose()));
    connect(ui->startSync, SIGNAL(clicked()), this, SLOT(startSync()));

    ui->fetchCollection->setIcon(KIcon("svn-update"));
    ui->addCollection->setIcon(KIcon("list-add"));
    ui->addCollection->hide();
    ui->removeCollection->setIcon(KIcon("list-remove"));
    ui->removeCollection->hide();
    connect(ui->fetchCollection, SIGNAL(clicked()), this, SLOT(fetchCollection()));

    ui->providerSelection->addItem(KIcon("storage-zotero"),QString("Zotero"));

    m_syncStorage = new SyncZotero;
    connect(m_syncStorage, SIGNAL(syncInProgress(bool)), this, SLOT(syncStatus(bool)));
    connect(m_syncStorage, SIGNAL(progress(int)), ui->progressBar, SLOT(setValue(int)));

    syncStatus(false);
}

SyncStorageUi::~SyncStorageUi()
{
    delete ui;
}

void SyncStorageUi::setBibTeXFile(File *fileToSync)
{
    m_fileToSync = fileToSync;
}

void SyncStorageUi::switchProvider()
{

}

void SyncStorageUi::fetchCollection()
{
    ReadFromZotero *rfz = new ReadFromZotero;

    rfz->setUserName(QString("795913"));
    rfz->setPassword(QString("TBydrlOdZo05mmzMhO8PlWCv"));

    rfz->fetchCollections();

    connect(rfz, SIGNAL(collectionsInfo(QList<CollectionInfo>)), this, SLOT(fillCollectionList(QList<CollectionInfo>)));
}

void SyncStorageUi::fillCollectionList(QList<CollectionInfo> collectionList)
{
    ui->listCollection->addItem(i18n("no collection"), QString());
    foreach(const CollectionInfo &ci, collectionList) {
        ui->listCollection->addItem(ci.name, ci.id);
    }

    delete sender();
}

void SyncStorageUi::startSync()
{
    //m_syncStorage->setUserName(ui->providerUserName->text());
    //m_syncStorage->setPassword(ui->providerPwd->text());

    m_syncStorage->setUserName(QString("795913"));
    m_syncStorage->setPassword(QString("TBydrlOdZo05mmzMhO8PlWCv"));

    m_syncStorage->setAdoptBibtexTypes(ui->cbAdaptToBibTeX->isChecked());
    m_syncStorage->setAskBeforeDeletion(ui->cbAskDeletion->isChecked());
    m_syncStorage->setDownloadOnly(ui->cbDownloadOnly->isChecked());

    int curIndex = ui->listCollection->currentIndex();
    QString collectionID = ui->listCollection->itemData(curIndex).toString();
    m_syncStorage->syncWithStorage(m_fileToSync, collectionID);
}

void SyncStorageUi::syncStatus(bool inProgress)
{
    m_syncInProgress = inProgress;

    if(inProgress) {
        ui->startSync->setIcon(KIcon("view-refresh")); // animate button
        ui->cancelCloseButton->setText(i18n("Cancel"));
        ui->cancelCloseButton->setIcon(KIcon("dialog-cancel"));
    }
    else {
        ui->startSync->setIcon(KIcon("view-refresh"));
        ui->cancelCloseButton->setText(i18n("Close"));
        ui->cancelCloseButton->setIcon(KIcon("dialog-close"));
    }
}

void SyncStorageUi::cancelSync()
{

}

void SyncStorageUi::cancelClose()
{
    if(m_syncInProgress) {
        cancelSync();
    }
    else {
        close();
    }
}
