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

#ifndef STRINGEDIT_H
#define STRINGEDIT_H

#include "propertyedit.h"

/**
  * @brief Very simple edit widget, interpretes all propertys as string
  */
class StringEdit : public PropertyEdit
{
public:
    StringEdit(QWidget *parent = 0);

private:
    void setupLabel();

    virtual void updateResource( const QString & text );
};

#endif // STRINGEDIT_H
