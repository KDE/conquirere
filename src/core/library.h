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

#ifndef LIBRARY_H
#define LIBRARY_H

#include "globals.h"

#include <Nepomuk/Resource>
#include <Nepomuk/Thing>
#include <Nepomuk/Tag>

#include <QtCore/QObject>
#include <QtCore/QFileInfo>
#include <QtCore/QString>

class LibraryWidget;
class QSortFilterProxyModel;
class TagCloud;
class NepomukModel;
class DirWatcher;
class NBibSync;
class BackgroundSync;
class ProjectSettings;

/**
  * @brief A Library is a collection of files and Nepomuk::Resource data of a specific topic
  *
  * The Library can be either a @c System @c library containing all known entities or a specific
  * library containing only a subset of the system entities grouped together for a
  * specific topic.
  *
  * The Library fulfils the task of organizing all necessary resource parts.
  *
  * For specific libraries the creation of the corresponding folder structure,
  * the creation of a pimo:Project resource and creation of a settings file is done by this class
  *
  * Also the possibility to relate resources from the system library is the
  * responsibility of this class.
  *
  * For all libraries the necessary model data used in connected views
  * are handled by this class.
  *
  * Usually a new unique System Library is created on the start of the program.
  * Additional librarys will be created/loaded/closed/deleted on request.
  */
class Library : public QObject
{
    Q_OBJECT
public:

    /**
      * Creates a new Library
      */
    explicit Library();
    virtual ~Library();

    /**
      * Creates a new project library
      *
      * This function creates the document structure, the Nepomuk::Thing and Nepomuk::Tag.
      *
      * Has to be called when a new project is created
      */
    static Nepomuk::Thing createLibrary(const QString & name, const QString & description, const QString & path);
    static QString createIniFile(Nepomuk::Thing & pimoProject, const QString & path = QString());

    /**
      * Loads an existing project .ini file
      *
      * @p projectFile the .ini project file
      * @p type the type of the library
      */
    void loadLibrary(const QString & projectFile, LibraryType type = Library_Project);
    void loadSystemLibrary();

    /**
      * Loads a project by its pimo:Project resource uri.
      *
      * This means it either has a .ini file attached as groundingOccurence already
      * So we check all groundingOccurences for a file in the applications project path
      *
      * Or it was a pimo:Project created from some other program and we have to create the .ini file again
      */
    void loadLibrary(Nepomuk::Thing & pimoProject);
    ProjectSettings * settings();

    /**
      * The type of the Library
      *
      * @return Either System or Project
      */
    LibraryType libraryType() const;

    /**
      * Deletes the current project library
      *
      * Removes the pimo:Project Resource and all connections to it.
      * Also deletres the .ini file
      */
    void deleteLibrary();

    /**
      * @return the tagcloud for this library
      */
    TagCloud *tagCloud();

    /**
      * Relates a Nepomuk::Resource to this project
      *
      * If this sycdetaisl for this item was in the "to be removed" section of the .ini provider settings
      * They will be removed from there too
      *
      * @p res the used Nepomuk::Resource
      */
    void addResource(Nepomuk::Resource & res);

    /**
      * removes the pimo::isRelated elation again and also adds the sync data to the syncprovider
      * ini files so we know we need to remove it from the online storage too.
      */
    void removeResource(Nepomuk::Resource & res);

    /**
      * Deletes the resource and also any connected resource that might not be necessary anymore
      *
      * like the reference for the publication, or a nbib:Series if it did not contain any other publications
      * or a nbib:Collection if we deleted the only article in it
      *
      * @p recursiveDeletion if this is true also the publication from a reference will be deleted. BEside they zotero sync shouldn't be necessary
      */
    void deleteResource(Nepomuk::Resource & resource, bool recursiveDeletion = false);

    /**
      * Updates all cached list data
      *
      * @todo can be removed when the ResourceWatcher is working
      */
    void updateCacheData();

    /**
      * Returns the model for a specific library Resource.
      *
      * @p selection one of the ResourceSelection types
      *
      * @return The abstract table model used to connect to a tableview
      */
    QSortFilterProxyModel* viewModel(ResourceSelection selection);

    /**
      * Returns all available sortmodels and their usage
      */
    QMap<ResourceSelection, QSortFilterProxyModel*> viewModels();

signals:
    /**
      * This signal gets thrown when the resource was changed and must be updated in the table model cache
      * redirects the signal from all propertywidgets
      *
      * @todo This should be replaced by the Nepomuk::ResourceWatcher later
      */
    void resourceCacheNeedsUpdate(Nepomuk::Resource resource);

private slots:
    void finishedInitialImport();

private:
    /**
      * Creates all data models for the library
      *
      * For each ResourceSelection there exists one model
      * The model fetches all data from the Nepomuk storage and update itself
      * when a Nepomuk::Resource is created or removed
      */
    void setupModels();
    void connectModelToTagCloud(NepomukModel *model);

    LibraryType m_libraryType;
    ProjectSettings *m_projectSettings;

    DirWatcher *m_dirWatcher;
    TagCloud *m_tagCloud;

    QMap<ResourceSelection, QSortFilterProxyModel*> m_resources;
    int m_initialImportFinished;

};

#endif // LIBRARY_H
