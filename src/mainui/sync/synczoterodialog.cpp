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

#include "nbibio/synczoteronepomuk.h"
#include "nbibio/synckbibtexfile.h"
#include "nbibio/pipe/bibtextonepomukpipe.h"

#include <kbibtex/file.h>
#include <kbibtex/entry.h>
#include <kbibtex/findduplicates.h>
#include <kbibtex/findduplicatesui.h>

#include <KDE/KProgressDialog>

#include <QtCore/QThread>
#include <KDE/KMessageBox>
#include <KDE/KDialog>

#include <QtCore/QDebug>

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

    m_pdlg->exec();
}

void SyncZoteroDialog::popDeletionQuestion(QList<SyncDetails> items)
{
    int ret = KMessageBox::warningYesNo(0,i18n("%1 items are deleted on the server.\n\nDo you want to delete them locally too?.\nOtherwise they will be uploaded again.", items.size()));

    if(ret == KMessageBox::Yes) {
        emit deleteLocalFiles(true);
    }
    else {
        emit deleteLocalFiles(false);
    }
}

void SyncZoteroDialog::popMergeDialog(QList<SyncDetails> items)
{
    qDebug() << "show blocking merge dialog for " << items.size() << "items";

    KMessageBox::sorry( this, QLatin1String("TODO:: User selected entry merging, default to use server verion for now."), QLatin1String("Sorry") );

    ProviderSyncDetails psd = m_ps->providerSettingsDetails();

    foreach(const SyncDetails &sd, items) {
        BibTexToNepomukPipe mergePipe;
        mergePipe.setSyncDetails(psd.url, psd.userName);
        mergePipe.merge(sd.syncResource, sd.externalResource, false);
    }

    emit mergeFinished();
}

void SyncZoteroDialog::setProgressStatus(const QString &status)
{
    m_pdlg->setLabelText(status);
}
