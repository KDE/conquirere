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

#ifndef PUBLICATIONEDIT_H
#define PUBLICATIONEDIT_H

#include "propertyedit.h"

class KJob;

/**
  * @brief Used to edit the publication for a specific reference
  *
  * Edits the @c nbib:publication to connect @c nbib:Publication to its Reference
  * The label edits/shows the @c nie:title of the publication
  */
class PublicationEdit : public PropertyEdit
{
    Q_OBJECT
public:
    PublicationEdit(QWidget *parent = 0);

private:
    void setupLabel();

    void updateResource( const QString & newPublicationTitle );
};

#endif // PUBLICATIONEDIT_H
