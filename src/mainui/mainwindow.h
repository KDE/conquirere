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

#include <kparts/mainwindow.h>
#include "../globals.h"

class Library;
class WelcomeWidget;
class ResourceTableWidget;
class LibraryWidget;
class SidebarWidget;
class DocumentPreview;

class MainWindow : public KParts::MainWindow
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
    void importBibTex();

    void connectKPartGui(KParts::Part * part);
    void switchView(ResourceSelection selection, ResourceFilter filter, Library *p);

    void DEBUGDELETEALLDATA();

private:
    void setupActions();
    void setupMainWindow();

    QMainWindow *m_centerWindow;
    WelcomeWidget *m_welcomeWidget;
    ResourceTableWidget *m_mainView;
    LibraryWidget *m_libraryWidget;
    SidebarWidget *m_sidebarWidget;
    DocumentPreview *m_documentPreview;

    QMap<Library *, QWidget *> m_libraryList; /**< holds the welcome widget for each opened library */
};

#endif // MAINWINDOW_H
