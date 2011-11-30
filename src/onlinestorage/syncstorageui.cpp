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

#include "storageinfo.h"
#include "syncstorage.h"
#include "readfromstorage.h"

#include "zotero/zoteroinfo.h"

#include <kbibtex/file.h>
#include <kbibtex/comment.h>

#include <KDE/KIcon>

#include <QtDebug>

SyncStorageUi::SyncStorageUi(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SyncStorageUi)
    , m_fileToSync(0)
    , m_syncInProgress(false)
{
    ui->setupUi(this);

    m_wallet = KWallet::Wallet::openWallet(KWallet::Wallet::NetworkWallet(),winId(), KWallet::Wallet::Synchronous);
    if(!m_wallet->hasFolder(QString("kbibtex"))) {
        m_wallet->createFolder(QString("kbibtex"));
    }
    m_wallet->setFolder(QString("kbibtex"));

    //ui->providerUserName->setText(QString("795913"));
    //ui->providerPwd->setText(QString("TBydrlOdZo05mmzMhO8PlWCv"));

    // add all available plugins for the syncronization
    StorageInfo *zotero = new ZoteroInfo;
    m_availableProvider.append(zotero);
    ui->providerSelection->addItem(zotero->providerIcon(),zotero->providerName());
    ui->providerSelection->setCurrentIndex(0);
    switchProvider();

    connect(ui->cancelCloseButton, SIGNAL(clicked()), this, SLOT(cancelClose()));
    connect(ui->startSync, SIGNAL(clicked()), this, SLOT(startSync()));

    ui->fetchCollection->setIcon(KIcon("svn-update"));
    ui->addCollection->setIcon(KIcon("list-add"));
    ui->addCollection->hide();
    ui->removeCollection->setIcon(KIcon("list-remove"));
    ui->removeCollection->hide();
    connect(ui->fetchCollection, SIGNAL(clicked()), this, SLOT(fetchCollection()));

    syncStatus(false);
}

SyncStorageUi::~SyncStorageUi()
{
    delete ui;

    foreach(StorageInfo* si, m_availableProvider) {
        //delete si;
    }

    delete m_wallet;
}

void SyncStorageUi::setBibTeXFile(File *fileToSync)
{
    m_fileToSync = fileToSync;

    // fill out the formular with previous sync info

    QString providerId;
    QString userName;
    QString url;

    foreach(Element *e, *m_fileToSync) {
        Comment *c = dynamic_cast<Comment *>(e);
        if(c && c->text().startsWith(QString("x-syncprovider="))){
            providerId = c->text().remove(QString("x-syncprovider="));
        }
        if(c && c->text().startsWith(QString("x-syncusername="))){
            userName = c->text().remove(QString("x-syncusername="));
        }
        if(c && c->text().startsWith(QString("x-syncurl="))){
            url = c->text().remove(QString("x-syncurl="));
        }
    }

    // now if we have a provider find it in our list and change the kcombobox
    // as we have only zoter right now I'll leave it out ;)

    if(!providerId.isEmpty()) {
        ui->providerUserName->setText(userName);

        QString pwdKey = QString("%1:%2:%3").arg(providerId).arg(userName).arg(url);
        if(m_wallet->hasEntry(pwdKey)) {
            QString pwd;
            m_wallet->readPassword(pwdKey, pwd);
            ui->providerPwd->setText(pwd);
        }
        else {
            ui->providerPwd->setText(QString());
        }

        ui->providerUrl->setText(url);
    }
}

void SyncStorageUi::switchProvider()
{
    int curIndex = ui->providerSelection->currentIndex();
    SyncStorage *syncStorage = m_availableProvider.at(curIndex)->syncHandle();

    connect(m_availableProvider.at(curIndex)->syncHandle(), SIGNAL(syncInProgress(bool)), this, SLOT(syncStatus(bool)));
    connect(m_availableProvider.at(curIndex)->syncHandle(), SIGNAL(progress(int)), ui->progressBar, SLOT(setValue(int)));
    connect(m_availableProvider.at(curIndex)->readHandle(), SIGNAL(collectionsInfo(QList<CollectionInfo>)), this, SLOT(fillCollectionList(QList<CollectionInfo>)));
}

