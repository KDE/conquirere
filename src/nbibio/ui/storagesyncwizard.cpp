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

#include "core/librarymanager.h"

#include "providersettings.h"
#include "nbibio/storageglobals.h"

#include "ui/itemdeletedialog.h"
#include "ui/itemmergedialog.h"

#include "nbibio/pipe/varianttonepomukpipe.h"
#include "nbibio/nepomuksyncclient.h"

#include <KDE/KMessageBox>
#include <KDE/KDebug>

#include <QtGui/QVBoxLayout>
#include <QtGui/QProgressBar>
#include <QtGui/QLabel>
#include <QtCore/QPointer>

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

void SettingsPage::setupUi()
{
    setTitle(i18n("Sync Storage Settings"));
    setSubTitle(i18n("Select the provider and specify the settings for it to sync your system library"));

    providerSettings = new ProviderSettings(this);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(providerSettings);
    setLayout(layout);

}


ProgressPage::ProgressPage(QWidget *parent)
    : QWizardPage(parent)
    , m_nepomukSyncClient(0)
    , isSyncFinished(false)
{
    setupUi();
}

bool ProgressPage::isComplete() const
{
    return isSyncFinished;
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

    // configure the provider via provider id
    delete m_nepomukSyncClient;
    m_nepomukSyncClient = new NepomukSyncClient();

    m_nepomukSyncClient->setProviderSettings(psd);

    connect(m_nepomukSyncClient, SIGNAL(progress(int)), progressBar, SLOT(setValue(int)));
    connect(m_nepomukSyncClient, SIGNAL(status(QString)), infoLabel, SLOT(setText(QString)));

    connect(m_nepomukSyncClient, SIGNAL(askForLocalDeletion(QList<Nepomuk2::Resource>)), this, SLOT(popLocalDeletionQuestion(QList<Nepomuk2::Resource>)) );
    connect(this, SIGNAL(deleteLocalFiles(bool)), m_nepomukSyncClient, SLOT(deleteLocalFiles(bool)));

    connect(m_nepomukSyncClient, SIGNAL(askForServerDeletion(QVariantList)), this, SLOT(popServerDeletionQuestion(QVariantList)));
    connect(this, SIGNAL(deleteServerFiles(bool)), m_nepomukSyncClient, SLOT(deleteServerFiles(bool)));

    connect(m_nepomukSyncClient, SIGNAL(userMerge(QList<SyncMergeDetails>)), this, SLOT(popMergeDialog(QList<SyncMergeDetails>)) );
    connect(this, SIGNAL(mergeFinished()), m_nepomukSyncClient, SLOT(mergeFinished()));

    connect(m_nepomukSyncClient, SIGNAL(finished()), this, SLOT(syncFinished()));

    QThread *newThread = new QThread;
    m_nepomukSyncClient->moveToThread(newThread);

    //what mode should we use?
    switch(psd.syncMode) {
    case Download_Only:
        connect(newThread, SIGNAL(started()),m_nepomukSyncClient, SLOT(importData()) );
        break;
    case Upload_Only:
        connect(newThread, SIGNAL(started()),m_nepomukSyncClient, SLOT(exportData()) );
        break;
    case Full_Sync:
        connect(newThread, SIGNAL(started()),m_nepomukSyncClient, SLOT(syncData()) );
        break;
    }

    newThread->start();
}

void ProgressPage::popLocalDeletionQuestion(const QList<Nepomuk2::Resource> &items)
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

void ProgressPage::popServerDeletionQuestion(const QVariantList &items)
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

void ProgressPage::popGroupRemovalQuestion(const QList<Nepomuk2::Resource> &items)
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

void ProgressPage::popMergeDialog(const QList<SyncMergeDetails> &items)
{
    ItemMergeDialog imd;

    StorageSyncWizard *ssw = qobject_cast<StorageSyncWizard *>(wizard());

    ProviderSyncDetails psd = ssw->sp->providerSettings->providerSettingsDetails();

    imd.setProviderDetails(psd);
    imd.setItemsToMerge(items);

    imd.exec();

    emit mergeFinished();
}


void ProgressPage::syncFinished()
{
    isSyncFinished = true;
    emit completeChanged();
}
