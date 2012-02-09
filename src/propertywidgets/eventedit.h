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

#ifndef EVENTEDIT_H
#define EVENTEDIT_H

#include "propertyedit.h"

class KJob;

/**
  * @brief Edits the @c pimo:Event and adds the @c nbib:event and @c nbib:eventPublication connection to the @c nbib:Publication
  */
class EventEdit : public PropertyEdit
{
    Q_OBJECT
public:
    explicit EventEdit(QWidget *parent = 0);

private:
    /**
      * Shows the @c nao:prefLabel of the @ nbib:event
      *
      * fall back to @c nie:title
      */
    void setupLabel();

    void updateResource( const QString & newEventTitle );
};

#endif // EVENTEDIT_H
