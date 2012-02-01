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

#include "newprojectwizard.h"
#include "ui_newprojectwizard.h"

#include "nbibio/nbibsync.h"
#include "nbibio/zotero/synczoteronepomuk.h"

#include "core/library.h"
#include "core/projectsettings.h"
#include "../settings/projectgeneralsettings.h"

#include "onlinestorage/storageinfo.h"

#include <KDE/KLineEdit>
#include <KDE/KUrlRequester>
#include <KDE/KGlobalSettings>
#include <KDE/KTextEdit>
#include <KDE/KComboBox>
#include <KDE/KDialog>
#include <KDE/KDebug>

#include <Akonadi/CollectionFetchJob>
#include <Akonadi/CollectionFetchScope>

#include <QtGui/QLabel>
#include <QtGui/QCheckBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QFormLayout>
#include <QtGui/QListWidget>
#include <QtCore/QList>

#include <QtCore/QDebug>

NewProjectWizard::NewProjectWizard(QWidget *parent)
    : QWizard(parent),
    ui(new Ui::NewProjectWizard)
{
    gp = new GeneralPage;
    sp = new SyncPage;

    addPage(gp);
    addPage(sp);

    ui->setupUi(this);
}

NewProjectWizard::~NewProjectWizard()
{
    delete gp;
    delete sp;
    delete ui;
}

Library * NewProjectWizard::newLibrary()
{
    return m_customLibrary;
}

void NewProjectWizard::done(int result)
{
    if(result == QDialog::Accepted) {

        QString path;
        if(sp->m_syncWithFolder->isChecked()) {
            path = sp->m_syncFolder->text() + QLatin1String("/") + gp->projectTitle();
        }

        Nepomuk::Thing newLibraryThing = Library::createLibrary(gp->projectTitle(),
                                                                gp->projectDescription(),
                                                                path);

        m_customLibrary = new Library;
        m_customLibrary->loadLibrary(newLibraryThing);
        foreach(const ProviderSyncDetails& psd, sp->m_psdList) {
            m_customLibrary->settings()->setProviderSyncDetails(psd, QString());
        }
    }

    QWizard::done(result);
}

/*
 * 1. General page
 *
 * Selection of the file and options
 */
GeneralPage::GeneralPage(QWidget *parent)
    : QWizardPage(parent)
{
    setFinalPage(true);

    m_ui = new ProjectGeneralSettings(this);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_ui);

    connect(m_ui, SIGNAL(projectNameChanged(QString)), this, SIGNAL(completeChanged()));

    setTitle(i18nc("New project wizard generalpage","New project"));
    setSubTitle(i18n("Create a new research topic and some initial settings"));
    //setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/watermark1.png"));

    setLayout(mainLayout);
}

QString GeneralPage::projectTitle() const
{
    return m_ui->projectTitle();
}

QString GeneralPage::projectDescription() const
{
    return m_ui->projectDescription();
}

bool GeneralPage::isComplete() const
{
    if(m_ui->projectTitle().isEmpty())
        return false;
    else
        return true;
}

/*
 * 2. sync page
 *
 */
