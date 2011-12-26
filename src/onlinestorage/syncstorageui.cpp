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
#include "providersettings.h"

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

    syncStatus(false);
}

SyncStorageUi::~SyncStorageUi()
{
    delete ui;
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
        if(c && c->text().startsWith(QLatin1String("x-syncprovider="))){
            providerId = c->text().remove(QLatin1String("x-syncprovider="));
        }
        if(c && c->text().startsWith(QLatin1String("x-syncusername="))){
            userName = c->text().remove(QLatin1String("x-syncusername="));
        }
        if(c && c->text().startsWith(QLatin1String("x-syncurl="))){
            url = c->text().remove(QLatin1String("x-syncurl="));
        }
    }

    // now if we have a provider find it in our list and change the kcombobox
    StorageInfo *si = ui->providerSettings->storageInfoByString(providerId);
    if(!si)
        return;

    ProviderSettings::ProviderSettingsDetails psd;
    psd.providerInfo = si;
    psd.userName;
    psd.url;

    ui->providerSettings->setProviderSettingsDetails(psd);
}

void SyncStorageUi::startSync()
{
    ProviderSettings::ProviderSettingsDetails psd = ui->providerSettings->providerSettingsDetails();

    SyncStorage *syncStorage = psd.providerInfo->syncHandle();
    syncStorage->setUserName(psd.userName);
    syncStorage->setPassword(psd.pwd);
    //syncStorage->setAdoptBibtexTypes(psd.);
    syncStorage->setAskBeforeDeletion(psd.askBeforeDeletion);
    if(psd.syncMode == 0)
        syncStorage->setDownloadOnly(true);

    syncStorage->setUrl(psd.url);
    syncStorage->syncWithStorage(m_fileToSync, psd.collection);
}

void SyncStorageUi::syncStatus(bool inProgress)
{
    m_syncInProgress = inProgress;

    if(inProgress) {
        ui->startSync->setIcon(KIcon(QLatin1String("view-refresh"))); // animate button
        ui->cancelCloseButton->setText(i18n("Cancel"));
        ui->cancelCloseButton->setIcon(KIcon(QLatin1String("dialog-cancel")));
    }
    // sync finished
    else {
        ui->startSync->setIcon(KIcon(QLatin1String("view-refresh")));
        ui->cancelCloseButton->setText(i18n("Close"));
        ui->cancelCloseButton->setIcon(KIcon(QLatin1String("dialog-close")));

        // write sync information into bibtex file
        if(m_fileToSync) {
            ProviderSettings::ProviderSettingsDetails psd = ui->providerSettings->providerSettingsDetails();
            QString providerId = psd.providerInfo->providerId();

            bool foundProvider = false;
            bool foundUsername = false;
            bool foundUrl = false;
            // find the right comment
            foreach(Element *e, *m_fileToSync) {
                Comment *c = dynamic_cast<Comment *>(e);
                if(c && c->text().startsWith(QLatin1String("x-syncprovider="))){
                    foundProvider = true;
                    QString sp = QLatin1String("x-syncprovider=");
                    sp.append(providerId);
                    c->setText(sp);
                    c->setUseCommand(true);
                }
                else if(c && c->text().startsWith(QLatin1String("x-syncusername="))){
                    foundUsername = true;
                    QString sp = QLatin1String("x-syncusername=");
                    sp.append(psd.userName);
                    c->setText(sp);
                    c->setUseCommand(true);
                }
                else if(c && c->text().startsWith(QLatin1String("x-syncurl="))){
                    foundUrl = true;
                    QString sp = QLatin1String("x-syncurl=");
                    sp.append(psd.url);
                    c->setText(sp);
                    c->setUseCommand(true);
                }
            }
            if(!foundProvider) {
                QString sp = QLatin1String("x-syncprovider=");
                sp.append(providerId);
                Comment *c =new Comment(sp, true);
                m_fileToSync->prepend(c);
            }
            if(!foundUsername) {
                QString sp = QLatin1String("x-syncusername=");
                sp.append(psd.userName);
                Comment *c =new Comment(sp, true);
                m_fileToSync->prepend(c);
            }
            if(!foundUrl) {
                QString sp = QLatin1String("x-syncurl=%1");
                sp.append(psd.url);
                Comment *c =new Comment(sp, true);
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
