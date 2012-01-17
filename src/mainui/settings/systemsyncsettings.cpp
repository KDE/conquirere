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

#include "systemsyncsettings.h"
#include "ui_systemsyncsettings.h"

#include "core/projectsettings.h"

#include "nbibio/conquirere.h"

SystemSyncSettings::SystemSyncSettings(QWidget *parent)
    :QWidget(parent)
    , ui(new Ui::SystemSyncSettings)
{
    ui->setupUi(this);
}

SystemSyncSettings::~SystemSyncSettings()
{
    delete ui;
}

void SystemSyncSettings::setProjectSettings(ProjectSettings *ps)
{
    m_settings = ps;
    ui->providerWidget->setProjectSettings(ps);

    resetSettings();
}

void SystemSyncSettings::resetSettings()
{
    ui->providerWidget->resetSettings();

    ui->cbEnableSync->setChecked(ConqSettings::enableBackgroundSync());
}

void SystemSyncSettings::applySettings()
{
    ui->providerWidget->applySettings();

    ConqSettings::setEnableBackgroundSync(ui->cbEnableSync->isChecked());
}
