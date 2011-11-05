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

#ifndef NBIBIMPORTER_H
#define NBIBIMPORTER_H

#include <QtCore/QObject>

class QIODevice;

class NBibImporter : public QObject
{
    Q_OBJECT
public:
    explicit NBibImporter();
    virtual ~NBibImporter();

    bool fromFile(QString fileName);
    virtual bool load(QIODevice *iodevice, QStringList *errorLog = NULL) = 0;

signals:
    void progress(int current);

public slots:
    virtual void cancel();

protected:
    bool m_cancel;
};

#endif // NBIBIMPORTER_H
