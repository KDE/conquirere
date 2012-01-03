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

#include <QtGui/QDockWidget>

namespace Ui {
    class DockWidget;
}

class MainWindow;
class QStackedLayout;
class MergeResourcesWidget;
class SidebarComponent;
class Library;

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
class SidebarWidget : public QDockWidget
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

    /**
      * sets a link to the mainwindow.
      *
      * Used to get the list of open libraries in the subcomponents
      */
    void setMainWindow(MainWindow *mw);

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
      * redirects the signal from all propertywidgets pf every SidbarComponent
      *
      * @todo This should be replaced by the Nepomuk::ResourceWatcher later
      */
    void resourceCacheNeedsUpdate(Nepomuk::Resource resource);

private:
    Ui::DockWidget *ui;
    MainWindow *m_parent;
    QStackedLayout *m_stackedLayout;
    QWidget *m_blankPage;
    MergeResourcesWidget *m_mergeWidget;
    SidebarComponent *m_currentWidget;
    ResourceSelection m_curSelection;
    Nepomuk::Resource m_curResource;
};

#endif // SIDEBARWIDGET_H
