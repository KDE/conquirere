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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <KDE/KXmlGuiWindow>

class Library;
class MainWidget;
class ProjectTreeWidget;
class SidebarWidget;

class MainWindow : public KXmlGuiWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void createLibrary();
    void loadLibrary();
    void openLibrary(Library *l);
    void deleteLibrary();
    void closeLibrary();
    void exportBibTex();

private:
    void setupActions();
    void setupMainWindow();

    ProjectTreeWidget *m_projectTree;
    MainWidget *m_mainView;
    SidebarWidget *m_sidebarWidget;

    QList<Library *> m_libraries;   /**< All opened libraries minimum the system library */
};

#endif // MAINWINDOW_H