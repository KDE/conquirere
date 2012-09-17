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
    , m_syncNepomuk(0)
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
/*
    //TODO: add NepomukSyncClient
    // configure the provider via provider id
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

    connect(m_syncNepomuk, SIGNAL(askForLocalDeletion(QList<SyncDetails>)), this, SLOT(popLocalDeletionQuestion(QList<SyncDetails>)));
    connect(this, SIGNAL(deleteLocalFiles(bool)), m_syncNepomuk, SLOT(deleteLocalFiles(bool)));
    connect(m_syncNepomuk, SIGNAL(askForServerDeletion(QList<SyncDetails>)), this, SLOT(popServerDeletionQuestion(QList<SyncDetails>)));
    connect(this, SIGNAL(deleteServerFiles(bool)), m_syncNepomuk, SLOT(deleteServerFiles(bool)));
    connect(m_syncNepomuk, SIGNAL(askForGroupRemoval(QList<SyncDetails>)), this, SLOT(popGroupRemovalQuestion(QList<SyncDetails>)));
    connect(this, SIGNAL(removeGroupFiles(bool)), m_syncNepomuk, SLOT(deleteFromGroup(bool)));

    connect(m_syncNepomuk, SIGNAL(userMerge(QList<SyncDetails>)), this, SLOT(popMergeDialog(QList<SyncDetails>)));
    connect(this, SIGNAL(mergeFinished()), m_syncNepomuk, SLOT(mergeFinished()));
    connect(m_syncNepomuk, SIGNAL(syncFinished()), this, SLOT(syncFinished()));

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
    */
}
/*
void ProgressPage::popLocalDeletionQuestion(QList<SyncDetails> items)
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

void ProgressPage::popServerDeletionQuestion(QList<SyncDetails> items)
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

void ProgressPage::popGroupRemovalQuestion(QList<SyncDetails> items)
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


void ProgressPage::popMergeDialog(QList<SyncDetails> items)
{
    ItemMergeDialog imd;

    StorageSyncWizard *ssw = qobject_cast<StorageSyncWizard *>(wizard());

    ProviderSyncDetails psd = ssw->sp->providerSettings->providerSettingsDetails();

    imd.setProviderDetails(psd);

    imd.setItemsToMerge(items);

    imd.exec();

    emit mergeFinished();
}
*/

void ProgressPage::syncFinished()
{
    isSyncFinished = true;
    emit completeChanged();
}
