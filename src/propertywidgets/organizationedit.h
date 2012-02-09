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

class KJob;

/**
  * @brief Changes the @c nbib:organization of a @c nbib:Publication
  *
  * show/edit the @c nco:fullname of the @c nco:OrganizationContact
  *
  * Tries to be intelligent. if the resource is an @c nbib:Article the organization
  * was attached to the @c nbib:Collection (Proceedings). For all other cases the
  * organization is taken directly from the resource. Happens for the @c institution of an
  * @c nbib:Thesis or the @c court for a @c nbib:LegalCaseDocument
  */
class OrganizationEdit : public PropertyEdit
{
    Q_OBJECT
public:
    OrganizationEdit(QWidget *parent = 0);

private:
    /**
      * Shows the @c nco:fullname of the @ nco:OrganizationContact
      */
    void setupLabel();

    /**
      * Removes any old organization connected to the publication and adds the new entered.
      *
      * Already existing resources will be used if available, or a complete new created
      */
    void updateResource( const QString & text );
};

#endif // ORGANIZATIONEDIT_H
