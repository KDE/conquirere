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

#include <Nepomuk2/Resource>

#include <QtCore/QObject>
#include <QtCore/QString>

class ProjectSettings;
class QSortFilterProxyModel;
class TagCloud;
class DirWatcher;
class KJob;

/**
  * @brief A Library is a collection of files and Nepomuk2::Resource data of a specific topic
  *
  * The Library can be either a <i> ystem library</i> containing all known entities or a
  * <i>Project library</i> containing only a subset of the system entities grouped together for a
  * specific topic.
  *
  * The Library fulfils the task of organizing all necessary resource parts.
  *
  * For specific libraries the creation of the corresponding folder structure,
  * the creation of a @c pimo:Project resource and creation of a settings file is done by this class
  *
  * Also the possibility to relate resources from the system library is the
  * responsibility of this class.
  *
  * For all libraries the necessary model data used in connected views
  * are handled by this class.
  *
  * Usually the <i>System Library</i> is created on the start of the program.
  * Additional librarys will be created/loaded/closed/deleted on request.
  */
class Library : public QObject
{
    Q_OBJECT
public:

    /**
      * Creates a new dummy Library
      */
    explicit Library();
    virtual ~Library();

    /**
      * @brief Creates a new project library
      *
      * This function creates the document structure, the Nepomuk2::Resource (@c pimo:Project ) and Nepomuk2::Tag.
      *
      * Has to be called when a new project is created.
      */
    static Nepomuk2::Resource createLibrary(const QString & name, const QString & description, const QString & path);
    static QString createIniFile(Nepomuk2::Resource & pimoProject, const QString & path = QString());

    /**
      * @brief Loads an existing project .ini file
      *
      * @p projectFile the .ini project file
      * @p type the type of the library
      */
    void loadLibrary(const QString & projectFile, LibraryType type = Library_Project);

    /**
     * @brief Load the system library with all resources in the Nepomuk database
     */
    void loadSystemLibrary( );

    /**
      * @brief Loads a project by its pimo:Project resource uri.
      *
      * This means it either has a .ini file attached as @c pimo:groundingOccurence already
      * So we check all @c pimo:groundingOccurences for a file in the applications project path
      *
      * Or it was a @c pimo:Project created from some other program and we have to create the .ini file again
      */
    void loadLibrary(Nepomuk2::Resource &pimoProject);
    ProjectSettings * settings();

    /**
      * The type of the Library
      *
      * @return Either @c System or @c Project
      */
    LibraryType libraryType() const;

    /**
      * brief Deletes the current project library
      *
      * Removes the @c pimo:Project Resource and all connections to it.
      * Also deletes the .ini file (all files connected via pimo::groundingOcurence)
      */
    void deleteLibrary();

    /**
      * @return the tagcloud for this library
      */
    TagCloud *tagCloud();

    /**
      * @brief Relates a Nepomuk2::Resource to this project
      *
      * @p res the used Nepomuk2::Resource
      */
    void addResource(const Nepomuk2::Resource & res);

    /**
      * @brief Removes the pimo::isRelated relation again
      */
    void removeResource(const Nepomuk2::Resource & res);

    /**
      * @brief Deletes the resource and also any subresources
      *
      * Also detects if this deletes the only @c article in a @c collection or
      * only @c publication in a @c series
      * and deletes them too.
      *
      * @todo TODO: subresource removal via NAO::hasSubresource, add to nepomukpipe
      */
    void deleteResource(const Nepomuk2::Resource & resource);

    /**
      * @brief Returns the model for a specific library Resource.
      *
      * @p selection one of the ResourceSelection types
      *
      * @return The abstract table model used to connect to a tableview
      */
    QSortFilterProxyModel* viewModel(ResourceSelection selection);

    /**
      * @brief Returns all available tablemodels as a map sorted by its Content
      *
      * @see ResourceSelection
      */
    QMap<ResourceSelection, QSortFilterProxyModel*> viewModels();

private slots:
    /**
     * @brief Small helper function to print the error string when the nepomuk DMS call failed
     * @param job the Nepomuk job for the DMS call
     */
    void nepomukDMSfinishedInfo(KJob *job);

private:
    /**
      * @brief Creates all data models for the library
      *
      * For each ResourceSelection there exists one model.
      * The model fetches all data from the Nepomuk storage and updates itself
      * when a Nepomuk2::Resource is created or removed.
      */
    void setupModels();

    LibraryType m_libraryType;
    ProjectSettings *m_projectSettings;

    DirWatcher *m_dirWatcher;
    TagCloud *m_tagCloud;

    QMap<ResourceSelection, QSortFilterProxyModel*> m_resources;

};

#endif // LIBRARY_H
