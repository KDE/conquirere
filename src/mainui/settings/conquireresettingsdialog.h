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

#ifndef CONQUIRERESETTINGSDIALOG_H
#define CONQUIRERESETTINGSDIALOG_H

#include <KDE/KPageDialog>

class AppearanceSettings;
class LibrarySettings;
class ExportSettings;
class SystemSyncSettings;
class ProjectSettings;

class ConquirereSettingsDialog : public KPageDialog
{
    Q_OBJECT
public:
    explicit ConquirereSettingsDialog(QWidget *parent = 0);
    virtual ~ConquirereSettingsDialog();

    void setProjectSettings(ProjectSettings *ps);

public slots:
    void contentChanged();

private slots:
    void applyChanges();

private:
    void setupPages();

    AppearanceSettings *m_appearanceSettings;
    LibrarySettings    *m_librarySettings;
    ExportSettings     *m_exportSettings;
    SystemSyncSettings *m_systemSyncSettings;

};

#endif // CONQUIRERESETTINGSDIALOG_H
