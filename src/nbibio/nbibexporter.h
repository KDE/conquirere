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

#ifndef NBIBEXPORTER_H
#define NBIBEXPORTER_H

#include <Nepomuk2/Resource>

#include <QtCore/QObject>

class QIODevice;

/**
  * @brief Abstract base class for any kind of file exporter
  *
  * If possible subclasses should make use of the NepomukToBibTexPipe and the actual exporter implemented in the KBibTeX
  * svn repo and reused here.
  *
  * Only if the way via KBibTeX is not possible a completely own way should be used.
  */
class NBibExporter : public QObject
{
    Q_OBJECT
public:
    explicit NBibExporter();
    virtual ~NBibExporter();

    /**
      * Exports a list of publication or reference resources to @p filename
      *
      * calls save() internally
      *
      * @p filename the path and name of the file the exporter writes into
      * @p referenceList list of all Nepomuk2::Resources used for the export
      * @p errorLog pointer to the error list
      *
      * @pre referenceList must be a list of NBIB::Publication or NBIB::Resource
      */
    bool toFile( const QString &filename, const QList<Nepomuk2::Resource> referenceList, QStringList *errorLog = NULL);

    /**
      * Exports a list of publication or reference resources
      *
      * Must be implemented in any subclass
      *
      * @p iodevice the iodevice the exporter writes into
      * @p referenceList list of all Nepomuk2::Resources used for the export
      * @p errorLog pointer to the error list
      *
      * @pre referenceList must be a list of NBIB::Publication or NBIB::Resource
      */
    virtual bool save(QIODevice *iodevice, const QList<Nepomuk2::Resource> referenceList, QStringList *errorLog = NULL) = 0;

signals:
    /**
      * Emits the current progress rate from 0-100
      */
    void progress(int current);

public slots:
    /**
      * cancels the export process.
      *
      * subclasses must check the m_cancel value to see if they have to cancel the process
      */
    virtual void cancel();

protected:
    bool m_cancel;
};

#endif // NBIBEXPORTER_H
