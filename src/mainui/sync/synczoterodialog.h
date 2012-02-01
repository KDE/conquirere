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

#include <KDialog>

#include "onlinestorage/storageglobals.h"
#include "nbibio/nbibsync.h"

#include <Akonadi/Collection>

class QAbstractButton;
class KProgressDialog;
class ProviderSettings;
class NBibSync;
class MergeWidget;

class SyncZoteroDialog : public KDialog
{
    Q_OBJECT

public:
    explicit SyncZoteroDialog(QWidget *parent = 0);
    virtual ~SyncZoteroDialog();

    void setupWidget(ProviderSyncDetails psd);

private slots:
    void slotButtonClicked(int button);
    void setProgressStatus(const QString &status);

    void popLocalDeletionQuestion(QList<SyncDetails> items);
    void popServerDeletionQuestion(QList<SyncDetails> items);
    void popGroupRemovalQuestion(QList<SyncDetails> items);
    void popMergeDialog(QList<SyncDetails> items);

    void akonadiContactCollectionFetched(const Akonadi::Collection::List &list);
    void akonadiEventCollectionFetched(const Akonadi::Collection::List &list);

signals:
    void deleteLocalFiles(bool deleteThem);
    void deleteServerFiles(bool deleteThem);
    void removeGroupFiles(bool deleteThem);
    void mergedResults(QList<SyncDetails> items);
    void mergeFinished();

private:
    KDialog *m_mainDialog;
    ProviderSettings *m_ps;
    NBibSync *m_syncNepomuk;

    KProgressDialog *m_pdlg;
    KDialog *m_MergeDialog;
    MergeWidget *m_mw;
};

#endif // SYNCZOTERODIALOG_H
