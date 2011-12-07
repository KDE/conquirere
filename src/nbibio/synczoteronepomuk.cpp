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

#include <kbibtex/element.h>
#include <kbibtex/entry.h>
#include <kbibtex/findduplicates.h>
#include <kbibtex/findduplicatesui.h>

#include "nbib.h"
#include "sync.h"
#include <Nepomuk/Variant>
#include <Nepomuk/Query/Term>
#include <Nepomuk/Query/ResourceTerm>
#include <Nepomuk/Query/ResourceTypeTerm>
#include <Nepomuk/Query/ComparisonTerm>
#include <Nepomuk/Query/LiteralTerm>
#include <Nepomuk/Query/AndTerm>
#include <Nepomuk/Query/QueryServiceClient>
#include <Nepomuk/Query/Result>
#include <Nepomuk/Query/QueryParser>

//DEBUG
#include <QDebug>
#include <QFile>
#include <kbibtex/fileexporterbibtex.h>

SyncZoteroNepomuk::SyncZoteroNepomuk(QObject *parent)
    : QObject(parent)
    , m_rfz(0)
    , m_wtz(0)
    , m_btnp(0)
    , m_ntnp(0)
    , m_askBeforeDeletion(false)
    , m_syncSteps(0)
    , m_curStep(0)
{
    qRegisterMetaType<CollectionInfo>("EntryClique*");
    qRegisterMetaType<QList<CollectionInfo> >("QList<EntryClique*>");
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

void SyncZoteroNepomuk::askBeforeDeletion(bool ask)
{
    m_askBeforeDeletion = ask;
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
    QString url = m_url + QLatin1String("/") + m_collection;
    m_ntnp->setSyncDetails(url, m_name);
    m_ntnp->pipeExport(exportList);
    File bibfile = m_ntnp->bibtexFile();


    QFile exportFile(QString("/home/joerg/zotero_upload_DEBUG.bib"));
    if (!exportFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    qDebug() << "SyncStorageUi save after sync"  << bibfile.size();

    FileExporterBibTeX feb;
    feb.save(&exportFile, &bibfile);
    emit progress(100);
    return;

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
    emit progressStatus(i18n("sync zotero data with local Nepomuk storage"));
    m_curStep = 1;

    // for each downloaded item from zotero we try to find the item in the local storage
    // we can itendify this via the unique zotero Key

    // In the case we found such an entry, we check if the zotero etag value are different
    // If this is the case, the item has been changed on the server side and we need to merge them

    File newEntries;

    foreach(Element* element, zoteroData) {
        Entry *entry = dynamic_cast<Entry *>(element);
        if(!entry) { continue; }

        // define what we are looking for in the nepomuk storage
        Nepomuk::Query::AndTerm andTerm;
        andTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::SYNC::ServerSyncData() ) );
        andTerm.addSubTerm( Nepomuk::Query::ComparisonTerm( Nepomuk::Vocabulary::SYNC::provider(), Nepomuk::Query::LiteralTerm( "zotero" ) ));
        //andTerm.addSubTerm( Nepomuk::Query::ComparisonTerm( Nepomuk::Vocabulary::SYNC::userId(), Nepomuk::Query::LiteralTerm( m_name ) ));
        //QString itemID = PlainTextValue::text(entry->value( QLatin1String("zoterokey")) );
        //andTerm.addSubTerm( Nepomuk::Query::ComparisonTerm( Nepomuk::Vocabulary::SYNC::id(),  Nepomuk::Query::LiteralTerm( itemID )));
        Nepomuk::Query::Query query( andTerm );

        QList<Nepomuk::Query::Result> queryResult = Nepomuk::Query::QueryServiceClient::syncQuery(query);

        // nothing found, means we have a new entry
        if(queryResult.isEmpty()) {
            qDebug() << "did not find any related items";
            newEntries.append(element);
        }
        // we found something, means we need to check if it changed on the server
        else {
            if(queryResult.size() > 1) {
                qWarning() << "error database error, found more than 1 item to sync the zotero data to size::" << queryResult.size();
            }

            Nepomuk::Resource r = queryResult.first().resource();

            QString localEtag = r.property(Nepomuk::Vocabulary::SYNC::etag()).toString();
            QString serverEtag = PlainTextValue::text(entry->value(QLatin1String("zoteroetag")));

            qDebug() << "check if the item changed on the server local" << localEtag << "server" << serverEtag;

            if(localEtag != serverEtag) {
                qDebug() << "item need merge action" << PlainTextValue::text(entry->value(QLatin1String("title")));
            }
            else {
                qDebug() << "item is up to date" << PlainTextValue::text(entry->value(QLatin1String("title")));
            }
        }
    }


    // up to this point we have a list of new entries we need to add
    // and a list of items that need an update, that either means merging automatic server/local or let the user decide how to merge
    return;

    emit progressStatus(i18n("push new Zotero data into Nepomuk"));
    QString url = m_url + QLatin1String("/") + m_collection;
    m_btnp->setSyncDetails(url, m_name);
    m_btnp->pipeExport(newEntries);

//    QFile exportFile(QString("/home/joerg/zotero_export.bib"));
//    if (!exportFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
//        return;
//    }

//    qDebug() << "SyncStorageUi save after sync"  << zoteroData.size();

//    FileExporterBibTeX feb;
//    feb.save(&exportFile, &zoteroData);
}

