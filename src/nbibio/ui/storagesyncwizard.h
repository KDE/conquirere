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

#ifndef STORAGESYNCWIZARD_H
#define STORAGESYNCWIZARD_H

#include <QtGui/QWizard>

#include <Akonadi/Collection>

//#include "nbibio/nbibsync.h"

namespace Ui {
    class StorageSyncWizard;
}

class SettingsPage;
class ProgressPage;
class LibraryManager;

class StorageSyncWizard : public QWizard
{
    Q_OBJECT

public:
    explicit StorageSyncWizard(QWidget *parent = 0);
    ~StorageSyncWizard();

    void setLibraryManager(LibraryManager *lm);

public:
    Ui::StorageSyncWizard *ui;

    SettingsPage *sp;
    ProgressPage *pp;

    LibraryManager *libraryManager;
};

class ProviderSettings;


/**
  * Settings page
  */
class SettingsPage : public QWizardPage
{
    Q_OBJECT

public:
    SettingsPage(QWidget *parent = 0);

private slots:
    bool isComplete() const;

public:
    void setupUi();
    ProviderSettings *providerSettings;
};

class QProgressBar;
class QLabel;
class NBibSync;

/**
  * Progress page
  */
class ProgressPage : public QWizardPage
{
    Q_OBJECT

public:
    ProgressPage(QWidget *parent = 0);

signals:
    void deleteLocalFiles(bool deleteThem);
    void deleteServerFiles(bool deleteThem);
    void removeGroupFiles(bool deleteThem);
    void mergeFinished();

private slots:
    bool isComplete() const;

//    void popLocalDeletionQuestion(QList<SyncDetails> items);
//    void popServerDeletionQuestion(QList<SyncDetails> items);
//    void popGroupRemovalQuestion(QList<SyncDetails> items);
//    void popMergeDialog(QList<SyncDetails> items);
    void syncFinished();

private:
    void setupUi();
    void initializePage();

private:
    QProgressBar *progressBar;
    QLabel *infoLabel;

    NBibSync *m_syncNepomuk;
    bool isSyncFinished;
};

#endif // STORAGESYNCWIZARD_H
