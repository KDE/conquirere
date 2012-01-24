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

#include "storagesyncwizard.h"
#include "ui_storagesyncwizard.h"

#include "mainui/librarymanager.h"

#include "onlinestorage/providersettings.h"
#include "onlinestorage/storageinfo.h"

#include "nbibio/synczoteronepomuk.h"
#include "nbibio/synckbibtexfile.h"
#include "nbibio/pipe/bibtextonepomukpipe.h"

#include <Akonadi/CollectionFetchJob>
#include <Akonadi/CollectionFetchScope>

#include <KDE/KMessageBox>
#include <KDE/KDebug>

#include <QtGui/QVBoxLayout>
#include <QtGui/QProgressBar>
#include <QtGui/QLabel>

#include <QtCore/QThread>

StorageSyncWizard::StorageSyncWizard(QWidget *parent)
    : QWizard(parent)
    , ui(new Ui::StorageSyncWizard)
{
    ui->setupUi(this);

    sp = new SettingsPage;
    pp = new ProgressPage;

    addPage(sp);
    addPage(pp);

    setMinimumHeight(550);
    setMinimumWidth(500);
}

StorageSyncWizard::~StorageSyncWizard()
{
    delete ui;
}

void StorageSyncWizard::setLibraryManager(LibraryManager *lm)
{
    libraryManager = lm;
}

//##################################################################
// Settings page
//##################################################################

SettingsPage::SettingsPage(QWidget *parent)
    : QWizardPage(parent)
{
    setupUi();
}

bool SettingsPage::isComplete() const
{
    return true;
}

void SettingsPage::contactCollectionsReceived( const Akonadi::Collection::List& list)
{
    QList<ProviderSettings::AkonadiDetails> contactList;
    foreach(const Akonadi::Collection & c, list) {
        ProviderSettings::AkonadiDetails ad;
        ad.collectionName = c.name();
        ad.collectionID = c.id();
        contactList.append(ad);
    }

    providerSettings->setAkonadiContactDetails(contactList);
}

void SettingsPage::eventCollectionsReceived( const Akonadi::Collection::List& list)
{
    QList<ProviderSettings::AkonadiDetails> evntList;
    foreach(const Akonadi::Collection & c, list) {
        ProviderSettings::AkonadiDetails ad;
        ad.collectionName = c.name();
        ad.collectionID = c.id();
        evntList.append(ad);
    }

    providerSettings->setAkonadiEventDetails(evntList);
}

void SettingsPage::setupUi()
{
    setTitle(i18n("Sync Storage Settings"));
    setSubTitle(i18n("Select the provider and specify the settings for it to sync your system library"));

    providerSettings = new ProviderSettings(this, true);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(providerSettings);
    setLayout(layout);

    // fetching all collections containing contacts recursively, starting at the root collection
    Akonadi::CollectionFetchJob *job = new Akonadi::CollectionFetchJob( Akonadi::Collection::root(), Akonadi::CollectionFetchJob::Recursive, this );
    job->fetchScope().setContentMimeTypes( QStringList() << "application/x-vnd.kde.contactgroup" );
    connect( job, SIGNAL(collectionsReceived(Akonadi::Collection::List)),
             this, SLOT(contactCollectionsReceived(Akonadi::Collection::List)) );

    Akonadi::CollectionFetchJob *job2 = new Akonadi::CollectionFetchJob( Akonadi::Collection::root(), Akonadi::CollectionFetchJob::Recursive, this );
    job2->fetchScope().setContentMimeTypes( QStringList() << "x-vnd.akonadi.calendar.event" << "application/x-vnd.akonadi.calendar.todo" );
    connect( job2, SIGNAL(collectionsReceived(Akonadi::Collection::List)),
             this, SLOT(eventCollectionsReceived(Akonadi::Collection::List)) );
}


ProgressPage::ProgressPage(QWidget *parent)
    : QWizardPage(parent)
    , m_syncNepomuk(0)
{
    setupUi();
}

bool ProgressPage::isComplete() const
{
    return true;
}

void ProgressPage::setupUi()
{
    setTitle(i18n("Sync Progress"));
    setSubTitle(i18n("The sync is in progress ... wait until its finished or cancel it."));

    progressBar = new QProgressBar();
    infoLabel = new QLabel();

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(progressBar);
    layout->addWidget(infoLabel);
    setLayout(layout);
}

void ProgressPage::initializePage()
{
     //start the actual sync
    StorageSyncWizard *ssw = qobject_cast<StorageSyncWizard *>(wizard());

    ProviderSyncDetails psd = ssw->sp->providerSettings->providerSettingsDetails();

    delete m_syncNepomuk;
    if(psd.providerInfo->providerId() == QLatin1String("zotero")) {
        m_syncNepomuk = new SyncZoteroNepomuk;
    }
    else if(psd.providerInfo->providerId() == QLatin1String("kbibtexfile")) {
        m_syncNepomuk = new SyncKBibTeXFile;
    }

    m_syncNepomuk->setProviderDetails(psd);

    m_syncNepomuk->setSystemLibrary(ssw->libraryManager->systemLibrary());
    m_syncNepomuk->setLibraryToSyncWith(ssw->libraryManager->systemLibrary());

    connect(m_syncNepomuk, SIGNAL(progress(int)), progressBar, SLOT(setValue(int)));
    connect(m_syncNepomuk, SIGNAL(progressStatus(QString)), infoLabel, SLOT(setText(QString)));

    connect(m_syncNepomuk, SIGNAL(askForDeletion(QList<SyncDetails>)), this, SLOT(popDeletionQuestion(QList<SyncDetails>)));
    connect(this, SIGNAL(deleteLocalFiles(bool)), m_syncNepomuk, SLOT(deleteLocalFiles(bool)));
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
}

void ProgressPage::popDeletionQuestion(QList<SyncDetails> items)
{
    int ret = KMessageBox::warningYesNo(0,i18n("%1 items are deleted on the server.\n\nDo you want to delete them locally too?.\nOtherwise they will be uploaded again.", items.size()));

    if(ret == KMessageBox::Yes) {
        emit deleteLocalFiles(true);
    }
    else {
        emit deleteLocalFiles(false);
    }
}

void ProgressPage::popMergeDialog(QList<SyncDetails> items)
{
    kDebug() << "show blocking merge dialog for " << items.size() << "items";

    KMessageBox::sorry( this, QLatin1String("TODO:: User selected entry merging, default to use server verion for now."), QLatin1String("Sorry") );

    StorageSyncWizard *ssw = qobject_cast<StorageSyncWizard *>(wizard());
    ProviderSyncDetails psd = ssw->sp->providerSettings->providerSettingsDetails();

    foreach(const SyncDetails &sd, items) {
        BibTexToNepomukPipe mergePipe;
        mergePipe.setSyncDetails(psd.url, psd.userName);
        mergePipe.merge(sd.syncResource, sd.externalResource, false);
    }

    emit mergeFinished();
}
