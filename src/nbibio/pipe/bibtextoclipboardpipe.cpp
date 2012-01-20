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

#include "nbibio/conquirere.h"

#include <kbibtex/entry.h>
#include <kbibtex/element.h>
#include <kbibtex/fileexporterbibtex.h>

#include <KDE/KDebug>

#include <QtGui/QClipboard>
#include <QtGui/QApplication>
#include <QtCore/QBuffer>
#include <QtCore/QTextStream>

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
        foreach(const QSharedPointer<Element> &e, bibEntries) {
            const Entry *bibEntry = dynamic_cast<const Entry *>(e.data());
            if(bibEntry) {
                switch(ConqSettings::referenceCommand()) {
                case ConqSettings::EnumReferenceCommand::none:
                    text.append(bibEntry->id() + QLatin1String(", "));
                    break;
                case ConqSettings::EnumReferenceCommand::cite:
                    text.append(QLatin1String("\\cite{") + bibEntry->id() + QLatin1String("} "));
                    break;
                case ConqSettings::EnumReferenceCommand::citealt:
                    text.append(QLatin1String("\\citealt{") + bibEntry->id() + QLatin1String("} "));
                    break;
                case ConqSettings::EnumReferenceCommand::citeauthor:
                    text.append(QLatin1String("\\citeauthor{") + bibEntry->id() + QLatin1String("} "));
                    break;
                case ConqSettings::EnumReferenceCommand::citeauthor_:
                    text.append(QLatin1String("\\citeauthor*{") + bibEntry->id() + QLatin1String("} "));
                    break;
                case ConqSettings::EnumReferenceCommand::citeyear:
                    text.append(QLatin1String("\\citeyear{") + bibEntry->id() + QLatin1String("} "));
                    break;
                case ConqSettings::EnumReferenceCommand::citeyearpar:
                    text.append(QLatin1String("\\citeyearpar{") + bibEntry->id() + QLatin1String("} "));
                    break;
                case ConqSettings::EnumReferenceCommand::shortcite:
                    text.append(QLatin1String("\\shortcite{") + bibEntry->id() + QLatin1String("} "));
                    break;
                case ConqSettings::EnumReferenceCommand::citet:
                    text.append(QLatin1String("\\citet{") + bibEntry->id() + QLatin1String("} "));
                    break;
                case ConqSettings::EnumReferenceCommand::citet_:
                    text.append(QLatin1String("\\citet*{") + bibEntry->id() + QLatin1String("} "));
                    break;
                case ConqSettings::EnumReferenceCommand::citep:
                    text.append(QLatin1String("\\citep{") + bibEntry->id() + QLatin1String("} "));
                    break;
                case ConqSettings::EnumReferenceCommand::citep_:
                    text.append(QLatin1String("\\citep*{") + bibEntry->id() + QLatin1String("} "));
                    break;
                }
            }
        }
        if(ConqSettings::referenceCommand() == ConqSettings::EnumReferenceCommand::none) {
            text.chop(2); // remove last ", "
        }
        else {
            text.chop(1); // remove last " "
        }

        break;
    }
    case Export_SOURCE: {
        FileExporterBibTeX exporter;
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
        kDebug() << "BibTexToClipboardPipe::pipeExport :: Export_REFERENCE not supported currently";
    }
    }

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(text);
}

void BibTexToClipboardPipe::setExportType( PipeExport exportType)
{
    m_exportType = exportType;
}
