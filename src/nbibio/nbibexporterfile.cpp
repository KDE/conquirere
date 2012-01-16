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

#include "nbibexporterfile.h"

#include "pipe/nepomuktobibtexpipe.h"

#include <kbibtex/file.h>

#include <kbibtex/fileexporterbibtex2html.h>
#include <kbibtex/fileexporterblg.h>
#include <kbibtex/fileexporterpdf.h>
#include <kbibtex/fileexporterps.h>
#include <kbibtex/fileexporterris.h>
#include <kbibtex/fileexporterrtf.h>
#include <kbibtex/fileexporterxml.h>
#include <kbibtex/fileexporterxslt.h>
#include <kbibtex/fileexporterbibtex.h>

#include "nbibio/conquirere.h"

NBibExporterFile::NBibExporterFile()
    : m_fileType(EXPORT_BIBTEX)
{
}

void NBibExporterFile::setFileType(FileType type)
{
    m_fileType = type;
}

bool NBibExporterFile::save(QIODevice *iodevice, const QList<Nepomuk::Resource> referenceList, QStringList *errorLog)
{
    if(referenceList.isEmpty()) {
        QString error = i18n("No resources specified for the export to bibtex.");
        errorLog->append(error);
        return false;
    }

    NepomukToBibTexPipe ntbp;
    ntbp.pipeExport(referenceList);
    File f;
    f.append(*ntbp.bibtexFile());

    emit progress( 50 );

    if(m_cancel) {
        QString error = i18n("The user canceled the export.");
        errorLog->append(error);
        return false;
    }

    switch(m_fileType) {
    case EXPORT_BIBTEX:
    {
        FileExporterBibTeX feb;
        /*
        feb.setEncoding(ConqSettings::encoding())
        feb.keyEncoding = ConqSettings::encoding();
        feb.keyStringDelimiter = ConqSettings::stringDelimiters();
        feb.keyQuoteComment = (FileExporterBibTeX::QuoteComment)ConqSettings::commentQuoting();

        feb.keyKeywordCasing = (KBibTeX::Casing)ConqSettings::keywordCasing();
        feb.keyProtectCasing;
*/
        feb.save(iodevice, &f, errorLog);
        break;
    }
    case EXPORT_PDF:
    {
        FileExporterPDF fepdf;
        fepdf.save(iodevice, &f, errorLog);
        break;
    }
    case EXPORT_HTML:
    {
        FileExporterBibTeX2HTML feb2html;
        feb2html.save(iodevice, &f, errorLog);
        break;
    }
    case EXPORT_BLG:
    {
        FileExporterBLG feblg;
        feblg.save(iodevice, &f, errorLog);
        break;
    }
    case EXPORT_PS:
    {
        FileExporterPS feps;
        feps.save(iodevice, &f, errorLog);
        break;
    }
    case EXPORT_RIS:
    {
        FileExporterRIS feris;
        feris.save(iodevice, &f, errorLog);
        break;
    }
    case EXPORT_RTF:
    {
        FileExporterRTF fertf;
        fertf.save(iodevice, &f, errorLog);
        break;
    }
    case EXPORT_XML:
    {
        FileExporterXML fexml;
        fexml.save(iodevice, &f, errorLog);
        break;
    }
    case EXPORT_XSLT:
    {
        FileExporterXSLT fexslt;
        fexslt.save(iodevice, &f, errorLog);
        break;
    }
    }

    emit progress( 100 );
    return true;
}
