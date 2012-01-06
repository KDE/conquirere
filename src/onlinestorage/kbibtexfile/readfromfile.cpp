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

#include "readfromfile.h"

#include <kbibtex/entry.h>
#include <kbibtex/fileimporter.h>
#include <kbibtex/fileimporterbibtex.h>
#include <kbibtex/fileimporterpdf.h>
#include <kbibtex/fileimporterris.h>

#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QDebug>

ReadFromFile::ReadFromFile(QObject *parent)
    : ReadFromStorage(parent)
{
}

void ReadFromFile::fetchItems(const QString &collection)
{
    Q_UNUSED(collection);

    // open file from url
    // TODO use kio to allow network transparency?

    QFile bibFile(m_psd.url);
    if (!bibFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "can't open file" << m_psd.url;
        return;
    }

    FileImporter *importer = 0;

    // select the right importer based on the file extension
    QFileInfo fi(m_psd.url);
    QString extension = fi.completeSuffix();

    if(extension == QLatin1String(".bib")) {
        importer = new FileImporterBibTeX;
    }
    else if(extension == QLatin1String(".pdf")) {
        importer = new FileImporterPDF;
    }
    else if(extension == QLatin1String(".ris")) {
        importer = new FileImporterRIS;
    }
    else if(extension == QLatin1String(".pdf")) {

    }
    else {
        qWarning() << "unknown file extension" << extension << "can't read file";
        return;
    }

    connect(importer, SIGNAL(progress(int,int)), this, SLOT(calculateProgress(int,int)));

    File *importedBibFile = importer->load(&bibFile);

    emit itemsInfo(*importedBibFile);
}

void ReadFromFile::fetchItem(const QString &id, const QString &collection )
{
    Q_UNUSED(collection);

    // not implemented right now
    // should do fetchItems() and filter the id as citekey and emit only this one
}

void ReadFromFile::fetchCollections(const QString &parent )
{
    Q_UNUSED(parent);
    //collections in files are not supported
}

void ReadFromFile::fetchCollection(const QString &collection )
{
    Q_UNUSED(collection);
    //collections in files are not supported
}

void ReadFromFile::requestFinished()
{
    // not used in this case
}
