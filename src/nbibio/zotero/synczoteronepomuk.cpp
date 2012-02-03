/*
 * Copyright 2011, 2012 Jörg Ehrichs <joerg.ehrichs@gmx.de>
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

#include "synczoteronepomuk.h"

#include "zoterodownload.h"
#include "zoteroupload.h"

#include <QtCore/QTimer>

SyncZoteroNepomuk::SyncZoteroNepomuk(QObject *parent)
    : NBibSync(parent)
    , m_syncMode(false)
    , m_cancel(false)
    , m_zoteroDownload(0)
    , m_zoteroUpload(0)
    , m_corruptedUploads(0)
{
}

SyncZoteroNepomuk::~SyncZoteroNepomuk()
{
    delete m_corruptedUploads;
}

void SyncZoteroNepomuk::startDownload()
{
    if(!m_syncMode) {
        m_syncSteps = 2;
        m_curStep = 0;
    }

    m_zoteroDownload = new ZoteroDownload;

    m_zoteroDownload->setProviderDetails(m_psd);
    m_zoteroDownload->setSystemLibrary(m_systemLibrary);
    m_zoteroDownload->setLibraryToSyncWith(m_libraryToSyncWith);

    if(m_corruptedUploads) {
        m_zoteroDownload->setCorruptedUploadEntries(m_corruptedUploads);
    }

    connect(m_zoteroDownload, SIGNAL(progress(int)), this, SLOT(calculateProgress(int)));
    connect(m_zoteroDownload, SIGNAL(progressStatus(QString)), this, SIGNAL(progressStatus(QString)));
    connect(m_zoteroDownload, SIGNAL(askForLocalDeletion(QList<SyncDetails>)), this, SIGNAL(askForLocalDeletion(QList<SyncDetails>)));
    connect(m_zoteroDownload, SIGNAL(userMerge(QList<SyncDetails>)), this, SIGNAL(userMerge(QList<SyncDetails>)));
    connect(m_zoteroDownload, SIGNAL(finished()), this, SLOT(downloadFinished()));

    m_zoteroDownload->startDownload();
}

void SyncZoteroNepomuk::mergeFinished()
{
    m_zoteroDownload->mergeFinished();
}

void SyncZoteroNepomuk::downloadFinished()
{
    delete m_zoteroDownload;
    m_zoteroDownload = 0;

    if(m_syncMode) {
        startUpload();
    }
    else {
        emit syncFinished();
    }
}

void SyncZoteroNepomuk::uploadFinished()
{
    m_syncMode = false;

    m_corruptedUploads = m_zoteroUpload->corruptedUploads();

    delete m_zoteroUpload;
    m_zoteroUpload = 0;

    if(m_corruptedUploads) {
        startDownload();
    }
    else {
        emit syncFinished();
    }
}

void SyncZoteroNepomuk::startUpload()
{
    if(!m_syncMode) {
        m_syncSteps = 1;
        m_curStep = 0;
    }
    else {
        m_curStep++;
    }

    m_zoteroUpload = new ZoteroUpload;

    m_zoteroUpload->setProviderDetails(m_psd);
    m_zoteroUpload->setSystemLibrary(m_systemLibrary);
    m_zoteroUpload->setLibraryToSyncWith(m_libraryToSyncWith);

    connect(m_zoteroUpload, SIGNAL(progress(int)), this, SLOT(calculateProgress(int)));
    connect(m_zoteroUpload, SIGNAL(progressStatus(QString)), this, SIGNAL(progressStatus(QString)));

    connect(m_zoteroUpload, SIGNAL(askForServerDeletion(QList<SyncDetails>)), this, SIGNAL(askForServerDeletion(QList<SyncDetails>)));
    connect(m_zoteroUpload, SIGNAL(askForGroupRemoval(QList<SyncDetails>)), this, SIGNAL(askForGroupRemoval(QList<SyncDetails>)));

    connect(m_zoteroUpload, SIGNAL(finished()), this, SLOT(uploadFinished()));

    m_zoteroUpload->startUpload();
}

void SyncZoteroNepomuk::startSync()
{
    m_syncMode = true;
    m_syncSteps = 2;
    m_curStep = 0;

    startDownload();
}

void SyncZoteroNepomuk::cancel()
{
    if(m_zoteroDownload) {
        m_zoteroDownload->cancel();
    }
    if(m_zoteroUpload) {
        m_zoteroUpload->cancel();
    }

    m_cancel = true;
}

void SyncZoteroNepomuk::deleteLocalFiles(bool deleteThem)
{
    m_zoteroDownload->deleteLocalFiles(deleteThem);
}

void SyncZoteroNepomuk::deleteServerFiles(bool deleteThem)
{
    m_zoteroUpload->removeFilesFromZotero(deleteThem);
}

void SyncZoteroNepomuk::deleteFromGroup(bool deleteThem)
{
    m_zoteroUpload->removeFilesFromGroup(deleteThem);
}
