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

#ifndef PROJECTGENERALSETTINGS_H
#define PROJECTGENERALSETTINGS_H

#include <QtGui/QWidget>

namespace Ui {
    class ProjectGeneralSettings;
}

class ProjectSettings;

class ProjectGeneralSettings : public QWidget
{
    Q_OBJECT

public:
    explicit ProjectGeneralSettings(QWidget *parent = 0);
    ~ProjectGeneralSettings();

    void setProjectSettings(ProjectSettings *ps);

    QString projectTitle() const;
    QString projectDescription() const;

signals:
    void contentChanged();
    void projectNameChanged(const QString &name);

public slots:
    void resetSettings();
    void applySettings();

private slots:
    void titleChanged(const QString &title);

private:
    Ui::ProjectGeneralSettings *ui;

    ProjectSettings *m_settings;
};

#endif // PROJECTGENERALSETTINGS_H
