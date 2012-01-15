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

#ifndef PROJECTSETTINGSDIALOG_H
#define PROJECTSETTINGSDIALOG_H

#include <KDE/KPageDialog>

#include "core/projectsettings.h"

class ProjectGeneralSettings;
class ProjectSyncSettings;

class ProjectSettingsDialog : public KPageDialog
{
    Q_OBJECT
public:
    explicit ProjectSettingsDialog(QWidget *parent = 0);
    virtual ~ProjectSettingsDialog();

    void setProjectSettings(ProjectSettings *ps);

public slots:
    void contentChanged();

private slots:
    void applyChanges();

private:
    void setupPages();

    ProjectGeneralSettings *m_pgs;
    ProjectSyncSettings *m_pss;
};

#endif // PROJECTSETTINGSDIALOG_H
