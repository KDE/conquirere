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

#include "syncfile.h"

#include "readfromfile.h"
#include "writetofile.h"

#include <QDebug>

SyncFile::SyncFile(QObject *parent)
    : SyncStorage(parent)
    , m_systemFiles(0)
    , m_rfz(0)
    , m_wtz(0)
{
}

SyncFile::~SyncFile()
{
    delete m_rfz;
    delete m_wtz;
}

void SyncFile::syncWithStorage(File *bibfile, const QString &collection)
{
}

void SyncFile::readSync(File serverFiles)
{
}

void SyncFile::writeSync(File serverFiles)
{

}

void SyncFile::writeProgress(int writeProgress)
{
    qreal curProgress = ((qreal)writeProgress * 0.5) + 50;

    emit progress(curProgress);
}
