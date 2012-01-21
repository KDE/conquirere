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

#ifndef VOLUMENUMBEREDIT_H
#define VOLUMENUMBEREDIT_H

#include "propertyedit.h"

class QStandardItemModel;

/**
  * @brief Allows to edit the volume/number of either a @c nbib:JournalIssue or usual @c nbib:Publication
  *
  * Multivolume books can have a volume/number too. Also we handle special cases for the @c nbib:CodeOfLaw and
  * @c nbib:CourtReporter
  *
  * This means even when we show the the Publication we can add this edit and manipulate the right collections
  * instead of the publication when we change values in teh widget,
  *
  * Shows manipulates either @c nbib:volume or @c nbib:numer depending on the propertyUrl()
  */
class VolumeNumberEdit : public PropertyEdit
{
public:
    VolumeNumberEdit(QWidget *parent = 0);

protected:
    void setupLabel();

    virtual void updateResource( const QString & text );
};

#endif // VOLUMENUMBEREDIT_H
