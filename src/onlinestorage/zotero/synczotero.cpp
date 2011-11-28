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

#include <kbibtex/file.h>
#include <kbibtex/entry.h>
#include <kbibtex/findduplicates.h>

#include <QDebug>

// debug
#include <QFile>
#include <kbibtex/fileexporterbibtex.h>

SyncZotero::SyncZotero(QObject *parent)
    : QObject(parent)
{
}

SyncZotero::~SyncZotero()
{
    delete m_rfz;
}

void SyncZotero::setUserName(const QString & name)
{
    m_name = name;
}

QString SyncZotero::userName() const
{
    return m_name;
}

void SyncZotero::setPassword(const QString & pwd)
{
    m_password = pwd;
}

QString SyncZotero::pasword() const
{
    return m_password;
}

void SyncZotero::syncWithStorage(File bibfile)
{
    m_systemFiles = bibfile;

    //lets start by retrieving all items from the server and merge them with the current files
    m_rfz = new ReadFromZotero;
    m_rfz->setUserName(m_name);
    m_rfz->setPassword(m_password);

    connect(m_rfz, SIGNAL(itemsInfo(File)), this, SLOT(readSync(File)));
    m_rfz->fetchItems();
}

void SyncZotero::readSync(File serverFiles)
{
    qDebug() << "itemsFromServer" << m_systemFiles.size() << " + new " << serverFiles.size();

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
        foreach(Element* checkElement, serverFiles) {
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
                else {
                    // #####################################################################################
                    // # TODO create QList<EntryClique*> from the two entries
                    // #####################################################################################
                    // this replace the find duplicates stuff so we don't start guessing
                    // which entries are duplicates and simple show the different entries

                    // also define the way of merging (automatic merge local/serveruser decision)
                    // #####################################################################################
                }

                break; // stop checking here we found an entry with the right zoterokey
            }
        }

        if(addEntry) {
            m_systemFiles.append(element);
        }
    }

    // now we have list of existing entries together with all server entries that changed
    // merge them

/*
    int sensitivity = 4000; // taken from KBibTeX
    FindDuplicates fd(0, sensitivity);
    QList<EntryClique*> cliques;
    fd.findDuplicateEntries(&m_systemFiles, cliques);

    qDebug() << "duplicates" << cliques.size() << "of entries" << m_systemFiles.size();

    MergeDuplicates md(0);
    md.mergeDuplicateEntries(cliques, &m_systemFiles);

    qDebug() << "entries after merge" << m_systemFiles.size();
*/

    // from this point on, we have all data from the server merged with the local items
    // now its time to send all data to the server, update changed items and create new ones
    m_wtz = new WriteToZotero;
    m_wtz->setUserName(m_name);
    m_wtz->setPassword(m_password);
    m_wtz->pushItems(m_systemFiles);

    connect(m_wtz, SIGNAL(itemsInfo(File)), this, SLOT(writeSync(File)));
}

void SyncZotero::writeSync(File serverFiles)
{
    // this function is called after all items are send to the server
    // as zotero sends updated infos with the new zoterokey and/or an updated zotero etag



    qDebug() << "sync complete debug write it to disk";
    // debug write all to disk

    QFile exportFile(QString("/home/joerg/zotero_export.bib"));
    if (!exportFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    FileExporterBibTeX feb;
    feb.save(&exportFile, &serverFiles);
}
