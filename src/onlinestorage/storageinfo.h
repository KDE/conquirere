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

#ifndef STORAGEINFO_H
#define STORAGEINFO_H

#include <KDE/KIcon>
#include <QtCore/QString>

class WriteToStorage;
class ReadFromStorage;
class SyncStorage;

class StorageInfo
{
public:
    StorageInfo() {}
    virtual ~StorageInfo() {}

    virtual QString providerId() = 0;
    virtual QString providerName() = 0;
    virtual KIcon providerIcon() = 0;

    virtual WriteToStorage *writeHandle() = 0;
    virtual ReadFromStorage *readHandle() = 0;
    virtual SyncStorage *syncHandle() = 0;
};

#endif // STORAGEINFO_H