void SyncZoteroNepomuk::startSync()
{
    m_rfz = new ReadFromZotero;
    connect(m_rfz, SIGNAL(progress(int)), this, SLOT(calculateProgress(int)));
    m_wtz = new WriteToZotero;
    connect(m_wtz, SIGNAL(progress(int)), this, SLOT(calculateProgress(int)));
    m_btnp = new BibTexToNepomukPipe;
    connect(m_btnp, SIGNAL(progress(int)), this, SLOT(calculateProgress(int)));
    m_ntnp = new NepomukToBibTexPipe;
    connect(m_ntnp, SIGNAL(progress(int)), this, SLOT(calculateProgress(int)));

    m_syncSteps = 6;
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
    m_bibCache = m_ntnp->bibtexFile();

    // step 3 fetch data from zotero
    emit progressStatus(i18n("fetch data from Zotero server"));
    m_curStep = 2;

    //lets start by retrieving all items from the server and merge them with the current files
    m_rfz->setUserName(m_name);
    m_rfz->setPassword(m_pwd);
    m_rfz->setUrl(m_url);
    m_rfz->setAdoptBibtexTypes(true);

    connect(m_rfz, SIGNAL(itemsInfo(File)), this, SLOT(readSyncronizeSync(File)));

    m_rfz->fetchItems(m_collection);
}

void SyncZoteroNepomuk::readSyncronizeSync(File zoteroData)
{
    // up to this point we have all nepomuk data as bibtex file
    // and all data from the server as bibtex file now combine them

    // step 4 merge data
    emit progressStatus(i18n("merge Zotero data with Nepomuk"));
    m_curStep = 3;

    calculateProgress(0);
    qDebug() << "SyncZoteroNepomuk::readSyncronizeSync || entries" << m_bibCache.size() << " + new" << zoteroData.size();

    // now go through all retrieved serverFiles and see if we have to merge something
    // if we find another entry with the same zoterokey and the same zoteroetag skip this entry
    // if the zoteroetag is different, the file changed on the server and must be merged
    // if no entry with the zoterokey exist, add the new entry

    QStringList updatedKeys;
    // go through all retrieved entries
    foreach(Element* element, zoteroData) {
        Entry *entry = dynamic_cast<Entry *>(element);
        if(!entry) { continue; }

        QString zoteroKey = PlainTextValue::text(entry->value(QLatin1String("zoterokey")));
        updatedKeys.append(zoteroKey);
        QString zoteroEtag = PlainTextValue::text(entry->value(QLatin1String("zoteroetag")));

        bool addEntry = true;
        // check if the zoterokey exist
        foreach(Element* checkElement, m_bibCache) {
            Entry *checkEntry = dynamic_cast<Entry *>(checkElement);
            if(!checkEntry) { continue; }

            QString checkZoteroKey = PlainTextValue::text(checkEntry->value(QLatin1String("zoterokey")));
            if(!checkZoteroKey.isEmpty() && checkZoteroKey == zoteroKey) {
                // ok the entry we retrieved from the server exist in the local version already

                // check if the entry changed on the server
                QString checkZoteroEtag = PlainTextValue::text(checkEntry->value(QLatin1String("zoteroetag")));
                if(zoteroEtag == checkZoteroEtag ) {
                    // item did not change, ignore it
                    addEntry = false;
                }

                break; // stop checking here we found an entry with the right zoterokey
            }
        }

        if(addEntry) {
            m_bibCache.append(element);
        }
    }

    calculateProgress(40);

    // now we delete all entries that have a zoterokey which we did not retrieve from the server
    // this means we deleted the entry on the server
    temp_toBeDeleted.clear();
    foreach(Element* element, m_bibCache) {
        Entry *entry = dynamic_cast<Entry *>(element);
        if(!entry) { continue; }

        QString checkZoteroKey = PlainTextValue::text(entry->value(QLatin1String("zoterokey")));

        if(!checkZoteroKey.isEmpty() && !updatedKeys.contains(checkZoteroKey)) {
            temp_toBeDeleted.append(element);
            qDebug() << "item to be deleted " << checkZoteroKey;
        }
    }

    if(m_askBeforeDeletion && !temp_toBeDeleted.empty()) {
        emit askForDeletion(temp_toBeDeleted.size()); // wait for a user responce
    }
    else {
        deleteLocalFiles(true); // or simply delete them
    }
}

