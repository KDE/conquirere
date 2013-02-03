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

#include "bibteximporter.h"

#include "config/bibglobals.h"

#include "bibtex/bibtexvariant.h"
#include "pipe/varianttonepomukpipe.h"

#include <kbibtex/fileimporterbibtex.h>
#include <kbibtex/fileimporterpdf.h>
#include <kbibtex/fileimporterris.h>
#include <kbibtex/findduplicates.h>

#include <QtCore/QFile>

using namespace Akonadi;

BibTexImporter::BibTexImporter()
    : BibFileImporter()
    , m_importedEntries(0)
    , m_findDuplicates(false)
    , m_selectedFileType(EXPORT_BIBTEX)
{
}

BibTexImporter::~BibTexImporter()
{
    delete m_importedEntries;
}

void BibTexImporter::setAkonadiAddressbook(Akonadi::Collection & addressbook)
{
    m_addressbook = addressbook;
}

void BibTexImporter::setFindDuplicates(bool findThem)
{
    m_findDuplicates = findThem;
}

void BibTexImporter::setFileType(BibTexImporter::FileType selectedFileType)
{
    m_selectedFileType = selectedFileType;
}

bool BibTexImporter::load(QIODevice *iodevice, QStringList *errorLog)
{
    Q_UNUSED(errorLog);

    FileImporter *importer = 0;
    switch(m_selectedFileType) {
    case EXPORT_BIBTEX:
    {
        FileImporterBibTeX *bibImporter = new FileImporterBibTeX;
        bibImporter->setKeysForPersonDetection(BibGlobals::keysForPersonDetection());
        importer = bibImporter;
        break;
    }
//    case EXPORT_COPAC:
//    {
//        //FileImporterBibUtils *importerBibUtils = new FileImporterBibTeX;
//        break;
//    }
//    case EXPORT_ENDNOTE:
//    {
//        break;
//    }
//    case EXPORT_ISI:
//    {
//        break;
//    }
//    case EXPORT_MEDLINE:
//    {
//        break;
//    }
//    case EXPORT_MODS:
//    {
//        break;
//    }
    case EXPORT_PDF:
    {
        importer = new FileImporterPDF;
        break;
    }
    case EXPORT_RIS:
    {
        importer = new FileImporterRIS;
        break;
    }

    }

    connect(importer, SIGNAL(progress(int,int)), this, SLOT(calculateProgress(int,int)));

    m_importedEntries = importer->load(iodevice);
    pipeToNepomuk();

    delete importer;

    return true;
}

bool BibTexImporter::readBibFile(const QString & filename, QStringList *errorLog)
{
    QFile bibFile(filename);
    if (!bibFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString error = i18n("can't open file %1", filename);
        errorLog->append(error);
        return false;
    }

    FileImporterBibTeX *importer = new FileImporterBibTeX;
    importer->setKeysForPersonDetection(BibGlobals::keysForPersonDetection());
    connect(importer, SIGNAL(progress(int,int)), this, SLOT(calculateImportProgress(int,int)));

    m_importedEntries = importer->load(&bibFile);
    m_importFile = KUrl::fromLocalFile(filename);

    delete importer;

    if(m_findDuplicates) {
        findDuplicates();
    }

    return true;
}

File *BibTexImporter::bibFile()
{
    return m_importedEntries;
}

bool BibTexImporter::findDuplicates()
{
    m_cliques.clear();
    int sensitivity = 4000; // taken from KBibTeX

    FindDuplicates fd(0, sensitivity);
    bool gotCanceled = fd.findDuplicateEntries(m_importedEntries, m_cliques);

    emit progress(100);

    if(gotCanceled || m_cliques.isEmpty()) {
        return false;
    }

    return true;
}

void BibTexImporter::setProjectPimoThing(Nepomuk2::Resource projectThing)
{
    m_projectThing = projectThing;
}

QList<EntryClique*> BibTexImporter::duplicates()
{
    return m_cliques;
}

bool BibTexImporter::pipeToNepomuk(QStringList *errorLog)
{
    Q_UNUSED(errorLog);

    // pipe bibtex file to variant
    QVariantList list = BibTexVariant::toVariant( *m_importedEntries );

    // push the Variantlist to nepomuk
    VariantToNepomukPipe vtnp;
    connect (&vtnp, SIGNAL(progress(int)), this, SIGNAL(progress(int)));

    vtnp.setProjectPimoThing(m_projectThing);
    vtnp.pipeExport(list);

    return true;
}

void BibTexImporter::calculateImportProgress(int current, int max)
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
