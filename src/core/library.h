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

#include "../globals.h"

#include <Nepomuk/Resource>
#include <Nepomuk/Tag>

#include <QtCore/QObject>
#include <QtCore/QFileInfo>
#include <QtCore/QString>

class QSettings;
class LibraryWidget;
class QSortFilterProxyModel;

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
      * Sets the path of the project library
      *
      * @pre Only for non System librarys
      * @p path The path of the project
      */
    void setPath(const QString & path);

    /**
      * Returns the path of the project library or
      * an empty string for the System library
      */
    QString path() const;

    /**
      * Creates a new project library
      *
      * This function creates the document structure, the Nepomuk::Resource.
      *
      * Has to be called when a new project is created and after the name
      * and path of the project has been selected
      */
    void createLibrary();

    /**
      * Loads an existing project .ini file
      *
      * @p projectFile the .ini project file
      */
    void loadLibrary(const QString & projectFile);

    /**
      * Deletes the current project library
      *
      * Removes the pimo:Project Resource and the document folder
      * structure.
      */
    void deleteLibrary();

    /**
      * Returns the Nepomuk::Resource used to relate other Resources
      * to this project.
      */
    Nepomuk::Resource pimoLibrary() const;

    /**
      * Checks if a specific file is available
      * in the project folder structure
      *
      * @p filename name of the file to check for
      */
    bool isInPath(const QString &filename);

    /**
      * Relates a Nepomuk::Resource to this project
      *
      * @p res the used Nepomuk::Resource
      */
    void addResource(Nepomuk::Resource & res);

    /**
      * Relates a file to the project library
      *
      * @p fileInfo the file to use
      */
    void addDocument(const QFileInfo &fileInfo);

    /**
      * Returns the model for a specific library Resource.
      *
      * @p selection one of the ResourceSelection types
      *
      * @return The abstract table model used to connect to a tableview
      *
      */
    QSortFilterProxyModel* viewModel(ResourceSelection selection);

    /**
      * Returns all available sortmodels and their usage
      */
    QMap<ResourceSelection, QSortFilterProxyModel*> viewModels();

    /**
      * Connects the fetch indicator signal/slots
      *
      * This allows the @p treeWidget to show a spinning indicator
      * as long as the model for a resource selection is fetching
      * data from nepomuk.
      */
    void connectFetchIndicator(LibraryWidget *treeWidget);

    QMap<QString, int> tagCloud();

public slots:
    /**
      * Scans the project Library for new documents
      *
      * Used to automatically related files in the project folder to the project
      */
    void scanLibraryFolders();

    void addTag(const QString & tag);

signals:
    void tagCloudChanged();
    /**
      * This signal gets thrown when the resource was changed and must be updated in the table model cache
      * redirects the signal from all propertywidgets
      *
      * @todo This should be replaced by the Nepomuk::ResourceWatcher later
      */
    void resourceUpdated(Nepomuk::Resource resource);

private:
    /**
      * Creates all data models for the library
      *
      * For each ResourceSelection there exists one model
      * The model fetches all data from the Nepomuk storage and update itself
      * when a Nepomuk::Resource is created or removed
      *
      */
    void setupModels();

    LibraryType m_libraryType;
    QString m_name;
    QString m_path;

    Nepomuk::Resource m_pimoLibrary;

    Nepomuk::Tag m_libraryTag; /**< @todo add tag to resource models to be able to list Nepomuk::Resource not only based on the isRelated to fact */
    QSettings *m_settings;

    QMap<ResourceSelection, QSortFilterProxyModel*> m_resources;
    QMap<QString, int> m_tagCloud;

};

#endif // LIBRARY_H
