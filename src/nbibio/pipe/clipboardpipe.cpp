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

#include "clipboardpipe.h"

#include "nbib.h"
#include "nbibio/pipe/nepomuktovariantpipe.h"

#include "nbibio/bibtex/bibtexvariant.h"
#include <kbibtex/file.h>
#include <kbibtex/fileexporterbibtex.h>

#include <Nepomuk2/Variant>

#include <QtGui/QClipboard>
#include <QtGui/QApplication>
#include <QtCore/QBuffer>
#include <QtCore/QTextStream>

ClipboardPipe::ClipboardPipe(QObject *parent)
    : QObject(parent)
    , m_exportType(Export_SOURCE)
    , m_citeCommand(ConqSettings::EnumReferenceCommand::none)
{
}

void ClipboardPipe::pipeExport(const QVariantList & bibEntries)
{
    QString text;
    switch(m_exportType) {

    // export all citekeys with the specified modifier from the settings. The modifier are used for LaTeX formatting
    case Export_CITEKEY: {
        foreach(const QVariant &entry, bibEntries) {
            QVariantMap entryMap = entry.toMap();

            QString citeKey = entryMap.value("bibtexcitekey").toString();
            if(citeKey.isEmpty()) {
                kDebug() << "no citekey found";
                continue;
            }

            switch(m_citeCommand) {
            case ConqSettings::EnumReferenceCommand::none:
                text.append(citeKey + QLatin1String(", "));
                break;
            case ConqSettings::EnumReferenceCommand::cite:
                text.append(QLatin1String("\\cite{") + citeKey + QLatin1String("} "));
                break;
            case ConqSettings::EnumReferenceCommand::citealt:
                text.append(QLatin1String("\\citealt{") + citeKey + QLatin1String("} "));
                break;
            case ConqSettings::EnumReferenceCommand::citeauthor:
                text.append(QLatin1String("\\citeauthor{") + citeKey + QLatin1String("} "));
                break;
            case ConqSettings::EnumReferenceCommand::citeauthor_:
                text.append(QLatin1String("\\citeauthor*{") + citeKey + QLatin1String("} "));
                break;
            case ConqSettings::EnumReferenceCommand::citeyear:
                text.append(QLatin1String("\\citeyear{") + citeKey + QLatin1String("} "));
                break;
            case ConqSettings::EnumReferenceCommand::citeyearpar:
                text.append(QLatin1String("\\citeyearpar{") + citeKey + QLatin1String("} "));
                break;
            case ConqSettings::EnumReferenceCommand::shortcite:
                text.append(QLatin1String("\\shortcite{") + citeKey + QLatin1String("} "));
                break;
            case ConqSettings::EnumReferenceCommand::citet:
                text.append(QLatin1String("\\citet{") + citeKey + QLatin1String("} "));
                break;
            case ConqSettings::EnumReferenceCommand::citet_:
                text.append(QLatin1String("\\citet*{") + citeKey + QLatin1String("} "));
                break;
            case ConqSettings::EnumReferenceCommand::citep:
                text.append(QLatin1String("\\citep{") + citeKey + QLatin1String("} "));
                break;
            case ConqSettings::EnumReferenceCommand::citep_:
                text.append(QLatin1String("\\citep*{") + citeKey + QLatin1String("} "));
                break;
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
        // frist transform our QVariantList into BibTeX format
        File *bibFile = BibTexVariant::fromVariant(bibEntries);

        // now export it to the source format
        FileExporterBibTeX exporter;
        //TODO: set latex export encoding etc
        QBuffer buffer;
        buffer.open(QBuffer::WriteOnly);
        exporter.save(&buffer, bibFile);
        buffer.close();

        buffer.open(QBuffer::ReadOnly);
        QTextStream ts(&buffer);
        text = ts.readAll();
        buffer.close();

        delete bibFile;
        break;
    }
    case Export_REFERENCE: {
        kDebug() << "BibTexToClipboardPipe::pipeExport :: Export_REFERENCE not supported currently";
    }
    }

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(text);
}

void ClipboardPipe::pipeExport(QList<Nepomuk2::Resource> resources)
{
    QString text;
    switch(m_exportType) {

    // export all citekeys with the specified modifier from the settings. The modifier are used for LaTeX formatting
    case Export_CITEKEY: {
        foreach(const Nepomuk2::Resource &entry, resources) {
            QString citeKey = entry.property(Nepomuk2::Vocabulary::NBIB::citeKey()).toString();
            if(citeKey.isEmpty()) {
                kDebug() << "no citekey found";
                continue;
            }

            switch(m_citeCommand) {
            case ConqSettings::EnumReferenceCommand::none:
                text.append(citeKey + QLatin1String(", "));
                break;
            case ConqSettings::EnumReferenceCommand::cite:
                text.append(QLatin1String("\\cite{") + citeKey + QLatin1String("} "));
                break;
            case ConqSettings::EnumReferenceCommand::citealt:
                text.append(QLatin1String("\\citealt{") + citeKey + QLatin1String("} "));
                break;
            case ConqSettings::EnumReferenceCommand::citeauthor:
                text.append(QLatin1String("\\citeauthor{") + citeKey + QLatin1String("} "));
                break;
            case ConqSettings::EnumReferenceCommand::citeauthor_:
                text.append(QLatin1String("\\citeauthor*{") + citeKey + QLatin1String("} "));
                break;
            case ConqSettings::EnumReferenceCommand::citeyear:
                text.append(QLatin1String("\\citeyear{") + citeKey + QLatin1String("} "));
                break;
            case ConqSettings::EnumReferenceCommand::citeyearpar:
                text.append(QLatin1String("\\citeyearpar{") + citeKey + QLatin1String("} "));
                break;
            case ConqSettings::EnumReferenceCommand::shortcite:
                text.append(QLatin1String("\\shortcite{") + citeKey + QLatin1String("} "));
                break;
            case ConqSettings::EnumReferenceCommand::citet:
                text.append(QLatin1String("\\citet{") + citeKey + QLatin1String("} "));
                break;
            case ConqSettings::EnumReferenceCommand::citet_:
                text.append(QLatin1String("\\citet*{") + citeKey + QLatin1String("} "));
                break;
            case ConqSettings::EnumReferenceCommand::citep:
                text.append(QLatin1String("\\citep{") + citeKey + QLatin1String("} "));
                break;
            case ConqSettings::EnumReferenceCommand::citep_:
                text.append(QLatin1String("\\citep*{") + citeKey + QLatin1String("} "));
                break;
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
        // trasnform Nepomuk Resources To QVariant
        NepomukToVariantPipe ntvp;
        ntvp.pipeExport(resources);
        QVariantList list = ntvp.variantList();

        // frist transform our QVariantList into BibTeX format
        File *bibFile = BibTexVariant::fromVariant(list);

        // now export it to the source format
        FileExporterBibTeX exporter;
        //TODO: set latex export encoding etc
        QBuffer buffer;
        buffer.open(QBuffer::WriteOnly);
        exporter.save(&buffer, bibFile);
        buffer.close();

        buffer.open(QBuffer::ReadOnly);
        QTextStream ts(&buffer);
        text = ts.readAll();
        buffer.close();

        delete bibFile;
        break;
    }
    case Export_REFERENCE: {
        kDebug() << "BibTexToClipboardPipe::pipeExport :: Export_REFERENCE not supported currently";
    }
    }

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(text);
}

void ClipboardPipe::setExportType( PipeExport exportType)
{
    m_exportType = exportType;
}

void ClipboardPipe::setCiteCommand(ConqSettings::EnumReferenceCommand::type citeCommand )
{
    m_citeCommand = citeCommand;
}

void ClipboardPipe::setCiteCommand( int citeCommand )
{
    m_citeCommand = (ConqSettings::EnumReferenceCommand::type)citeCommand;
}
