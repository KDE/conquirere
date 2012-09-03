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

#include "synczoterodialog.h"

#include "onlinestorage/providersettings.h"
#include "onlinestorage/storageinfo.h"

#include "nbibio/zotero/synczoteronepomuk.h"
#include "nbibio/bibtex/synckbibtexfile.h"
//#include "nbibio/pipe/bibtextonepomukpipe.h"

#include "mainui/sync/itemdeletedialog.h"
#include "mainui/sync/itemmergedialog.h"

#include <kbibtex/file.h>
#include <kbibtex/entry.h>
#include <kbibtex/findduplicates.h>
#include <kbibtex/findduplicatesui.h>

#include <KDE/KProgressDialog>

#include <QtCore/QThread>
#include <KDE/KMessageBox>
#include <KDE/KDialog>
#include <KDE/KDebug>

#include <Akonadi/CollectionFetchJob>
#include <Akonadi/CollectionFetchScope>

#include <QtCore/QPointer>

SyncZoteroDialog::SyncZoteroDialog(QWidget *parent)
    : KDialog(parent)
    , m_mainDialog(0)
    , m_ps(0)
    , m_syncNepomuk(0)
    , m_pdlg(0)
    , m_MergeDialog(new KDialog)
    , m_mw(0)
{
    m_ps = new ProviderSettings(this, true);
    setMainWidget(m_ps);

    // fetching all collections containing contacts recursively, starting at the root collection
    Akonadi::CollectionFetchJob *job = new Akonadi::CollectionFetchJob( Akonadi::Collection::root(), Akonadi::CollectionFetchJob::Recursive, this );
    job->fetchScope().setContentMimeTypes( QStringList() << "application/x-vnd.kde.contactgroup" );
    connect( job, SIGNAL(collectionsReceived(Akonadi::Collection::List)),
             this, SLOT(akonadiContactCollectionFetched(Akonadi::Collection::List)) );

    Akonadi::CollectionFetchJob *job2 = new Akonadi::CollectionFetchJob( Akonadi::Collection::root(), Akonadi::CollectionFetchJob::Recursive, this );
    job2->fetchScope().setContentMimeTypes( QStringList() << "x-vnd.akonadi.calendar.event" << "application/x-vnd.akonadi.calendar.todo" );
    connect( job2, SIGNAL(collectionsReceived(Akonadi::Collection::List)),
             this, SLOT(akonadiEventCollectionFetched(Akonadi::Collection::List)) );

    KMessageBox::information(this, i18n("Be aware that the sync is still experimental.\nBackup your data before you start to upload something."),
    i18n("Warning!"),QLatin1String("syncwarning"));
}

SyncZoteroDialog::~SyncZoteroDialog()
{
    delete m_syncNepomuk;
    delete m_ps;
    delete m_mainDialog;
    delete m_MergeDialog;
    delete m_mw;
}

void SyncZoteroDialog::setupWidget(ProviderSyncDetails psd)
{
    m_ps->setProviderSettingsDetails(psd);
}

void SyncZoteroDialog::slotButtonClicked(int button)
{
    if(button != KDialog::Ok) {
         KDialog::slotButtonClicked(button);
         return;
    }

    ProviderSyncDetails psd = m_ps->providerSettingsDetails();

    delete m_syncNepomuk;
    if(psd.providerInfo->providerId() == QLatin1String("zotero")) {
        m_syncNepomuk = new SyncZoteroNepomuk;
    }
    else if(psd.providerInfo->providerId() == QLatin1String("kbibtexfile")) {
        m_syncNepomuk = new SyncKBibTeXFile;
    }

    m_syncNepomuk->setProviderDetails(psd);
    m_ps->savePasswordInKWallet();

    delete m_pdlg;
    m_pdlg = new KProgressDialog;
    m_pdlg->setFocus();
    delete m_mw;
    m_mw = 0;

    connect(m_syncNepomuk, SIGNAL(progress(int)), m_pdlg->progressBar(), SLOT(setValue(int)));
    connect(m_syncNepomuk, SIGNAL(progressStatus(QString)), this, SLOT(setProgressStatus(QString)));

    connect(m_syncNepomuk, SIGNAL(askForLocalDeletion(QList<SyncDetails>)), this, SLOT(popLocalDeletionQuestion(QList<SyncDetails>)));
    connect(this, SIGNAL(deleteLocalFiles(bool)), m_syncNepomuk, SLOT(deleteLocalFiles(bool)));
    connect(m_syncNepomuk, SIGNAL(askForServerDeletion(QList<SyncDetails>)), this, SLOT(popServerDeletionQuestion(QList<SyncDetails>)));
    connect(this, SIGNAL(deleteServerFiles(bool)), m_syncNepomuk, SLOT(deleteServerFiles(bool)));
    connect(m_syncNepomuk, SIGNAL(askForGroupRemoval(QList<SyncDetails>)), this, SLOT(popGroupRemovalQuestion(QList<SyncDetails>)));
    connect(this, SIGNAL(removeGroupFiles(bool)), m_syncNepomuk, SLOT(deleteFromGroup(bool)));



    connect(m_syncNepomuk, SIGNAL(userMerge(QList<SyncDetails>)), this, SLOT(popMergeDialog(QList<SyncDetails>)));
    connect(this, SIGNAL(mergeFinished()), m_syncNepomuk, SLOT(mergeFinished()));

    QThread *newThread = new QThread;
    m_syncNepomuk->moveToThread(newThread);

    //what mode should we use?
    switch(psd.syncMode) {
    case Download_Only:
        connect(newThread, SIGNAL(started()),m_syncNepomuk, SLOT(startDownload()) );
        break;
    case Upload_Only:
        connect(newThread, SIGNAL(started()),m_syncNepomuk, SLOT(startUpload()) );
        break;
    case Full_Sync:
        connect(newThread, SIGNAL(started()),m_syncNepomuk, SLOT(startSync()) );
        break;
    }

    newThread->start();

    m_pdlg->exec();
}

void SyncZoteroDialog::popLocalDeletionQuestion(QList<SyncDetails> items)
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

void SyncZoteroDialog::popServerDeletionQuestion(QList<SyncDetails> items)
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

void SyncZoteroDialog::popGroupRemovalQuestion(QList<SyncDetails> items)
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

void SyncZoteroDialog::popMergeDialog(QList<SyncDetails> items)
{
    ItemMergeDialog imd;

    imd.setProviderDetails(m_ps->providerSettingsDetails());

    imd.setItemsToMerge(items);

    imd.exec();

    emit mergeFinished();
}

void SyncZoteroDialog::akonadiContactCollectionFetched(const Akonadi::Collection::List &list)
{
    QList<ProviderSettings::AkonadiDetails> contactList;
    foreach(const Akonadi::Collection & c, list) {
        ProviderSettings::AkonadiDetails ad;
        ad.collectionName = c.name();
        ad.collectionID = c.id();
        contactList.append(ad);
    }

    m_ps->setAkonadiContactDetails(contactList);
}

void SyncZoteroDialog::akonadiEventCollectionFetched(const Akonadi::Collection::List &list)
{
    QList<ProviderSettings::AkonadiDetails> evntList;
    foreach(const Akonadi::Collection & c, list) {
        ProviderSettings::AkonadiDetails ad;
        ad.collectionName = c.name();
        ad.collectionID = c.id();
        evntList.append(ad);
    }

    m_ps->setAkonadiEventDetails(evntList);
}


void SyncZoteroDialog::setProgressStatus(const QString &status)
{
    m_pdlg->setLabelText(status);
}
