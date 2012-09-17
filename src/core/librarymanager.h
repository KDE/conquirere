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

/**
  * @brief Manager to hold all Library instances used in the system
  *
  * Used to interact with and retrieve all opened Libraries
  */
class LibraryManager : public QObject
{
    Q_OBJECT
public:
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

    void doOpenSettings(Library *l = 0);
    void doImportFile(Library *l = 0);
    void doExportFile(Library *l = 0);
    void doSyncStorage(Library *l = 0);

signals:
    void libraryAdded(Library *l);
    void libraryRemoved(const QUrl &projectThingUrl);

    void openSettings(Library *l);
    void importFile(Library *l);
    void exportFile(Library *l);
    void syncStorage(Library *l);

private:
    QList<Library*> m_openProjectList;
    Library *m_systemLibrary;
    Library *m_currentUsedLibrary;
};

#endif // LIBRARYMANAGER_H
