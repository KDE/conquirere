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

#ifndef NBIBEXPORTERBIBTEX_H
#define NBIBEXPORTERBIBTEX_H

#include "nbibexporter.h"

/**
  * @brief Exports a list of publication or reference resources to a bibtex file
  *
  */
class NBibExporterBibTex : public NBibExporter
{
    Q_OBJECT
public:
    explicit NBibExporterBibTex();
    virtual ~NBibExporterBibTex();

    /**
      * Exports a list of publication or reference resources to a bibtex file
      *
      * calls the FileExporterBibTeX from KBibTeX to do its task after all resoureces are piped to a KBibTex File
      *
      * @see NepomukToBibTexPipe
      * @see FileExporterBibTeX
      */
    bool save(QIODevice *iodevice, const QList<Nepomuk::Resource> referenceList, QStringList *errorLog = NULL);
};

#endif // NBIBEXPORTERBIBTEX_H
