/*
 * Copyright 2012 Jörg Ehrichs <joerg.ehrichs@gmx.de>
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

#include "synckbibtexfile.h"

#include "pipe/bibtextonepomukpipe.h"
#include "pipe/nepomuktobibtexpipe.h"
#include "onlinestorage/kbibtexfile/kbtfileinfo.h"
#include "onlinestorage/kbibtexfile/readfromfile.h"
#include "onlinestorage/kbibtexfile/writetofile.h"
#include "onlinestorage/providersettings.h"

#include "sync.h"
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Vocabulary/NUAO>
#include <Nepomuk/Variant>
#include <Nepomuk/Query/Term>
#include <Nepomuk/Query/ResourceTerm>
#include <Nepomuk/Query/ResourceTypeTerm>
#include <Nepomuk/Query/ComparisonTerm>
#include <Nepomuk/Query/NegationTerm>
#include <Nepomuk/Query/LiteralTerm>
#include <Nepomuk/Query/AndTerm>
#include <Nepomuk/Query/OrTerm>
#include <Nepomuk/Query/QueryServiceClient>
#include <Nepomuk/Query/Result>
#include <Nepomuk/Query/QueryParser>

SyncKBibTeXFile::SyncKBibTeXFile(QObject *parent)
    : NBibSync(parent)
    , m_rff(0)
    , m_wtf(0)
    , m_btnp(0)
    , m_ntnp(0)
    , m_syncMode(false)
{
}

SyncKBibTeXFile::~SyncKBibTeXFile()
{
    delete m_btnp;
    delete m_ntnp;
}


void SyncKBibTeXFile::startDownload()
{
    qDebug() << "SyncKBibTeXFile::startDownload()";
    m_rff = new ReadFromFile;
    m_rff->setProviderSettings(m_psd);

    connect(m_rff, SIGNAL(progress(int)), this, SLOT(calculateProgress(int)));
    emit progressStatus(i18n("fetch data from File: %1", m_psd.url));

    // if we do not use syncmode, we initialize the syncSteps here
    // they are used to properly calculate the current progress
    if(!m_syncMode) {
        m_syncSteps = 4;
        m_curStep = 0;
    }
    else {
        m_curStep++;
    }
    emit calculateProgress(0);

    //lets start by retrieving all items from the input file
    connect(m_rff, SIGNAL(itemsInfo(File)), this, SLOT(readDownloadSync(File)));
    m_rff->fetchItems(m_psd.collection);
}

void SyncKBibTeXFile::readDownloadSync(const File & zoteroData)
{

    //m_tmpUserDeleteRequest.clear();
    //findDeletedEntries(zoteroData, m_tmpUserDeleteRequest);

    m_bibCache.clear();
    m_bibCache = zoteroData;

    readDownloadSyncAfterDelete(m_bibCache);
}

void SyncKBibTeXFile::readDownloadSyncAfterDelete(const File &zoteroData)
{
    m_tmpUserMergeRequest.clear();
    File newEntries = zoteroData;
//    findDuplicates(zoteroData, newEntries, m_tmpUserMergeRequest);

//    if(!m_tmpUserMergeRequest.isEmpty()) {
//        qDebug() << "SyncZoteroNepomuk::readDownloadSync user merge request necessary for " << m_tmpUserMergeRequest.size() << "items";

//        if(m_psd.mergeMode == Manual) {
//            emit userMerge(m_tmpUserMergeRequest);
//        }
//    }

    // up to this point we have a list of new entries we need to add
    // and a list of items that need an update, that either means merging automatic server/local or let the user decide how to merge
    delete m_btnp;
    m_btnp = new BibTexToNepomukPipe;
    connect(m_btnp, SIGNAL(progress(int)), this, SLOT(calculateProgress(int)));

    emit progressStatus(i18n("push new File data into Nepomuk"));
    m_curStep++;
//    if(m_psd.collection.isEmpty()) {
//        m_btnp->setSyncDetails(m_psd.url, m_psd.userName);
//    }
//    else {
//        QString url = m_psd.url + QLatin1String("/") + m_psd.collection;
//        m_btnp->setSyncDetails(url, m_psd.userName);
//    }

    Akonadi::Collection contactBook(m_psd.akonadiContactsUUid);
    if(contactBook.isValid())
        m_btnp->setAkonadiAddressbook(contactBook);

    m_btnp->pipeExport(newEntries);

    m_curStep++;
    emit calculateProgress(50);

    // wait until the user merged all entries on its own
    if(m_tmpUserMergeRequest.size() > 0) {
        emit progressStatus(i18n("wait until user merge is finished"));
    }
    else {
        mergeFinished();
    }
}

void SyncKBibTeXFile::mergeFinished()
{
    emit calculateProgress(100);
    //we finished everything, so cleanup
    delete m_btnp;
    m_btnp = 0;
    m_rff->deleteLater();
    m_rff = 0;

    if(m_syncMode) {
        startUpload();
    }
}

void SyncKBibTeXFile::startUpload()
{
    m_wtf = new WriteToFile;
    m_wtf->setProviderSettings(m_psd);
    m_wtf->setAdoptBibtexTypes(true);
    connect(m_wtf, SIGNAL(progress(int)), this, SLOT(calculateProgress(int)));

    delete m_ntnp;
    m_ntnp = new NepomukToBibTexPipe;
    connect(m_ntnp, SIGNAL(progress(int)), this, SLOT(calculateProgress(int)));

    // step 1 fetch data from nepomuk
    emit progressStatus(i18n("fetch data from Nepomuk"));

    // if we do not use syncmode, we initialize the syncSteps here
    // they are used to properly calculate the current progress
    if(!m_syncMode) {
        m_syncSteps = 4;
        m_curStep = 0;
    }
    else {
        m_curStep++;
    }

    emit calculateProgress(0);

    Nepomuk::Query::Query query( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NBIB::Reference() ) );
    QList<Nepomuk::Query::Result> queryResult = Nepomuk::Query::QueryServiceClient::syncQuery(query);

    QList<Nepomuk::Resource> exportList;
    foreach(const Nepomuk::Query::Result & r, queryResult) {
        exportList.append(r.resource());
    }

    // step 2 pipe to bibtex
    emit progressStatus(i18n("prepare data for File upload"));
    m_curStep++;

//    if(m_psd.collection.isEmpty()) {
//        m_ntnp->setSyncDetails(m_psd.url, m_psd.userName);
//    }
//    else {
//        QString url = m_psd.url + QLatin1String("/") + m_psd.collection;
//        m_ntnp->setSyncDetails(url, m_psd.userName);
//    }

    m_ntnp->addNepomukUries(true);
    m_ntnp->pipeExport(exportList);
    m_bibCache.clear();
    m_bibCache.append(*m_ntnp->bibtexFile());

    // step 3 upload to zotero
    emit progressStatus(i18n("upload to File: %1", m_psd.url));
    m_curStep++;

    connect(m_wtf, SIGNAL(itemsInfo(File)), this, SLOT(readUploadSync(File)));
    m_wtf->pushItems(m_bibCache, m_psd.collection);
}

void SyncKBibTeXFile::readUploadSync(File zoteroData)
{
    qDebug() << "SyncKBibTeXFile::readUploadSync(File zoteroData)";
    m_curStep++;
    emit calculateProgress(100);
    //we finished everything, so cleanup
    m_wtf->deleteLater();
    m_wtf = 0;

    //delete m_ntnp;
    m_ntnp = 0;

    m_syncMode = false;
    return;
}

void SyncKBibTeXFile::startSync()
{
    m_syncMode = true;
    m_syncSteps = 9;
    m_curStep = 0;
    startDownload();
}


void SyncKBibTeXFile::deleteLocalFiles(bool deleteThem)
{

}

