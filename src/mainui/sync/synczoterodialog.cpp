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
#include "ui_synczoterodialog.h"

#include "onlinestorage/zotero/readfromzotero.h"
#include "../../nbibio/synczoteronepomuk.h"

#include <kbibtex/file.h>
#include <kbibtex/findduplicates.h>
#include <kbibtex/findduplicatesui.h>

#include <Akonadi/CollectionFetchJob>
#include <Akonadi/CollectionFetchScope>
#include <KDE/KProgressDialog>

#include <QtCore/QThread>
#include <KDE/KMessageBox>
#include <KDE/KDialog>

#include <QtCore/QDebug>

SyncZoteroDialog::SyncZoteroDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SyncZoteroDialog)
    , m_pdlg(0)
    , m_MergeDialog(new KDialog)
    , m_mw(0)
{
    ui->setupUi(this);

    connect(ui->syncMode, SIGNAL(currentIndexChanged(int)), this, SLOT(checkSyncMode(int)));

    // fetching all collections containing emails recursively, starting at the root collection
    Akonadi::CollectionFetchJob *job = new Akonadi::CollectionFetchJob( Akonadi::Collection::root(), Akonadi::CollectionFetchJob::Recursive, this );
    job->fetchScope().setContentMimeTypes( QStringList() << "application/x-vnd.kde.contactgroup" );
    connect( job, SIGNAL(collectionsReceived(Akonadi::Collection::List)),
             this, SLOT(collectionsReceived(Akonadi::Collection::List)) );

    m_wallet = KWallet::Wallet::openWallet(KWallet::Wallet::NetworkWallet(),winId(), KWallet::Wallet::Synchronous);
    if(!m_wallet->hasFolder(QLatin1String("kbibtex"))) {
        m_wallet->createFolder(QLatin1String("kbibtex"));
    }
    m_wallet->setFolder(QLatin1String("kbibtex"));
    connect(ui->userID, SIGNAL(textEdited(QString)), this, SLOT(checkWalletForPwd()));

    m_rfz = new ReadFromZotero;
    connect(m_rfz, SIGNAL(collectionsInfo(QList<CollectionInfo>)), this, SLOT(processCollectionResults(QList<CollectionInfo>)));
    ui->fetchCollection->setIcon(KIcon(QLatin1String("svn-update")));
    connect(ui->fetchCollection, SIGNAL(clicked()), this, SLOT(fetchCollection()));

    m_szn = new SyncZoteroNepomuk;
}

SyncZoteroDialog::~SyncZoteroDialog()
{
    delete ui;
    delete m_szn;
    //delete m_rfz;
    delete m_wallet;
    delete m_MergeDialog;
    delete m_mw;
}

void SyncZoteroDialog::collectionsReceived( const Akonadi::Collection::List& list)
{
    foreach(const Akonadi::Collection & c, list) {
        ui->contactCollection->addItem(c.name(), c.id());
    }
}

void SyncZoteroDialog::fetchCollection()
{
    m_rfz->setUserName(ui->userID->text());
    m_rfz->setPassword(ui->apiKey->text());

    m_rfz->fetchCollections();
}

