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

#ifndef BIBTEXTOVARIANT_H
#define BIBTEXTOVARIANT_H

#include <QObject>
#include <QVariantMap>

class File;
class Entry;

class BibTexVariant : public QObject
{
    Q_OBJECT
public:
    static QVariantList toVariant(const File &bibtexFile);
    static QVariantMap entryToMap(const Entry *e, QMap<QString, QString> lookup);

    static File *fromVariant(const QVariantList &bibtexList);
};

#endif // BIBTEXTOVARIANT_H
