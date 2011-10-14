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
#include "conflictmanager.h"

#include <QFile>
#include <QIODevice>

#include <QDebug>

NBibImporter::NBibImporter()
    : QObject(0)
    , m_cancel(false)
    , m_solveConflicts(false)
    , m_duplicatesDetected(0)
    , m_newEntries(0)
{
    m_conflictManager = new ConflictManager();
}

NBibImporter::~NBibImporter()
{
    delete m_conflictManager;
}

bool NBibImporter::fromFile(QString fileName)
{
    QFile bibFile(fileName);
    if (!bibFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "can't open file " << fileName;
        return false;
    }

    return load(&bibFile);
}

void NBibImporter::setAutomaticConflictSolving(bool solve)
{
    m_solveConflicts = solve;
}

bool NBibImporter::solveConflicts()
{
    return m_solveConflicts;
}

void NBibImporter::duplicateDetected()
{
    m_duplicatesDetected++;
}

int NBibImporter::duplicates()
{
    return m_duplicatesDetected;
}

void NBibImporter::newEntryAdded()
{
    m_newEntries++;
}

int NBibImporter::newEntries()
{
    return m_newEntries;
}

void NBibImporter::cancel()
{
    m_cancel=true;
}

ConflictManager *NBibImporter::conflictManager()
{
    return m_conflictManager;
}
