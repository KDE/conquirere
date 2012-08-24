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

#ifndef LIBRARYMANAGER_H
#define LIBRARYMANAGER_H

#include <Nepomuk2/Resource>

#include <QtCore/QObject>
#include <QtCore/QMap>
#include <QtCore/QUrl>

class Library;
class QWidget;
class BackgroundSync;

/**
  * @brief Manager to hold all Library instances used in the system
  *
  * Used to interact with and retrieve all opened Libraries
  */
class LibraryManager : public QObject
{
    Q_OBJECT
public:
    enum ModeSelection {
        KBibTeX_Local,
        KBibTeX_Sync,
        Zotero_Sync,
        Select_Mode
    };

    explicit LibraryManager(QObject *parent = 0);
    ~LibraryManager();

    QList<Library *> openProjects();
    Library *systemLibrary();
    void setCurrentUsedLibrary(Library *l);
    Library *currentUsedLibrary();

    Library *libFromResourceUri(const QUrl &projectThing);

public slots:
    void addLibrary(Library *l);
    void addSystemLibrary(Library *l);
    void closeLibrary(Library *l);
    void deleteLibrary(Library *l);

    void openSettings();
    void openSettings(Library *l);

    void importData(ModeSelection mode = Select_Mode);
    void importData(Library *l, ModeSelection mode = Select_Mode);

    void exportData(ModeSelection mode = Select_Mode);
    void exportData(Library *l, ModeSelection mode = Select_Mode);

    void syncData(ModeSelection mode = Select_Mode);
    void syncData(Library *l, ModeSelection mode = Select_Mode);

signals:
    void libraryAdded(Library *l);
    void libraryRemoved(const QUrl &projectThingUrl);

private:
    QList<Library*> m_openProjectList;
    Library *m_systemLibrary;
    Library *m_currentUsedLibrary;
    BackgroundSync *m_backgroundSyncManager;
};

#endif // LIBRARYMANAGER_H
