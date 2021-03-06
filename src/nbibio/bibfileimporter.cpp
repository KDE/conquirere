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

#include "bibfileimporter.h"

#include <KDE/KLocalizedString>

#include <QtCore/QFile>
#include <QtCore/QIODevice>

BibFileImporter::BibFileImporter()
    : QObject(0)
    , m_cancel(false)
{
}

BibFileImporter::~BibFileImporter()
{
}

bool BibFileImporter::fromFile(QString fileName, QStringList *errorLog)
{
    QFile bibFile(fileName);
    if (!bibFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString error = i18n("can't open file %1", fileName);
        errorLog->append(error);
        return false;
    }

    return load(&bibFile, errorLog);
}

void BibFileImporter::cancel()
{
    m_cancel=true;
}
