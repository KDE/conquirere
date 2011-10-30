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

#ifndef BIBTEXPIPE_H
#define BIBTEXPIPE_H

#include <kbibtex/file.h>

/**
  * Interface for any pipe action that transforms a KBibTeX File into something else
  */
class BibTexPipe
{
public:
    BibTexPipe() { }
    virtual ~BibTexPipe() {}

    /**
      * Does the piping action
      *
      * @p bibEntries File is a list of all Entry elements which form a bibtex entry
      */
    virtual void pipeExport(File & bibEntries) = 0;
};
#endif // BIBTEXPIPE_H
