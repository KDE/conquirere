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

#include "splashscreen.h"

#include <KDE/KStandardDirs>
#include <KDE/KGlobalSettings>

#include <QtGui/QPainter>
#include <QtGui/QPixmap>

#include "version.h"

SplashScreen::SplashScreen()
    : KSplashScreen(QPixmap())
{
    setPixmap(KStandardDirs::locate("appdata","splashscreen.png"));
}

void SplashScreen::drawContents(QPainter* p)
{
    QFont fnt(KGlobalSettings::generalFont());
    fnt.setPixelSize(10);
    fnt.setBold(false);
    p->setFont(fnt);

    QRect r = rect();
    r.setCoords(200, 280, 300, 500);

    // Draw message at given position, limited to 49 chars
    // If message is too long, string is truncated
    if (m_message.length() > 50)
    {
        m_message.truncate(49);
    }

    p->setPen(Qt::white);
    p->drawText(r, Qt::AlignLeft, m_message);

    // -- Draw version string -------------------------------------------------

    QFontMetrics fontMt(fnt);
    QRect r2 = fontMt.boundingRect(rect(), 0, VERSION);
    r2.moveTopLeft(QPoint(width()-r2.width()-10, r.y()));
    p->setPen(Qt::white);
    p->drawText(r2, Qt::AlignLeft, VERSION );
}

void SplashScreen::message(const QString & message)
{
    m_message = message;

    QSplashScreen::showMessage(m_message);
}
