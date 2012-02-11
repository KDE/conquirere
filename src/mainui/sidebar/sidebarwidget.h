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

#ifndef SIDEBARWIDGET_H
#define SIDEBARWIDGET_H

#include "globals.h"

#include <Nepomuk/Resource>

#include <QtGui/QWidget>

namespace Ui {
    class DockWidget;
}

class LibraryManager;
class Library;
class QStackedLayout;
class MergeResourcesWidget;
class SidebarComponent;
class LibraryInfoWidget;

/**
  * @brief The SidebarWidget contains all contents for the right side widget
  *
  * The widget is used to show the DocumentWidget, ReferenceWidget, PublicationWidget and so on.
  *
  * When the newSelection() slot is called (usually from the ProjectTreeWidget) the specified selection is shown
  *
  * When something is selected in the mainWidget (from the TableView) setResource() is called
  * The currently active widget receives the newResource and updates its data
  *
  * @see SidebarComponent
  */
class SidebarWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SidebarWidget(QWidget *parent = 0);
    virtual ~SidebarWidget();

public slots:
    /**
      * Called when something is selected in the project tree
      *
      * Change the currentwidget to something that can display the @p selection
      *
      * @p selection the resource type (Documents, Publications, Mails etc)
      * @p filter some additional filter like Article, Book etc
      * @p library the library to use
      */
    void newSelection(ResourceSelection selection, BibEntryType filter, Library *library);

    /**
      * Special case to show the data from
      *
      * When the search mode is activated (search results table is shown)
      * we need to find the right @c ResourceSelection type on our own for each resource and
      * change the sidbarwidged accordingly
      *
      * @see findResourceSelection
      */
    void showSearchResults();

    void findResourceSelection(Nepomuk::Resource & resource);

    /**
      * Called when a Nepomuk::Resource was selected in the MainWidget
      *
      * Updates the current widget
      */
    void setResource(Nepomuk::Resource & resource);

    void setMultipleResources(QList<Nepomuk::Resource> resourcelist);

    void clear();

    /**
      * called when the new button was clicked
      *
      * redirects the call to the current SidebarComponent
      */
    void newButtonClicked();

    /**
      * called when the delete button was clicked
      *
      * redirects the call to the current SidebarComponent
      */
    void deleteButtonClicked();

    void addToProject();
    void addToSelectedProject();
    void removeFromProject();
    void removeFromSelectedProject();

    void findPdf();

    void setLibraryManager(LibraryManager* lm);

    /**
      * called from the DocumentWidget
      *
      * enable/disable the publication icons
      */
    void hasPublication(bool publication);

    /**
      * called from the PublicationWidget
      *
      * enable/disable the reference icons
      */
    void hasReference(bool reference);

signals:
    /**
      * This signal gets thrown when the resource was changed and must be updated in the table model cache
      * redirects the signal from all propertywidgets of every SidbarComponent
      *
      * @todo This should be replaced by the Nepomuk::ResourceWatcher later
      */
    void resourceCacheNeedsUpdate(Nepomuk::Resource resource);

private:
    void setupUi();

    Ui::DockWidget *ui;
    LibraryManager* m_libraryManager;

    QStackedLayout *m_stackedLayout;
    SidebarComponent *m_documentWidget;
    SidebarComponent *m_publicationWidget;
    SidebarComponent *m_referenceWidget;
    SidebarComponent *m_noteWidget;
    SidebarComponent *m_eventWidget;
    SidebarComponent *m_seriesWidget;
    LibraryInfoWidget *m_libraryInfoWidget;
    SidebarComponent *m_searchResultInfoWidget;
    SidebarComponent *m_mailWidget;

    MergeResourcesWidget *m_mergeWidget;

    bool m_searchResultVisible;
    SidebarComponent *m_currentWidget;
    Nepomuk::Resource m_curResource;
};

#endif // SIDEBARWIDGET_H
