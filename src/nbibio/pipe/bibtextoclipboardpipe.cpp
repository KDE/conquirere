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

#include "bibtextoclipboardpipe.h"

#include <kbibtex/entry.h>
#include <kbibtex/element.h>
#include <kbibtex/fileexporterbibtex.h>

#include <QtGui/QClipboard>
#include <QtGui/QApplication>
#include <QtCore/QBuffer>
#include <QtCore/QTextStream>

#include <QtCore/QDebug>

BibTexToClipboardPipe::BibTexToClipboardPipe()
    : m_exportType(Export_SOURCE)
{
}

BibTexToClipboardPipe::~BibTexToClipboardPipe()
{

}

void BibTexToClipboardPipe::pipeExport(File & bibEntries)
{
    QString text;
    switch(m_exportType) {
    case Export_CITEKEY: {
        foreach(const Element *e, bibEntries) {
            const Entry *bibEntry = static_cast<const Entry *>(e);
            if(bibEntry) {
                text.append(bibEntry->id());
                text.append(QLatin1String(", "));
            }
        }
        text.chop(2);

        break;
    }
    case Export_SOURCE: {
        FileExporterBibTeX exporter;
        exporter.setEncoding(QLatin1String("latex"));
        QBuffer buffer;
        buffer.open(QBuffer::WriteOnly);
        exporter.save(&buffer, &bibEntries);
        buffer.close();

        buffer.open(QBuffer::ReadOnly);
        QTextStream ts(&buffer);
        text = ts.readAll();
        buffer.close();
        break;
    }
    case Export_REFERENCE: {
        qDebug() << "TODO :: Export_REFERENCE not supported currently";
    }
    }

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(text);
}

void BibTexToClipboardPipe::setExportType( PipeExport exportType)
{
    m_exportType = exportType;
}
