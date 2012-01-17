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

#include "projectsyncsettings.h"
#include "ui_projectsyncsettings.h"

#include "core/projectsettings.h"

#include <KDE/KGlobalSettings>
#include <KDE/KMessageBox>

ProjectSyncSettings::ProjectSyncSettings(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ProjectSyncSettings)
{
    ui->setupUi(this);

    connect(ui->editFolder, SIGNAL(textChanged(QString)), this, SLOT(updateFolderTextLabel(QString)));
    connect(ui->syncFolderBox, SIGNAL(clicked()), this, SIGNAL(contentChanged()));
}

ProjectSyncSettings::~ProjectSyncSettings()
{
    delete ui;
}

void ProjectSyncSettings::setProjectSettings(ProjectSettings *ps)
{
    m_settings = ps;
    ui->providerWidget->setProjectSettings(ps);

    resetSettings();
}

void ProjectSyncSettings::resetSettings()
{
    //################################
    //# Folder settings
    QString baseDir = m_settings->projectDir();

    if(baseDir.isEmpty()) {
        ui->syncFolderBox->setChecked(false);
        baseDir = KGlobalSettings::documentPath();
    }
    else {
        ui->syncFolderBox->setChecked(true);
        baseDir.remove(m_settings->name());
    }

    ui->editFolder->setText(baseDir);

    //################################
    //# sync provider settings

    ui->providerWidget->resetSettings();
}

void ProjectSyncSettings::applySettings()
{
    // sync provider settings are saved automatically
    // here we just save folder sync settings

    if(ui->syncFolderBox->isChecked()) {
        m_settings->setProjectDir(ui->labelSelectedFolder->text());
    }
    else {
        if(!m_settings->projectDir().isEmpty()) {
            int ret = KMessageBox::questionYesNo(this, i18n("Do you want to delete the old project folder %1 and all its content?", m_settings->projectDir()),
            i18n("Delete project folder"));

            if(ret == KMessageBox::Yes) {
                m_settings->deleteProjectDir();
            }
        }

        m_settings->setProjectDir(QString());
    }


    ui->providerWidget->applySettings();
}

void ProjectSyncSettings::updateFolderTextLabel(const QString &folder)
{
    if(folder.isEmpty()) {
        ui->labelSelectedFolder->clear();
    }
    else {
        QString folderAndName= folder + QLatin1String("/") + m_settings->name();
        folderAndName.replace(QLatin1String("//"), QLatin1String("/"));
        ui->labelSelectedFolder->setText(folderAndName);
    }

    if(folder != m_settings->projectDir())
        emit contentChanged();
}
