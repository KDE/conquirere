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

#include "backgroundsync.h"

#include "core/library.h"
#include "core/librarymanager.h"

#include "nbibio/ui/itemdeletedialog.h"
#include "nbibio/ui/itemmergedialog.h"

#include <KDE/KMessageBox>
#include <KWallet/Wallet>
#include <KDE/KDebug>

#include <QtCore/QThread>
#include <QtCore/QPointer>

BackgroundSync::BackgroundSync(QObject *parent)
    : QObject(parent)
    , m_libraryManager(0)
    , m_libraryToSync(0)
    , m_syncThread(0)
    , m_syncSteps(1)
    , m_curStep(0)
{
}

BackgroundSync::~BackgroundSync()
{
}

void BackgroundSync::setLibraryManager(LibraryManager *lm)
{
    m_libraryManager = lm;
}

void BackgroundSync::setLibraryToSyncWith(Library *l)
{
    m_libraryToSync = l;
}

void BackgroundSync::startSync()
{
    if(m_libraryToSync) {
        m_syncList = m_libraryToSync->settings()->allProviderSyncDetails();
    }
    else {
        m_syncSteps = 1;
        m_syncList = m_libraryManager->systemLibrary()->settings()->allProviderSyncDetails();
        foreach(Library *l, m_libraryManager->openProjects()) {
            m_syncList.append(l->settings()->allProviderSyncDetails());
        }
    }

    if(m_syncList.isEmpty()) {
        emit allSyncTargetsFinished();
        kDebug() << "no provider to sync with found";
        return;
    }
    else {
        m_syncSteps = m_syncList.size();
        ProviderSyncDetails psd = m_syncList.first();
        startSync(psd);
    }
}


void BackgroundSync::startSync(const ProviderSyncDetails &psd)
{
    m_currentPsd = psd;

    if( !findPasswordInKWallet(m_currentPsd) ) {
        kError() << "Could not find password for sync";
        currentSyncFinished();
        return;
    }

    // configure the provider via provider id
    delete m_nepomukSyncClient;
    m_nepomukSyncClient = new NepomukSyncClient();

    if(m_libraryToSync) {
        m_nepomukSyncClient->setProject( m_libraryToSync->settings()->projectThing() );
        kDebug() << "Sync project library" << m_libraryToSync->settings()->projectThing().genericLabel();
    }
    else {
        kDebug() << "Sync System Library";
    }

    kDebug() << "user" << m_currentPsd.userName;
    kDebug() << "pwd" << m_currentPsd.pwd;

    m_nepomukSyncClient->setProviderSettings(m_currentPsd);

    connect(m_nepomukSyncClient, SIGNAL(progress(int)), this, SLOT( calculateProgress(int)) );
    //connect(m_nepomukSyncClient, SIGNAL(status(QString)), infoLabel, SLOT(setText(QString)));

    connect(m_nepomukSyncClient, SIGNAL(askForLocalDeletion(QList<Nepomuk2::Resource>)), this, SLOT(popLocalDeletionQuestion(QList<Nepomuk2::Resource>)) );
    connect(this, SIGNAL(deleteLocalFiles(bool)), m_nepomukSyncClient, SLOT(deleteLocalFiles(bool)));

    connect(m_nepomukSyncClient, SIGNAL(askForServerDeletion(QVariantList)), this, SLOT(popServerDeletionQuestion(QVariantList)));
    connect(this, SIGNAL(deleteServerFiles(bool)), m_nepomukSyncClient, SLOT(deleteServerFiles(bool)));

    connect(m_nepomukSyncClient, SIGNAL(userMerge(QList<SyncMergeDetails>)), this, SLOT(popMergeDialog(QList<SyncMergeDetails>)) );
    connect(this, SIGNAL(mergeFinished()), m_nepomukSyncClient, SLOT(mergeFinished()));

    delete m_syncThread;
    m_syncThread = new QThread;
    m_nepomukSyncClient->moveToThread(m_syncThread);

    //what mode should we use?
    switch(psd.syncMode) {
    case Download_Only:
        connect(m_syncThread, SIGNAL(started()),m_nepomukSyncClient, SLOT(importData()) );
        break;
    case Upload_Only:
        connect(m_syncThread, SIGNAL(started()),m_nepomukSyncClient, SLOT(exportData()) );
        break;
    case Full_Sync:
        connect(m_syncThread, SIGNAL(started()),m_nepomukSyncClient, SLOT(syncData()) );
        break;
    }
    connect(m_nepomukSyncClient, SIGNAL(finished()), m_syncThread, SLOT(quit()));
    connect(m_syncThread, SIGNAL(finished()), this, SLOT(currentSyncFinished()));

    m_syncThread->start();
}


