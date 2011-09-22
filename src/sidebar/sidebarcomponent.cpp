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

#include "sidebarcomponent.h"

SidebarComponent::SidebarComponent(QWidget *parent)
    : QWidget(parent)
    , m_project(0)
{
}

void SidebarComponent::setProject(Project *p)
{
    if(p) {
        m_project = p;
        m_libraryType == Library_Project;
    }
    else {
        m_project = 0;
        m_libraryType == Library_System;
    }
}
Project *SidebarComponent::project()
{
    return m_project;
}

void SidebarComponent::setLibraryType(LibraryType type)
{
    m_libraryType =type;
}

LibraryType SidebarComponent::libraryType()
{
    return m_libraryType;
}
