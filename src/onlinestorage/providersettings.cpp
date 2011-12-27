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

#include "providersettings.h"
#include "ui_providersettings.h"

#include "storageinfo.h"
#include "syncstorage.h"
#include "readfromstorage.h"

#include "zotero/zoteroinfo.h"
#include "kbibtexfile/kbtfileinfo.h"

ProviderSettings::ProviderSettings(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ProviderSettings)
{
    ui->setupUi(this);

    m_wallet = KWallet::Wallet::openWallet(KWallet::Wallet::NetworkWallet(),winId(), KWallet::Wallet::Synchronous);
    if(!m_wallet->hasFolder(QLatin1String("kbibtex"))) {
        m_wallet->createFolder(QLatin1String("kbibtex"));
    }
    m_wallet->setFolder(QLatin1String("kbibtex"));

    // add all available plugins for the synchronization
    StorageInfo *zotero = new ZoteroInfo;
    m_availableProvider.append(zotero);
    ui->providerSelection->addItem(zotero->providerIcon(),zotero->providerName());

    StorageInfo *kbibtexfile = new KBTFileInfo();
    m_availableProvider.append(kbibtexfile);
    ui->providerSelection->addItem(kbibtexfile->providerIcon(),kbibtexfile->providerName());

    ui->providerSelection->setCurrentIndex(0);
    switchProvider(0);

    connect(ui->providerUserName, SIGNAL(textChanged(QString)), this, SLOT(findPasswordInKWallet()));

    ui->fetchCollection->setIcon(KIcon(QLatin1String("svn-update")));
    ui->addCollection->setIcon(KIcon(QLatin1String("list-add")));
    ui->addCollection->hide();
    ui->removeCollection->setIcon(KIcon(QLatin1String("list-remove")));
    ui->removeCollection->hide();
    connect(ui->fetchCollection, SIGNAL(clicked()), this, SLOT(fetchCollection()));
}

ProviderSettings::~ProviderSettings()
{
    delete ui;
    delete m_wallet;
    //qDeleteAll(m_availableProvider);
}

void ProviderSettings::setProviderSettingsDetails(const ProviderSyncDetails &psd)
{
    ui->providerUserName->setText(psd.userName);
    ui->providerUrl->setText(psd.url);

    QString pwdKey;
    pwdKey.append(psd.providerInfo->providerId());
    pwdKey.append(QLatin1String(":"));
    pwdKey.append(psd.userName);
    pwdKey.append(QLatin1String(":"));
    pwdKey.append(psd.url);

    if(m_wallet->hasEntry(pwdKey)) {
        QString pwd;
        m_wallet->readPassword(pwdKey, pwd);
        ui->providerPwd->setText(pwd);
    }
    else {
        ui->providerPwd->setText(QString());
    }
}

ProviderSyncDetails ProviderSettings::providerSettingsDetails() const
{
    ProviderSyncDetails psd;

    int curProviderIndex = ui->providerSelection->currentIndex();
    psd.providerInfo = m_availableProvider.at(curProviderIndex);
    psd.userName = ui->providerUserName->text();
    psd.pwd = ui->providerPwd->text();
    psd.url = ui->providerUrl->text();
    int curCollectionIndex = ui->listCollection->currentIndex();
    psd.collection = ui->listCollection->itemData(curCollectionIndex).toString();
    psd.syncMode = SyncMode(ui->syncMode->currentIndex());
    psd.mergeMode = MergeStrategy (ui->mergeMode->currentIndex());
    psd.askBeforeDeletion = ui->askDeletion->isChecked();
    psd.importAttachments = ui->importAttachments->isChecked();
    psd.exportAttachments = ui->exportAttachments->isChecked();
    psd.akonadiContactsUUid;
    psd.akonadiEventsUUid;

    return psd;
}

StorageInfo *ProviderSettings::storageInfoByString(const QString &providerId)
{
    foreach(StorageInfo *si, m_availableProvider) {
        if(si->providerId() == providerId)
            return si;
    }

    return 0;
}

void ProviderSettings::savePasswordInKWallet()
{
    int curIndex = ui->providerSelection->currentIndex();

    QString pwdKey;
    pwdKey.append(m_availableProvider.at(curIndex)->providerId());
    pwdKey.append(QLatin1String(":"));
    pwdKey.append(ui->providerUserName->text());
    pwdKey.append(QLatin1String(":"));
    pwdKey.append(ui->providerUrl->text());
    m_wallet->writePassword(pwdKey, ui->providerPwd->text());
}

void ProviderSettings::findPasswordInKWallet()
{
    int curIndex = ui->providerSelection->currentIndex();

    QString pwdKey;
    pwdKey.append(m_availableProvider.at(curIndex)->providerId());
    pwdKey.append(QLatin1String(":"));
    pwdKey.append(ui->providerUserName->text());
    pwdKey.append(QLatin1String(":"));
    pwdKey.append(ui->providerUrl->text());

    if(m_wallet->hasEntry(pwdKey)) {
        QString pwd;
        m_wallet->readPassword(pwdKey, pwd);
        ui->providerPwd->setText(pwd);
    }
    else {
        ui->providerPwd->setText(QString());
    }
}

void ProviderSettings::switchProvider(int curIndex)
{
    foreach(StorageInfo* si, m_availableProvider)
        disconnect(si->readHandle(), SIGNAL(collectionsInfo(QList<CollectionInfo>)), this, SLOT(fillCollectionList(QList<CollectionInfo>)));

    connect(m_availableProvider.at(curIndex)->readHandle(), SIGNAL(collectionsInfo(QList<CollectionInfo>)), this, SLOT(fillCollectionList(QList<CollectionInfo>)));
}

void ProviderSettings::fetchCollection()
{
    int curIndex = ui->providerSelection->currentIndex();

    ReadFromStorage *rfs = m_availableProvider.at(curIndex)->readHandle();

    ProviderSyncDetails psd;
    psd.userName = ui->providerUserName->text();
    psd.pwd = ui->providerPwd->text();

    rfs->setProviderSettings(psd);

    rfs->fetchCollections();
}

void ProviderSettings::fillCollectionList(QList<CollectionInfo> collectionList)
{
    ui->listCollection->addItem(i18n("no collection"), QString());
    foreach(const CollectionInfo &ci, collectionList) {
        ui->listCollection->addItem(ci.name, ci.id);
    }
}
