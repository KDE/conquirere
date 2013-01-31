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

#include "nbibio/provider/onlinestorage.h"
#include "nbibio/provider/zotero/zoterosync.h"

#include <KDE/KMessageBox>
#include <QtCore/QDebug>

ProviderSettings::ProviderSettings(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ProviderSettings)
{
    ui->setupUi(this);

    ui->localStorageUrl->setEnabled(false);
    ui->storageLabel->setEnabled(false);

    m_wallet = KWallet::Wallet::openWallet(KWallet::Wallet::NetworkWallet(),winId(), KWallet::Wallet::Synchronous);
    if(!m_wallet->hasFolder(QLatin1String("kbibtex"))) {
        m_wallet->createFolder(QLatin1String("kbibtex"));
    }
    m_wallet->setFolder(QLatin1String("kbibtex"));

    // add all available plugins for the synchronization
    OnlineStorage *zotero = new ZoteroSync;
    m_availableProvider.append(zotero);
    ui->providerSelection->addItem(zotero->providerIcon(),zotero->providerName());

//    StorageInfo *kbibtexfile = new KBTFileInfo();
//    m_availableProvider.append(kbibtexfile);
//    ui->providerSelection->addItem(kbibtexfile->providerIcon(),kbibtexfile->providerName());

    ui->providerSelection->setCurrentIndex(0);
    switchProvider(0);

    connect(ui->providerUserName, SIGNAL(textChanged(QString)), this, SLOT(findPasswordInKWallet()));
    connect(ui->providerUserName, SIGNAL(textChanged(QString)), this, SLOT(showLocalStoragePathCompletion()));
    connect(ui->localStorageUrl, SIGNAL(textChanged(QString)), this, SLOT(showLocalStoragePathCompletion()));
    connect(ui->providerSelection, SIGNAL(currentIndexChanged(int)), this, SLOT(showLocalStoragePathCompletion()));

    ui->fetchCollection->setIcon(KIcon(QLatin1String("svn-update")));
    ui->addCollection->setIcon(KIcon(QLatin1String("list-add")));
    ui->addCollection->hide();
    ui->removeCollection->setIcon(KIcon(QLatin1String("list-remove")));
    ui->removeCollection->hide();
    connect(ui->fetchCollection, SIGNAL(clicked()), this, SLOT(fetchCollection()));

    ui->helpButton->setIcon(KIcon("help-contents"));
    connect(ui->helpButton, SIGNAL(clicked()), this, SLOT(showHelp()));
}

ProviderSettings::~ProviderSettings()
{
    delete ui;
    delete m_wallet;
    //qDeleteAll(m_availableProvider); // do not delete here, pointer to providerInfo still needed in some cases
}

void ProviderSettings::setProviderSettingsDetails(const ProviderSyncDetails &psd)
{
    m_oldPsd = psd;
    ui->providerUserName->setText(psd.userName);

    // switch to the selected provider
    int curIndex=0;
    foreach(OnlineStorage *si, m_availableProvider) {
        if(si->providerId() == psd.providerId)
            break;
        else
            curIndex++;
    }

    ui->providerSelection->setCurrentIndex(curIndex);

    if(ui->providerUrl->text().isEmpty()) {
        ui->providerUrl->setText(m_availableProvider.at(curIndex)->defaultUrl());
    }
    else {
        ui->providerUrl->setText(psd.url);
    }

    if(ui->providerUrlRequester->text().isEmpty()) {
        ui->providerUrlRequester->setText(m_availableProvider.at(curIndex)->defaultUrl());
    }
    else {
        ui->providerUrlRequester->setText(psd.url);
    }

    QString pwdKey;
    pwdKey.append(psd.providerId);
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

    if(!psd.collection.isEmpty()) {
        ui->listCollection->setCurrentItem(psd.collection,true);
    }

    ui->syncMode->setCurrentIndex(psd.syncMode);
    ui->mergeMode->setCurrentIndex(psd.mergeMode);
    ui->askDeletion->setChecked(psd.askBeforeDeletion);
    ui->importAttachments->setChecked(psd.importAttachments);
    ui->exportAttachments->setChecked(psd.exportAttachments);

    ui->localStorageUrl->setEnabled( psd.importAttachments );
    ui->localStorageUrl->setUrl( psd.localStoragePath );
}

