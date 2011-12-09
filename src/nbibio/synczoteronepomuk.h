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

#ifndef SYNCZOTERONEPOMUK_H
#define SYNCZOTERONEPOMUK_H

#include "nbibsync.h"

#include <kbibtex/file.h>

class ReadFromZotero;
class WriteToZotero;
class BibTexToNepomukPipe;
class NepomukToBibTexPipe;
class Element;
class Entry;
class EntryClique;

class SyncZoteroNepomuk : public NBibSync
{
    Q_OBJECT
public:
    explicit SyncZoteroNepomuk(QObject *parent = 0);
    virtual ~SyncZoteroNepomuk();

public slots:
    void startUpload();
    void startDownload();
    void startSync();

private slots:
    /**
      * process all data retrieved from the zotero server
      */
    void readDownloadSync(File zoteroData);
    void readDownloadSyncAfterDelete(File zoteroData);

    /**
      * process syncdata retrived from zotero server when new items are send to the server
      */
    void readUploadSync(File zoteroData);

    void deleteLocalFiles(bool deleteThem);

private:
    void findDuplicates(const File &zoteroData, File &newEntries, QList<SyncDetails> &userMergeRequest);
    void findDeletedEntries(const File &zoteroData, QList<SyncDetails> &userDeleteRequest);
    void writeSyncDetailsToNepomuk(Entry *localData, Entry *zoteroData);

private:
    ReadFromZotero *m_rfz;
    WriteToZotero *m_wtz;
    BibTexToNepomukPipe *m_btnp;
    NepomukToBibTexPipe *m_ntnp;
    File m_bibCache;
    bool m_syncMode;
    QList<SyncDetails> m_tmpUserDeleteRequest;
};

#endif // SYNCZOTERONEPOMUK_H
