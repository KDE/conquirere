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

#include <Nepomuk/Resource>

#include <QtCore/QObject>

#include <QtCore/QMap>
#include <QtCore/QUrl>

class Library;
class QWidget;

class LibraryManager : public QObject
{
    Q_OBJECT
public:
    enum ImportMode {
        Import_KBibTeX_Local,
        Import_KBibTeX_Sync,
        Import_Zotero_Sync,
        Import_Select_Mode
    };

    enum ExportMode {
        Export_KBibTeX_Local,
        Export_Zotero_Sync,
        Export_PDF,
        Export_OtherFile,
        Export_Select_Mode
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

    void importData(ImportMode mode = Import_Select_Mode);
    void importData(Library *l, ImportMode mode = Import_Select_Mode);

    void exportData(ExportMode mode = Export_Select_Mode);
    void exportData(Library *l, ExportMode mode = Export_Select_Mode);

    void updateListCache();

signals:
    void libraryAdded(Library *l);
    void libraryRemoved(const QUrl &projectThingUrl);

    /**
      * This signal gets thrown when the resource was changed and must be updated in the table model cache
      * redirects the signal from all propertywidgets of every SidbarComponent
      *
      * @todo This should be replaced by the Nepomuk::ResourceWatcher later
      */
    void resourceCacheNeedsUpdate(Nepomuk::Resource resource);

private:
    QList<Library*> m_openProjectList;
    Library *m_systemLibrary;
    Library *m_currentUsedLibrary;
};

#endif // LIBRARYMANAGER_H
