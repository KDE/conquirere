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

#ifndef BIBTEXTOCLIPBOARDPIPE_H
#define BIBTEXTOCLIPBOARDPIPE_H

#include "bibtexpipe.h"

/**
  * @brief Pipes the content of a KBibTex File to the clipboard.
  *
  * Depending on the PipeExport eiter the citekeys, the raw bibtex source or a pre-formated
  * reference output is used.
  */
class BibTexToClipboardPipe : public BibTexPipe
{
public:
    /**
      * Defines what should be exported to the clipboard
      */
    enum PipeExport {
        Export_CITEKEY,
        Export_SOURCE,
        Export_REFERENCE
    };

    BibTexToClipboardPipe();
    ~BibTexToClipboardPipe();

    /**
      * Do the actual export
      *
      * @p bibEntries File is a list of all Entry elements which form a bibtex entry
      *
      * @todo implement Export_REFERENCE
      */
    void pipeExport(File & bibEntries);

    /**
      * Defines what should be exported to the clipboard
      *
      * @p exportType the format to use for the piping
      */
    void setExportType( PipeExport exportType);

private:
    PipeExport m_exportType;
};

#endif // BIBTEXTOCLIPBOARDPIPE_H
