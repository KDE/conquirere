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

#ifndef NBIBEXPORTER_H
#define NBIBEXPORTER_H

#include <Nepomuk/Resource>

#include <QObject>

class NBibExporter : public QObject
{
    Q_OBJECT
public:
    NBibExporter();
    virtual ~NBibExporter();

    virtual bool save(QIODevice *iodevice, const QList<Nepomuk::Resource> referenceList, QStringList *errorLog = NULL) = 0;

signals:
    void progress(int current, int total);

public slots:
    virtual void cancel();
};

#endif // NBIBEXPORTER_H