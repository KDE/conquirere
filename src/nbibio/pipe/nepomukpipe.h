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

#ifndef NEPOMUKPIPE_H
#define NEPOMUKPIPE_H

#include <Nepomuk/Resource>
#include <QList>

/**
  * Interface for any pipe action that transforms a list of Nepomuk resources into something else
  *
  * Only used for NBIB:;Publication and NBIB::Reference
  */
class NepomukPipe
{
public:
    NepomukPipe() {}

    /**
      * Does the piping action
      *
      * @p resources list of publication or reference resources
      */
    virtual void pipeExport(QList<Nepomuk::Resource> resources) = 0;
};
#endif // NEPOMUKPIPE_H
