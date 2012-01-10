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

#include "writetofile.h"

#include <kbibtex/entry.h>
#include <kbibtex/fileexporter.h>
#include <kbibtex/fileexporterbibtex.h>
#include <kbibtex/fileexporterbibtex2html.h>
#include <kbibtex/fileexporterblg.h>
#include <kbibtex/fileexporterpdf.h>
#include <kbibtex/fileexporterps.h>
#include <kbibtex/fileexporterris.h>
#include <kbibtex/fileexporterrtf.h>
#include <kbibtex/fileexporterxml.h>
#include <kbibtex/fileexporterxslt.h>

#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QDebug>

WriteToFile::WriteToFile(QObject *parent)
    : WriteToStorage(parent)
{
}

WriteToFile::~WriteToFile()
{

}

void WriteToFile::pushItems(File items, const QString &collection)
{
    Q_UNUSED(collection);

    exportFile(items);
}

void WriteToFile::pushNewItems(File items, const QString &collection)
{
    Q_UNUSED(collection);

    exportFile(items);
}

void WriteToFile::updateItem(QSharedPointer<Element> item)
{
    qDebug() << "WriteToFile::updateItem npot implemented right now";
    // should read in current file and replace the entry with the citekey from "item"
    // and save it again
}

void WriteToFile::exportFile(File items)
{
    emit progress(0);

    QFile bibFile(m_psd.url);
    if (!bibFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "can't open file " << m_psd.url;
        return;
    }

    // select the right importer based on the file extension
    QFileInfo fi(m_psd.url);
    QString extension = fi.completeSuffix();

    QStringList errorLog;

    if(extension == QLatin1String("bib")) {
        FileExporterBibTeX feb;
        feb.save(&bibFile, &items, &errorLog);
    }
    else if(extension == QLatin1String("pdf")) {
        FileExporterPDF fepdf;
        fepdf.save(&bibFile, &items, &errorLog);
    }
    else if(extension == QLatin1String("html")) {
        FileExporterBibTeX2HTML feb2html;
        feb2html.save(&bibFile, &items, &errorLog);
    }
    else if(extension == QLatin1String("blg")) {
        FileExporterBLG feblg;
        feblg.save(&bibFile, &items, &errorLog);
    }
    else if(extension == QLatin1String("ps")) {
        FileExporterPS feps;
        feps.save(&bibFile, &items, &errorLog);
    }
    else if(extension == QLatin1String("ris")) {
        FileExporterRIS feris;
        feris.save(&bibFile, &items, &errorLog);
    }
    else if(extension == QLatin1String("rtf")) {
        FileExporterRTF fertf;
        fertf.save(&bibFile, &items, &errorLog);
    }
    else if(extension == QLatin1String("xml")) {
        FileExporterXML fexml;
        fexml.save(&bibFile, &items, &errorLog);
    }
    else if(extension == QLatin1String("xslt")) {
        FileExporterXSLT fexslt;
        fexslt.save(&bibFile, &items, &errorLog);
    }
    else {
        qWarning() << "WriteToFile::exportFile # unknown file extension " << extension;
    }

    emit progress(100);

    File emptyFile;

    emit itemsInfo(emptyFile);
}

void WriteToFile::addItemsToCollection(QList<QString> ids, const QString &collection )
{
    Q_UNUSED(ids);
    Q_UNUSED(collection);
    // collections in files are not supported
}

void WriteToFile::removeItemsFromCollection(QList<QString> ids, const QString &collection )
{
    Q_UNUSED(ids);
    Q_UNUSED(collection);
    // collections in files are not supported
}

void WriteToFile::deleteItems(File items)
{

}

void WriteToFile::createCollection(const CollectionInfo &ci)
{
    Q_UNUSED(ci);
    // collections in files are not supported
}

void WriteToFile::editCollection(const CollectionInfo &ci)
{
    Q_UNUSED(ci);
    // collections in files are not supported
}

void WriteToFile::deleteCollection(const CollectionInfo &ci)
{
    Q_UNUSED(ci);
    // collections in files are not supported
}

void WriteToFile::requestFinished()
{
    //not used in this case
}
