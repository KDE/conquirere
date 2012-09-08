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

#include "bibfileexporter.h"

#include <QtCore/QFile>

BibFileExporter::BibFileExporter()
    : QObject(0)
    , m_cancel(false)
{
}

BibFileExporter::~BibFileExporter()
{
}

bool BibFileExporter::toFile( const QString &filename, const QList<Nepomuk2::Resource> referenceList, QStringList *errorLog)
{
    QFile bibFile(filename);
    if (!bibFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QString error = i18n("Can't open file %1", filename);
        errorLog->append(error);

        return false;
    }

    bool result = save(&bibFile, referenceList, errorLog);

    bibFile.close();

    return result;
}

void BibFileExporter::cancel()
{
    m_cancel=true;
}
