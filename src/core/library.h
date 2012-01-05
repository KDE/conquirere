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

/**
  * @brief A Library is a collection of files and Nepomuk::Resource data of a specific topic
  *
  * The Library can be either a System library containing all known entities or a specific
  * library containing only a subset of the system entities grouped together for a
  * specific topic.
  *
  * The Library fulfils the task of organizing all necessary resource parts.
  *
  * For specific libraries the creation of the corresponding folder structure,
  * the creation of a pimo:Project resource and creation of a settings file.
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
      *
      * @p type Either System or Project
      *
      * @todo check if I can support network libraries somehow (samba share)
      */
    explicit Library(LibraryType type);
    virtual ~Library();

    /**
      * Creates all data models for the library
      *
      * For each ResourceSelection there exists one model
      * The model fetches all data from the Nepomuk storage and update itself
      * when a Nepomuk::Resource is created or removed
      *
      */
    void setupModels();

    /**
      * The type of the Library
      *
      * @return Either System or Project
      */
    LibraryType libraryType() const;

    /**
      * Sets the name of the project library
      *
      * @pre Only for non System librarys
      * @p name The name of the project
      */
    void setName(const QString & name);

    /**
      * Returns the name of the project or the system library name
      */
    QString name() const;

    /**
      * Sets the description of the project library
      *
      * @pre Only for non System librarys
      * @p description The description of the project
      */
    void setDescription(const QString & description);

    /**
      * Returns the description of the project library name
      */
    QString description() const;

    /**
      * Sets the path of the project library
      *
      * When the library has a path assigned a specific folder structure will be created
      * In this folder structure all documents will be added automatically to the library
      * Also downloaded files will be stored here if not selected otherwise.
      *
      * A DirListener keeps track of file changes
      *
      * @pre Only for non System librarys
      * @p path The path of the project
      */
    void setLibraryDir(const QString & path);

    /**
      * Retuns the dir for this project library
      */
    QString libraryDir() const;
    QString libraryDocumentDir() const;

    void addSyncProvider(NBibSync* provider);
    void removeSyncProvider(NBibSync* provider);
    BackgroundSync *backgroundSync() const;

    /**
      * Creates a new project library
      *
      * This function creates the document structure, the Nepomuk::Resource.
      *
      * Has to be called when a new project is created and after the name
      * and path of the project has been selected
      */
    void createLibrary(const QString & name, const QString & description, const QString & path);

    /**
      * Loads an existing project .ini file
      *
      * @p projectFile the .ini project file
      */
    void loadLibrary(const QString & projectFile);
    void loadLibrary(const Nepomuk::Resource & pimoProject);

    /**
      * Deletes the current project library
      *
      * Removes the pimo:Project Resource and the document folder
      * structure.
      *
      * @todo implement library deletion
      */
    void deleteLibrary();

    /**
      * Returns the Nepomuk::Resource used to relate other Resources
      * to this project.
      */
    Nepomuk::Resource pimoLibrary() const;

    /**
      * Returns the Nepomuk::Tag used to relate other Resources
      * to this project.
      */
    Nepomuk::Resource pimoTag() const;

    /**
      * Relates a Nepomuk::Resource to this project
      *
      * @p res the used Nepomuk::Resource
      */
    void addResource(Nepomuk::Resource & res);

    /**
      * Updates all cached list data
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

    TagCloud *tagCloud();

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
    void connectModelToTagCloud(NepomukModel *model);

    LibraryType m_libraryType;
    QString m_name;
    QString m_description;
    QString m_libraryDir;
    DirWatcher *m_dirWatcher;
    BackgroundSync *m_backgroundSync;

    Nepomuk::Resource m_pimoLibrary;
    Nepomuk::Tag m_libraryTag;

    QMap<ResourceSelection, QSortFilterProxyModel*> m_resources;
    TagCloud *m_tagCloud;
    int m_initialImportFinished;

};

#endif // LIBRARY_H
