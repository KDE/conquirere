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

#ifndef NOTEWIDGET_H
#define NOTEWIDGET_H

#include "sidebarcomponent.h"
#include <Nepomuk/Resource>

namespace Ui {
    class NoteWidget;
}

/**
  * @brief shows a sidebar widget to create/edit/remove @c pimo:Note resources
  *
  */
class NoteWidget : public SidebarComponent
{
    Q_OBJECT

public:
    explicit NoteWidget(QWidget *parent = 0);
    ~NoteWidget();

public slots:
    void setResource(Nepomuk::Resource & resource);

signals:
    /**
      * notify connected editwidgets to update their info
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

    void saveNote();
    void discardNote();
    void changeRating(int newRating);

    /**
      * Sends changes in the connected property widgets to the connected table model to update the cache
      *
      * @todo This should be replaced by the Nepomuk::ResourceWatcher later
      */
    void subResourceUpdated();

private:
    Nepomuk::Resource m_note;
    Ui::NoteWidget *ui;
};

#endif // NOTEWIDGET_H
