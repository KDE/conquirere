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

#include "projectsettingsdialog.h"

#include "core/projectsettings.h"
#include "projectgeneralsettings.h"
#include "projectsyncsettings.h"

#include <KDE/KPageWidgetItem>
#include <KDE/KConfigSkeleton>
#include <KDE/KIcon>
#include <QLabel>

ProjectSettingsDialog::ProjectSettingsDialog(QWidget *parent)
    :KPageDialog(parent)
{
    setupPages();

    connect(this, SIGNAL(applyClicked()), this, SLOT(applyChanges()));
}

ProjectSettingsDialog::~ProjectSettingsDialog()
{
}

void ProjectSettingsDialog::setProjectSettings(ProjectSettings *ps)
{
    m_pgs->setProjectSettings(ps);
    m_pss->setProjectSettings(ps);
}

void ProjectSettingsDialog::applyChanges()
{
    enableButtonApply(false);
}

void ProjectSettingsDialog::setupPages()
{
    setFaceType( List );
    setWindowTitle(i18n("Preferences"));
    setButtons(Reset | Ok | Apply | Cancel);
    setDefaultButton(Ok);
    enableButtonApply(false);
    setModal(true);
    showButtonSeparator(true);

    m_pgs = new ProjectGeneralSettings();
    connect(this, SIGNAL(applyClicked()), m_pgs, SLOT(applySettings()));
    connect(this, SIGNAL(resetClicked()), m_pgs, SLOT(resetSettings()));
    connect(this, SIGNAL(okClicked()), m_pgs, SLOT(applySettings()));
    connect(m_pgs, SIGNAL(contentChanged()), this, SLOT(contentChanged()));

    KPageWidgetItem *pwitem = addPage( m_pgs, i18n( "General Settings" ) );
    pwitem->setIcon( KIcon( "preferences-desktop-personal" ) );

    m_pss = new ProjectSyncSettings();
    connect(this, SIGNAL(applyClicked()), m_pss, SLOT(applySettings()));
    connect(this, SIGNAL(resetClicked()), m_pss, SLOT(resetSettings()));
    connect(this, SIGNAL(okClicked()), m_pss, SLOT(applySettings()));
    connect(m_pss, SIGNAL(contentChanged()), this, SLOT(contentChanged()));

    pwitem = addPage( m_pss, i18n( "Sync Settings" ) );
    pwitem->setHeader( i18n( "Sync Settings" ) );
    pwitem->setIcon( KIcon( "view-refresh" ) );
}

void ProjectSettingsDialog::contentChanged()
{
    enableButtonApply(true);
}
