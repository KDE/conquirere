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
#include <Nepomuk2/Resource>

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

    Nepomuk2::Resource resource();

public slots:
    void setResource(Nepomuk2::Resource & resource);
    /**
      * creates a new @c pimo:Note
      */
    void newButtonClicked();

    /**
      * deletes the current @c pimo:Note
      */
    void deleteButtonClicked();

private slots:
    void saveNote();
    void discardNote();
    void changeRating(uint newRating);

private:
    Nepomuk2::Resource m_note;
    Ui::NoteWidget *ui;
};

#endif // NOTEWIDGET_H
