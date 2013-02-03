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

#include "config/bibglobals.h"
#include "config/conquirere.h"

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
   ui->cb_splashScreen->setChecked( ConqSettings::splashScreen() );

    // categories part
    for(int i=0; i < BibGlobals::Max_SeriesTypes; i++) {
        QListWidgetItem *listItem = new QListWidgetItem();
        listItem->setText(BibGlobals::SeriesTypeTranslation((BibGlobals::SeriesType)i));
        listItem->setIcon(KIcon(BibGlobals::SeriesTypeIcon((BibGlobals::SeriesType)i)));

        listItem->setData(BibGlobals::Role_ResourceType,BibGlobals::Resource_Series);
        listItem->setData(BibGlobals::Role_ResourceFilter,BibGlobals::SeriesType(i));

        if(ConqSettings::hiddenNbibSeriesOnRestart().contains(i)) {
            ui->hiddenSelector->selectedListWidget()->addItem(listItem);
        }
        else {
            ui->hiddenSelector->availableListWidget()->addItem(listItem);
        }
    }

    for(int i=0; i < BibGlobals::Max_BibTypes; i++) {
        QListWidgetItem *listItem = new QListWidgetItem();
        listItem->setText(BibGlobals::BibEntryTypeTranslation((BibGlobals::BibEntryType)i));
        listItem->setIcon(KIcon(BibGlobals::BibEntryTypeIcon((BibGlobals::BibEntryType)i)));

        listItem->setData(BibGlobals::Role_ResourceType,BibGlobals::Resource_Publication);
        listItem->setData(BibGlobals::Role_ResourceFilter,BibGlobals::BibEntryType(i));

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
        if(listItem->data(BibGlobals::Role_ResourceType).toInt() == (int)BibGlobals::Resource_Publication) {
            hiddenPublications.append( listItem->data(BibGlobals::Role_ResourceFilter).toInt() );
        }
        else if(listItem->data(BibGlobals::Role_ResourceType).toInt() == (int)BibGlobals::Resource_Series) {
            hiddenSeries.append( listItem->data(BibGlobals::Role_ResourceFilter).toInt() );
        }
    }

    ConqSettings::setHiddenNbibSeriesOnRestart(hiddenSeries);
    ConqSettings::setHiddenNbibPublicationsOnRestart(hiddenPublications);

    ConqSettings::setSplashScreen(ui->cb_splashScreen->isChecked());

    ConqSettings::self()->writeConfig();
}

void AppearanceSettings::setupGui()
{
}
