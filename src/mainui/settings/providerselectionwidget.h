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

#ifndef PROVIDERSELECTIONWIDGET_H
#define PROVIDERSELECTIONWIDGET_H

#include <QtGui/QWidget>

#include "nbibio/ui/providersettings.h"

namespace Ui {
    class ProviderSelectionWidget;
}

class ProjectSettings;

class ProviderSelectionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProviderSelectionWidget(QWidget *parent = 0);
    ~ProviderSelectionWidget();

    void setProjectSettings(ProjectSettings *ps);

signals:
    void contentChanged();

public slots:
    void resetSettings();
    void applySettings();

private slots:
    void editProvider();
    void addProvider();
    void removeProvider();

private:
    Ui::ProviderSelectionWidget *ui;
    ProjectSettings *m_settings;

    void setupGui();
};

#endif // PROVIDERSELECTIONWIDGET_H
