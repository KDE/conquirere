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

#ifndef SIDEBARCOMPONENT_H
#define SIDEBARCOMPONENT_H

#include "globals.h"

#include <Nepomuk2/Resource>

#include <QtGui/QWidget>

#include <QtCore/QDebug>

class Library;
class LibraryManager;
class MainWindow;

/**
  * @brief Abstract base class for all widgets in the SidbarWidget
  *
  */
class SidebarComponent : public QWidget
{
    Q_OBJECT
public:
    explicit SidebarComponent(QWidget *parent = 0);
    virtual ~SidebarComponent() {}

    virtual void setLibraryManager(LibraryManager *lm);
    LibraryManager *libraryManager();

    virtual Nepomuk2::Resource resource() = 0;

signals:
    /**
      * This signal gets thrown when the resource was changed and must be updated in the table model cache
      * redirects the signal from all propertywidgets
      *
      * @todo This should be replaced by the Nepomuk2::ResourceWatcher later
      */
    void resourceCacheNeedsUpdate(Nepomuk2::Resource resource);

public slots:
    /**
      * Sets the resource which should be shown in the widget
      *
      * Has to be implemented in the subclasses to fill the right PropertyEdit widgets
      */
    virtual void setResource(Nepomuk2::Resource & resource) = 0;

    virtual void newButtonClicked() = 0;
    virtual void deleteButtonClicked() = 0;

private:
    LibraryManager *m_libraryManager;
};

#endif // SIDEBARCOMPONENT_H
