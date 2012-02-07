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

#ifndef CHAPTEREDIT_H
#define CHAPTEREDIT_H

#include "propertyedit.h"

/**
  * @brief Used to add a chapter to a bibreference
  *
  * @pre the resource must be a @c nbib:Reference
  */
class ChapterEdit : public PropertyEdit
{
public:
    ChapterEdit(QWidget *parent = 0);

private:
    /**
      * Use @c nie:title and @c nbib:chapterNumber of the @c nbib:Chapter
      */
    void setupLabel();

    /**
      * not allowed use external edit dialog
      */
    void updateResource( const QString & text );
};

#endif // CHAPTEREDIT_H
