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

#ifndef KILELYXPIPE_H
#define KILELYXPIPE_H

#include <QtCore/QObject>
#include <QtCore/QVariantList>

#include <Nepomuk2/Resource>

/**
  * @brief pipe the current entry to @c LyX or @c Kile
  *
  * The path for the pipe file has to be set in the settings first
  */
class KileLyxPipe : public QObject
{
public:
    KileLyxPipe(QObject *parent = 0);
    virtual ~KileLyxPipe();

    /**
      * @brief Sets the pointer for the error log
      *
      * used to find out what errors happend during the export
      */
    void setErrorLog(QStringList *errorLog = NULL);

    void pipeExport(const QVariantList & bibEntries);

    void pipeExport(QList<Nepomuk2::Resource> bibEntries);

private:
    /**
      * @todo TODO: automatically find the pipe name from LyX / Kile pipe settings
      */
    QString findLyXKilePipe();
    void sendReferences(const QString &refs);

protected:
    QStringList *m_errorLog;
};

#endif // KILELYXPIPE_H