void SyncStorageUi::fetchCollection()
{
    int curIndex = ui->providerSelection->currentIndex();

    ReadFromStorage *rfs = m_availableProvider.at(curIndex)->readHandle();

    rfs->setUserName(ui->providerUserName->text());
    rfs->setPassword(ui->providerPwd->text());

    rfs->fetchCollections();
}

void SyncStorageUi::fillCollectionList(QList<CollectionInfo> collectionList)
{
    ui->listCollection->addItem(i18n("no collection"), QString());
    foreach(const CollectionInfo &ci, collectionList) {
        ui->listCollection->addItem(ci.name, ci.id);
    }
}

void SyncStorageUi::startSync()
{
    int curIndex = ui->providerSelection->currentIndex();
    SyncStorage *syncStorage = m_availableProvider.at(curIndex)->syncHandle();

    syncStorage->setUserName(ui->providerUserName->text());

    // check if password is in the KWallet storage, if not ask if the user wants to save it
    QString pwdKey = QString("%1:%2:%3").arg(m_availableProvider.at(curIndex)->providerId()).arg(ui->providerUserName->text()).arg(ui->providerUrl->text());
    m_wallet->writePassword(pwdKey, ui->providerPwd->text());

    syncStorage->setPassword(ui->providerPwd->text());

    syncStorage->setAdoptBibtexTypes(ui->cbAdaptToBibTeX->isChecked());
    syncStorage->setAskBeforeDeletion(ui->cbAskDeletion->isChecked());
    syncStorage->setDownloadOnly(ui->cbDownloadOnly->isChecked());
    syncStorage->setUrl(ui->providerUrl->text());

    QString collectionID = ui->listCollection->itemData(curIndex).toString();
    syncStorage->syncWithStorage(m_fileToSync, collectionID);
}

void SyncStorageUi::syncStatus(bool inProgress)
{
    m_syncInProgress = inProgress;

    if(inProgress) {
        ui->startSync->setIcon(KIcon("view-refresh")); // animate button
        ui->cancelCloseButton->setText(i18n("Cancel"));
        ui->cancelCloseButton->setIcon(KIcon("dialog-cancel"));
    }
    // sync finished
    else {
        ui->startSync->setIcon(KIcon("view-refresh"));
        ui->cancelCloseButton->setText(i18n("Close"));
        ui->cancelCloseButton->setIcon(KIcon("dialog-close"));

        // write sync information into bibtex file
        if(m_fileToSync) {
            int curIndex = ui->providerSelection->currentIndex();
            QString providerId = m_availableProvider.at(curIndex)->providerId();

            bool foundProvider = false;
            bool foundUsername = false;
            bool foundUrl = false;
            // find the right comment
            foreach(Element *e, *m_fileToSync) {
                Comment *c = dynamic_cast<Comment *>(e);
                if(c && c->text().startsWith(QString("x-syncprovider="))){
                    foundProvider = true;
                    c->setText(QString("x-syncprovider=%1").arg(providerId));
                }
                else if(c && c->text().startsWith(QString("x-syncusername="))){
                    foundUsername = true;
                    c->setText(QString("x-syncusername=%1").arg(ui->providerUserName->text()));
                }
                else if(c && c->text().startsWith(QString("x-syncurl="))){
                    foundUrl = true;
                    c->setText(QString("x-syncurl=%1").arg(ui->providerUrl->text()));
                }
            }
            if(!foundProvider) {
                Comment *c =new Comment(QString("x-syncprovider=%1").arg(providerId), true);
                m_fileToSync->prepend(c);
            }
            if(!foundUsername) {
                Comment *c =new Comment(QString("x-syncusername=%1").arg(ui->providerUserName->text()), true);
                m_fileToSync->prepend(c);
            }
            if(!foundUrl) {
                Comment *c =new Comment(QString("x-syncurl=%1").arg(ui->providerUrl->text()), true);
                m_fileToSync->prepend(c);
            }
        }
    }
}

void SyncStorageUi::cancelSync()
{
    // TODO cancel the actual sync progress

    syncStatus(false);
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