SyncPage::SyncPage(QWidget *parent)
    : QWizardPage(parent)
{
    setFinalPage(true);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    //#########################################
    // sync with folder
    m_syncWithFolder = new QGroupBox(this);
    m_syncWithFolder->setCheckable(true);
    m_syncWithFolder->setChecked(false);
    m_syncWithFolder->setTitle(i18n("Use folder on disk"));
    m_syncWithFolder->setToolTip(i18n("Connects all documents in this folder automatically to the project and saves the pfoject settings there"));
    registerField("syncWithFolder", m_syncWithFolder);

    QVBoxLayout *groupBoxLayout = new QVBoxLayout(m_syncWithFolder);

    QHBoxLayout *horizontalLayout = new QHBoxLayout();
    QLabel *label1 = new QLabel(m_syncWithFolder);
    label1->setText(i18n("Folder:"));
    horizontalLayout->addWidget(label1);
    m_syncFolder = new KUrlRequester(m_syncWithFolder);
    m_syncFolder->setMode(KFile::Directory);
    m_syncFolder->setUrl(KGlobalSettings::documentPath());
    connect(m_syncFolder, SIGNAL(textChanged(QString)), this, SLOT(updateFolderTextLabel(QString)));
    registerField("folderToSync*", m_syncFolder);
    horizontalLayout->addWidget(m_syncFolder);
    groupBoxLayout->addLayout(horizontalLayout);

    m_syncFolderText = new QLabel(m_syncWithFolder);
    groupBoxLayout->addWidget(m_syncFolderText);

    mainLayout->addWidget(m_syncWithFolder);

    //#########################################
    // sync with online storage
    m_syncWithOnlineStorage = new QGroupBox(this);
    m_syncWithOnlineStorage->setTitle(i18n("Synchronize Settings"));
    m_syncWithOnlineStorage->setToolTip(i18n("Syncs the documents and publications automatically in the background"));

    QVBoxLayout *formLayout = new QVBoxLayout(m_syncWithOnlineStorage);
    m_syncList = new QListWidget;
    formLayout->addWidget(m_syncList);

    QHBoxLayout *editLayout = new QHBoxLayout(m_syncWithOnlineStorage);

    m_editProvider = new QPushButton(KIcon("document-edit"), i18n("edit"));
    connect(m_editProvider, SIGNAL(clicked()), this, SLOT(editProvider()));
    m_addProvider = new QPushButton(KIcon("list-add"), i18n("add"));
    connect(m_addProvider, SIGNAL(clicked()), this, SLOT(addProvider()));
    m_removeProvider = new QPushButton(KIcon("list-remove"), i18n("remove"));
    connect(m_removeProvider, SIGNAL(clicked()), this, SLOT(removeProvider()));

    editLayout->addWidget(m_editProvider);
    editLayout->addStretch(0);
    editLayout->addWidget(m_addProvider);
    editLayout->addWidget(m_removeProvider);
    formLayout->addLayout(editLayout);

    mainLayout->addWidget(m_syncWithOnlineStorage);

    setTitle(i18n("Research Settings"));
    setSubTitle(i18n("How to synchronize the data"));
    //setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/watermark1.png"));

    setLayout(mainLayout);
}

void SyncPage::initializePage()
{
    updateFolderTextLabel(KGlobalSettings::documentPath());
}

bool SyncPage::isComplete() const
{
    if(m_syncWithFolder->isChecked()) {
        if(m_syncFolder->text().isEmpty()) {
            return false;
        }
        else {
            return true;
        }
    }
    else {
        return true;
    }
}

void SyncPage::updateFolderTextLabel(const QString &folder)
{
    if(folder.isEmpty()) {
        m_syncFolderText->clear();
    }
    else {
        QString folderAndName= folder + QLatin1String("/") + field("projectName").toString();
        folderAndName.replace(QLatin1String("//"), QLatin1String("/"));
        m_syncFolderText->setText(folderAndName);
    }
}

void SyncPage::editProvider()
{
    int selectedProvider = m_syncList->currentIndex().row();
    ProviderSyncDetails oldPsd = m_psdList.at(selectedProvider);

    KDialog dlg;
    ProviderSettings ps(&dlg, true);
    ps.setProviderSettingsDetails(oldPsd);
    dlg.setMainWidget(&ps);

    connect(this, SIGNAL(addContactCollection(QList<ProviderSettings::AkonadiDetails>)), &ps, SLOT(setAkonadiContactDetails(QList<AkonadiDetails>)));
    connect(this, SIGNAL(addEventCollection(QList<ProviderSettings::AkonadiDetails>)), &ps, SLOT(setAkonadiEventDetails(QList<AkonadiDetails>)));

    fetchAkonadiCollection();

    int ret = dlg.exec();

    if(ret == KDialog::Accepted) {
        ProviderSyncDetails newPsd = ps.providerSettingsDetails();
        m_psdList.replace(selectedProvider, newPsd);
        QListWidgetItem *qlwi = m_syncList->item(selectedProvider);
        if(qlwi) {
            QString itemName = newPsd.providerInfo->providerName();
            qlwi->setText(itemName);
        }
        ps.savePasswordInKWallet();
    }
}

