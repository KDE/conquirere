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

#include <KIO/NetAccess>
#include <KDE/KTemporaryFile>
#include <KDE/KStandardDirs>
#include <KDE/KDebug>

#include <QtCore/QFile>
#include <QtCore/QFileInfo>

WriteToFile::WriteToFile(QObject *parent)
    : WriteToStorage(parent)
    , m_emptyFile(0)
{
}

WriteToFile::~WriteToFile()
{

}

File *WriteToFile::getFile()
{
    return m_emptyFile;
}

void WriteToFile::pushItems(File *items, const QString &collection)
{
    Q_UNUSED(collection);

    exportFile(items);
}

void WriteToFile::pushNewItems(File *items, const QString &collection)
{
    Q_UNUSED(collection);

    exportFile(items);
}

void WriteToFile::updateItem(QSharedPointer<Element> item)
{
    Q_UNUSED(item);

    // should read in current file and replace the entry with the citekey from "item"
    // and save it again
    // for now ignore and use pushItems instead to update everything
}

void WriteToFile::exportFile(File *items)
{
    emit progress(0);

    // first write output to temporary file
    KTemporaryFile tmpFile;
    if (!tmpFile.open()) {
        kDebug() << "can't open file " << m_psd.url;
        return;
    }

    // select the right importer based on the file extension
    QFileInfo fi(m_psd.url);
    QString extension = fi.completeSuffix();

    QStringList errorLog;

    if(extension == QLatin1String("bib")) {
        FileExporterBibTeX feb;
        feb.save(&tmpFile, items, &errorLog);
    }
    else if(extension == QLatin1String("pdf")) {
        FileExporterPDF fepdf;
        fepdf.save(&tmpFile, items, &errorLog);
    }
    else if(extension == QLatin1String("html")) {
        FileExporterBibTeX2HTML feb2html;
        feb2html.save(&tmpFile, items, &errorLog);
    }
    else if(extension == QLatin1String("blg")) {
        FileExporterBLG feblg;
        feblg.save(&tmpFile, items, &errorLog);
    }
    else if(extension == QLatin1String("ps")) {
        FileExporterPS feps;
        feps.save(&tmpFile, items, &errorLog);
    }
    else if(extension == QLatin1String("ris")) {
        FileExporterRIS feris;
        feris.save(&tmpFile, items, &errorLog);
    }
    else if(extension == QLatin1String("rtf")) {
        FileExporterRTF fertf;
        fertf.save(&tmpFile, items, &errorLog);
    }
    else if(extension == QLatin1String("xml")) {
        FileExporterXML fexml;
        fexml.save(&tmpFile, items, &errorLog);
    }
    else if(extension == QLatin1String("xslt")) {
        FileExporterXSLT fexslt;
        fexslt.save(&tmpFile, items, &errorLog);
    }
    else {
        qWarning() << "WriteToFile::exportFile # unknown file extension " << extension;
    }

    // get tmp file name
    QString tmpFileUrl = KGlobal::dirs()->findResource("tmp",tmpFile.fileName());

    // upload to wherever we need it
    if( !KIO::NetAccess::upload(tmpFileUrl, m_psd.url, 0) ) {
        kDebug() << "upload" << tmpFileUrl << "to" << m_psd.url << "failed";
    }

    emit progress(100);

    emit finished();
}

void WriteToFile::addItemsToCollection(const QList<QString> &ids, const QString &collection )
{
    Q_UNUSED(ids);
    Q_UNUSED(collection);
    // collections in files are not supported
}

void WriteToFile::removeItemsFromCollection(const QList<QString> &ids, const QString &collection )
{
    Q_UNUSED(ids);
    Q_UNUSED(collection);
    // collections in files are not supported
}

void WriteToFile::deleteItems(File *items)
{

}

void WriteToFile::deleteItems(QList<QPair<QString, QString> > items)
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
