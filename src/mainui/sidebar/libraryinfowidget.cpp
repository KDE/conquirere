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

#include "libraryinfowidget.h"
#include "ui_libraryinfowidget.h"

#include "core/library.h"
#include "core/projectsettings.h"
#include "nbibio/backgroundsync.h"

#include "core/librarymanager.h"

#include "mainui/dialogs/newprojectwizard.h"

#include <KDE/KProgressDialog>

LibraryInfoWidget::LibraryInfoWidget(QWidget *parent)
    : SidebarComponent(parent)
    , ui(new Ui::LibraryInfoWidget)
    , m_curLibrary(0)
    , m_kpd(0)
{
    ui->setupUi(this);

    setupUI();
}

LibraryInfoWidget::~LibraryInfoWidget()
{
    delete ui;
    delete m_kpd;
}

Nepomuk2::Resource LibraryInfoWidget::resource()
{
    qWarning() << "this shouldn't be used .... LibraryInfoWidget can't return a resource";
    Nepomuk2::Resource empty;
    return empty;
}

void LibraryInfoWidget::setLibrary(Library *p)
{
    m_curLibrary = p;

    if(!p)
        return;

    ui->nameLabel->setText(p->settings()->name());
    ui->descriptionLabel->setText(p->settings()->description());

    if(p->libraryType() == BibGlobals::Library_System) {
        ui->deleteButton->setEnabled(false);
        ui->closeButton->setEnabled(false);
    }
    else {
        ui->deleteButton->setEnabled(true);
        ui->closeButton->setEnabled(true);
    }

    if(p->settings()->allProviderSyncDetails().isEmpty()) {
        ui->syncButton->setEnabled(false);
    }
    else {
        ui->syncButton->setEnabled(true);
    }
}

void LibraryInfoWidget::setResource(Nepomuk2::Resource & resource)
{
    Q_UNUSED(resource);
}

void LibraryInfoWidget::newButtonClicked()
{
    NewProjectWizard npw;

    int ret = npw.exec();

    if(ret == QDialog::Accepted) {
        libraryManager()->addLibrary( npw.newLibrary() );
    }
}

void LibraryInfoWidget::deleteButtonClicked()
{
    deleteLibrary();
}

void LibraryInfoWidget::importData()
{
    libraryManager()->doImportFile(m_curLibrary);
}

void LibraryInfoWidget::exportData()
{
    libraryManager()->doExportFile(m_curLibrary);
}

void LibraryInfoWidget::openSettings()
{
    libraryManager()->doOpenSettings(m_curLibrary);
}

void LibraryInfoWidget::syncData()
{
    m_kpd = new KProgressDialog;
    m_kpd->setMinimumWidth(400);
    BackgroundSync *backgroundSyncManager = new BackgroundSync;
    backgroundSyncManager->setLibraryManager(libraryManager());

    backgroundSyncManager->setLibraryToSyncWith(m_curLibrary);

    connect(backgroundSyncManager, SIGNAL(progress(int)), this, SLOT(setSyncProgress(int)));
    connect(backgroundSyncManager, SIGNAL(progressStatus(QString)), this, SLOT(setSyncStatus(QString)));
    connect(backgroundSyncManager, SIGNAL(allSyncTargetsFinished()), this, SLOT(syncFinished()));
    connect(m_kpd, SIGNAL(cancelClicked()), backgroundSyncManager, SLOT(cancelSync()));

    backgroundSyncManager->startSync();

    m_kpd->exec();
}

void LibraryInfoWidget::closeLibrary()
{
    libraryManager()->closeLibrary(m_curLibrary);
}

void LibraryInfoWidget::deleteLibrary()
{
    libraryManager()->deleteLibrary(m_curLibrary);
}

void LibraryInfoWidget::setSyncProgress(int value)
{
    if(m_kpd)
        m_kpd->progressBar()->setValue(value);
//    kDebug() << "new value" << value;
}

void LibraryInfoWidget::setSyncStatus(const QString &status)
{
    if(m_kpd)
        m_kpd->setLabelText(status);
//    kDebug() << "new status" << status;
}

void LibraryInfoWidget::syncFinished()
{
    delete m_kpd;
    m_kpd = 0;

    BackgroundSync *bs = qobject_cast<BackgroundSync *>(sender());
    if(bs)
        delete bs;
}

void LibraryInfoWidget::setupUI()
{
    ui->editButton->setIcon(KIcon("preferences-other"));
    ui->importButton->setIcon(KIcon("document-import"));
    ui->exportButton->setIcon(KIcon("document-export"));
    ui->syncButton->setIcon(KIcon("view-refresh"));
    ui->closeButton->setIcon(KIcon("document-close"));
    ui->deleteButton->setIcon(KIcon("document-close"));

    connect(ui->editButton, SIGNAL(clicked()), this, SLOT(openSettings()));
    connect(ui->importButton, SIGNAL(clicked()), this, SLOT(importData()));
    connect(ui->exportButton, SIGNAL(clicked()), this, SLOT(exportData()));
    connect(ui->syncButton, SIGNAL(clicked()), this, SLOT(syncData()));
    connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(closeLibrary()));
    connect(ui->deleteButton, SIGNAL(clicked()), this, SLOT(deleteLibrary()));
}
