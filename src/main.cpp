/*
 * Copyright 2011 Jörg Ehrichs <joerg.ehichs@gmx.de>
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

#include "version.h"
#include "mainui/mainwindow.h"

#include <KDE/KApplication>
#include <KDE/KAboutData>
#include <KDE/KCmdLineArgs>
#include <KDE/KLocale>

#include <Nepomuk2/ResourceManager>

int main(int argc, char *argv[])
{
    Nepomuk2::ResourceManager::instance()->init();

    KAboutData aboutData( "conquirere", 0,
                          ki18n("Conquirere"), VERSION,
                          ki18n("Research Helper Tool"),
                          KAboutData::License_GPL,
                          ki18n("Copyright (c) 2011-2012 Jörg Ehrichs"), KLocalizedString(),
                          "https://projects.kde.org/projects/playground/edu/conquirere" );

    aboutData.addAuthor(ki18n("Jörg Ehrichs"), ki18n("Maintainer"), "joerg.ehrichs@gmx.de");

    KCmdLineArgs::init( argc, argv, &aboutData );

    KApplication app;

    MainWindow* window = new MainWindow();
    window->show();

    return app.exec();
}
