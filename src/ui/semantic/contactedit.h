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

#include "labeledit.h"

#include <Nepomuk/Resource>

class QWidget;

/**
  * @todo add pushbutton + dialog to edit contacts in full detail (address and so on)
  */
class ContactEdit : public LabelEdit
{
    Q_OBJECT
public:
    explicit ContactEdit(QWidget *parent = 0);
    ~ContactEdit();

public slots:
    virtual void updateResource(const QString & text);
    virtual void updateLabel();

};

#endif // CONTACTEDIT_H
