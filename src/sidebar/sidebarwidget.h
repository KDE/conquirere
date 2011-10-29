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

#include <QDockWidget>

#include "../globals.h"

#include <Nepomuk/Resource>

namespace Ui {
    class DockWidget;
}

class QStackedLayout;
class MergeResourcesWidget;
class SidebarComponent;
class Library;

/**
  * The SidebarWidget contains all contents for the right side widget
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
    void newSelection(ResourceSelection selection, ResourceFilter filter, Library *library);

    /**
      * Called when a Nepomuk::Resource was selected in the MainWidget
      *
      * Updates the current widget
      */
    virtual void setResource(Nepomuk::Resource & resource);

    virtual void setMultipleResources(QList<Nepomuk::Resource> resourcelist);

    void clear();

    void newButtonClicked();
    void deleteButtonClicked();

private:
    Ui::DockWidget *ui;
    QStackedLayout *m_stackedLayout;
    QWidget *m_blankPage;
    MergeResourcesWidget *m_mergeWidget;
    SidebarComponent *m_currentWidget;
    ResourceSelection m_curSelection;
};

#endif // SIDEBARWIDGET_H
