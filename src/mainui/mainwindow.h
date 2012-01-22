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

class LibraryManager;
class Library;
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

public slots:
    // Slots for the menu KAction events

    // File menu
    void createLibrary();
    void loadLibrary();

    void deleteLibrarySelection();
    void closeLibrarySelection();

    // database menu
    void importZotero();
    void exportZotero();
    void syncZotero();

    void dbCheck();
    void dbBackup();

    void showConqSettings();

    // other helping slots
    void connectKPartGui(KParts::Part * part);

    /**
      * This will be called from the LibraryWidget and reflects the users choice of new data to display
      */
    void switchView(ResourceSelection selection, BibEntryType filter, Library *p);

    /**
      * This will be called from the LibraryWidget when user clicks on "Search Results" or from the SearchWidget when  a new search was started
      */
    void showSearchResults();

private slots:
    void openLibrary(Library *l);
    void closeLibrary(const QUrl &projectThingUrl);

private:
    bool queryExit();
    void setupActions();
    void setupMainWindow();

    void loadConfig();

    LibraryManager *m_libraryManager;
    QMap<QUrl, QWidget *> m_libraryList; /**< holds the welcome widget for each opened library */

    QMainWindow *m_centerWindow;
    ResourceTableWidget *m_mainView;
    LibraryWidget *m_libraryWidget;
    SidebarWidget *m_sidebarWidget;
    DocumentPreview *m_documentPreview;
    SearchWidget *m_searchWidget;
    SyncButton *m_syncButton;
};

#endif // MAINWINDOW_H
