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

#include <QObject>

#include <kbibtex/file.h>

/**
  * @brief Interface for any pipe action that transforms a KBibTeX File into something else
  */
class BibTexPipe : public QObject
{
    Q_OBJECT

public:
    BibTexPipe();
    virtual ~BibTexPipe();

    /**
      * Sets the pointer for the error log
      *
      * used to find out what errors happend during the export
      */
    void setErrorLog(QStringList *errorLog = NULL);

    /**
      * Does the piping action
      *
      * @p bibEntries is a list of all Entry elements which form a bibtex entry
      */
    virtual void pipeExport(File & bibEntries) = 0;

signals:
    void progress(int value);

protected:
    QStringList *m_errorLog;
};
#endif // BIBTEXPIPE_H