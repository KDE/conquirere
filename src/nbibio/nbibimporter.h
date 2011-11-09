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

#ifndef NBIBIMPORTER_H
#define NBIBIMPORTER_H

#include <QtCore/QObject>
#include <QtCore/QStringList>

class QIODevice;
/**
  * @brief Abstract base class for any kind of file importer
  *
  * If possible subclasses should make use of the BibTexToNepomukPipe and the actual importer implemented in the KBibTeX
  * svn repo and reused here.
  *
  * Only if the way via KBibTeX is not possible a completly own way should be used.
  */
class NBibImporter : public QObject
{
    Q_OBJECT
public:
    explicit NBibImporter();
    virtual ~NBibImporter();

    /**
      * imports the file from @p fileName
      *
      * calls load() internally
      * @p filename the path and name of the file the importer reads from
      * @p errorLog pointer to the error list
      */
    bool fromFile(QString fileName, QStringList *errorLog = NULL);

    /**
      * Imports a file into the Nepomuk storage using the NBIB ontology
      *
      * subclasses must define how their fileformat fits into the nepomuk ontology
      *
      * @p iodevice the iodevice the importer writes from
      * @p errorLog pointer to the error list
      */
    virtual bool load(QIODevice *iodevice, QStringList *errorLog = NULL) = 0;

signals:
    /**
      * Emits the current progress rate from 0-100
      */
    void progress(int current);

public slots:
    /**
      * cancels the import process.
      *
      * subclasses must check the m_cancel value to see if they have to cancel the process
      */
    virtual void cancel();

protected:
    bool m_cancel;
};

#endif // NBIBIMPORTER_H
