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

#include "projectgeneralsettings.h"
#include "ui_projectgeneralsettings.h"

#include "core/projectsettings.h"

#include <KDE/KMessageBox>

#include <QtDebug>

ProjectGeneralSettings::ProjectGeneralSettings(QWidget *parent)
    : QWidget(parent),
    ui(new Ui::ProjectGeneralSettings)
{
    ui->setupUi(this);

    connect(ui->editTitle, SIGNAL(textEdited(QString)), this, SLOT(titleChanged(QString)));
    connect(ui->editDescription, SIGNAL(textChanged()), this, SIGNAL(contentChanged()));
}

ProjectGeneralSettings::~ProjectGeneralSettings()
{
    delete ui;
}

void ProjectGeneralSettings::setProjectSettings(ProjectSettings *ps)
{
    m_settings = ps;

    resetSettings();
}

void ProjectGeneralSettings::resetSettings()
{
    ui->editTitle->setText(m_settings->name());
    ui->editDescription->setText(m_settings->description());
}

void ProjectGeneralSettings::applySettings()
{
    if(ui->editTitle->text().isEmpty()) {
        KMessageBox::error(this, i18n("The project name can not be empty"), i18n("Settings error"));
        ui->editTitle->setText(m_settings->name());
        return;
    }

    m_settings->setName(ui->editTitle->text());
    m_settings->setDescription(ui->editDescription->document()->toPlainText());
}

void ProjectGeneralSettings::titleChanged(const QString &title)
{
    if(title != m_settings->name())
        emit contentChanged();
}
