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
#include "../librarymanager.h"

#include "onlinestorage/storageinfo.h"
#include "onlinestorage/storageinfo.h"

#include "nbibio/zotero/synczoteronepomuk.h"
#include "nbibio/synckbibtexfile.h"

#include "nbibio/pipe/bibtextonepomukpipe.h" //just until we created the merge dialog

#include <KDE/KMessageBox>
#include <KWallet/Wallet>
#include <KDE/KDebug>

#include <QtCore/QThread>

BackgroundSync::BackgroundSync(QObject *parent)
    : QObject(parent)
    , m_libraryToSync(0)
    , m_syncThread(0)
    , m_syncNepomuk(0)
    , m_syncSteps(1)
    , m_curStep(0)
{
}

BackgroundSync::~BackgroundSync()
{
//    m_syncThread->deleteLater();
    delete m_syncNepomuk;
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
    }

    ProviderSyncDetails psd = m_syncList.first();

    bool found = findPasswordInKWallet(psd);

    startSync(psd);
}

void BackgroundSync::startSync(const ProviderSyncDetails &psd)
{
//    kDebug() << "start to sync" << m_libraryToSync->settings()->name() << "with" << psd.providerInfo->providerName();
    kDebug() << "user" << psd.userName;
    kDebug() << "pwd" << psd.pwd;
    kDebug() << "collection" << psd.collection;

//    delete m_syncNepomuk;
    if(psd.providerInfo->providerId() == QLatin1String("zotero")) {
        m_syncNepomuk = new SyncZoteroNepomuk;
    }
    else if(psd.providerInfo->providerId() == QLatin1String("kbibtexfile")) {
        m_syncNepomuk = new SyncKBibTeXFile;
    }

    m_syncNepomuk->setProviderDetails(psd);
    m_syncNepomuk->setSystemLibrary(m_libraryManager->systemLibrary());
    if(m_libraryToSync) {
        m_syncNepomuk->setLibraryToSyncWith(m_libraryToSync);
    }

    // connect the sync handler with the outside world
    connect(m_syncNepomuk, SIGNAL(progress(int)), this, SLOT(calculateProgress(int)));
    connect(m_syncNepomuk, SIGNAL(progressStatus(QString)), this, SIGNAL(progressStatus(QString)));

    connect(m_syncNepomuk, SIGNAL(askForLocalDeletion(QList<SyncDetails>)), this, SLOT(popLocalDeletionQuestion(QList<SyncDetails>)));
    connect(this, SIGNAL(deleteLocalFiles(bool)), m_syncNepomuk, SLOT(deleteLocalFiles(bool)));
    connect(m_syncNepomuk, SIGNAL(askForServerDeletion(QList<SyncDetails>)), this, SLOT(popServerDeletionQuestion(QList<SyncDetails>)));
    connect(this, SIGNAL(deleteServerFiles(bool)), m_syncNepomuk, SLOT(deleteServerFiles(bool)));
    connect(m_syncNepomuk, SIGNAL(askForGroupRemoval(QList<SyncDetails>)), this, SLOT(popGroupRemovalQuestion(QList<SyncDetails>)));
    connect(this, SIGNAL(removeGroupFiles(bool)), m_syncNepomuk, SLOT(deleteFromGroup(bool)));


    connect(m_syncNepomuk, SIGNAL(userMerge(QList<SyncDetails>)), this, SLOT(popMergeDialog(QList<SyncDetails>)));
    connect(this, SIGNAL(mergeFinished()), m_syncNepomuk, SLOT(mergeFinished()));

    connect(m_syncNepomuk, SIGNAL(syncFinished()), this, SLOT(currentSyncFinished()));

    delete m_syncThread;
    m_syncThread = new QThread;
    m_syncNepomuk->moveToThread(m_syncThread);

    //what mode should we use?
    switch(psd.syncMode) {
    case Download_Only:
        connect(m_syncThread, SIGNAL(started()),m_syncNepomuk, SLOT(startDownload()) );
        break;
    case Upload_Only:
        connect(m_syncThread, SIGNAL(started()),m_syncNepomuk, SLOT(startUpload()) );
        break;
    case Full_Sync:
        connect(m_syncThread, SIGNAL(started()),m_syncNepomuk, SLOT(startSync()) );
        break;
    }

    connect(m_syncNepomuk, SIGNAL(syncFinished()), m_syncThread, SLOT(quit()));

    m_syncThread->start();
}

void BackgroundSync::popLocalDeletionQuestion(QList<SyncDetails> items)
{
    int ret = KMessageBox::warningYesNo(0,i18n("%1 items are deleted on the server.\n\nDo you want to delete them locally too?.\nOtherwise they will be uploaded again with the next sync.", items.size()));

    if(ret == KMessageBox::Yes) {
        emit deleteLocalFiles(true);
    }
    else {
        emit deleteLocalFiles(false);
    }
}

void BackgroundSync::popServerDeletionQuestion(QList<SyncDetails> items)
{
    int ret = KMessageBox::warningYesNo(0,i18n("%1 items are deleted locally.\n\nDo you want to delete them on the server too?.\nOtherwise they will be downloaded again with the next sync.", items.size()));

    if(ret == KMessageBox::Yes) {
        emit deleteServerFiles(true);
    }
    else {
        emit deleteServerFiles(false);
    }
}

void BackgroundSync::popGroupRemovalQuestion(QList<SyncDetails> items)
{
    int ret = KMessageBox::warningYesNo(0,i18n("%1 items are removed from the local project.\n\nDo you want to remove them from the server group too?.\nOtherwise they will be attached to the group again with the next sync.", items.size()));

    if(ret == KMessageBox::Yes) {
        emit removeGroupFiles(true);
    }
    else {
        emit removeGroupFiles(false);
    }
}

void BackgroundSync::popMergeDialog(QList<SyncDetails> items)
{
    kDebug() << "show blocking merge dialog for " << items.size() << "items";

    KMessageBox::sorry( 0, QLatin1String("TODO:: User selected entry merging, default to use server verion for now."), QLatin1String("Sorry") );

    ProviderSyncDetails psd = m_syncList.first();

    foreach(const SyncDetails &sd, items) {
        BibTexToNepomukPipe mergePipe;
        mergePipe.setSyncDetails(psd.url, psd.userName);
        mergePipe.merge(sd.syncResource, sd.externalResource, false);
    }

    emit mergeFinished();
}

void BackgroundSync::cancelSync()
{
    m_syncNepomuk->cancel();
    m_syncList.clear();
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
    delete m_syncNepomuk;
    m_syncNepomuk = 0;
//    m_syncThread->deleteLater();
//    m_syncThread = 0;

    m_syncList.takeFirst();

    if(m_syncList.isEmpty()) {
        emit allSyncTargetsFinished();
        kDebug() << "finished the sync process";
        return;
    }

    ProviderSyncDetails psd = m_syncList.first();
    startSync(psd);
}

bool BackgroundSync::findPasswordInKWallet(ProviderSyncDetails &psd)
{
    QString pwdKey;
    pwdKey.append(psd.providerInfo->providerId());
    pwdKey.append(QLatin1String(":"));
    pwdKey.append(psd.userName);
    pwdKey.append(QLatin1String(":"));
    pwdKey.append(psd.url);

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
        psd.pwd = QString();

        delete m_wallet;
        return false;
    }
}
