/*
 * Copyright 2011 Jörg Ehrichs <joerg.ehrichs@gmx.de>
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

#ifndef PROCEEDINGSEDIT_H
#define PROCEEDINGSEDIT_H

#include "propertyedit.h"

/**
  * @brief Allows o edit the @c nbib:proceedings of an @c nbib:article
  *
  * @pre the resource must be a @c nbib:article that should be in a proceedings
  */
class ProceedingsEdit : public PropertyEdit
{
    Q_OBJECT
public:
    explicit ProceedingsEdit(QWidget *parent = 0);

protected:
    /**
      * Shows the @c nie:title of the proceedings
      */
    void setupLabel();

    /**
      * Adds the @c nie:title of all availabe proceedings to teh QCompleter
      */
    virtual void createCompletionModel( const QList< Nepomuk::Query::Result > &entries );

    /**
      * removes any old proceedings and connect it to the new one specified in the KLineEdit
      */
    virtual void updateResource( const QString & text );
};

#endif // PROCEEDINGSEDIT_H