void SyncZoteroNepomuk::deleteLocalFiles(bool deleteThem)
{
    if(deleteThem) {
        foreach(Element* e, temp_toBeDeleted) {
            m_bibCache.removeAll(e);
            delete e;
        }
    }
    else {
        // remove zoteroinfo so they will be uploaded again
        foreach(Element* element, temp_toBeDeleted) {
            Entry *entry = dynamic_cast<Entry *>(element);
            entry->remove(QLatin1String("zoterokey"));
            entry->remove(QLatin1String("zoteroetag"));
            entry->remove(QLatin1String("zoteroupdated"));
            entry->remove(QLatin1String("zoterochildren"));
        }
    }

    temp_toBeDeleted.clear();

    calculateProgress(50);

    //################################################################################################
    // now we have the list of existing entries together with all server entries that changed
    // merge them automatically or via user interaction
    //
    // this means we also get duplicates if an item changed on the server
    // this item will have the same zoterokey as another one but a different etag and some other keys will differ too
    // these must be merged here somehow.

    int sensitivity = 4000; // taken from KBibTeX
    FindDuplicates fd(0, sensitivity);
    QList<EntryClique*> cliques;
    fd.findDuplicateEntries(&m_bibCache, cliques);

    qDebug() << "duplicates" << cliques.size() << "of entries" << m_bibCache.size() << "ask user what he wants to do with it";

    // if we have some duplicates, let the user merge them together
    if(cliques.size() > 0) {
        emit mergeResults(cliques, &m_bibCache);
    }
    else {
        resultsMerged(cliques);
    }

}

void SyncZoteroNepomuk::resultsMerged(QList<EntryClique*> cliques)
{
    MergeDuplicates md(0);
    md.mergeDuplicateEntries(cliques, &m_bibCache);

    // and proceed with the sync
    calculateProgress(100);

    // everything in order now, lets push all items to zotero
    // the m_bibCache holds only new items without zoterokeys
    // and changed items that need an update on the server

    // step 5 upload data
    emit progressStatus(i18n("upload data to Zotero"));
    m_curStep = 4;

    connect(m_wtz, SIGNAL(itemsInfo(File)), this, SLOT(writeSync(File)));
    m_wtz->setUserName(m_name);
    m_wtz->setPassword(m_pwd);
    m_wtz->setUrl(m_url);
    m_wtz->setAdoptBibtexTypes(true);
    m_wtz->pushItems(m_bibCache, m_collection);
}

void SyncZoteroNepomuk::writeSync(File zoteroData)
{
    // this function is called after all items are send to the server
    // items that where simply updated are handled by the WroteToZotero class directly
    // whats left are the newly created items
    // these are identical to some other entry but they have no "citekey" and the zoterotags added

    qDebug() << "TODO!!!! new entries we need to add a zoterokey to" << zoteroData.size();

    m_bibCache.append(zoteroData); //just add them, this will result in lots of duplicates, fix this later on


    // step 6 push new data into nepomuk again
    emit progressStatus(i18n("push Zotero data into Nepomuk"));
    m_curStep = 5;
    m_btnp->pipeExport(m_bibCache);
}

void SyncZoteroNepomuk::calculateProgress(int value)
{
    qreal curProgress = ((qreal)value * 1.0/m_syncSteps);

    curProgress += (qreal)(100.0/m_syncSteps) * m_curStep;

    emit progress(curProgress);
}
