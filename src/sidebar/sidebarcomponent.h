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

#ifndef SIDEBARCOMPONENT_H
#define SIDEBARCOMPONENT_H

#include <QWidget>
#include <Nepomuk/Resource>

#include "../globals.h"

class Library;

class SidebarComponent : public QWidget
{
    Q_OBJECT
public:
    explicit SidebarComponent(QWidget *parent = 0);
    virtual ~SidebarComponent() {}

    void setLibrary(Library *p);
    Library *library();

public slots:
    virtual void setResource(Nepomuk::Resource & resource) = 0;
    virtual void clear() = 0;

private:
    Library *m_library;
};

#endif // SIDEBARCOMPONENT_H
