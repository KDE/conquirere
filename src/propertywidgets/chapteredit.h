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
  * @pre setResource must be a @c nbib:Publication
  */
class ChapterEdit : public PropertyEdit
{
public:
    ChapterEdit(QWidget *parent = 0);

protected:
    /**
      * Use nie:title of the @c nbib:Chapter
      */
    void setupLabel();

    /**
      * @p entries consits of all chapters in the nepomuk storage
      *
      * Fill the completer with the @c nie:title strings
      *
      * @todo restrict to only the available chapters of the current publication (needs filtering and a way to find all chapters beforehand )
      */
    virtual void createCompletionModel( const QList< Nepomuk::Query::Result > &entries );

    /**
      * update the resource with the @p text from the edit field
      *
      * Interprete @p text as @c nie:title of the @c nbib:Chapter
      */
    virtual void updateResource( const QString & text );
};

#endif // CHAPTEREDIT_H
