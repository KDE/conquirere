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

#ifndef ZOTEROINFO_H
#define ZOTEROINFO_H

#include "../storageinfo.h"

#include "writetozotero.h"
#include "readfromzotero.h"
#include "synczotero.h"

class ZoteroInfo : public StorageInfo
{
public:
    ZoteroInfo():StorageInfo(),wts(new WriteToZotero),rfs(new ReadFromZotero),ss(new SyncZotero) {}
    virtual ~ZoteroInfo() { delete wts; delete rfs; delete ss;}

    QString providerId() { return QString("zotero"); }
    QString providerName() { return QString("Zotero"); }
    KIcon providerIcon() { return KIcon("storage-zotero"); }

    WriteToStorage *writeHandle() { return wts; }
    ReadFromStorage *readHandle() { return rfs; }
    SyncStorage *syncHandle() { return ss; }

private:
    WriteToStorage *wts;
    ReadFromStorage *rfs;
    SyncStorage *ss;
};

#endif // ZOTEROINFO_H
