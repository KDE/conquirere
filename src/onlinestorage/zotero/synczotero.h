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

#ifndef SYNCZOTERO_H
#define SYNCZOTERO_H

#include <QObject>

#include <kbibtex/file.h>

class ReadFromZotero;
class WriteToZotero;

class SyncZotero : public QObject
{
    Q_OBJECT
public:
    explicit SyncZotero(QObject *parent = 0);
    virtual ~SyncZotero();

    void setUserName(const QString & name);
    QString userName() const;
    void setPassword(const QString & pwd);
    QString pasword() const;

    void syncWithStorage(File bibfile);

private slots:
    /**
      * Called by the ReadFromZotero
      */
    void readSync(File serverFiles);

    void writeSync(File serverFiles);

private:
    File m_systemFiles;
    ReadFromZotero *m_rfz;
    WriteToZotero *m_wtz;

    QString m_name;
    QString m_password;
};

#endif // SYNCZOTERO_H