void SyncPage::addProvider()
{
    KDialog dlg;

    ProviderSettings ps(&dlg, true);
    dlg.setMainWidget(&ps);

    connect(this, SIGNAL(addContactCollection(QList<ProviderSettings::AkonadiDetails>)), &ps, SLOT(setAkonadiContactDetails(QList<ProviderSettings::AkonadiDetails>)));
    connect(this, SIGNAL(addEventCollection(QList<ProviderSettings::AkonadiDetails>)), &ps, SLOT(setAkonadiEventDetails(QList<ProviderSettings::AkonadiDetails>)));

    fetchAkonadiCollection();

    int ret = dlg.exec();

    if(ret == KDialog::Accepted) {
        ProviderSyncDetails newPsd = ps.providerSettingsDetails();
        m_psdList.append(newPsd);
        QListWidgetItem *qlwi = new QListWidgetItem(newPsd.providerInfo->providerIcon(), newPsd.providerInfo->providerName());
        m_syncList->addItem(qlwi);
        ps.savePasswordInKWallet();
    }
}

void SyncPage::removeProvider()
{
    int selectedProvider = m_syncList->currentIndex().row();

    QListWidgetItem *qlwi = m_syncList->item(selectedProvider);
    m_syncList->removeItemWidget(qlwi);
    delete qlwi;
    m_psdList.removeAt(selectedProvider);
}

void SyncPage::fetchAkonadiCollection()
{
    // fetching all collections containing contacts recursively, starting at the root collection
    Akonadi::CollectionFetchJob *job = new Akonadi::CollectionFetchJob( Akonadi::Collection::root(), Akonadi::CollectionFetchJob::Recursive, this );
    job->fetchScope().setContentMimeTypes( QStringList() << "application/x-vnd.kde.contactgroup" );
    connect( job, SIGNAL(collectionsReceived(Akonadi::Collection::List)),
             this, SLOT(akonadiContactCollectionFetched(Akonadi::Collection::List)) );

    Akonadi::CollectionFetchJob *job2 = new Akonadi::CollectionFetchJob( Akonadi::Collection::root(), Akonadi::CollectionFetchJob::Recursive, this );
    job2->fetchScope().setContentMimeTypes( QStringList() << "x-vnd.akonadi.calendar.event" << "application/x-vnd.akonadi.calendar.todo" );
    connect( job2, SIGNAL(collectionsReceived(Akonadi::Collection::List)),
             this, SLOT(akonadiEventCollectionFetched(Akonadi::Collection::List)) );
}

void SyncPage::akonadiContactCollectionFetched(const Akonadi::Collection::List &list)
{
    QList<ProviderSettings::AkonadiDetails> contactList;
    foreach(const Akonadi::Collection & c, list) {
        ProviderSettings::AkonadiDetails ad;
        ad.collectionName = c.name();
        ad.collectionID = c.id();
        contactList.append(ad);
    }

    emit addContactCollection(contactList);
}

void SyncPage::akonadiEventCollectionFetched(const Akonadi::Collection::List &list)
{
    QList<ProviderSettings::AkonadiDetails> evntList;
    foreach(const Akonadi::Collection & c, list) {
        ProviderSettings::AkonadiDetails ad;
        ad.collectionName = c.name();
        ad.collectionID = c.id();
        evntList.append(ad);
    }

    emit addEventCollection(evntList);
}
