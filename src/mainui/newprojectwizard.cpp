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

#include "../core/library.h"

#include <KDE/KLineEdit>
#include <KDE/KUrlRequester>
#include <KDE/KGlobalSettings>
#include <KDE/KTextEdit>
#include <KDE/KComboBox>

#include <QtGui/QLabel>
#include <QtGui/QCheckBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QFormLayout>

#include <QDebug>

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

Library *NewProjectWizard::newLibrary()
{
    return customLibrary;
}

void NewProjectWizard::done(int result)
{
    if(result == QDialog::Accepted) {
        customLibrary = new Library(Library_Project);
        customLibrary->setName(gp->projectTitle->text());
        customLibrary->setDescription(gp->projectDescription->toPlainText());

//        if(sp->syncWithFolder->isChecked()) {
//            customLibrary->setPath(sp->syncFolder->text());
//        }

        customLibrary->createLibrary();

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
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QLabel *labelTitel = new QLabel(i18n("Research Title:"));
    mainLayout->addWidget(labelTitel);
    projectTitle = new KLineEdit(this);
    registerField("projectName*", projectTitle);
    mainLayout->addWidget(projectTitle);
    QLabel *labelDescription = new QLabel(i18n("Description:"));
    mainLayout->addWidget(labelDescription);
    projectDescription = new KTextEdit(this);
    registerField("projectDescription", projectDescription);
    mainLayout->addWidget(projectDescription);

    connect(projectTitle, SIGNAL(textChanged(QString)), this, SLOT(isComplete()));

    setTitle(i18n("New research"));
    setSubTitle(i18n("Create a new research topic and some initial settings"));
    //setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/watermark1.png"));

    setLayout(mainLayout);
}

bool GeneralPage::isComplete() const
{
    if(projectTitle->text().isEmpty())
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
    syncWithFolder = new QGroupBox(this);
    syncWithFolder->setCheckable(true);
    syncWithFolder->setChecked(false);
    syncWithFolder->setTitle(i18n("Use folder on disk"));
    syncWithFolder->setToolTip(i18n("Connects all document in this folder automatically to the project"));
    registerField("syncWithFolder", syncWithFolder);

    QVBoxLayout *groupBoxLayout = new QVBoxLayout(syncWithFolder);

    QHBoxLayout *horizontalLayout = new QHBoxLayout();
    QLabel *label1 = new QLabel(syncWithFolder);
    label1->setText(i18n("Folder:"));
    horizontalLayout->addWidget(label1);
    syncFolder = new KUrlRequester(syncWithFolder);
    syncFolder->setMode(KFile::Directory);
    syncFolder->setUrl(KGlobalSettings::documentPath());
    registerField("folderToSync*", syncFolder);
    horizontalLayout->addWidget(syncFolder);
    groupBoxLayout->addLayout(horizontalLayout);

    syncFolderBibtex = new QCheckBox(syncWithFolder);
    syncFolderBibtex->setText(i18n("Synchronize BibTeX file"));
    registerField("autoFolderBibTeXSync", syncFolderBibtex);
    groupBoxLayout->addWidget(syncFolderBibtex);

    mainLayout->addWidget(syncWithFolder);

    //#########################################
    // sync with online storage
    syncWithOnlineStorage = new QGroupBox(this);
    syncWithOnlineStorage->setCheckable(true);
    syncWithOnlineStorage->setChecked(false);
    registerField("syncWithOnlineStorage", syncWithOnlineStorage);
    syncWithOnlineStorage->setTitle(i18n("Sync with online storage"));
    syncWithOnlineStorage->setToolTip(i18n("Syncs the documents and publications to an online storage"));

    QFormLayout *formLayout = new QFormLayout(syncWithOnlineStorage);

    QLabel *label2 = new QLabel(syncWithOnlineStorage);
    label2->setText(i18n("Service:"));
    formLayout->setWidget(0, QFormLayout::LabelRole, label2);

    syncOnlineServce = new KComboBox(syncWithOnlineStorage);
    syncOnlineServce->insertItems(0, QStringList()
                                  << i18n("Zotero")
                                  << i18n("ownCloud")
                                  << i18n("ftp")
                                  );
    registerField("syncOnlineServce", syncOnlineServce);
    formLayout->setWidget(0, QFormLayout::FieldRole, syncOnlineServce);

    QLabel *label3 = new QLabel(syncWithOnlineStorage);
    label3->setText("todo...");
    formLayout->setWidget(1, QFormLayout::FieldRole, label3);

    mainLayout->addWidget(syncWithOnlineStorage);

    setTitle(i18n("Research Settings"));
    setSubTitle(i18n("How to synchronize the data"));
    //setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/watermark1.png"));

    setLayout(mainLayout);
}

bool SyncPage::isComplete() const
{
    if(syncWithFolder->isChecked()) {
        if(syncFolder->text().isEmpty()) {
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
