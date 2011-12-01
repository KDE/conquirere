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

#include "synczoteronepomuk.h"

#include "pipe/bibtextonepomukpipe.h"
#include "pipe/nepomuktobibtexpipe.h"
#include "../onlinestorage/zotero/readfromzotero.h"
#include "../onlinestorage/zotero/writetozotero.h"

#include <kbibtex/file.h>

#include "nbib.h"
#include <Nepomuk/Query/Term>
#include <Nepomuk/Query/ResourceTerm>
#include <Nepomuk/Query/ResourceTypeTerm>
#include <Nepomuk/Query/ComparisonTerm>
#include <Nepomuk/Query/QueryServiceClient>
#include <Nepomuk/Query/Result>
#include <Nepomuk/Query/QueryParser>

SyncZoteroNepomuk::SyncZoteroNepomuk(QObject *parent)
    : QObject(parent)
    , m_rfz(0)
    , m_wtz(0)
    , m_bibCache(0)
    , m_btnp(0)
    , m_ntnp(0)
    , m_syncSteps(0)
    , m_curStep(0)
{
}

SyncZoteroNepomuk::~SyncZoteroNepomuk()
{
//    delete m_rfz;
//    delete m_wtz;
//    delete m_btnp;
//    delete m_ntnp;
}

void SyncZoteroNepomuk::setUserName(const QString &name)
{
    m_name = name;
}

void SyncZoteroNepomuk::setPassword(const QString &pwd)
{
    m_pwd = pwd;
}

void SyncZoteroNepomuk::setUrl(const QString &url)
{
    m_url = url;
}

void SyncZoteroNepomuk::setCollection(const QString &collection)
{
    m_collection = collection;
}

void SyncZoteroNepomuk::startUpload()
{
    m_rfz = new ReadFromZotero;
    connect(m_rfz, SIGNAL(progress(int)), this, SLOT(calculateProgress(int)));
    m_wtz = new WriteToZotero;
    connect(m_wtz, SIGNAL(progress(int)), this, SLOT(calculateProgress(int)));
    m_btnp = new BibTexToNepomukPipe;
    connect(m_btnp, SIGNAL(progress(int)), this, SLOT(calculateProgress(int)));
    m_ntnp = new NepomukToBibTexPipe;
    connect(m_ntnp, SIGNAL(progress(int)), this, SLOT(calculateProgress(int)));

    m_syncSteps = 3;
    m_curStep = 0;

    // setp 1 fetch data from nepomuk
    emit progressStatus(i18n("fetch data from Nepomuk"));
    Nepomuk::Query::Query query( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NBIB::Publication() ) );

    QList<Nepomuk::Query::Result> queryResult = Nepomuk::Query::QueryServiceClient::syncQuery(query);

    QList<Nepomuk::Resource> exportList;
    foreach(const Nepomuk::Query::Result & r, queryResult) {
        exportList.append(r.resource());
    }

    // step 2 pipe to bibtex
    emit progressStatus(i18n("prepare data for Zotero"));
    m_curStep = 1;
    m_ntnp->pipeExport(exportList);
    File bibfile = m_ntnp->bibtexFile();

    // step 3 upload to zotero
    emit progressStatus(i18n("upload to Zotero"));
    m_curStep = 2;
    m_wtz->setUserName(m_name);
    m_wtz->setPassword(m_pwd);
    m_wtz->setUrl(m_url);
    m_wtz->setAdoptBibtexTypes(true);

    m_wtz->pushItems(bibfile, m_collection);
}

void SyncZoteroNepomuk::startDownload()
{
    m_rfz = new ReadFromZotero;
    connect(m_rfz, SIGNAL(progress(int)), this, SLOT(calculateProgress(int)));
    m_wtz = new WriteToZotero;
    connect(m_wtz, SIGNAL(progress(int)), this, SLOT(calculateProgress(int)));
    m_btnp = new BibTexToNepomukPipe;
    connect(m_btnp, SIGNAL(progress(int)), this, SLOT(calculateProgress(int)));
    m_ntnp = new NepomukToBibTexPipe;
    connect(m_ntnp, SIGNAL(progress(int)), this, SLOT(calculateProgress(int)));

    emit progressStatus(i18n("fetch data from Zotero server"));
    m_syncSteps = 2;
    m_curStep = 0;

    //lets start by retrieving all items from the server and merge them with the current files
    m_rfz->setUserName(m_name);
    m_rfz->setPassword(m_pwd);
    m_rfz->setUrl(m_url);
    m_rfz->setAdoptBibtexTypes(true);

    connect(m_rfz, SIGNAL(itemsInfo(File)), this, SLOT(readDownloadSync(File)));

    m_rfz->fetchItems(m_collection);
}

void SyncZoteroNepomuk::readDownloadSync(File zoteroData)
{
    emit progressStatus(i18n("push Zotero data into Nepomuk"));
    //as we download the files only we push them directly into nepomuk
    m_curStep = 1;
    m_btnp->pipeExport(zoteroData);
}

void SyncZoteroNepomuk::startSync()
{

}

void SyncZoteroNepomuk::calculateProgress(int value)
{
    qreal curProgress = ((qreal)value * 1.0/m_syncSteps);

    curProgress += (qreal)(100.0/m_syncSteps) * m_curStep;

    emit progress(curProgress);
}
