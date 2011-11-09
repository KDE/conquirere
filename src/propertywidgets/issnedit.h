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

#ifndef ISSNEDIT_H
#define ISSNEDIT_H

#include "propertyedit.h"

/**
  * @brief Allows to change the International Standard Serial Number for articles/journals
  *
  * The ISSN is forced to be unique (UNIQUE_PROPERTY) as it makes no sense to have more than one number
  */
class IssnEdit : public PropertyEdit
{
public:
    IssnEdit(QWidget *parent = 0);

protected:
    /**
      * Simply shows the connected ISSN of the connected NBIB::Series object
      */
    void setupLabel();

    /**
      * No completion is used
      */
    virtual void createCompletionModel( const QList< Nepomuk::Query::Result > &entries );

    /**
      * sets the ISSN t othe string from the KLineEdit to the connected Series
      */
    virtual void updateResource( const QString & text );
};

#endif // ISSNEDIT_H
