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

#ifndef BIBTEXEXPORTER_H
#define BIBTEXEXPORTER_H

#include "../bibfileexporter.h"

/**
  * @brief Exports a list of @c nbib:Publication or @c nbib:Rreference resources to a file
  *
  * Makes use of the NepomukToBibTexPipe to transform all Nepomuk data into @c KBibTeX format and
  * uses @c KBibTeX::FileExporter* to export it to a file.
  *
  * The files supported are listed in the @p FileType enum
  *
  */
class BibTexExporter : public BibFileExporter
{
public:
    /**
     * @brief Enum for all supported filetypes from KBibTeX
     */
    enum FileType {
        EXPORT_BIBTEX,
        EXPORT_BLG,
        EXPORT_HTML,
        EXPORT_PDF,
        EXPORT_PS,
        EXPORT_RIS,
        EXPORT_RTF,
        EXPORT_XML,
        EXPORT_XSLT
    };

    explicit BibTexExporter();

    void setFileType(FileType type);

    /**
      * Exports a list of @c nbib:Publication or @c nbib:Rreference resources to a file
      *
      * calls the FileExporter* from KBibTeX to do its task after all resources are piped to a KBibTex File
      *
      * @see NepomukToBibTexPipe
      * @see FileExporterBibTeX
      * @see FileExporterPDF
      * @see FileExporterRIS
      * @see FileExporterBibTeX2HTML
      * @see FileExporterPS
      *
      * @p iodevice the device the exporter writes into
      * @p referenceList list of all Nepomuk2::Resources used for the export
      * @p errorLog pointer to the error list
      *
      * @pre referenceList must be a list of NBIB::Publication or NBIB::Resource
      */
    bool save(QIODevice *iodevice, const QList<Nepomuk2::Resource> referenceList, QStringList *errorLog = NULL);

private:
    FileType m_fileType;
};

#endif // BIBTEXEXPORTER_H
