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

#ifndef FILEOBJECTEDIT_H
#define FILEOBJECTEDIT_H

#include "propertyedit.h"

class FileObjectEdit : public PropertyEdit
{
    Q_OBJECT
public:
    explicit FileObjectEdit(QWidget *parent = 0);

    enum Mode {
        Local,
        Remote
    };

    void setMode(Mode mode);

protected:
    void setupLabel();

    virtual void createCompletionModel( const QList< Nepomuk::Query::Result > &entries );

    /**
      * @todo add http://oscaf.sourceforge.net/ndo.html#ndo:copiedFrom
      */
    virtual void updateResource( const QString & text );

private slots:
    void showFileSelection();

private:
    Mode m_mode;
};

#endif // FILEOBJECTEDIT_H