void SyncZoteroDialog::clicked(QAbstractButton* button)
{
    if(ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole)
    {
        m_szn->setUserName(ui->userID->text());
        m_szn->setPassword(ui->apiKey->text());
        int urlIndex = ui->libTypeSelection->currentIndex();
        QString url;
        if(urlIndex == 1) {
            url = QLatin1String("groups");
        }
        else {
            url = QLatin1String("users");
        }
        m_szn->setUrl(url);

        // TODO ask if we really should save this pwd
        QString pwdKey;
        pwdKey.append(QLatin1String("zotero"));
        pwdKey.append(QLatin1String(":"));
        pwdKey.append(ui->userID->text());
        pwdKey.append(QLatin1String(":"));
        pwdKey.append(url);
        m_wallet->writePassword(pwdKey, ui->apiKey->text());

        int curIndex = ui->collectionSelection->currentIndex();
        QString collectionID = ui->collectionSelection->itemData(curIndex).toString();
        m_szn->setCollection(collectionID);

        int curMergeIndex = ui->mergeMode->currentIndex();
        m_szn->mergeStrategy( (SyncZoteroNepomuk::MergeStrategy)curMergeIndex );

        delete m_pdlg;
        m_pdlg = new KProgressDialog;
        m_pdlg->setFocus();
        delete m_mw;
        m_mw = 0;

        connect(m_szn, SIGNAL(progress(int)), m_pdlg->progressBar(), SLOT(setValue(int)));
        connect(m_szn, SIGNAL(progressStatus(QString)), this, SLOT(setProgressStatus(QString)));

        connect(m_szn, SIGNAL(askForDeletion(int)), this, SLOT(popDeletionQuestion(int)));
        connect(this, SIGNAL(deleteLocalFiles(bool)), m_szn, SLOT(deleteLocalFiles(bool)));
        //connect(m_szn, SIGNAL(mergeResults(QList<EntryClique*>,File*)), this, SLOT(popMergeDialog(QList<EntryClique*>,File*)));
        //connect(this, SIGNAL(mergedResults(QList<EntryClique*>)), m_szn, SLOT(resultsMerged(QList<EntryClique*>)));

        QThread *newThread = new QThread;
        m_szn->moveToThread(newThread);

        //what mode should we use?
        int mode = ui->syncMode->currentIndex();
        if(mode == 0) {
            connect(newThread, SIGNAL(started()),m_szn, SLOT(startDownload()) );
        }
        else if(mode == 1) {
            connect(newThread, SIGNAL(started()),m_szn, SLOT(startUpload()) );
        }
        else {
            connect(newThread, SIGNAL(started()),m_szn, SLOT(startSync()) );
        }
        newThread->start();

        m_pdlg->exec();
    }
}

void SyncZoteroDialog::popDeletionQuestion(int items)
{
    int ret = KMessageBox::warningYesNo(0,i18n("%1 items are deleted on the server.\n\nDo you want to delete them locally too?.\nOtherwise they will be uploaded again.", items));

    if(ret == KMessageBox::Yes) {
        emit deleteLocalFiles(true);
    }
    else {
        emit deleteLocalFiles(false);
    }
}

void SyncZoteroDialog::popMergeDialog(QList<EntryClique*> cliques, File *bibCache)
{
    m_mw = new MergeWidget(bibCache, cliques, m_MergeDialog);
    m_MergeDialog->setMainWidget(m_mw);

    m_MergeDialog->exec(); // do not delete the KDialog yet, as this crashes the sync importer when we try to access QStrig refs that will be deleted

    emit mergedResults(cliques);
}

void SyncZoteroDialog::setProgressStatus(const QString &status)
{
    m_pdlg->setLabelText(status);
}

void SyncZoteroDialog::processCollectionResults(QList<CollectionInfo> collectionList)
{
    ui->collectionSelection->addItem(i18n("no collection"), QString());
    foreach(const CollectionInfo &ci, collectionList) {
        ui->collectionSelection->addItem(ci.name, ci.id);
    }
}

void SyncZoteroDialog::checkWalletForPwd()
{
    QString url;
    int urlIndex = ui->libTypeSelection->currentIndex();
    if(urlIndex == 1) {
        url = QLatin1String("groups");
    }
    else {
        url = QLatin1String("users");
    }

    QString pwdKey;
    pwdKey.append(QLatin1String("zotero"));
    pwdKey.append(QLatin1String(":"));
    pwdKey.append(ui->userID->text());
    pwdKey.append(QLatin1String(":"));
    pwdKey.append(url);

    if(m_wallet->hasEntry(pwdKey)) {
        QString pwd;
        m_wallet->readPassword(pwdKey, pwd);
        ui->apiKey->setText(pwd);
    }
}

void SyncZoteroDialog::checkSyncMode(int mode)
{
    if(mode == 1) {
        ui->addContactsToAkonadi->setEnabled(false);
    }
    else {
        ui->addContactsToAkonadi->setEnabled(true);
    }
}