ProviderSyncDetails ProviderSettings::providerSettingsDetails() const
{
    ProviderSyncDetails psd;

    int curProviderIndex = ui->providerSelection->currentIndex();
    psd.providerId = m_availableProvider.at(curProviderIndex)->providerId();
    psd.userName = ui->providerUserName->text();
    psd.pwd = ui->providerPwd->text();

    if(m_availableProvider.at(curProviderIndex)->useUrlSelector()) {
        psd.url = ui->providerUrlRequester->text();
    }
    else {
        psd.url = ui->providerUrl->text();
    }

    int curCollectionIndex = ui->listCollection->currentIndex();
    psd.collection = ui->listCollection->itemData(curCollectionIndex).toString();
    psd.syncMode = SyncMode(ui->syncMode->currentIndex());
    psd.mergeMode = MergeStrategy (ui->mergeMode->currentIndex());
    psd.askBeforeDeletion = ui->askDeletion->isChecked();
    psd.importAttachments = ui->importAttachments->isChecked();
    psd.exportAttachments = ui->exportAttachments->isChecked();

    psd.localStoragePath = ui->localStorageUrl->text();

    return psd;
}

OnlineStorage *ProviderSettings::storageInfoByString(const QString &providerId)
{
    foreach(OnlineStorage *si, m_availableProvider) {
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
    if(m_availableProvider.at(curIndex)->useUrlSelector()) {
        pwdKey.append(ui->providerUrlRequester->text());
    }
    else {
        pwdKey.append(ui->providerUrl->text());
    }
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
    if(m_availableProvider.at(curIndex)->useUrlSelector()) {
        pwdKey.append(ui->providerUrlRequester->text());
    }
    else {
        pwdKey.append(ui->providerUrl->text());
    }

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
    foreach(OnlineStorage* si, m_availableProvider)
        disconnect(si, SIGNAL(finished()), this, SLOT(fillCollectionList()));

    ui->listCollection->clear();

    if(m_availableProvider.at(curIndex)->useUrlSelector()) {
        ui->providerUrl->setVisible(false);
        ui->providerUrlRequester->setVisible(true);
    }
    else {
        ui->providerUrl->setVisible(true);
        ui->providerUrlRequester->setVisible(false);
    }

    if(ui->providerUrl->text().isEmpty())
        ui->providerUrl->setText(m_availableProvider.at(curIndex)->defaultUrl());

    if(ui->providerUrlRequester->text().isEmpty())
        ui->providerUrlRequester->setText(m_availableProvider.at(curIndex)->defaultUrl());

    if(m_availableProvider.at(curIndex)->supportCollections()) {
        ui->collectionLabel->setEnabled(true);
        ui->addCollection->setEnabled(true);
        ui->removeCollection->setEnabled(true);
        ui->fetchCollection->setEnabled(true);
        ui->listCollection->setEnabled(true);
        connect(m_availableProvider.at(curIndex), SIGNAL(finished()), this, SLOT(fillCollectionList()));
    }
    else {
        ui->collectionLabel->setEnabled(false);
        ui->addCollection->setEnabled(false);
        ui->removeCollection->setEnabled(false);
        ui->fetchCollection->setEnabled(false);
        ui->listCollection->setEnabled(false);
    }
}

void ProviderSettings::showHelp()
{
    int curIndex = ui->providerSelection->currentIndex();

    KMessageBox::information(this, m_availableProvider.at(curIndex)->helpText(),
                             i18n("Provider Help"));
}

void ProviderSettings::fetchCollection()
{
    int curIndex = ui->providerSelection->currentIndex();

    OnlineStorage *storage = m_availableProvider.at(curIndex);

    ProviderSyncDetails psd;
    psd.userName = ui->providerUserName->text();
    psd.pwd = ui->providerPwd->text();
    if(m_availableProvider.at(curIndex)->useUrlSelector()) {
        psd.url = ui->providerUrlRequester->text();
    }
    else {
        psd.url = ui->providerUrl->text();
    }

    storage->setProviderSettings(psd);

    storage->fetchCollections();
}

void ProviderSettings::fillCollectionList()
{
    OnlineStorage *storage = qobject_cast<OnlineStorage *>(sender());
    QList<CollectionInfo> collectionList = storage->collectionInfo();

    ui->listCollection->clear();

    ui->listCollection->addItem(i18n("no collection"), QString());
    foreach(const CollectionInfo &ci, collectionList) {
        ui->listCollection->addItem(ci.name, ci.id);
    }

    if(!m_oldPsd.collection.isEmpty()) {
        ui->listCollection->setCurrentItem(m_oldPsd.collection,true);
    }
}

void ProviderSettings::showLocalStoragePathCompletion()
{
    QString path = ui->localStorageUrl->text();
    if(!path.isEmpty()) {
        int curProviderIndex = ui->providerSelection->currentIndex();

        path.append( QLatin1String("/") + m_availableProvider.at(curProviderIndex)->providerId() + QLatin1String("/") + ui->providerUserName->text());
    }

    ui->localStoragePath->setText(path);
}
