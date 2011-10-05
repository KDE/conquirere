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

#include "nbibimporter.h"

#include <QFile>
#include <QIODevice>

#include <QDebug>

NBibImporter::NBibImporter() :
    QObject(0)
{
}

bool NBibImporter::fromFile(const QString &fileName, QStringList *errorLog)
{
    QFile bibFile(fileName);
    if (!bibFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "can't open file " << fileName;
        return false;
    }

    return load(&bibFile, errorLog);
}

void NBibImporter::cancel()
{
    //do nothing here ...
}
