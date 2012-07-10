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

#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include <KDE/KSplashScreen>

/**
  * @brief Shows a small SplashScreen on startup.
  *
  * Informs the user what part of the database cache is loading currently.
  * Can be switched off in the Options
  */
class SplashScreen : public KSplashScreen
{
    Q_OBJECT

public:
    SplashScreen();

protected:
    void drawContents(QPainter* p);

public slots:
    void message(const QString & message);

private:
    QString m_message;
};

#endif // SPLASHSCREEN_H
