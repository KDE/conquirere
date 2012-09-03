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

#ifndef BIBFILEIMPORTER_H
#define BIBFILEIMPORTER_H

#include <QtCore/QObject>
#include <QtCore/QStringList>

class QIODevice;
/**
  * @brief Abstract base class for any kind of file to Nepomuk importer
  *
  * The FileImporterBibTex handles most filetypes via @c KBibTeX.
  */
class BibFileImporter : public QObject
{
    Q_OBJECT
public:
    explicit BibFileImporter();
    virtual ~BibFileImporter();

    /**
      * @brief imports the file from @p fileName
      *
      * calls load() internally
      * @p filename the path and name of the file the importer reads from
      * @p errorLog pointer to the error list
      */
    bool fromFile(QString fileName, QStringList *errorLog = NULL);

    /**
      * @brief Imports a file into the Nepomuk storage using the @c NBIB @c ontology
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
      * @brief cancels the import process.
      *
      * subclasses must check the m_cancel value to see if they have to cancel the process
      */
    virtual void cancel();

protected:
    bool m_cancel;
};

#endif // BIBFILEIMPORTER_H
