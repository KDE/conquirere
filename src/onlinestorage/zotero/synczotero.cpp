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

#include "synczotero.h"

#include "readfromzotero.h"
#include "writetozotero.h"

#include <kbibtex/entry.h>
#include <kbibtex/findduplicates.h>

#include <QDebug>

SyncZotero::SyncZotero(QObject *parent)
    : SyncStorage(parent)
{
}

SyncZotero::~SyncZotero()
{
    delete m_rfz;
    delete m_wtz;
}

void SyncZotero::syncWithStorage(File *bibfile)
{
    emit syncInProgress(true);

    m_systemFiles = bibfile;

    //lets start by retrieving all items from the server and merge them with the current files
    m_rfz = new ReadFromZotero;
    m_rfz->setUserName(userName());
    m_rfz->setPassword(pasword());

    connect(m_rfz, SIGNAL(itemsInfo(File)), this, SLOT(readSync(File)));
    m_rfz->fetchItems();
}

void SyncZotero::readSync(File serverFiles)
{
    qDebug() << "SyncZotero::itemsFromServer || entries" << m_systemFiles->size() << " + new" << serverFiles.size();

    // now go through all retrieved serverFiles and see if we have to merge something
    // if we find another entry with the same zoterokey and the same zoteroetag skip this entry
    // if the zoteroetag is different, the file changed on the server and must be merged
    // if no entry with the zoterokey exist, add the new entry

    // go through all retrieved entries
    foreach(Element* element, serverFiles) {
        Entry *entry = dynamic_cast<Entry *>(element);
        if(!entry) { continue; }

        QString zoteroKey = PlainTextValue::text(entry->value("zoterokey"));
        QString zoteroEtag = PlainTextValue::text(entry->value("zoteroetag"));

        bool addEntry = true;
        // check if the zoterokey exist
        foreach(Element* checkElement, *m_systemFiles) {
            Entry *checkEntry = dynamic_cast<Entry *>(checkElement);
            if(!entry) { continue; }

            QString checkZoteroKey = PlainTextValue::text(checkEntry->value("zoterokey"));
            if(!checkZoteroKey.isEmpty() && checkZoteroKey == zoteroKey) {
                // ok the entry we retrieved from the server exist in the local version already

                // check if the entry changed on the server
                QString checkZoteroEtag = PlainTextValue::text(checkEntry->value("zoteroetag"));
                if(zoteroEtag == checkZoteroEtag ) {
                    // item did not change, ignore it
                    addEntry = false;
                }

                break; // stop checking here we found an entry with the right zoterokey
            }
        }

        if(addEntry) {
            m_systemFiles->append(element);
        }
    }

    //################################################################################################
    // now we have the list of existing entries together with all server entries that changed
    // merge them automatically or via user interaction
    //
    // this means we also get duplicates if an item changed on the server
    // this item will have the same zoterokey as another one but a different etag and some other keys will differ too
    // these must be merged here somehow.

    int sensitivity = 100; // taken from KBibTeX
    FindDuplicates fd(0, sensitivity);
    QList<EntryClique*> cliques;
    fd.findDuplicateEntries(m_systemFiles, cliques);

    qDebug() << "duplicates" << cliques.size() << "of entries" << m_systemFiles->size() << "ask user what he wants to do with it";

    MergeDuplicates md(0);
    md.mergeDuplicateEntries(cliques, m_systemFiles);

    // from this point on, we have all data from the server merged with the local items
    // now its time to send all data to the server, update changed items and create new ones
    m_wtz = new WriteToZotero;
    m_wtz->setUserName(userName());
    m_wtz->setPassword(pasword());
    m_wtz->pushItems(*m_systemFiles);

    connect(m_wtz, SIGNAL(itemsInfo(File)), this, SLOT(writeSync(File)));
}

void SyncZotero::writeSync(File serverFiles)
{
    // this function is called after all items are send to the server
    // items that where simply updated are handled by the WroteToZotero function directly
    // whats left are the newly created items
    // these are identical to some other entry but they have no "citekey" and the zoterotags added

    qDebug() << "new entries we need to merge and add a zoterokey to" << serverFiles.size();

    m_systemFiles->append(serverFiles);
    int sensitivity = 100; // taken from KBibTeX
    FindDuplicates fd(0, sensitivity);
    QList<EntryClique*> cliques;
    fd.findDuplicateEntries(m_systemFiles, cliques);
    foreach(EntryClique* ec, cliques) {
        ec->setEntryChecked(ec->entryList().first(), true);
    }

    qDebug() << "duplicates" << cliques.size() << "of entries" << m_systemFiles->size();

    MergeDuplicates md(0);
    md.mergeDuplicateEntries(cliques, m_systemFiles);

    qDebug() << "entries after merge" << m_systemFiles->size();

    emit syncInProgress(false);
}