void BackgroundSync::popLocalDeletionQuestion(const QList<Nepomuk2::Resource> &items)
{
    QPointer<ItemDeleteDialog> idd = new ItemDeleteDialog(ItemDeleteDialog::LocalDelete);

    idd->setItems(items);
    int ret = idd->exec();

    if(ret == QDialog::Accepted) {
        emit deleteLocalFiles(true);
    }
    else {
        emit deleteLocalFiles(false);
    }

    delete idd;
}

void BackgroundSync::popServerDeletionQuestion(const QVariantList &items)
{
    QPointer<ItemDeleteDialog> idd = new ItemDeleteDialog(ItemDeleteDialog::ServerDelete);

    idd->setItems(items);
    int ret = idd->exec();

    if(ret == QDialog::Accepted) {
        emit deleteServerFiles(true);
    }
    else {
        emit deleteServerFiles(false);
    }

    delete idd;
}
/*
void BackgroundSync::popGroupRemovalQuestion(QList<SyncDetails> items)
{
    QPointer<ItemDeleteDialog> idd = new ItemDeleteDialog(ItemDeleteDialog::ServerGroupRemoval);

    idd->setItems(items);
    int ret = idd->exec();

    if(ret == QDialog::Accepted) {
        emit removeGroupFiles(true);
    }
    else {
        emit removeGroupFiles(false);
    }

    delete idd;
}
*/
void BackgroundSync::popMergeDialog(const QList<SyncMergeDetails> &items)
{
    ItemMergeDialog imd;

    imd.setProviderDetails(m_currentPsd);
    if(m_libraryToSync) {
        imd.setLibraryToSyncWith(m_libraryToSync);
    }

    imd.setItemsToMerge(items);

    imd.exec();

    emit mergeFinished();
}

void BackgroundSync::cancelSync()
{
    m_nepomukSyncClient->cancel();
    m_syncList.clear();
    m_syncThread->quit();
    m_syncThread->deleteLater();
}

void BackgroundSync::calculateProgress(int value)
{
    qreal curProgress = ((qreal)value * 1.0/m_syncSteps);

    curProgress += (qreal)(100.0/m_syncSteps) * m_curStep;

    emit progress(curProgress);
}

void BackgroundSync::currentSyncFinished()
{
    m_curStep++;
    delete m_nepomukSyncClient;
    m_nepomukSyncClient = 0;
    m_syncThread->deleteLater();
    m_syncThread = 0;

    m_syncList.takeFirst();

    if(m_syncList.isEmpty()) {
        emit allSyncTargetsFinished();
        kDebug() << "finished the sync process";
        return;
    }
    else {
        ProviderSyncDetails psd = m_syncList.first();
        startSync(psd);
    }
}

bool BackgroundSync::findPasswordInKWallet(ProviderSyncDetails &psd)
{
    QString pwdKey;
    pwdKey.append(psd.providerId);
    pwdKey.append(QLatin1String(":"));
    pwdKey.append(psd.userName);
    pwdKey.append(QLatin1String(":"));
    pwdKey.append(psd.url);

    kDebug() << "find password for " << pwdKey;

    KWallet::Wallet *m_wallet;

    m_wallet = KWallet::Wallet::openWallet(KWallet::Wallet::NetworkWallet(),0, KWallet::Wallet::Synchronous);
    if(!m_wallet->hasFolder(QLatin1String("kbibtex"))) {
        m_wallet->createFolder(QLatin1String("kbibtex"));
    }
    m_wallet->setFolder(QLatin1String("kbibtex"));

    if(m_wallet->hasEntry(pwdKey)) {
        QString pwd;
        m_wallet->readPassword(pwdKey, pwd);
        psd.pwd = pwd;
        delete m_wallet;
        return true;
    }
    else {
        kDebug() << "todo ask user to provide passowrd";
        psd.pwd.clear();

        delete m_wallet;
        return false;
    }
}
