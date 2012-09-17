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

#ifndef NEWPROJECTWIZARD_H
#define NEWPROJECTWIZARD_H

#include <QtGui/QWizard>
//#include "onlinestorage/providersettings.h"
#include "nbibio/storageglobals.h"

#include <Akonadi/Collection>

namespace Ui {
    class NewProjectWizard;
}

class Library;
class GeneralPage;
class SyncPage;

/**
  * @brief QWizard Dialog to create a new project Library
  */
class NewProjectWizard : public QWizard
{
    Q_OBJECT

public:
    explicit NewProjectWizard(QWidget *parent = 0);
    virtual ~NewProjectWizard();

    Library * newLibrary();

protected:
    void done(int result);

private:
    Ui::NewProjectWizard *ui;
    Library *m_customLibrary;

    GeneralPage *gp;
    SyncPage *sp;
};

class ProjectGeneralSettings;

/**
  * @brief NewProjectWizard @c general @c page to specify the name and description
  */
class GeneralPage : public QWizardPage
{
    Q_OBJECT

public:
    GeneralPage(QWidget *parent = 0);

    QString projectTitle() const;
    QString projectDescription() const;

private slots:
    bool isComplete() const;

public:
    ProjectGeneralSettings *m_ui;
};

class QGroupBox;
class KUrlRequester;
class QLabel;
class QListWidget;
class QPushButton;

/**
  * @brief NewProjectWizard @c sync @c page to specify the folder and provider to sync with
  *
  * @todo adapt and reuse ProviderSyncSettings
  */
class SyncPage : public QWizardPage
{
    Q_OBJECT

public:
    SyncPage(QWidget *parent = 0);

    void initializePage();

signals:
//    void addContactCollection( const QList<ProviderSettings::AkonadiDetails> &contactCollections );
//    void addEventCollection( const QList<ProviderSettings::AkonadiDetails> &eventCollections );

private slots:
    bool isComplete() const;
    void updateFolderTextLabel(const QString &folder);
    void editProvider();
    void addProvider();
    void removeProvider();
    void akonadiContactCollectionFetched(const Akonadi::Collection::List &list);
    void akonadiEventCollectionFetched(const Akonadi::Collection::List &list);

private:
    void fetchAkonadiCollection();

public:
    QGroupBox *m_syncWithFolder;
    KUrlRequester *m_syncFolder;
    QLabel *m_syncFolderText;
    QGroupBox *m_syncWithOnlineStorage;
    QListWidget *m_syncList;
    QPushButton *m_addProvider;
    QPushButton *m_editProvider;
    QPushButton *m_removeProvider;
    QList<ProviderSyncDetails> m_psdList;
};

#endif // NEWPROJECTWIZARD_H
