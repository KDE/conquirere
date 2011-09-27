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

#ifndef GLOBALS_H
#define GLOBALS_H

#include <Qt>

enum ProjectTreeRole {
    Role_Library = Qt::UserRole,
    Role_ResourceType,
    Role_Project
};

enum LibraryType {
    Library_System,
    Library_Project
};
enum ResourceSelection {
    Resource_Library,
    Resource_Document,
    Resource_Mail,
    Resource_Media,
    Resource_Reference,
    Resource_Publication,
    Resource_Website,
    Resource_Note
};
#endif // GLOBALS_H
