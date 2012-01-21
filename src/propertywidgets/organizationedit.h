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

#ifndef ORGANIZATIONEDIT_H
#define ORGANIZATIONEDIT_H

#include "propertyedit.h"

class QStandardItemModel;

/**
  * @brief Changes the @c nbib:organization of a proceedings
  *
  * @pre resource must be @c nbib:proceedings or @c nbib:article
  */
class OrganizationEdit : public PropertyEdit
{
public:
    OrganizationEdit(QWidget *parent = 0);

protected:
    /**
      * Shows the @c nco:fillname of the @ nco:OrganizationContact
      */
    void setupLabel();

    /**
      * Removes any old organization connected to the proceedings and adds the new entered.
      *
      * Already existing resources will be used if available, or a complete new created
      */
    virtual void updateResource( const QString & text );
};

#endif // ORGANIZATIONEDIT_H
