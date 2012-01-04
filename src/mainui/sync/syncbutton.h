/*
 * Copyright 2011 Jörg Ehrichs <joerg.ehrichs@gmx.de>
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

#ifndef SYNCBUTTON_H
#define SYNCBUTTON_H

#include <QWidget>

class MainWindow;

/**
  * @todo create a animated button from it for the toolbar so it can run run silently in the background
  *       indicate that the user must interact and let him click on the button then
  */
class SyncButton : public QWidget
{
    Q_OBJECT
public:
    explicit SyncButton(QWidget *parent = 0);

    /**
      * sets a link to the mainwindow.
      *
      * Used to get the list of open libraries in the subcomponents
      */
    void setMainWindow(MainWindow *mw);

public slots:
    void startSync();
    void needsAttention();

private:
    MainWindow *m_parent;

};

#endif // SYNCBUTTON_H
