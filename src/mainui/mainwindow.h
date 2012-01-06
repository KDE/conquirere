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

#include "globals.h"

#include <KParts/MainWindow>

#include <kbibtex/file.h>

class Library;
class WelcomeWidget;
class ResourceTableWidget;
class LibraryWidget;
class SidebarWidget;
class DocumentPreview;
class SearchWidget;
class SyncButton;

class MainWindow : public KParts::MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QList<Library *> openLibraries();

public slots:
    // Slots for the menu KAction events

    // File menu
    void createLibrary();
    void loadLibrary();
    void openLibrary(Library *l);
    void deleteLibrary();
    void closeLibrary();

    // database menu
    void importBibTex();
    void importZotero();

    void exportZotero();
    void exportBibTex();
    void exportPdf();
    void exportOtherFile();

    void syncZotero();

    void dbCheck();
    void dbBackup();

    void updateListCache();

    // other helping slots
    void connectKPartGui(KParts::Part * part);
    void switchView(ResourceSelection selection, BibEntryType filter, Library *p);
    void showSearchResults();

    void DEBUGDELETEALLDATA();

private:
    void setupActions();
    void setupMainWindow();

    void loadConfig();

    QMainWindow *m_centerWindow;
    WelcomeWidget *m_welcomeWidget;
    ResourceTableWidget *m_mainView;
    LibraryWidget *m_libraryWidget;
    SidebarWidget *m_sidebarWidget;
    DocumentPreview *m_documentPreview;
    SearchWidget *m_searchWidget;
    SyncButton *m_syncButton;

    QMap<Library *, QWidget *> m_libraryList; /**< holds the welcome widget for each opened library */
    Library *m_systemLibrary;
    Library *m_curLibrary;

    File *m_bibFile;
};

#endif // MAINWINDOW_H
