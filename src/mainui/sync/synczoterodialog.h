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

#ifndef SYNCZOTERODIALOG_H
#define SYNCZOTERODIALOG_H

#include <QDialog>

#include "onlinestorage/storageglobals.h"
#include "nbibio/synczoteronepomuk.h"

#include <Akonadi/Collection>
#include <KWallet/Wallet>

namespace Ui {
    class SyncZoteroDialog;
}

class QAbstractButton;
class SyncZoteroNepomuk;
class ReadFromZotero;
class KProgressDialog;
class File;
class KDialog;
class MergeWidget;

class SyncZoteroDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SyncZoteroDialog(QWidget *parent = 0);
    ~SyncZoteroDialog();

private slots:
    void collectionsReceived( const Akonadi::Collection::List& );

    void fetchCollection();
    void processCollectionResults(QList<CollectionInfo> collectionList);

    void clicked(QAbstractButton* button);
    void setProgressStatus(const QString &status);

    void checkWalletForPwd();
    void checkSyncMode(int mode);

    void popDeletionQuestion(QList<SyncDetails> items);
    void popMergeDialog(QList<SyncDetails> items);

signals:
    void deleteLocalFiles(bool deleteThem);
    void mergedResults(QList<SyncDetails> items);
    void mergeFinished();

private:
    Ui::SyncZoteroDialog *ui;
    KWallet::Wallet* m_wallet;

    SyncZoteroNepomuk *m_szn;
    ReadFromZotero *m_rfz;

    KProgressDialog *m_pdlg;
    KDialog *m_MergeDialog;
    MergeWidget *m_mw;
};

#endif // SYNCZOTERODIALOG_H
