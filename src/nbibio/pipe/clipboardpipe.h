/*
 * Copyright 2012 Jörg Ehrichs <joerg.ehrichs@gmx.de>
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

#ifndef CLIPBOARDPIPE_H
#define CLIPBOARDPIPE_H

#include <QtCore/QObject>

#include "config/conquirere.h"

#include <Nepomuk2/Resource>

/**
  * @brief Pipes the content of a Publication QVariantList to the clipboard.
  *
  * Depending on the PipeExport either the citekeys, the raw bibtex source or a pre-formated
  * reference output is used.
  */
class ClipboardPipe : public QObject
{
    Q_OBJECT
public:
    /**
      * @brief Defines what should be exported to the clipboard
      */
    enum PipeExport {
        Export_CITEKEY,
        Export_SOURCE,
        Export_REFERENCE
    };

    explicit ClipboardPipe(QObject *parent = 0);

    /**
      * @brief Do the actual export
      *
      * @p bibEntries List of all publication elements
      *
      * @todo TODO: implement Export_REFERENCE: create reference text from nepomuk reference. Make the style configurable
      */
    void pipeExport(const QVariantList & bibEntries);
    void pipeExport(QList<Nepomuk2::Resource> resources);

    /**
      * @brief Defines what should be exported to the clipboard
      *
      * @p exportType the format to use for the piping
      */
    void setExportType( PipeExport exportType);

    /**
     * @brief Sets the command added around the citekey for the export
     * @param citeCommand the sued citeCommand usually ConqSettings::referenceCommand()
     */
    void setCiteCommand( ConqSettings::EnumReferenceCommand::type citeCommand );
    void setCiteCommand( int citeCommand );

private:
    PipeExport m_exportType;
    ConqSettings::EnumReferenceCommand::type m_citeCommand;
};

#endif // CLIPBOARDPIPE_H
