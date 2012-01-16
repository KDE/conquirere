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

#include "exportsettings.h"

#include <KDE/KLocale>
#include <KDE/KPageWidgetItem>
#include <KDE/KConfigSkeleton>
#include <KDE/KIcon>

ConquirereSettingsDialog::ConquirereSettingsDialog(QWidget *parent)
    :KPageDialog(parent)
{
    setupPages();

    connect(this, SIGNAL(applyClicked()), this, SLOT(applyChanges()));
}

ConquirereSettingsDialog::~ConquirereSettingsDialog()
{

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

    m_exportSettings = new ExportSettings();
    connect(this, SIGNAL(applyClicked()), m_exportSettings, SLOT(applySettings()));
    connect(this, SIGNAL(resetClicked()), m_exportSettings, SLOT(resetSettings()));
    connect(this, SIGNAL(okClicked()), m_exportSettings, SLOT(applySettings()));
    connect(m_exportSettings, SIGNAL(contentChanged()), this, SLOT(contentChanged()));

    KPageWidgetItem *pwitem = addPage( m_exportSettings, i18n( "Export Settings" ) );
    pwitem->setIcon( KIcon( "document-export" ) );

}
