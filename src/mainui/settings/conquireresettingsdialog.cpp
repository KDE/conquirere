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

#include "conquireresettingsdialog.h"

#include "appearancesettings.h"
#include "librarysettings.h"
#include "exportsettings.h"
#include "systemsyncsettings.h"

#include <KDE/KLocale>
#include <KDE/KPageWidgetItem>
#include <KDE/KConfigSkeleton>
#include <KDE/KIcon>

ConquirereSettingsDialog::ConquirereSettingsDialog(QWidget *parent)
    : KPageDialog(parent)
{
    setupPages();

    connect(this, SIGNAL(applyClicked()), this, SLOT(applyChanges()));
}

ConquirereSettingsDialog::~ConquirereSettingsDialog()
{

}

void ConquirereSettingsDialog::setProjectSettings(ProjectSettings *ps)
{
    m_systemSyncSettings->setProjectSettings(ps);
}

void ConquirereSettingsDialog::contentChanged()
{
    enableButtonApply(true);
}

void ConquirereSettingsDialog::applyChanges()
{
    enableButtonApply(false);
}

void ConquirereSettingsDialog::setupPages()
{
    setFaceType( List );
    setWindowTitle(i18n("Preferences"));
    setButtons(Reset | Ok | Apply | Cancel);
    setDefaultButton(Ok);
    enableButtonApply(false);
    setModal(true);
    showButtonSeparator(true);

    m_appearanceSettings = new AppearanceSettings();
    connect(this, SIGNAL(applyClicked()), m_appearanceSettings, SLOT(applySettings()));
    connect(this, SIGNAL(resetClicked()), m_appearanceSettings, SLOT(resetSettings()));
    connect(this, SIGNAL(okClicked()), m_appearanceSettings, SLOT(applySettings()));
    connect(m_appearanceSettings, SIGNAL(contentChanged()), this, SLOT(contentChanged()));

    KPageWidgetItem *asitem = addPage( m_appearanceSettings, i18n( "Appearance" ) );
    asitem->setIcon( KIcon( "view-choose" ) );

    m_librarySettings = new LibrarySettings();
    connect(this, SIGNAL(applyClicked()), m_librarySettings, SLOT(applySettings()));
    connect(this, SIGNAL(resetClicked()), m_librarySettings, SLOT(resetSettings()));
    connect(this, SIGNAL(okClicked()), m_librarySettings, SLOT(applySettings()));
    connect(m_librarySettings, SIGNAL(contentChanged()), this, SLOT(contentChanged()));

    KPageWidgetItem *libsitem = addPage( m_librarySettings, i18n( "Library" ) );
    libsitem->setIcon( KIcon( "folder-database" ) );

    m_exportSettings = new ExportSettings();
    connect(this, SIGNAL(applyClicked()), m_exportSettings, SLOT(applySettings()));
    connect(this, SIGNAL(resetClicked()), m_exportSettings, SLOT(resetSettings()));
    connect(this, SIGNAL(okClicked()), m_exportSettings, SLOT(applySettings()));
    connect(m_exportSettings, SIGNAL(contentChanged()), this, SLOT(contentChanged()));

    KPageWidgetItem *esitem = addPage( m_exportSettings, i18n( "Export" ) );
    esitem->setIcon( KIcon( "document-export" ) );

    m_systemSyncSettings = new SystemSyncSettings();
    connect(this, SIGNAL(applyClicked()), m_systemSyncSettings, SLOT(applySettings()));
    connect(this, SIGNAL(resetClicked()), m_systemSyncSettings, SLOT(resetSettings()));
    connect(this, SIGNAL(okClicked()), m_systemSyncSettings, SLOT(applySettings()));
    connect(m_systemSyncSettings, SIGNAL(contentChanged()), this, SLOT(contentChanged()));

    KPageWidgetItem *sssitem = addPage( m_systemSyncSettings, i18n( "Synchronize" ) );
    sssitem->setIcon( KIcon( "view-refresh" ) );

}
