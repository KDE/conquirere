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

#ifndef CONTACTEDIT_H
#define CONTACTEDIT_H

#include "propertyedit.h"

/**
  * Used to manipulate an @c nco:Contact property.
  *
  * The entered/shown text represents the fullname() of the contact
  */
class ContactEdit : public PropertyEdit
{
public:
    ContactEdit(QWidget *parent = 0);
    virtual ~ContactEdit();

protected:
    void setupLabel();

    virtual void createCompletionModel( const QList< Nepomuk::Query::Result > &entries );

    virtual void updateResource( const QString & text );
};

#endif // CONTACTEDIT_H
