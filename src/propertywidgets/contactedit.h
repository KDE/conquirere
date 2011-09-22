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

class ContactEdit : public PropertyEdit
{
public:
    ContactEdit(QWidget *parent = 0);
    virtual ~ContactEdit();

    void setUseInternalDialog(bool useIt);

public slots:
    void detailEditRequested();

protected:
    void setupLabel();
    /**
      * Has to be reimplemented for any subclass
      */
    virtual void createCompletionModel( const QList< Nepomuk::Query::Result > &entries );

    /**
      * update the resource with the @p text from the edit field
      */
    virtual void updateResource( const QString & text );

private:
    bool m_useInternalDialog;
};

#endif // CONTACTEDIT_H