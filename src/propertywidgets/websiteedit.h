/*
 * Copyright 2012 Jörg Ehrichs <joerg.ehrichs@gmx.de>
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

#ifndef WEBSITEEDIT_H
#define WEBSITEEDIT_H

#include "propertyedit.h"

/**
  * @brief displayes the websites connected via @c nie:links
  */
class WebsiteEdit : public PropertyEdit
{
    Q_OBJECT
public:
    explicit WebsiteEdit(QWidget *parent = 0);

private slots:
    /**
      * opens the file selection dialog AddDataObject
      */
    void showUrlSelection();

private:
    /**
      * Simply shows the connected ISSN of the connected NBIB::Series object
      */
    void setupLabel();

    /**
      * sets the ISSN to the string from the KLineEdit to the connected Series
      */
    void updateResource( const QString & text );
};

#endif // WEBSITEEDIT_H
