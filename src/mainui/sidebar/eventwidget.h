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
#include <Nepomuk/Thing>

namespace Ui {
    class EventWidget;
}

/**
  * @brief Manipulates the @c pimo:Event and ncal:Event resource
  *
  * All conquierere related changes (publications, rating, tags) are saved in the pimo:Event
  * resource. The place, date and attendees are saved in the pimoEvent if the event is not available
  * as akonadi resource.
  *
  * If the ncal:Event exist as groundingOccurence of the pimo:Event the changes to attendee, date, place, title
  * and summary are made directly via Akonadi.
  *
  * @todo implement setting of the changes voa Akonadi
  * @todo add pushToAkonadi function
  * @todo implement Akonadi event edit dialog in the ui file
  */
class EventWidget : public SidebarComponent
{
    Q_OBJECT

public:
    explicit EventWidget(QWidget *parent = 0);
    ~EventWidget();

   void setLibraryManager(LibraryManager *lm);
   Nepomuk::Resource resource();

public slots:
    virtual void setResource(Nepomuk::Resource & resource);

private slots:
    /**
      * creates a new @c pimo:Event
      */
    void newButtonClicked();

    /**
      * Deletes the current @c pimo:Event
      *
      * Also takes care of all publication backlinks that are set via nbib:event
      */
    void deleteButtonClicked();

    void changeRating(int newRating);

    /**
      * Sends changes in the connected property widgets to the connected table model to update the cache
      *
      * Updates not only the eventname cache entry but also the entries for the publication connected here
      *
      * @todo This should be replaced by the Nepomuk::ResourceWatcher later
      */
    void subResourceUpdated();

private:
    Ui::EventWidget *ui;
    Nepomuk::Thing m_eventThing;
    Nepomuk::Resource m_eventResource;
};

#endif // EVENTWIDGET_H
