/*
 * Copyright 2011 Jörg Ehrichs <joerg.ehrichs@gmx.de>
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

#ifndef EVENTWIDGET_H
#define EVENTWIDGET_H

#include "sidebarcomponent.h"
#include <Nepomuk/Resource>

namespace Ui {
    class EventWidget;
}

class EventWidget : public SidebarComponent
{
    Q_OBJECT

public:
    explicit EventWidget(QWidget *parent = 0);
    ~EventWidget();

    void setLibrary(Library *p);

public slots:
    virtual void setResource(Nepomuk::Resource & resource);

signals:
    /**
      * Sends changes in the connected property widgets to the connected table model to update the cache
      *
      * @todo This should be replaced by the Nepomuk::ResourceWatcher later
      */
    void resourceChanged(Nepomuk::Resource resource);

private slots:
    /**
      * creates a new @c pimo:Note
      */
    void newButtonClicked();

    /**
      * deletes the current @c pimo:Note
      */
    void deleteButtonClicked();

    void changeRating(int newRating);

    /**
      * Sends changes in the connected property widgets to the connected table model to update the cache
      *
      * @todo This should be replaced by the Nepomuk::ResourceWatcher later
      */
    void subResourceUpdated();

private:
    Ui::EventWidget *ui;
    Nepomuk::Resource m_event;
};

#endif // EVENTWIDGET_H
