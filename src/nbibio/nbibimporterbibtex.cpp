/*
 * Copyright 2011 Jörg Ehrichs <joerg.ehichs@gmx.de>
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

#include "nbibimporterbibtex.h"

#include "pipe/bibtextonepomukpipe.h"
#include <kbibtex/fileimporterbibtex.h>
#include <kbibtex/findduplicates.h>

#include <Akonadi/Item>
#include <KABC/Addressee>
#include <Akonadi/ItemFetchJob>
#include <Akonadi/ItemCreateJob>
#include <Akonadi/CollectionCreateJob>
#include <Akonadi/CollectionFetchJob>
#include <Akonadi/CollectionFetchScope>

#include <QtCore/QFile>

#include <QtCore/QDebug>

using namespace Akonadi;

NBibImporterBibTex::NBibImporterBibTex()
    : NBibImporter()
{
}

NBibImporterBibTex::~NBibImporterBibTex()
{
    while (!m_importedEntries->isEmpty()) {
        Element *ec = m_importedEntries->first();
        m_importedEntries->removeFirst();
        delete ec;
    }

    while (!m_cliques.isEmpty()) {
        EntryClique *ec = m_cliques.first();
        m_cliques.removeFirst();
        delete ec;
    }
}

void NBibImporterBibTex::setAkonadiAddressbook(Akonadi::Collection & addressbook)
{
    m_addressbook = addressbook;
}

void NBibImporterBibTex::setImportContactToAkonadi(bool import)
{
    m_contactToAkonadi = import;
}

void NBibImporterBibTex::setFindDuplicates(bool findThem)
{
    m_findDuplicates = findThem;
}

bool NBibImporterBibTex::load(QIODevice *iodevice, QStringList *errorLog)
{
    FileImporterBibTeX *importer = new FileImporterBibTeX;
    connect(importer, SIGNAL(progress(int,int)), this, SLOT(calculateProgress(int,int)));

    m_importedEntries = importer->load(iodevice);

    pipeToNepomuk();

    delete importer;

    return true;
}

bool NBibImporterBibTex::readBibFile(const QString & filename)
{
    QFile bibFile(filename);
    if (!bibFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "can't open file " << filename;
        return false;
    }

    FileImporterBibTeX *importer = new FileImporterBibTeX;
    connect(importer, SIGNAL(progress(int,int)), this, SLOT(calculateImportProgress(int,int)));

    m_importedEntries = importer->load(&bibFile);

    delete importer;

    if(m_findDuplicates) {
        findDuplicates();
    }

    return true;
}

File *NBibImporterBibTex::bibFile()
{
    return m_importedEntries;
}

bool NBibImporterBibTex::findDuplicates()
{
    int sensitivity = 4000;

    FindDuplicates fd(0, sensitivity);
    bool gotCanceled =fd.findDuplicateEntries(m_importedEntries, m_cliques);

    emit progress(100);

    if(gotCanceled || m_cliques.isEmpty()) {
        return false;
    }

    return true;
}

QList<EntryClique*> NBibImporterBibTex::duplicates()
{
    return m_cliques;
}

bool NBibImporterBibTex::pipeToNepomuk()
{
    BibTexToNepomukPipe * importer = new BibTexToNepomukPipe;

    connect (importer, SIGNAL(progress(int)), this, SIGNAL(progress(int)));
    importer->pipeExport(*m_importedEntries);

    delete importer;

    return true;
}

void NBibImporterBibTex::calculateImportProgress(int current, int max)
{
    int max2;
    if(false && m_findDuplicates) { // ignore for now
        //maxvalue is only 50% of the progressbar
        max2 = max * 2;
    }
    else {
        max2 = max;
    }

    qreal curValue = (100.0/max2) * current;
    emit progress(curValue);
}

/*
void NBibImporterBibTex::addAuthor(const QString &content, Nepomuk::Resource publication, Nepomuk::Resource reference, const QString & originalEntryType)
{
    QList<NBibImporterBibTex::Name> allNames = parseName(content);

    //in case of @incollection or @inbook the author is used to identify who wrote the chapter not the complete book/collection
    Nepomuk::Resource authorResource;

    if(originalEntryType == QLatin1String("inbook") || originalEntryType == QLatin1String("incollection") ) {
        Nepomuk::Resource chapter = reference.property(Nepomuk::Vocabulary::NBIB::referencedChapter()).toResource();

        if(!chapter.isValid()) {
            chapter = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::Chapter());
            reference.setProperty(Nepomuk::Vocabulary::NBIB::referencedChapter(), chapter);
            publication.addProperty(Nepomuk::Vocabulary::NBIB::chapter(), chapter);
            chapter.setProperty(Nepomuk::Vocabulary::NBIB::chapterOf(), publication);
        }
        authorResource = chapter;
    }
    else {
        authorResource = publication;
    }

    foreach(const NBibImporterBibTex::Name & author, allNames) {
        //check if the publisher already exist in the database
        Nepomuk::Resource a;
        foreach(const Nepomuk::Resource & r, m_allContacts) {
            if(r.property(Nepomuk::Vocabulary::NCO::fullname()).toString() == author.full ||
               r.label() == author.full ) {
                a = r;
                break;
            }
        }

        if(!a.isValid()) {
            qDebug() << "create a new Contact resource for " << author.full;

            //KABC::Addressee addr;
            //addr.setFamilyName( author.last );
            //addr.setGivenName( author.first );
            //addr.setAdditionalName( author.middle );
            //addr.setName( author.full );
            //addr.setFormattedName( author.full );

            //Akonadi::Item item;
            //item.setMimeType( KABC::Addressee::mimeType() );
            //item.setPayload<KABC::Addressee>( addr );

            //Akonadi::ItemCreateJob *job = new Akonadi::ItemCreateJob( item, m_collection );

            //if ( !job->exec() ) {
            //    qDebug() << "Error:" << job->errorString();
            //} else {

                //thats horrible, at the end two different nepomuk resources will be available
                // because the akonadi feeder creates another resource for the contact which is unknown at this point
                // but I need a proper resource to be able to connect it to the publication here
                a = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NCO::PersonContact());
                //a.setProperty("http://akonadi-project.org/ontologies/aneo#akonadiItemId", job->item().id());

                a.setProperty(Nepomuk::Vocabulary::NCO::fullname(), author.full);

                if(!author.first.isEmpty())
                    a.setProperty(Nepomuk::Vocabulary::NCO::nameGiven(), author.first);
                if(!author.last.isEmpty())
                    a.setProperty(Nepomuk::Vocabulary::NCO::nameFamily(), author.last);
                if(!author.middle.isEmpty())
                    a.setProperty(Nepomuk::Vocabulary::NCO::nameAdditional(), author.middle);
            //}

            m_allContacts.append(a);
        }


        authorResource.addProperty(Nepomuk::Vocabulary::NCO::creator(), a);
    }
}
*/
