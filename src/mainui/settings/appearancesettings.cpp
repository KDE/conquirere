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

#include "appearancesettings.h"
#include "ui_appearancesettings.h"

#include "globals.h"
#include "nbibio/conquirere.h"

#include <QtGui/QListWidget>
#include <QtGui/QListWidgetItem>

AppearanceSettings::AppearanceSettings(QWidget *parent)
    :QWidget(parent)
    , ui(new Ui::AppearanceSettings)
{
    ui->setupUi(this);

    setupGui();
    resetSettings();

    connect(ui->hiddenSelector, SIGNAL(added(QListWidgetItem*)), this, SIGNAL(contentChanged()));
    connect(ui->hiddenSelector, SIGNAL(removed(QListWidgetItem*)), this, SIGNAL(contentChanged()));
}

AppearanceSettings::~AppearanceSettings()
{
    delete ui;
}

void AppearanceSettings::resetSettings()
{
    // view part

    // categories part
    for(int i=0; i < Max_SeriesTypes; i++) {
        QListWidgetItem *listItem = new QListWidgetItem();
        listItem->setText(SeriesTypeTranslation.at(i));
        listItem->setIcon(KIcon(SeriesTypeIcon.at(i)));

        listItem->setData(Role_ResourceType,Resource_Series);
        listItem->setData(Role_ResourceFilter,SeriesType(i));

        if(ConqSettings::hiddenNbibSeriesOnRestart().contains(i)) {
            ui->hiddenSelector->selectedListWidget()->addItem(listItem);
        }
        else {
            ui->hiddenSelector->availableListWidget()->addItem(listItem);
        }
    }

    for(int i=0; i < Max_BibTypes; i++) {
        QListWidgetItem *listItem = new QListWidgetItem();
        listItem->setText(BibEntryTypeTranslation.at(i));
        listItem->setIcon(KIcon(BibEntryTypeIcon.at(i)));

        listItem->setData(Role_ResourceType,Resource_Publication);
        listItem->setData(Role_ResourceFilter,BibEntryType(i));

        if(ConqSettings::hiddenNbibPublicationsOnRestart().contains(i)) {
            ui->hiddenSelector->selectedListWidget()->addItem(listItem);
        }
        else {
            ui->hiddenSelector->availableListWidget()->addItem(listItem);
        }
    }
}

void AppearanceSettings::applySettings()
{
    QList<int> hiddenSeries;
    QList<int> hiddenPublications;

    int maxItems = ui->hiddenSelector->selectedListWidget()->count();
    for(int i=0; i < maxItems; i++) {
        QListWidgetItem *listItem = ui->hiddenSelector->selectedListWidget()->item(i);
        if(listItem->data(Role_ResourceType).toInt() == (int)Resource_Publication) {
            hiddenPublications.append( listItem->data(Role_ResourceFilter).toInt() );
        }
        else if(listItem->data(Role_ResourceType).toInt() == (int)Resource_Series) {
            hiddenSeries.append( listItem->data(Role_ResourceFilter).toInt() );
        }
    }

    ConqSettings::setHiddenNbibSeriesOnRestart(hiddenSeries);
    ConqSettings::setHiddenNbibPublicationsOnRestart(hiddenPublications);

    ConqSettings::self()->writeConfig();
}

void AppearanceSettings::setupGui()
{
}
