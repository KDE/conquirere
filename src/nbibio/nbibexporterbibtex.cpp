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

#include "nbibexporterbibtex.h"

#include "pipe/nepomuktobibtexpipe.h"

#include <kbibtex/file.h>
#include <kbibtex/fileexporterbibtex.h>

NBibExporterBibTex::NBibExporterBibTex()
    : NBibExporter()
{
}

NBibExporterBibTex::~NBibExporterBibTex()
{

}

bool NBibExporterBibTex::save(QIODevice *iodevice, const QList<Nepomuk::Resource> referenceList, QStringList *errorLog)
{
    if(referenceList.isEmpty()) {
        QString error = i18n("No resources specified for the export to bibtex.");
        errorLog->append(error);
        return false;
    }

    NepomukToBibTexPipe ntbp;
    ntbp.pipeExport(referenceList);
    File f = ntbp.bibtexFile();

    emit progress( 50 );

    if(m_cancel) {
        QString error = i18n("The user canceled the export.");
        errorLog->append(error);
        return false;
    }

    FileExporterBibTeX feb;
    feb.save(iodevice, &f, errorLog);

    emit progress( 100 );

    return true;
}
