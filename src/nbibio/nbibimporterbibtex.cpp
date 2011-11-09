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

#include <QtCore/QFile>

using namespace Akonadi;

NBibImporterBibTex::NBibImporterBibTex()
    : NBibImporter()
    , m_importedEntries(0)
    , m_findDuplicates(false)
{
}

NBibImporterBibTex::~NBibImporterBibTex()
{
    while (!m_importedEntries->isEmpty()) {
        Element *ec = m_importedEntries->first();
        m_importedEntries->removeFirst();
        delete ec;
    }

    delete m_importedEntries;

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

bool NBibImporterBibTex::readBibFile(const QString & filename, QStringList *errorLog)
{
    QFile bibFile(filename);
    if (!bibFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString error = i18n("can't open file %1", filename);
        errorLog->append(error);
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
    int sensitivity = 4000; // taken from KBibTeX

    FindDuplicates fd(0, sensitivity);
    bool gotCanceled = fd.findDuplicateEntries(m_importedEntries, m_cliques);

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

bool NBibImporterBibTex::pipeToNepomuk(QStringList *errorLog)
{
    BibTexToNepomukPipe * importer = new BibTexToNepomukPipe;

    connect (importer, SIGNAL(progress(int)), this, SIGNAL(progress(int)));

    importer->setAkonadiAddressbook(m_addressbook);
    importer->setErrorLog(errorLog);
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
