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

#ifndef FILEINFO_H
#define FILEINFO_H

#include "../storageinfo.h"

#include "writetofile.h"
#include "readfromfile.h"
#include "syncfile.h"

class KBTFileInfo : public StorageInfo
{
public:
    KBTFileInfo():StorageInfo(),wts(new WriteToFile),rfs(new ReadFromFile),ss(new SyncFile) {}
    virtual ~KBTFileInfo() { delete wts; delete rfs; delete ss;}

    QString providerId() { return QLatin1String("kbibtexfile"); }
    QString providerName() { return QLatin1String("KBibTeXFile"); }
    KIcon providerIcon() { return KIcon(QLatin1String("storage-kbibtex")); }

    bool supportCollections() { return false; }
    QString helpText()  { return i18n("Provider to sync your data as file\n\n"
                                      "Name : the name for identification if necessary\n"
                                      "Password : the password for identification if necessary\n"
                                      "url : full path and file name to sync with\n"
                                      "This provider supports different filetypes use the right file extension to export it into this type\n"
                                      "Supported filetypes: .bib, .ris, .pdf, .html, .blg, .ps, .xml, .xslt"); }

    virtual QString defaultUrl() { return QLatin1String(""); }

    WriteToStorage *writeHandle() { return wts; }
    ReadFromStorage *readHandle() { return rfs; }
    SyncStorage *syncHandle() { return ss; }

private:
    WriteToStorage *wts;
    ReadFromStorage *rfs;
    SyncStorage *ss;
};

#endif // FILEINFO_H